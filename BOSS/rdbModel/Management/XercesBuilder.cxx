// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Management/XercesBuilder.cxx,v 1.2 2011/02/22 06:16:29 maqm Exp $
#include "rdbModel/Management/XercesBuilder.h"
#include "rdbModel/Management/Manager.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"
#include "rdbModel/Tables/Assertion.h"
#include "rdbModel/Tables/Datatype.h"
#include "rdbModel/Tables/Index.h"
#include "rdbModel/Tables/Set.h"
#include "rdbModel/Tables/Query.h"
#include "rdbModel/Tables/InterRow.h"
#include "rdbModel/Tables/Supersede.h"
#include "rdbModel/Tables/InsertNew.h"
#include "facilities/Util.h"
#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include <iostream>
#include <cstdlib>

// Following are what code expects:
#define SCHEMA_MAJOR_VERSION 2
#define SCHEMA_MINOR_VERSION 0
namespace rdbModel {
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;

  XercesBuilder::XercesBuilder() : Builder(), m_doc(0), m_rdb(0) {
  }

  unsigned int XercesBuilder::parseInput(const std::string& filename) {
    xmlBase::XmlParser parser;

    parser.doSchema(true);

    //    m_doc = parser.parse(filename.c_str(), "rdbms");
    m_doc = parser.parse(filename.c_str());

    return (m_doc == 0) ? 0xffffffff : 0;
  }

  int XercesBuilder::buildRdb() {
    using xmlBase::Dom;

    Manager* man = Manager::getManager();

    if (m_doc == 0 ) return 0;
    m_rdb = man->getRdb();
    DOMElement* docElt = m_doc->getDocumentElement();

    
    //  save attribute information associated with outermost (rdbms) element.
    m_rdb->m_dbName = Dom::getAttribute(docElt, "dbs");
    m_rdb->m_majorVersion = 0;
    m_rdb->m_minorVersion = 0;

    std::string versionString = Dom::getAttribute(docElt, "SchemaVersion");
    if (!versionString.size()) {
      versionString = Dom::getAttribute(docElt, "DTDversion");
    }

    unsigned dotPos = versionString.find('.');

    std::string minorStr = std::string(versionString, dotPos+1);
    //                               versionString.size() - (dotPos+1));
    versionString.resize(dotPos);    // now contains just major #

    try {
      m_rdb->m_majorVersion = facilities::Util::stringToInt(versionString);
      m_rdb->m_minorVersion = facilities::Util::stringToInt(minorStr);
    }
    catch (facilities::WrongType ex) {
      std::cerr << "rdbModel::XercesBuilder: Bad version string " << std::endl;
    }
    m_rdb->m_CVSid = Dom::getAttribute(docElt, "CVSid");
    if (m_rdb->m_majorVersion != SCHEMA_MAJOR_VERSION) {
      std::cerr << "Schema major version " << m_rdb->m_majorVersion 
                << " doesn't match expected " << SCHEMA_MAJOR_VERSION 
                << std::endl;
      std::cerr << "Bye for now";
      std::cerr.flush();
      exit(1);
    }

    // Get vector of table elements.  
    std::vector<DOMElement*> tables;
    Dom::getChildrenByTagName(docElt, "table", tables);
    unsigned int nTable = tables.size();
    unsigned int processed = 0;

    for (unsigned int iTable = 0; iTable < nTable; iTable++) {
      Table* newTable = buildTable(tables[iTable]);

      if (newTable) {
        m_rdb->addTable(newTable);
        processed++;
      }
    }
    return nTable - processed;
  }

