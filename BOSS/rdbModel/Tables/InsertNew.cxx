// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/InsertNew.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Query.h"
#include "rdbModel/Tables/InsertNew.h"
#include "rdbModel/Tables/InterRow.h"
#include "rdbModel/RdbException.h"

namespace rdbModel {

  InsertNew::~InsertNew() {
    while (m_interRow.size() ) {
      InterRow* i  = m_interRow.back();
      m_interRow.pop_back();
      delete i;
    }
  }
  // Pass the buck to InterRow
  Visitor::VisitorState InsertNew::accept(Visitor* v) {
    Visitor::VisitorState state = v->visitInsertNew(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;

    unsigned n = m_interRow.size();
    for (unsigned i = 0; i < n; i++) {
      state = m_interRow[i]->accept(v);
      if (state != Visitor::VCONTINUE) return state;
    }
    return state;
  }
}
