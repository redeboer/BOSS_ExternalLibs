// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/Index.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_INDEX_H
#define RDBMODEL_INDEX_H
#include <vector>
#include <string>
#include "rdbModel/Management/Visitor.h"

namespace rdbModel {

  class Column;
  class Table;

  class XercesBuilder;


  /** 
   * rdbModel representation of a(n SQL-like) key (aka index) description
   */
  class Index {
  public:
    Index(Table* myTable=0) : m_myTable(myTable) {};
    ~Index() {};

    const std::string& getName() const {return m_name; };
    bool isPrimary() const {return m_primary;}
    const std::vector<std::string>& getColumnNames();

    Visitor::VisitorState accept(Visitor* v);
    //    Visitor::VisitorState acceptNotRec(Visitor* v);

  private: 
    friend class rdbModel::XercesBuilder;

    /// Is it a primary key?
    bool m_primary;

    /// Names of  columns it's indexing
    std::vector<std::string> m_indexCols;

    /// Point back to owning table
    Table* m_myTable;
    std::string m_name;
  };
}

#endif
