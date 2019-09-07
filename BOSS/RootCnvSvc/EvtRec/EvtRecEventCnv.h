#ifndef EvtRecEventCnv_H
#define EvtRecEventCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"
#include "RootEventData/TEvtRecEvent.h"

class RootAddress;

extern const CLID& CLID_EvtRecEvent;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecEventCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecEventCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecEvent; }

   virtual ~EvtRecEventCnv() {}

protected:
   EvtRecEventCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TEvtRecEvent*   m_evtRecEvent;
};

#endif
