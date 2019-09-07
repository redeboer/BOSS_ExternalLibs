#ifndef HltRaw_H
#define HltRaw_H 
#include <vector>
#include "GaudiKernel/ObjectVector.h"
#include "RawEvent/RawData.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_HltRaw;

/*
 *
 */

class HltRaw : public RawData { 
 public:
  // Constructor 
  HltRaw(const Identifier& id, const unsigned int timeChannel=0, const unsigned int chargeChannel=0);
  HltRaw(const unsigned int id);

  void setIntChannel(const unsigned int intChannel){setTimeChannel(intChannel);};
  unsigned int getIntChannel(){return getTimeChannel();};

  void setFloatChannel(const float value){m_chargeChannel= *((unsigned int*)(&value));};
  float getFloatChannel(){return *((float*)(&m_chargeChannel));};

  // Retrieve reference to class definition structure
  virtual const CLID& clID() const   { return HltRaw::classID(); }
  static  const CLID& classID()       { return CLID_HltRaw; }
  
 private:
  
};

typedef ObjectVector<HltRaw> HltRawCol;

#endif
