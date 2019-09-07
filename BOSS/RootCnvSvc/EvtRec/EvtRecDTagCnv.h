#ifndef EvtRecDTagCnv_H
#define EvtRecDTagCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EvtRecDTagCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecDTagCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecDTagCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecDTagCol; }

   virtual ~EvtRecDTagCnv() {}

protected:
   EvtRecDTagCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TObjArray*   m_evtRecDTagCol;
};

#endif
