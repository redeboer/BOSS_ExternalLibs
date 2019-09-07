#ifndef MC_Event_H
#define MC_Event_H

#include "GaudiKernel/DataObject.h"

extern const  CLID  &CLID_McEvent;

class McEvent : public DataObject {

  public:
	McEvent() {}
	virtual ~McEvent() {}
	
  virtual const CLID& clID()  const { return  McEvent::classID();}
  static  const CLID& classID() { return CLID_McEvent; }

  private:

	
};

#endif //MC_Event_H
