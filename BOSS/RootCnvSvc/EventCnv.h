#ifndef EventCnv_H
#define EventCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "EventModel/EventModel.h"
#include "TObject.h"
#include "RootCnvSvc/commonData.h"

namespace Event{ class EventHeader; }
extern const CLID& CLID_Event;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


/** @class EventCnv
 * @brief Concrete converter for the Event header stored in the TDS /Event
 * Based on the EventCnv of GLAST.
 */ 

class EventCnv : public RootEventBaseCnv { 

  friend class CnvFactory<EventCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_Event;
  }

  virtual ~EventCnv() { };

protected:

  EventCnv(ISvcLocator* svc);


  /// do the concrete transformation from TDS to ROOT
  StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr) {return StatusCode::SUCCESS;}
  /// transform from TDS to ROOT
  StatusCode createRep(DataObject* obj, RootAddress* addr);

  /// transform from ROOT to TDS
   StatusCode createObj(IOpaqueAddress *addr,DataObject*& refpObject);

  /// do the concrete transformation from ROOT to TDS
  virtual StatusCode TObjectToDataObject(DataObject*& dat) {return StatusCode::SUCCESS;}

  commonData   m_common;
 };



#endif // EventCnv_H

