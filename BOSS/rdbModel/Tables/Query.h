// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/Query.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_QUERY_H
#define RDBMODEL_QUERY_H
#include <vector>
#include <string>
#include "rdbModel/Management/Visitor.h"

namespace rdbModel{

  class Assertion;
  class Table;

  /** 
      A  query consists of a list of columns to be selected
      from rows in the table satisfying an assertion. For now,
      all elements in the assertion must refer to the same table.
  */
  class Query {
  public:
    Query(Table* table, const std::vector<std::string>* toSelect = 0, 
          Assertion* pAssert=0);
    ~Query();

    const std::vector<std::string>& getToSelect() const {return m_toSelect;}
    const Assertion* getAssertion() const {return m_assert;}
    const Table* getTable() const {return m_myTable;}
    void addSelect(const std::string& col) {
      m_toSelect.push_back(col);
    }

    Visitor::VisitorState accept(Visitor* v);


  private:
    Table* m_myTable;
    //    bool   m_keepOp;
    std::vector<std::string> m_toSelect;
    Assertion* m_assert;
  };
}
#endif

