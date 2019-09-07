#ifndef EVENT_TRIG_EVENT_H
#define EVENT_TRIG_EVENT_H

#include "GaudiKernel/DataObject.h"

extern const CLID& CLID_TrigEvent;

class TrigEvent : public DataObject {
public:
  TrigEvent() : DataObject() {}
  virtual ~TrigEvent() {}
  
  void initialize(bool fromMc) { m_fromMc = fromMc; }

  virtual const CLID& clID() const { return TrigEvent::classID(); }
  static const CLID& classID() { return CLID_TrigEvent; }

  ///  Retrieve flag of origin
  bool fromMc () const {
     return m_fromMc;
  }

private:
  //.....
  bool m_fromMc;
};

#endif
