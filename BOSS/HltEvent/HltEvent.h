#ifndef EVENT_HLT_EVENT_H
#define EVENT_HLT_EVENT_H

#include "GaudiKernel/DataObject.h"

extern const CLID& CLID_HltEvent;

class HltEvent : public DataObject {
public:
  HltEvent() : DataObject() {}
  virtual ~HltEvent() {}
  
  void initialize(bool fromMc) { m_fromMc = fromMc; }

  virtual const CLID& clID() const { return HltEvent::classID(); }
  static const CLID& classID() { return CLID_HltEvent; }

  ///  Retrieve flag of origin
  bool fromMc () const {
     return m_fromMc;
  }

private:
  //.....
  bool m_fromMc;
};

#endif
