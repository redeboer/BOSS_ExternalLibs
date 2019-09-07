#ifndef EvtRecPi0Cnv_H
#define EvtRecPi0Cnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EvtRecPi0Col;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EvtRecPi0Cnv : public RootEventBaseCnv {

   friend class CnvFactory<EvtRecPi0Cnv>;

public:
   static const CLID& classID() { return CLID_EvtRecPi0Col; }

   virtual ~EvtRecPi0Cnv() {}

protected:
   EvtRecPi0Cnv(ISvcLocator* svc);

   /// transformation to root
   virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

   /// transformation from root
   virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
   /// relational maps
   commonData m_common;

   TObjArray*   m_evtRecPi0Col;
};

#endif
