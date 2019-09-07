#ifndef Event_EvtRecObject_H
#define Event_EvtRecObject_H

#include "GaudiKernel/DataObject.h"
#include "EventModel/EventModel.h"

using namespace EventModel;

extern const CLID& CLID_EvtRecObject;

class EvtRecObject : public DataObject {

   public:

      virtual const CLID& clID()  const { return  EvtRecObject::classID();}
      static  const CLID& classID() { return CLID_EvtRecObject; }

      // default constructor and destructor
      EvtRecObject() : DataObject() {}
      virtual ~EvtRecObject() {}

   private:

};

#endif
