// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Table.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"
#include "rdbModel/Tables/Index.h"
#include "rdbModel/Tables/Assertion.h"
#include "rdbModel/Db/Connection.h"
#include "rdbModel/Db/ResultHandle.h"
#include "rdbModel/RdbException.h"
#include "rdbModel/Tables/InsertNew.h"
#include "rdbModel/Tables/InterRow.h"
#include "rdbModel/Tables/Query.h"
#include "rdbModel/Tables/Set.h"
#include "rdbModel/Tables/Supersede.h"
#include "facilities/Util.h"
#include "facilities/Timestamp.h"
#include <algorithm>

namespace rdbModel {
  Table::Table() : m_primaryKeyCol(""), m_sorted(false), m_nEndUser(0), 
                   m_validRow(0), m_iNew(0), m_sup(0), m_connect(0)    {
    m_sortedCols.clear(); m_programCols.clear(); 
    m_userCols.clear(); m_mayDefault.clear();
    m_out = &std::cout;    m_err = &std::cerr;
  }


  Table::~Table() {
    while (m_cols.size() ) {
      Column* c = m_cols.back();
      m_cols.pop_back();
      delete c;
    }

    while (m_asserts.size() ) {
      Assertion* a = m_asserts.back();
      m_asserts.pop_back();
      delete a;
    }

    while (m_indices.size() ) {
      Index* i = m_indices.back();
      m_indices.pop_back();
      delete i;
    }
  }


  void Table::setConnection(Connection* connect) {
    m_connect = connect;
    m_out = connect->getOut();
    m_err = connect->getErrOut();
  }
  // Columns are stored in order; binary cuts should do better than
  // just dumb compare one by one.
  Column* Table::getColumnByName(const std::string& name) const {
    unsigned nCol = m_sortedCols.size();
    unsigned minI = 0;
    unsigned maxI = nCol;

    unsigned guess = maxI/2;
    unsigned oldGuess = nCol;

    int cmp = name.compare(m_sortedCols[guess]->getName());

    while (cmp != 0)  {
      if (guess == oldGuess) return 0;        // not found

      if (cmp < 0 ) {    // thing we tried is > colName, so decrease maxI
        maxI = guess;
      }  else {   // thing we tried is > colName, so increase minI
        minI =  guess;
      }
      oldGuess = guess;
      guess = (minI + maxI)/2;
      cmp = name.compare(m_sortedCols[guess]->getName());
    }
    return m_sortedCols[guess];

  }

  Index* Table::getIndexByName(const std::string& name) const {
    unsigned nIx = m_indices.size();
    for (unsigned iIx = 0; iIx < nIx; iIx++) {
      Index* index = m_indices[iIx];
      if (index->getName() == name) return index;
    }
    return 0;
  }

  Assertion* Table::getAssertionByName(const std::string& name) const {
    unsigned nAssert = m_asserts.size();
    for (unsigned i = 0; i < nAssert; i++) {
      Assertion* a = m_asserts[i];
      if (a->getName() == name) return a;
    }
    return 0;

  }

