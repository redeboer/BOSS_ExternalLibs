//
//    Linear weighted average position with correction is used by BELLE.
//    Loglinarithmic weighted average position is used by BaBar.
//    And many others...
//
//    This one is logarithmic weight.
//
//    Created by Zhe Wang 2004, 5, 16
//
#ifndef EMC_REC_SHOWER_POS_LOGLIN_H
#define EMC_REC_SHOWER_POS_LOGLIN_H

#include "EmcRec/EmcRecShowerPosAbs.h"



class EmcRecShowerPosLoglin:public EmcRecShowerPosAbs
{
 public:

  EmcRecShowerPosLoglin() {}
   virtual ~EmcRecShowerPosLoglin() {}
   
 public:
   virtual void Position(RecEmcShower& aShower);

};

#endif // EMC_REC_SHOWER_POS_LOG_H

