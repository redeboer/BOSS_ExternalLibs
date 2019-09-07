// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Query.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Query.h"
#include "rdbModel/Tables/Assertion.h"

namespace rdbModel {

  Query::Query(Table* table, const std::vector<std::string>* toSelect,
               Assertion* pAssert) : m_myTable(table), m_assert(pAssert) {
    m_toSelect.clear();

    if (toSelect != 0) {
      unsigned n = toSelect->size();
      m_toSelect.reserve(n);
      for (unsigned i = 0; i < n; i++) {
        m_toSelect.push_back((*toSelect)[i]);
      }
    }
    
  }

  Query::~Query() {
    m_toSelect.clear();
    //    delete pAssert;   // ??   This is probably already done by ~Table
  }

  Visitor::VisitorState Query::accept(Visitor* v) {
    Visitor::VisitorState state = v->visitQuery(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;
    return state;
  }
}