  // Traversal order is self, columns, indices, asserts
  Visitor::VisitorState Table::accept(Visitor* v) {
    
    Visitor::VisitorState state = v->visitTable(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;
    if (state != Visitor::VCONTINUE)       return state;

    unsigned n = m_cols.size();

    for (unsigned i = 0; i < n; i++) {
      state = m_cols[i]->accept(v);
      if (state != Visitor::VCONTINUE) return state;
    }

    n = m_indices.size();
    for (unsigned i = 0; i < n; i++) {
      state = m_indices[i]->accept(v);
      if (state != Visitor::VCONTINUE) return state;
    }

    n = m_asserts.size();
    for (unsigned i = 0; i < n; i++) {
      state = m_asserts[i]->accept(v);
      if (state != Visitor::VCONTINUE) return state;
    }
    return state;
  }

  void  Table::sortColumns() {
    if (m_sorted) return;
    m_sortedCols = m_cols;

    ColCompare  cmpObject;

    // Keep around original set so that rdbGUI will display them in
    // this order
    std::sort (m_sortedCols.begin(), m_sortedCols.end(), cmpObject); 
    m_sorted = true;

  }

  void Table::addColumn(Column* c) {
    m_cols.push_back(c);
    if (c->getSourceType() == Column::FROMendUser) {
      m_nEndUser++;
      // if not nullable and no default, user must supply it
      if (!(c->nullAllowed()) && ((c->getDefault()).size() == 0)) {
        m_userCols.push_back(c);
      }
      else {
        m_mayDefault.push_back(c);
      }
    }
    else if (c->getSourceType() == Column::FROMdefault) {
      m_mayDefault.push_back(c);
    }
    else if (c->getSourceType() == Column::FROMprogram) {
      m_programCols.push_back(c);
    }
  }

  int Table::insertLatest(Row& row, int* serial) const {

    if (!m_connect) {
      throw RdbException("Table::insertLatest Need matching connection");
    }
    row.rowSort();

    // Fill in columns in m_programCols list
    fillProgramCols(row, true);

    // Check that all required columns are there and not null
    for (unsigned i = 0; i < m_userCols.size(); i++) {
      FieldVal* f = row.find(m_userCols[i]->getName());
      if ((!f) || (f->m_null)) {
        std::string 
          msg("Table::insertLatest Row to be inserted missing req'd field ");
        msg = msg + m_userCols[i]->getName();
        throw RdbException(msg);
      }
    }
    // Fill in defaults
    fillDefaults(row);

    // Check against consistency conditions specified by <insertNew>
    const Assertion* cond = m_iNew->getInternal();
    bool satisfied;

    try {
      satisfied = cond->verify(row, row);
    }
    catch (RdbException ex) {
      (*m_out) << ex.getMsg() << std::endl;
      return -1;
    }

    if (!satisfied) return -1;   // or maybe throw exception here?

    // Check against official conditions specified by <insertNew>
    const Assertion* condO = m_iNew->getOfficial();
    Row empty;
    std::vector<std::string> colNames;
    std::vector<std::string> colValues;
    std::vector<std::string> nullCols;

    row.regroup(colNames, colValues, nullCols);

    satisfied = condO->verify(empty, row);

    // If not official, do insert and exit
    if (!satisfied) {
      bool ok = m_connect->insertRow(m_name, colNames, colValues,
                                     serial, &nullCols);
      return (ok) ?  0 : -1;
    }

    const std::vector<InterRow* >& inter = m_iNew->getInterRow();

    unsigned nInter = inter.size();
    Assertion* subsAssert;
    for (unsigned iInter = 0; iInter < nInter; iInter++) {
      const Query* q = inter[iInter]->getQuery();
      const std::vector<std::string>& toSelect = q->getToSelect();

      // Need to make a new Assertion which substitues our proposed row
      // values for all "toBe" entries
      subsAssert = new Assertion(q->getAssertion(), &row);

      std::vector<std::string> orderCols;
      orderCols.clear();
      ResultHandle* r = m_connect->select(m_name, toSelect, orderCols, 
                                          subsAssert);

      if (r->getNRows() == 0) {    // no conflicts found
        delete subsAssert;
        continue;   // go on to next InterRow, if any.  
      }

      // otherwise
      //    if quit  go ahead and quit
      if (inter[iInter]->getQuit()) {
        delete subsAssert;
        (*m_out) << "insert latest not done because of irreconcilable " 
                 << std::endl << "conflict with existing rows" << std::endl;
        return -1;
      }
      //    else modify conflicting rows as specified in <interRow>.
      //    (i.e., do an update with where clause = subsAssert

      std::vector<Set> sets = inter[iInter]->getSets();

      bool iu = doInterUpdate(sets, subsAssert, row);
      if (!iu) return -1;
    }
    // Insert and exit
    bool ok = m_connect->insertRow(m_name, colNames, colValues,
                                   serial, &nullCols);
    return (ok) ?  0 : -1;
  }

  int Table::insertRow(Row& row, int* serial) const {

    if (!m_connect) {
      throw RdbException("Table::insertRow Need matching connection");
    }
    row.rowSort();

    // Fill in columns in m_programCols list
    fillProgramCols(row, true);


    // Check that all required columns are there and not null
    for (unsigned i = 0; i < m_userCols.size(); i++) {
      FieldVal* f = row.find(m_userCols[i]->getName());
      if ((!f) || (f->m_null)) {
        std::string 
          msg("Table::insertRow Row to be inserted missing req'd field ");
        msg = msg + m_userCols[i]->getName();
        throw RdbException(msg);
      }
    }

    // Fill in defaults
    fillDefaults(row);

    // Check standard valid row condition if there is one
    if (m_validRow) {
      bool satisfied;
      try {
        satisfied = m_validRow->verify(row, row);
      }
      catch (RdbException ex) {
        (*m_out) << ex.getMsg() << std::endl;
        return -1;
      }
      if (!satisfied) {
        // throw RdbException("Table::insertRow Row to be inserted is not valid");
        (*m_out) << "Table::insertRow Row to be inserted is not valid " 
                  << std::endl;
        m_out->flush();
        return -1;
      }
    }
    // Insert and exit
    std::vector<std::string> colNames;
    std::vector<std::string> colValues;
    std::vector<std::string> nullCols;

    row.regroup(colNames, colValues, nullCols);
    bool ok = m_connect->insertRow(m_name, colNames, colValues,
                                   serial, &nullCols);
    return (ok) ?  0 : -1;
  }    

  int Table::supersedeRow(Row& row, int oldKey, int* serial) const {
    std::string oldKeyStr;
    facilities::Util::itoa(oldKey, oldKeyStr);

    if (m_primaryKeyCol.size() == 0) {
      throw RdbException("Table::supersedeRow No primary key column!");
    }

    // Confirm that row does not contain any of the fixed or oldForced columns
    row.rowSort();
    const std::vector<std::string>& forced = m_sup->getForced();
    for (unsigned i = 0; i < forced.size(); i++) {
      if (row.find(forced[i])) {
        //        throw RdbException
        //          (std::string("Table::supersedeRow bad column in input row ") +
        // forced[i]);
        (*m_out) << "Table::supersedeRow bad column in input row '"
                 << forced[i] << "'" << std::endl;
        m_out->flush();
        return -1;
      }
    }
    const std::vector<FieldVal>& fixed = m_sup->getFixed();
    for (unsigned i = 0; i < fixed.size(); i++) {
      if (row.find(fixed[i].m_colname)) {
        //        throw RdbException
        //          (std::string("Table::supersedeRow bad column in input row ") +
        //           fixed[i].m_colname);
        (*m_out) << "Table::supersedeRow bad column in input row " <<
          fixed[i].m_colname;
        m_out->flush();
        return -1;
      }
    }

    // Check that old row is supersedable
    if (!isSupersedable(oldKeyStr)) {
      *m_out << "Row " << oldKey << " is not supersedable" << std::endl;
      m_out->flush();
      return -1;
    }
    
    // Fill in fixed fields
    const std::vector<std::string>& fixedInterp = m_sup->getFixedInterp();
    for (unsigned i = 0; i < fixed.size(); i++) {

      FieldVal fv = fixed[i];
      if (fixedInterp[i].size() > 0) {
        Column* c = getColumnByName(fixed[i].m_colname);
        c->interpret(fixedInterp[i], fv.m_val);
      }
      row.addField(fv);
    }
    row.rowSort();

    // Fetch needed fields from row to be superseded (oldKey)
    Assertion::Operator* whereOp = 
      new Assertion::Operator(OPTYPEequal, m_primaryKeyCol,
                              oldKeyStr, FIELDTYPEold, FIELDTYPElit);
    Assertion* where = new Assertion(whereOp);

    std::vector<std::string> noCols;
    const std::vector<std::string>& fromOld = m_sup->getFromOld();
    noCols.clear();
    ResultHandle* results = m_connect->select(m_name, fromOld, 
                                              noCols, where);
    //    std::vector<std::string> vals;
    //    results->getRow(vals);
    std::vector<std::string*> vals;
    results->getRowPtrs(vals);
    // Merge as needed into our row.  First bunch may already be in the row
    unsigned nDef = m_sup->getOldDefaulted().size();
    for (unsigned i = 0; i < nDef; i++) {
      if (!(row.find(fromOld[i])) ) {
        if (vals[i] == 0) {
          row.addField(FieldVal(fromOld[i], "", true));
        }
        else {
          row.addField(FieldVal(fromOld[i], *vals[i]));
        }
        row.rowSort();
      }
    }
    // Remainder definitely have to be merged in
    for (unsigned i = nDef; i < fromOld.size(); i++) {
      if (vals[i] == 0) {
        row.addField(FieldVal(fromOld[i], "", true));
      }
      else {
        row.addField(FieldVal(fromOld[i], *vals[i]));
      }
    }
    results->cleanFieldPtrs(vals);
    
    // Insert the row and update old row
    int insRet;
    try {
      insRet = insertRow(row, serial);
    }
    catch (RdbException ex) {
      (*m_out) << ex.getMsg() << std::endl;
      insRet = -1;
    }

    if (insRet) { // something went wrong
      delete where;
      return insRet;
    }

    // Do the update of old row
    const std::vector<Set*>& setOld = m_sup->getSetOld();
    std::vector<FieldVal> oldFields;
    oldFields.reserve(setOld.size());
    for (unsigned i = 0; i < setOld.size(); i++) {
      std::string src = setOld[i]->getSrcValue();
      std::string col = setOld[i]->getDestColName();
      if (setOld[i]->hasInterp() ) {
        Column* c = getColumnByName(col);
        c->interpret(setOld[i]->getInterp(), src);
      }
      oldFields.push_back(FieldVal(col, src));
    }
    Row updateArg(oldFields);
    try {
      int iUpdated = updateRows(updateArg, where);
      if (iUpdated == 1) return 0;
      return -1;
    }
    catch (RdbException uEx) {
      (*m_out) << uEx.getMsg() << std::endl;
      return -1;
    }
  }

  int Table::updateRows(Row &row, Assertion* where) const {

    if (!m_connect) {
      throw RdbException("Table::insertLatest Need matching connection");
    }
    row.rowSort();

    // Fill in columns in m_programCols list
    fillProgramCols(row, false);

    std::vector<std::string> colNames;
    std::vector<std::string> colValues;
    std::vector<std::string> nullCols;

    row.regroup(colNames, colValues, nullCols);

    return m_connect->update(m_name, colNames, colValues, where, &nullCols);
  }

  bool Table::fillProgramCols(Row& row, bool newRow) const {
    std::string val;

    for (unsigned i = 0; i < m_programCols.size(); i++) {
      Column* col = m_programCols[i];
      switch (col->getContentsType() ) {
      case Column::CONTENTSserviceName: 
        val="rdbModel";
        break;
      case Column::CONTENTSusername: {
#ifdef    __GNUG__ 
        val = std::string("$(USER)");
#else
        val = std::string("$(USERNAME)");
#endif

        int nsub = facilities::Util::expandEnvVar(&val);
        if (nsub != 1) {
          val="no username";
        }
        break;
      }

      case Column::CONTENTSinsertTime:
        if (!newRow) continue;   // otherwise, same as updateTime case
      case Column::CONTENTSupdateTime: {
        facilities::Timestamp curTime;
        val = curTime.getString();
        break;
      }
      default:
        return false;    // unrecognized type
      }
      // look for this field in Row input. If there, overwrite value
      FieldVal* f = row.find(col->getName());
      if (f) {
        f->m_val = val;
        f->m_null = false;
      } 
      else {
        FieldVal g(col->getName(), val, false);
        row.addField(g);
        row.rowSort();
      }
    }
    return true;
  }

  void Table::fillDefaults(Row& row) const {
    for (unsigned i = 0; i < m_mayDefault.size(); i++) {
      Column* hasDef = m_mayDefault[i];
      FieldVal* f = row.find(hasDef->getName());
      if (f) continue;  
      // otherwise add it it
      if ((hasDef->getDefault()).size() > 0) {
        FieldVal g(hasDef->getName(), hasDef->getDefault());
        row.addField(g);
      }
      else {   // null
        FieldVal g(hasDef->getName(), "", true);
        row.addField(g);
      }
      row.rowSort();
    }
  }

      
  //  Source in <set>
  // should either be FIELDTYPElit or FIELDTYPEtoBe.   In latter
  // case, substitute from row argument.
  bool Table::doInterUpdate(const std::vector<Set>& sets, 
                            Assertion* subsAssert, Row& toBe)  const     {
    unsigned nSets = sets.size();
    std::vector<FieldVal> fields;
    fields.reserve(nSets);

    for (unsigned iSet = 0; iSet < sets.size(); iSet++) {
      std::string col = sets[iSet].getDestColName();
      std::string src;
      //      updateCols.push_back(sets[iSet].getDestColName());
      FIELDTYPE srcType = sets[iSet].getSrcType();
      if ((srcType == FIELDTYPElit) || (srcType == FIELDTYPElitDef)) {
        src = sets[iSet].getSrcValue();
        // May require special handling
        if (sets[iSet].hasInterp()) {
          Column* c = getColumnByName(col);
          c->interpret(sets[iSet].getInterp(), src);
          /*  should know how to substitute, e.g. val="NOW" should
           be transformed into a timestamp           */
        }
      }
      else {   // must be toBe
        std::string toBeCol = sets[iSet].getSrcValue();
        FieldVal* f = toBe.find(toBeCol);
        if (f == 0) {
          delete subsAssert;
          (*m_out) << "Table::InsertNew Row argument missing needed field"
                   << toBeCol << std::endl;
          m_out->flush();
          return false;
          //          throw RdbException
          //            ("Table::InsertNew Row argument missing needed field");
        }
        src = f->m_val;
        //        updateVals.push_back(f->m_val);
      }
      FieldVal g(col, src);
      fields.push_back(g);
    }
    Row row(fields);

    fillProgramCols(row, false);

    std::vector<std::string> updateCols;
    std::vector<std::string> updateVals;
    std::vector<std::string> nullCols;

    row.regroup(updateCols, updateVals, nullCols);

    // Make specified changes (update)
    m_connect->update(m_name, updateCols, updateVals, subsAssert);
    
    delete subsAssert;
    return true;
  }

  const std::string& Table::setPrimaryKeyCol() {
    static const std::string empty("");
    if (m_primaryKeyCol.size() > 0) return m_primaryKeyCol;

    for (unsigned iCol = 0; iCol < m_cols.size(); iCol++) {
      if (m_cols[iCol]->isPrimaryKey()) {
        m_primaryKeyCol = m_cols[iCol]->getName();
        return m_primaryKeyCol;
      }
    }
    return empty;
  }

  bool Table::isSupersedable(std::string oldKeyStr) const {
    // Make a copy of supersedable condition
    Row emptyRow;
    Assertion::Operator* onlyIf = 
      new Assertion::Operator((m_sup->getOnlyIf())->getOperator(), 
                              &emptyRow);
    // Make operator for key == oldKey
    Assertion::Operator* ser =
      new Assertion::Operator(OPTYPEequal,  m_primaryKeyCol,
                              oldKeyStr, FIELDTYPEold, FIELDTYPElit);
    std::vector<Assertion::Operator*> children;
    children.push_back(onlyIf);
    children.push_back(ser);
    Assertion::Operator* whereOp = 
      new Assertion::Operator(OPTYPEand, children);

    std::vector<std::string> queryCols;
    queryCols.push_back(m_primaryKeyCol);
    Assertion* where = new Assertion(whereOp);
    ResultHandle* results = m_connect->select(m_name, queryCols, queryCols,
                                              where);
    delete where;

    return (results->getNRows() > 0);
  }

}
