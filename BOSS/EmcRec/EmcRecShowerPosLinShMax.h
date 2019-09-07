//
//    Linear weighted average position with correction is used by BELLE.
//    Logarithmic weighted average position is used by BaBar.
//    And many others...
//
//    This one is linear weight.
//
//    Created by Zhe Wang 2004, 5, 16
//
#ifndef EMC_REC_SHOWER_POS_LIN_SH_MAX_H
#define EMC_REC_SHOWER_POS_LIN_SH_MAX_H

#include "EmcRec/EmcRecShowerPosAbs.h"

class EmcRecShowerPosLinShMax:public EmcRecShowerPosAbs
{
 public:
   EmcRecShowerPosLinShMax() {}
   virtual ~EmcRecShowerPosLinShMax() {}
   
 public:
   virtual void Position(RecEmcShower& aShower);

};

#endif // EMC_REC_SHOWER_POS_LIN_H

