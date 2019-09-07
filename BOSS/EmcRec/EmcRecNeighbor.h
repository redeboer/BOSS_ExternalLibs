//
//  Define the relationship of crystals
//
//  Zhe Wang 2004, 3, 29
//
#ifndef EMC_REC_NEIGHBOR_H
#define EMC_REC_NEIGHBOR_H

#include "EmcRecEventModel/RecEmcEventModel.h"

class EmcRecNeighbor
{
 public:
   // Constructors and destructors
   EmcRecNeighbor();
   ~EmcRecNeighbor();
   
   // 3*3
   bool IsNeighbor(const Identifier& id1,
			  const Identifier& id2);
   
   // 5*5
   bool IsNextNeighbor(const Identifier& id1,
			      const Identifier& id2);
   
   // 3*3
   RecEmcIDVector GetNeighbors(const Identifier& id);
   
   // 5*5
   RecEmcIDVector GetNextNeighbors(const Identifier& id);
};

#endif

