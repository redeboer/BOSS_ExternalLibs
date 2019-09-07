// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/InsertNew.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_INSERTNEW_H
#define RDBMODEL_INSERTNEW_H
#include <vector>
#include <string>
#include "rdbModel/Management/Visitor.h"

namespace rdbModel{

  class InterRow;
  class Assertion;
  class Table;

  /** 
      Keep track of constraints on insertion of new row, not intended to
      replace any existing row.
              o   Internal constraints to insure new row is self-consistent
              o   Look for conflicts with existing rows; specify what to
                  do to correct if found.
  */
  class InsertNew {
  public:
    InsertNew(Table* table, Assertion* internal, Assertion* official) 
      : m_myTable(table), m_internal(internal), m_official(official) {
      m_interRow.clear();
    }

    ~InsertNew();

    const Assertion* getInternal() const {return m_internal;}
    const Assertion* getOfficial() const {return m_official;}
    const std::vector<InterRow*>& getInterRow() const {return m_interRow;}

    const Table* getTable() const {return m_myTable;}

    Visitor::VisitorState accept(Visitor* v);
    //    void addInternal(Assertion* internal) {m_internal = internal;}
    //    void addOfficial(Assertion* official) {m_official = official;}
    void addInterRow(InterRow* interRow) {m_interRow.push_back(interRow);}


  private:
    Table* m_myTable;
    //    bool   m_keepOp;
    /// internal consistency conditions
    Assertion* m_internal;  
    /// conditions to be satisfied to show row is worth the extra trouble
    Assertion* m_official;  
    std::vector<InterRow*>   m_interRow;
  };
}
#endif