  Table* XercesBuilder::buildTable(DOMElement* tableElt) {
    using xmlBase::Dom;

    Table* newTable = new Table;
    newTable->m_name = Dom::getAttribute(tableElt, "name");
    newTable->m_version = Dom::getAttribute(tableElt, "version");
    newTable->m_comment = Dom::getAttribute(tableElt, "comment");

    std::vector<DOMElement* > children;
    Dom::getChildrenByTagName(tableElt, "col", children);
    unsigned int nChild = children.size();

    // Delegate handling of columns associated with this table
    for (unsigned int iCol = 0; iCol < nChild; iCol++) {
      Column* newCol = buildColumn(children[iCol], newTable);

      if (newCol) {
        newTable->addColumn(newCol);
      }
    }

    newTable->sortColumns();

    // Look for primary key element, if any
    DOMElement* primaryKey = 
      Dom::findFirstChildByName(tableElt, "primary");
    if (primaryKey != 0) {
      Index* newIndex = buildIndex(primaryKey, true, newTable);
      if (newIndex) {
        newIndex->m_myTable = newTable;
        newTable->addIndex(newIndex);
      }
    }
    newTable->setPrimaryKeyCol();

    // Handle any other indices
    Dom::getChildrenByTagName(tableElt, "index", children);
    nChild = children.size();

    for (unsigned int iIndex = 0; iIndex < nChild; iIndex++) {
      Index* newIndex = buildIndex(children[iIndex], false, newTable);
      if (newIndex) {
        newTable->addIndex(newIndex);
      }
    }
    
    // Check that there is at most one primary key??

    // Handle assertion elements
    Dom::getChildrenByTagName(tableElt, "assert", children);
    nChild = children.size();

    for (unsigned int iAssert = 0; iAssert < nChild; iAssert++) {
      Assertion* newAssert = buildAssertion(children[iAssert], newTable);
      if (newAssert) {
        newTable->addAssert(newAssert);
      }
    }
    // If there was a 'validRow' assertion, make that explicit
    Assertion* v = newTable->getAssertionByName("validRow");
    newTable->setValidRow(v);

    DOMElement* iNewElt = Dom::findFirstChildByName(tableElt, "insertNew");
    if (iNewElt) {
      newTable->m_iNew = buildInsertNew(iNewElt, newTable);
    }
    DOMElement* supElt = Dom::findFirstChildByName(tableElt, "supersede");
    if (supElt) {
      newTable->m_sup = buildSupersede(supElt, newTable);
    }

    return newTable;
  }  

  Column* XercesBuilder::buildColumn(DOMElement* e, Table* myTable) {
    using xmlBase::Dom;

    Column* newCol = new Column(myTable);
    //    m_default.clear();
    newCol->m_name = Dom::getAttribute(e, "name");
    DOMElement* com = Dom::findFirstChildByName(e, "comment");
    newCol->m_comment = Dom::getTextContent(com);

    DOMElement* src = Dom::findFirstChildByName(e, "src");

    newCol->m_null = (Dom::getAttribute(src, "null") == "true");
    newCol->m_stickyInsert = 
      (Dom::getAttribute(src, "stickyInsert") == "true");

    DOMElement* child = Dom::getFirstChildElement(src);
    if (Dom::checkTagName(child, "default")) {
      newCol->m_from = Column::FROMdefault;
      newCol->m_default = Dom::getAttribute(child, "value");
    }
    else if (Dom::checkTagName(child, "from")) {
      std::string agent = Dom::getAttribute(child, "agent");
      if (agent == "auto_increment") {
        newCol->m_from = Column::FROMautoIncrement;
      }
      else if (agent == "now") {
        newCol->m_from = Column::FROMnow;
      }
      else if (agent == "enduser") {
        newCol->m_from = Column::FROMendUser;
      }
      else if (agent == "service") {
        newCol->m_from = Column::FROMprogram;
        std::string contents = Dom::getAttribute(child, "contents");
        if (contents == "service_name") {
          newCol->m_contents = Column::CONTENTSserviceName;
        }
        else if (contents == "username") {
          newCol->m_contents = Column::CONTENTSusername;
        }
        else if (contents == "insert_time") {
          newCol->m_contents = Column::CONTENTSinsertTime;
        }
        else if (contents == "update_time") {
          newCol->m_contents = Column::CONTENTSupdateTime;
        }
        // otherwise just stick with default value of CONTENTSunspecified
      }
      // shouldn't be anything else
    } 

    DOMElement* dtype = Dom::findFirstChildByName(e, "type");
    newCol->m_type = buildDatatype(dtype);
    
    return newCol;
  }

