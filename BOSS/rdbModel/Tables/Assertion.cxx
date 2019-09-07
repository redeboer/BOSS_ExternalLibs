// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Assertion.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#include "rdbModel/Rdb.h"
#include "rdbModel/Tables/Assertion.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"
#include "rdbModel/Tables/Datatype.h"
#include "rdbModel/RdbException.h"
#include "facilities/Util.h"
#include "facilities/Timestamp.h"
namespace rdbModel {

  Assertion::Operator::~Operator() {
    if (!m_keepChildren) {
      while (m_operands.size() ) {
        Operator* op = m_operands.back();
        m_operands.pop_back();
        delete op;
      }
    }
  }

  // Substitute values from toBe row as needed.
  Assertion::Assertion(const Assertion* orig, Row* toBe) 
    : m_op(0), m_myTable(orig->m_myTable), m_keepOp(false), m_name(""),
      m_compiled("") {
    toBe->rowSort();
    m_op = new Assertion::Operator(orig->m_op, toBe);

  }

  Assertion::~Assertion() {
    if (!m_keepOp) delete m_op;
  }

  // Constructor for comparisons
  // It's also used for isNull and isEmpty, but in those cases rightArg 
  // and rightLiteral are ignored.
  // Internally they get set to "" and FIELDTYPElit, resp.
  Assertion::Operator::Operator(OPTYPE type, const std::string& leftArg, 
                                const std::string& rightArg, 
                                FIELDTYPE leftLiteral, 
                                FIELDTYPE rightLiteral) 
    :  m_opType(type), m_keepChildren(false), m_toBe(false), m_old(false) {

    m_tableName.clear();
    m_operands.clear();
    if (!isCompareOp()) {
      m_opType = OPTYPEundefined;
      return;
    }

    m_compareArgs[0] = leftArg;
    m_compareArgs[1] = rightArg;
    m_compareType[0] = leftLiteral;
    m_compareType[1] = rightLiteral;
    if ((type == OPTYPEisNull) || (type ==OPTYPEisEmpty)) {
      m_compareType[1] = FIELDTYPElit;
      m_compareArgs[1] = "";
    }
    m_toBe = ((leftLiteral == FIELDTYPEtoBe) || 
              (rightLiteral == FIELDTYPEtoBe));
    m_old = ((leftLiteral == FIELDTYPEold) || 
             (rightLiteral == FIELDTYPEold));
  }

  /// Constructor for EXISTS
  Assertion::Operator::Operator(OPTYPE type, const std::string& tableName,
                                Operator* child) : m_opType(type),
                                                   m_tableName(tableName),
                                                   m_keepChildren(false)
  {
    if (type != OPTYPEexists) {
      m_opType = OPTYPEundefined;
      return;
    }
    m_operands.clear();
    m_operands.push_back(child);
  }

  /// Constructor for OR, AND, NOT
  Assertion::Operator::Operator(OPTYPE type, 
                                const std::vector<Operator*>& children,
                                bool keepChildren)  :
    m_opType(type), m_keepChildren(keepChildren) {

    m_toBe = false;
    m_old = false;
    if ((type == OPTYPEor) || (type == OPTYPEand) || (type = OPTYPEnot)) {
      m_tableName.clear();
      unsigned int nChild = children.size();
      if (!nChild) {
        m_opType = OPTYPEundefined;
        return;
      }
      for (unsigned int iChild = 0; iChild < nChild; iChild++) {
        m_toBe |= (children[iChild]->m_toBe);
        m_old |= (children[iChild]->m_old);
        m_operands.push_back(children[iChild]);
      }
    }
    else m_opType = OPTYPEundefined;
  }


  /// Copy an operator, substituting from toBe row as appropriate
  Assertion::Operator::Operator(Operator* op, Row* toBe) 
    : m_opType(op->m_opType), m_tableName(op->m_tableName), 
      m_keepChildren(false), m_toBe(false), m_old(op->m_old) {
    m_operands.clear();

    switch (m_opType) {
      // OPTYPEor, and, not and exists all have child operators
    case OPTYPEor:
    case OPTYPEand: 
    case OPTYPEnot: 
    case OPTYPEexists:
      {
        unsigned nChild = op->m_operands.size();
        for (unsigned iChild = 0; iChild < nChild; iChild++) {
          Operator* child = new Operator((op->m_operands)[iChild], toBe);
          appendChild(child);
        }
        break;
      }
    case OPTYPEequal:
    case OPTYPEnotEqual:
    case OPTYPElessThan:
    case OPTYPEgreaterThan:
    case OPTYPElessOrEqual:
    case OPTYPEgreaterOrEqual:
    case OPTYPEisEmpty:
    case OPTYPEisNull: {
      for (unsigned i = 0; i < 2; i++) {
        if ((op->m_compareType[i] == FIELDTYPEtoBe) || 
            (op->m_compareType[i] == FIELDTYPEtoBeDef) ) { 
          // have to supply value from row
          FieldVal* f = toBe->find(op->m_compareArgs[i]);
          if (!f) {
            throw RdbException
              ("Assertion::Operator constructor can't resolve field");
          }
          m_compareArgs[i] = f->m_val;
          m_compareType[i] = FIELDTYPElit;
        }
        else {   // just copy what's there
          m_compareArgs[i] = op->m_compareArgs[i];
          m_compareType[i] =op->m_compareType[i];
        }
      }
      break;
    }
    default:
      throw RdbException("Assertion::Operator constructor - Unknown OP type");
    }
  }

