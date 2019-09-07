//
//    Logear weighted average position with correction is used by BELLE.
//    Logarithmic weighted average position is used by BaBar.
//    And many others...
//
//    This one is linear weight.
//
//    Created by Zhe Wang 2004, 5, 16
//
#ifndef EMC_REC_SHOWER_POS_LOG_SH_MAX_H
#define EMC_REC_SHOWER_POS_LOG_SH_MAX_H

#include "EmcRec/EmcRecShowerPosAbs.h"

class EmcRecShowerPosLogShMax:public EmcRecShowerPosAbs
{
 public:
   EmcRecShowerPosLogShMax() {}
   virtual ~EmcRecShowerPosLogShMax() {}
   
 public:
   virtual void Position(RecEmcShower& aShower);

};

#endif // EMC_REC_SHOWER_POS_LIN_H