  Datatype* XercesBuilder::buildDatatype(DOMElement* e) {
    using xmlBase::Dom;

    Datatype* newType = new Datatype;
    newType->setType(Dom::getAttribute(e, "typename"));

    if (Dom::hasAttribute(e, "size")) {
      try {
        newType->m_outputSize = Dom::getIntAttribute(e, "size");
      }
      catch (xmlBase::DomException ex) {
        std::cerr << "Error in rdb database description file" << std::endl;
        std::cerr << ex.getMsg() << std::endl;
        std::cerr << "Ignoring column size specification " << std::endl;
        newType->m_outputSize = -1;        // treat as unspecified
      }
    }
    else newType->m_outputSize = -1;
    if ((newType->m_outputSize == -1) &&
        (newType->getType() == Datatype::TYPEchar) ) newType->m_outputSize = 1;
    if ((newType->m_outputSize == -1) &&
        (newType->getType() == Datatype::TYPEvarchar) ) { // not allowed
      std::cerr << "Error in rdb database description file: " << std::endl;
      std::cerr << "Missing size spec. for varchar field " << std::endl;
      delete newType;
      newType = 0;
      return newType;
    }


    DOMElement* restrict = Dom::getFirstChildElement(e);

    if (restrict != 0) {
      DOMElement* rtype = Dom::getFirstChildElement(restrict);
      std::string tagname = Dom::getTagName(rtype);
      if ((newType->m_type == Datatype::TYPEenum) &&
          (tagname != std::string("enum") ) ) {
        std::cerr << "From rdbMode::XercesBuilder::buildDatatype" << std::endl;
        std::cerr << "Bad enum type. Missing value list " << std::endl;
        delete newType;
        newType = 0;
        return newType;
      }

      if (tagname == std::string("nonnegative")) {
        newType->m_restrict = Datatype::RESTRICTnonneg;
        if (newType->m_isInt) newType->m_minInt = 0;
      }
      else if (tagname == std::string("positive")) {
        newType->m_restrict = Datatype::RESTRICTpos;
        if (newType->m_isInt) newType->m_minInt = 1;
      }
      else if (tagname == std::string("interval")) {
        newType->setInterval(Dom::getAttribute(rtype, "min"),
                             Dom::getAttribute(rtype, "max"));
      }
      else if (tagname == std::string("file")) {
        newType->m_restrict = Datatype::RESTRICTfile;
      }
      else if (tagname == std::string("enum")) {
        newType->m_restrict = Datatype::RESTRICTenum;
        Enum* newEnum  = new Enum();
        newEnum->m_required = 
          (Dom::getAttribute(rtype, "use") == "require");
        if (!(newEnum->m_required) && 
            (newType->m_type == Datatype::TYPEenum)) { //improper enum decl.
          delete newEnum;
          delete newType;
          std::cerr << "From rdbMode::XercesBuilder::buildDatatype" 
                    << std::endl;
          std::cerr << "Bad enum type. List must be 'required' " << std::endl;
          newType = 0;
          return newType;
        }  // end improprer enum decl.
          
        std::string enums = Dom::getAttribute(rtype, "values");

        unsigned int start = 0;
        std::string::size_type blankLoc = enums.find(std::string(" "), start);

        while (blankLoc != std::string::npos) {
          newEnum->m_choices.push_back(enums.substr(start, blankLoc-start));
          start = blankLoc + 1;
          blankLoc = enums.find(std::string(" "), start);
        }   // parsing enum list
        newEnum->m_choices.push_back(enums.substr(start));
        newType->m_enum = newEnum;
      }   // end processing of enum restriction
    }
    else {   // no restriction specified
      newType->m_restrict = Datatype::RESTRICTnone;
      if (newType->m_type == Datatype::TYPEenum) { 
        std::cerr << "From rdbMode::XercesBuilder::buildDatatype" 
                  << std::endl;
        std::cerr << "Bad enum type. Missing value list " << std::endl;
        delete newType;
        newType = 0;
      }
    }
    return newType;
  }

  Index* XercesBuilder::buildIndex(DOMElement* e, bool primaryElt,
                                   Table* myTable) {
    using xmlBase::Dom;

    Index* newIndex = new Index(myTable);

    if (primaryElt) { // DOMElement* is a <primary> 
      newIndex->m_primary = true;
      std::string col = newIndex->m_name = Dom::getAttribute(e, "col");
      newIndex->m_indexCols.push_back(newIndex->m_name);
      Column* myCol = myTable->getColumnByName(col);
      myCol->m_isPrimaryKey = true;
    }
    else { // DOMElement* is <index>
      newIndex->m_name = Dom::getAttribute(e, "name");

      std::string primaryVal = 
        Dom::getAttribute(e, "primary");
      newIndex->m_primary = (primaryVal == "yes");

      // Value of "cols" attribute is a blank-separated list of column names
      std::string cols = Dom::getAttribute(e, "cols");

      // Could make this more robust by checking there is really just one below
      if (newIndex->m_primary) {   // had better be just one column
        Column* myCol = myTable->getColumnByName(cols);
        myCol->m_isPrimaryKey = true;
      }

      unsigned int start = 0;
      std::string::size_type blankLoc = cols.find(std::string(" "), start);

      while (blankLoc != std::string::npos) {
        newIndex->m_indexCols.push_back(cols.substr(start, blankLoc-start));
        start = blankLoc + 1;
        blankLoc = cols.find(std::string(" "), start);
      }
      newIndex->m_indexCols.push_back(cols.substr(start));

    }
    return newIndex;
  }

