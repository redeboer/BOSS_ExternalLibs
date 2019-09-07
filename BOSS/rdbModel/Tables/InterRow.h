// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/InterRow.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_INTERROW_H
#define RDBMODEL_INTERROW_H
#include <vector>
#include <string>
#include "rdbModel/Management/Visitor.h"

namespace rdbModel{

  class Query;
  class Set;
  class Table;

  /** 
      InterRow is a class to describe interrow conditions between a proposed
      row and existing rows, and, if found, what action to take
  */
  class InterRow {
  public:
    InterRow(Table* table, Query* query=0, bool quit=true);

    void addSet(const Set& set);
    const std::vector<Set>& getSets() const {return m_sets;}
    const Query* getQuery() const {return m_query;}
    bool getQuit() const {return m_quit;}
    const Table* getTable() const {return m_myTable;}

    /**  
         Normally, operator associated with the assertion will be deleted
         when the assertion itself is deleted, but this won't happen if
         keepOp is set to true.
     */

    ~InterRow();
    //    WHEN getWhen() const {return m_when;}
    Visitor::VisitorState accept(Visitor* v);


  private:
    Table* m_myTable;
    //    bool   m_keepOp;
    std::vector<Set> m_sets;
    Query* m_query;
    bool   m_quit;
  };
}
#endif