  // This only makes sense for conjunction-style operators AND, OR
  bool Assertion::Operator::appendChild(Operator* child) {
    m_toBe |= child->m_toBe;
    m_old |= child->m_old;
    if  ((m_opType == OPTYPEor) || (m_opType == OPTYPEand) ) {
      m_operands.push_back(child);
      return true;
    }
    else if ((m_opType == OPTYPEnot) && (m_operands.size() == 0) ) {
      m_operands.push_back(child);
      return true;
    }
    throw RdbException("Assertion::Operator::appendChild: wrong parent operator type");
    return false;
  }


  bool Assertion::Operator::validCompareOp(Table* myTable) const {
    if (m_compareType[0] != FIELDTYPElit) {
      Column* col0 = myTable->getColumnByName(m_compareArgs[0]);
      if (m_compareType[1] != FIELDTYPElit) {
        Column* col1 = myTable->getColumnByName(m_compareArgs[1]);
        return col1->isCompatible(col0);
      }
      else {  // one column, one literal
        return col0->okValue(m_compareArgs[1], false);
      }
    }
    else { // 1st arg is a literal; second arg must be column
      Column* col1 = myTable->getColumnByName(m_compareArgs[1]);
      return col1->okValue(m_compareArgs[0], false);        
    }
  }
  

  /// Throw exception if Operator is not a comparison operator
  const std::string* Assertion::Operator::getCompareArgs() const {
    if (!isCompareOp()) 
      throw RdbException("Assertion::Operator::getCompareArgs: wrong type");
    return &m_compareArgs[0];
  }

  /// Throw exception if Operator is not a comparison operator
  //  const bool* Assertion::Operator::getLiteralness() const {
  const FIELDTYPE* Assertion::Operator::getCompareArgTypes() const {
    if (!isCompareOp()) 
      throw RdbException("Assertion::Operator::getLiteralness: wrong type");
    return &m_compareType[0];
  }

  /// Throw exception if Operator is not EXISTS
  const std::string& Assertion::Operator::getTableName() const {
    if (m_opType != OPTYPEexists) 
      throw RdbException("Assertion::Operator::getTableName: wrong type");
    return m_tableName;
  }


  bool Assertion::Operator::verify(Row& old, Row& toBe, Table* t)  const {
    switch(m_opType) {
    case OPTYPEor: {
      unsigned nChild = m_operands.size();
      for (unsigned i = 0; i < nChild; i++) {
        if (m_operands[i]->verify(old, toBe, t)) return true;
      }
      return false;
    }

    case OPTYPEand: {
      unsigned nChild = m_operands.size();
      for (unsigned i = 0; i < nChild; i++) {
        if (!(m_operands[i]->verify(old, toBe, t))) return false;
      }
      return true;
    }
    case OPTYPEnot:
      return (!(m_operands[0]->verify(old, toBe, t)));

      
    case OPTYPEisNull: 
    case OPTYPEisEmpty: 
      // These two are almost the same
      {
      Row* r = 0;
      if ( (m_compareType[0] == FIELDTYPEtoBe) || 
           (m_compareType[0] == FIELDTYPEtoBeDef) ) r = &toBe;
      else r = &old;

      FieldVal* f = r->find(m_compareArgs[0]);
      if (f) {
        if (m_opType == OPTYPEisNull) return f->m_null;
        return ((f->m_val).size() == 0);
      }
      else {  // improper input.  Field should have been found
        throw RdbException("Assertion::Operator::verify missing isNull field");
      }
    }
      // handle all 2-argument compare operators together
    case OPTYPEequal:
    case OPTYPEnotEqual:
    case OPTYPElessThan:
    case OPTYPEgreaterThan:
    case OPTYPElessOrEqual:
    case OPTYPEgreaterOrEqual:
      return verifyCompare(old, toBe, t);

    default:
      return false;
    }
    return false;
  }