  Assertion* XercesBuilder::buildAssertion(DOMElement* e, Table* myTable) {

    
    //    std::string when = xmlBase::Dom::getAttribute(e, "case");
    
    //    Assertion::WHEN whenType = (when == "globalCheck") ? 
    //      Assertion::WHENglobalCheck  : Assertion::WHENchangeRow;
    std::string name = xmlBase::Dom::getAttribute(e, "name");
    DOMElement* opElt = xmlBase::Dom::getFirstChildElement(e);
    Assertion::Operator* op = buildOperator(opElt, myTable);

    Assertion* newAssert = new Assertion(op, myTable);

    newAssert->setName(name);
    return newAssert;
  }


  Assertion::Operator* XercesBuilder::buildOperator(DOMElement* e, 
                                                    Table* myTable) {
    using xmlBase::Dom;

    std::string opName = Dom::getTagName(e);
    OPTYPE opType = OPTYPEisNull;
    if ((opName == "isNull") || (opName == "isEmpty")) {
      if (opName == "isEmpty") opType = OPTYPEisEmpty;
      DOMElement* child = Dom::getFirstChildElement(e);
      FIELDTYPE valType;
      std::string which = Dom::getAttribute(child, "which");
      valType =  (which == std::string("old")) ? FIELDTYPEold 
        : FIELDTYPEtoBe;

      return new Assertion::Operator(opType,
                                     Dom::getAttribute(child, "col"),
                                     std::string(""), 
                                     valType, valType);
                                     
      //                                     std::string(""), false, false);
    }
    else if (opName == "compare") {
      std::string relation = Dom::getAttribute(e, "relation");
      if (relation == "lessThan") opType = OPTYPElessThan;
      else if (relation == "greaterThan") {
        opType = OPTYPEgreaterThan;
      }
      else if (relation == "equal") opType = OPTYPEequal;
      else if (relation == "notEqual") 
        opType = OPTYPEnotEqual;
      else if (relation == "lessOrEqual") {
        opType = OPTYPElessOrEqual;
      }
      else if (relation == "greaterOrEqual") {
        opType = OPTYPEgreaterOrEqual;
      }
      DOMElement* child[2];
      child[0] = Dom::getFirstChildElement(e);
      child[1] = Dom::getSiblingElement(child[0]);

      std::string compareArgs[2];
      //      bool isLit[2];
      FIELDTYPE  valueType[2];
      for (unsigned iChild = 0; iChild < 2; iChild++) {
        /*  Do 
        compareArgs[iChild] = 
          xmlBase::Dom::getAttribute(child[iChild], "val");
        */
      
        // Element is either a <colRef> or a <value> 
        if (Dom::checkTagName(child[iChild], "value")) {
          valueType[iChild] =  FIELDTYPElit;
          compareArgs[iChild] = /* content of <value> */
            Dom::getTextContent(child[iChild]);
        }
        else {  // get compareArgs from 'col' attribute
          compareArgs[iChild] = 
            Dom::getAttribute(child[iChild], "col");
          // need to look at 'which' attribute
          std::string which = Dom::getAttribute(child[iChild], 
                                                         "which");
          if  (which == std::string("old")) {
            valueType[iChild] = FIELDTYPEold;
          } else  if  (which == std::string("toBe")) {
            valueType[iChild] = FIELDTYPEtoBe;
          }
          else valueType[iChild] = FIELDTYPEask;
        }
      }
      Assertion::Operator* newOp = 
        new Assertion::Operator(opType, compareArgs[0], compareArgs[1],
                                valueType[0], valueType[1]);
      if (!newOp->validCompareOp(myTable)) {
        delete newOp;
        return 0;
      }
      return newOp;
    } 

    // All other cases have other operators as children
    else if (opName == "exists") {
      std::string tableName;
      opType = OPTYPEexists;
      if (Dom::hasAttribute(e, "tableName") ) {
        tableName = 
          Dom::getAttribute(e, "tableName");
      }
      else tableName = myTable->getName();
      DOMElement* child = Dom::getFirstChildElement(e);
      Assertion::Operator* childOp = buildOperator(child, myTable);
      return new Assertion::Operator(opType, tableName, childOp);
    }

    else if (opName == "or") opType = OPTYPEor;
    else if (opName == "and") opType = OPTYPEand;
    else if (opName == "not") opType = OPTYPEnot;

    // Recursively handle child operators
    std::vector<DOMElement*> children;
    std::vector<Assertion::Operator*> childOps;
    Dom::getChildrenByTagName(e, "*", children);
    unsigned nChild = children.size();
    for (unsigned iChild = 0; iChild < nChild; iChild++) {
      Assertion::Operator* childOp = buildOperator(children[iChild], myTable);
      if (childOp) {
        childOps.push_back(childOp);
      }
      else { // one bad apple and we're dead
        return 0;
      }
    }
    return new Assertion::Operator(opType, childOps);
  }

