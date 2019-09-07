//
//  Spliter abstract base class
//
//  Zhe Wang 2004, 8, 31
//
//  There are many methods to split a overlapped cluster.
//  Usually, these algorithms based on two aspects.
//  One is the understanding the lateral shape of shower.
//  The other is how to optimise or how to get the best solution.
//  However, most important, only the experiment result can
//  tell which is best.

#ifndef EMC_REC_SPLIT_ABS_H
#define EMC_REC_SPLIT_ABS_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRec/EmcRecNeighbor.h"

class EmcRecSplitAbs
{
 public:
   // Constructors and destructors
   EmcRecSplitAbs() {}
   virtual ~EmcRecSplitAbs() {}
   
 public:
   virtual void Split(RecEmcCluster& aCluster,
		      const RecEmcIDVector& aMaxVec,
		      RecEmcShowerMap& aShowerMap) =0;
};

#endif  // EMC_REC_SPLIT_ABS_H

