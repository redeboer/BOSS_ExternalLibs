// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Management/XercesBuilder.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_XERCESBUILDER_H
#define RDBMODEL_XERCESBUILDER_H

#include "rdbModel/Management/Builder.h"
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include "rdbModel/Tables/Column.h"
#include "rdbModel/Tables/Assertion.h"
namespace rdbModel{

  class Table;    // single rdbms table
  class Index;    // index/key (may be primary or not)
  class Datatype;
  class Supersede;
  class InterRow;
  class InsertNew;
  class Query;
  class Set;
  /**
   * This class is a concrete Builder that use the Xerces parser and the
   * DOM functionalities to parse the xml file and build the generic model
   * hierarchy
   * @author D.Favretto & R.Giannitrapani
   */
  class XercesBuilder : public Builder {
  public:

    /**
     *  The constructor 
     */
    XercesBuilder();

    virtual ~XercesBuilder() {};
  
    /**  
         Invoke xml parser to produce DOM (in-memory) representation 
         Concrete implementation of Builder::parseInput
    */
    virtual unsigned int parseInput(const std::string& inputPath);

    /** Concrete implementation of pure virtual Builder::buildRdb */
    virtual int buildRdb();

  private:
    // A bunch of private methods to handle the details of creating an
    // object from the corresponding xml description

    /** Build an individual Table object from its xml description     */
    Table* buildTable(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e);

    /**  build a Column from its xml description */
    Column* buildColumn(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e,Table* t);

    /**
     * build an Index object (corresponding to MySQL index or key) from its 
     * xml description
     */
    Index* buildIndex(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                      bool primary, Table* t);

    Assertion* buildAssertion(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                              Table* t);

    Assertion::Operator* 
    buildOperator(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, Table* t);

    Datatype* buildDatatype(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e);
    Set* buildSet(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, Table* t);

    InsertNew* buildInsertNew(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                              Table* t);

    InterRow* buildInterRow(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                            Table* t);

    Query* buildQuery(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                      Table* t);

    //   Set* buildSet(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, Table* t);

    Supersede* buildSupersede(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, 
                             Table* t);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* m_doc;
    Rdb* m_rdb;

  };
}
#endif //XERCESBUILDER_H