  bool Assertion::Operator::verifyCompare(Row& old, Row& toBe, Table* t) const 
  {
    // Assume we already know the comparison is sensible; that is, that
    // the two args are of compatible types.  This check should be
    // done when the operator is constructed (by XercesBuilder)
    std::string values[2];
    std::string colname;
    colname.clear();   // used to determine type (string, int,..) of compare

    for (unsigned i = 0; i < 2; i++) {
      switch (m_compareType[i]) {
      case FIELDTYPElit:
      case FIELDTYPElitDef:
        values[i] = m_compareArgs[i];
        break;
      case FIELDTYPEold:
      case FIELDTYPEoldDef: {
        FieldVal* f = old.find(m_compareArgs[i]);
        if (!f) {
          std::string msg = 
            std::string("Assertion::Operator::verifyCompare missing field ")
            + m_compareArgs[i];
          throw
            RdbException(msg);
        }
        values[i] = f->m_val;
        colname = f->m_colname;
        break;
      }
      case FIELDTYPEtoBe:
      case FIELDTYPEtoBeDef: {
        FieldVal* f = toBe.find(m_compareArgs[i]);
        if (!f) {
          throw
            RdbException("Assertion::Operator::verifyCompare missing field");
        }
        values[i] = f->m_val;
        colname = f->m_colname;
        break;
      }
      default:
        throw
          RdbException("Assertion::Operator::verifyCompare bad arg type");
      }
    }
    if (colname.size() > 0) {// determine type to convert to
      Column* c = t->getColumnByName(colname);
      Datatype* d = c->getDatatype();
      
      switch (d->getType()) {
      case Datatype::TYPEtimestamp:
      case Datatype::TYPEdatetime:
        return compareTs(&values[0], m_opType);
      case Datatype::TYPEint:
      case Datatype::TYPEmediumint:
      case Datatype::TYPEsmallint:
        return compareInt(&values[0], m_opType);
      case Datatype::TYPEreal:
      case Datatype::TYPEdouble:
        return compareFloat(&values[0], m_opType);
      default:  // do nothing
        ;
      }
    }
    // Otherwise they're strings.
    return compareString(&values[0], m_opType);
  }
  /// Throw exception if Operator is a comparison operator
  const std::vector<Assertion::Operator* >& 
  Assertion::Operator::getChildren() const {
    if (isCompareOp()) 
      throw RdbException("Assertion::Operator::getChildren: wrong type");
    return m_operands;
  }

  Visitor::VisitorState  Assertion::accept(Visitor* v) {
    Visitor::VisitorState state = v->visitAssertion(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;
    return state;
  }

  bool Assertion::verify(Row& old, Row& toBe)  const {

    if (getOld() ) { // will actually use old vector, so sort
      old.rowSort();
    }
    if (getToBe() ) { // will actually use toBe vector, so sort
      toBe.rowSort();
    }
    return m_op->verify(old, toBe, m_myTable);
  }

  bool Assertion::Operator::compareTs(const std::string* vals, 
                                      OPTYPE type)     const {
    using facilities::Timestamp;
    Timestamp left, right;
    try {
      left = Timestamp(*vals);
      right = Timestamp(*(vals + 1));
    }
    catch(facilities::BadTimeInput ex) {
      throw
        RdbException("Assertion::Operator::CompareTs illegal input");
    }

    switch (type) {
    case OPTYPEequal:
      return left == right;
    case OPTYPEnotEqual:
      return left != right;
    case OPTYPElessThan:
      return left < right;
    case OPTYPEgreaterThan:
      return left > right;
    case OPTYPElessOrEqual:
      return left <= right;
    case OPTYPEgreaterOrEqual:
      return right >= right;
    default:
      throw RdbException("Assertion::Operator::compareTs bad OPTYPE");
    }
    return false;

    
  }

  bool Assertion::Operator::compareInt(const std::string* vals, 
                                       OPTYPE type)   const   {
    using facilities::Util;

    try {
      int i= Util::stringToInt(*vals);
      i = Util::stringToInt(*(vals + 1));
    }
    catch (facilities::WrongType ex) {
      throw
        RdbException("Assertion::Operator::compareInt illegal input");
    }
    return compareFloat(vals, type);
  }

  /// Handling specific to floating point data
  bool Assertion::Operator::compareFloat(const std::string* vals, 
                                         OPTYPE type) const       {
    using facilities::Util;
    double left, right;
    try {
      left = Util::stringToDouble(*vals);
      right = Util::stringToDouble(*(vals + 1));
    }
    catch (facilities::WrongType ex) {
      throw
        RdbException("Assertion::Operator::compareFloat illegal input");
    }
    switch (type) {
    case OPTYPEequal:
      return left == right;
    case OPTYPEnotEqual:
      return left != right;
    case OPTYPElessThan:
      return left < right;
    case OPTYPEgreaterThan:
      return left > right;
    case OPTYPElessOrEqual:
      return left <= right;
    case OPTYPEgreaterOrEqual:
      return right >= right;
    default:
      throw RdbException("Assertion::Operator::compareFloat bad OPTYPE");
    }
    return false;
  }

  /// Handling specific to string data.  Only supported operators for
  /// strings are == and !=
  bool Assertion::Operator::compareString(const std::string* vals, 
                                          OPTYPE type) const       {
    switch (type) {
    case OPTYPEequal: 
      return ( (*vals).compare(*(vals+1)) == 0 );
    case OPTYPEnotEqual:
      return ( (*vals).compare(*(vals+1)) != 0 );
    default:
      throw 
        RdbException("Assertion::Operator::compareString Unsupported OPTYPE");
    }
    return false;
  }

}
