#ifndef EvtRecVeeVertexCnv_H
#define EvtRecVeeVertexCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EvtRecVeeVertexCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecVeeVertexCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecVeeVertexCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecVeeVertexCol; }

   virtual ~EvtRecVeeVertexCnv() {}

protected:
   EvtRecVeeVertexCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TObjArray*   m_evtRecVeeVertexCol;
};

#endif
