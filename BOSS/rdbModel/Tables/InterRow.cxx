// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/InterRow.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Query.h"
#include "rdbModel/Tables/Set.h"
#include "rdbModel/Tables/InterRow.h"

namespace rdbModel {

  InterRow::InterRow(Table* table, Query* query, bool quit)
    : m_myTable(table), m_query(query), m_quit(quit) {
    m_sets.clear();
  }

  InterRow::~InterRow() {
    delete m_query;

    // Don't know yet whether anything more needs to be done here
    m_sets.clear();
  }    

  void InterRow::addSet(const Set& set) {
    m_sets.push_back(set);
  }


  Visitor::VisitorState InterRow::accept(Visitor* v) {
    Visitor::VisitorState state = v->visitInterRow(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;

    unsigned n = m_sets.size();
    for (unsigned i = 0; i < n; i++) {
      state = m_sets[i].accept(v);
      if (state != Visitor::VCONTINUE) return state;
    }
    return state;
  }

}
