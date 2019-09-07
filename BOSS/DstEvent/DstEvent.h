#ifndef DST_Event_H
#define DST_Event_H

#include "GaudiKernel/DataObject.h"

extern const  CLID  &CLID_DstEvent;

class DstEvent : public DataObject {

  public:
  	DstEvent() {}
	virtual ~DstEvent() {}
	
  virtual const CLID& clID()  const { return  DstEvent::classID();}
  static  const CLID& classID() { return CLID_DstEvent; }

};

#endif //DST_Event_H
