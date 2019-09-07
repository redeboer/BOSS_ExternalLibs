#ifndef MCEVENTSELECTOR_MCEVENTCNV_H
#define MCEVENTSELECTOR_MCEVENTCNV_H
#include "GaudiKernel/Converter.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_Event;

class IOpaqueAddress;
class DataObject;
class StatusCode;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
class McEventCnv: public Converter {
  friend class CnvFactory<McEventCnv>;

 protected:
  McEventCnv(ISvcLocator* svcloc);

 public:
  StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 

  /// Storage type and class ID
  static unsigned char storageType();
  virtual long repSvcType() const;

  static const CLID& classID() {
    return CLID_Event;
  }                 

};
#endif

