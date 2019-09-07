#ifndef RECON_Event_H
#define RECON_Event_H

#include "GaudiKernel/DataObject.h"

extern const  CLID  &CLID_ReconEvent;

class ReconEvent : public DataObject {

  public:
  	ReconEvent() {}
	virtual ~ReconEvent() {}
	
  virtual const CLID& clID()  const { return  ReconEvent::classID();}
  static  const CLID& classID() { return CLID_ReconEvent; }

};

#endif //RECON_Event_H
