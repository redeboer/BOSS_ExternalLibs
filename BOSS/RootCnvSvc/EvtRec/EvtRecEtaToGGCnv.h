#ifndef EvtRecEtaToGGCnv_H
#define EvtRecEtaToGGCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EvtRecEtaToGGCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecEtaToGGCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecEtaToGGCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecEtaToGGCol; }

   virtual ~EvtRecEtaToGGCnv() {}

protected:
   EvtRecEtaToGGCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TObjArray*   m_evtRecEtaToGGCol;
};

#endif
