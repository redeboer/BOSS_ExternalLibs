/*
 *
 *  this class models relation between "RecMdcHit" and "RecMdcTrack" 
 *  used in Mdc Reconstruction
 * 
 * 
 * ********************************************************/

#ifndef RECMDCHITONTRACK_H
#define RECMDCHITONTRACK_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h" 
#include "RelTable/Relation.h"
#include "EventModel/EventModel.h"

extern const CLID& CLID_RecMdcHitOnTrack;


class RecMdcHit;
class RecRecMdcTrack;	 


class RecMdcHitOnTrack : public Event::Relation<RecMdcHit,RecMdcTrack>  
{

 public:
   virtual const CLID& clID() const   { return RecMdcHitOnTrack::classID(); 
   }
   
   static const CLID& classID()       { return CLID_RecMdcHitOnTrack; 
   }

 private:
   
};

typedef ObjectVector<RecMdcHitOnTrack> RecMdcHOTCol;

#endif
   