  Set* XercesBuilder::buildSet(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                               Table* t) {
    using xmlBase::Dom;

    std::string destCol = Dom::getAttribute(e, "destCol");

    std::string destRow = Dom::getAttribute(e, "destRow");
    FIELDTYPE destType = (destRow == std::string("old") ) ? FIELDTYPEold :
      FIELDTYPEtoBe;

    // Now find out what kind of source there is for the <set>
    FIELDTYPE srcType;
    std::string srcValue;
    std::string interp("");
    DOMElement* srcElt = Dom::findFirstChildByName(e, "*"); 
    std::string tag = Dom::getTagName(srcElt);
    if (tag == std::string("ask")) {
      srcType = FIELDTYPEask;
      srcValue = "";
    }
    else if (tag == std::string("value") ) {
      srcType = FIELDTYPElit;
      srcValue = Dom::getTextContent(srcElt);
      interp = Dom::getAttribute(srcElt, "interp");
    }
    else { // it's a setColRef element
      std::string forceStr= Dom::getAttribute(srcElt, "force"); 
      bool force = (forceStr == std::string("true"));
      srcValue = Dom::getAttribute(srcElt, "col");
      std::string which = Dom::getAttribute(srcElt, "which");
      srcType = (which == std::string("old")) ? FIELDTYPEold 
        : FIELDTYPEtoBe;
      if (!force) {
        if  (srcType == FIELDTYPEold) srcType = FIELDTYPEoldDef;
        else srcType = FIELDTYPEtoBeDef;
      }
    }
    return new Set(t, destCol, destType, srcValue, srcType, interp);
  }


  Supersede* 
  XercesBuilder::buildSupersede(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                                Table* t) {
    using xmlBase::Dom;

    // <supersede> has an optional attribute which is a reference to
    // an assertion.  If the attribute is there, it refers to an
    // assertion which will already have been processed by XercesBuilder,
    // so we can look it up by name.
    std::string onlyIfStr = Dom::getAttribute(e, "onlyIf");
    Assertion* onlyIf = t->getAssertionByName(onlyIfStr);
    Supersede* super = new Supersede(t, onlyIf);

    // Now handle child elements:  a bunch of <set>s.  Sort into two
    // lists, depending on whether destination is old row or new
    e = Dom::findFirstChildByName(e, "*");
    while (e != 0) {
      Set* s = buildSet(e, t);
      super->addSet(s);
      e = Dom::getSiblingElement(e);
    }
    super->normalize();
    return super;
  }

  Query* 
  XercesBuilder::buildQuery(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e,
                            Table* t) {
    using xmlBase::Dom;
    std::string whereStr = Dom::getAttribute(e, "assertRef");
    Assertion* where = t->getAssertionByName(whereStr);

    Query* q = new Query(t, 0, where);
    e = Dom::findFirstChildByName(e, "*");
    while (e != 0) {
      q->addSelect(Dom::getTextContent(e));
      e = Dom::getSiblingElement(e);
    }
    return q;
  }

  InsertNew* 
  XercesBuilder::buildInsertNew(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                                Table* t) {
    using xmlBase::Dom;

    std::string internalStr = Dom::getAttribute(e, "internalCond");
    Assertion* internal = t->getAssertionByName(internalStr);

    std::string officialStr = Dom::getAttribute(e, "official");
    Assertion* official = t->getAssertionByName(officialStr);

    InsertNew* in = new InsertNew(t, internal, official);
    e = Dom::findFirstChildByName(e, "*");
    while (e != 0) {  // process <interRow>
      InterRow* ir = buildInterRow(e, t);
      
      in->addInterRow(ir);
      e = Dom::getSiblingElement(e);
    }
    return in;
  }

  InterRow* 
  XercesBuilder::buildInterRow(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                               Table* t) {
    using xmlBase::Dom;


    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* queryElt =
      Dom::findFirstChildByName(e, "*");
    Query* q = buildQuery(queryElt, t);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* sib = 
      Dom::getSiblingElement(queryElt);
    bool quit = Dom::checkTagName(sib, "quit");

    InterRow* inter = new InterRow(t, q, quit);
    if (quit) return inter;
    
    // else we have one or more <set>
    while (sib != 0) {
      Set* s = buildSet(sib, t);
      inter->addSet(*s);
      sib  = Dom::getSiblingElement(sib);
      
    }
    return inter;
  }
}
