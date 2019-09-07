#ifndef EvtRecCnv_H
#define EvtRecCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"
#include "RootEventData/TEvtRecObject.h"

class RootAddress;

extern const CLID& CLID_EvtRecObject;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecCnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecCnv>;

public:
   static const CLID& classID() { return CLID_EvtRecObject; }

   virtual ~EvtRecCnv() {}

   virtual StatusCode initialize();

   static TEvtRecObject* getWriteObject() { return m_objWrite; }

protected:
   EvtRecCnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:

   /// relational maps
   commonData m_common;

   static TEvtRecObject*   m_objWrite;
};

#endif
