#ifndef EvtRecTrackCnv_H
#define EvtRecTrackCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EvtRecTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecTrackCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecTrackCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecTrackCol; }

   virtual ~EvtRecTrackCnv() {}

protected:
   EvtRecTrackCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TObjArray*   m_evtRecTrackCol;
};

#endif
