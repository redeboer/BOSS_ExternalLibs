// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Set.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Set.h"

// more includes??

namespace rdbModel {

  Visitor::VisitorState Set::accept(Visitor* v) {    
    Visitor::VisitorState state = v->visitSet(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;
    return state;
  }
}

