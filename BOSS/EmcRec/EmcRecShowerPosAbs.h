//
//    Abstract base class for
//    shower position reconstruction
//    You can inherit this class to develop your code.
//    
//    Created by Zhe Wang 2004, 5, 16
//
#ifndef EMC_REC_SHOWER_POS_ABS_H
#define EMC_REC_SHOWER_POS_ABS_H

#include "EmcRecEventModel/RecEmcEventModel.h"

class EmcRecShowerPosAbs
{
 public:
   EmcRecShowerPosAbs() {}
   virtual ~EmcRecShowerPosAbs() {}
   
 public:
   virtual void Position(RecEmcShower& aShower) =0;
   
};

#endif //EMC_REC_SHOWER_POS_ABS_H

