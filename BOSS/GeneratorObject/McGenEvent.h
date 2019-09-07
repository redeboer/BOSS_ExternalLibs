#ifndef McGenEvent_H
#define McGenEvent_H 
#include <vector>
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "HepMC/GenEvent.h"

extern const CLID& CLID_GenEvent;
using namespace EventModel;
using namespace HepMC;

/*
 *
 */

class McGenEvent : virtual public ContainedObject{ 
public:
  // Constructor 
  McGenEvent(GenEvent* evt);

  // Destructor 
  virtual ~McGenEvent();

  // Public accessors:
  std::string generatorName() const;

  GenEvent* getGenEvt() const;

  // Retrieve reference to class definition structure
  virtual const CLID& clID() const   { return McGenEvent::classID(); }
  static  const CLID& classID()       { return CLID_GenEvent; }
 
private:
   std::string m_generatorName;
   GenEvent* m_pEvt;
};

typedef ObjectVector<McGenEvent> McGenEventCol;

#endif
