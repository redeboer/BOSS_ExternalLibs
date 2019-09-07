#ifndef EvtRecPrimaryVertexCnv_H
#define EvtRecPrimaryVertexCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"
#include "RootEventData/TEvtRecPrimaryVertex.h"

class RootAddress;

extern const CLID& CLID_EvtRecPrimaryVertex;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecPrimaryVertexCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecPrimaryVertexCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecPrimaryVertex; }

   virtual ~EvtRecPrimaryVertexCnv() {}

protected:
   EvtRecPrimaryVertexCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TEvtRecPrimaryVertex*   m_evtRecPrimaryVertex;
};

#endif
