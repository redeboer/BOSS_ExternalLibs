//====================================================================
//      RawDataCnv.h
//====================================================================
//
// Description:
//      RawDataCnv is the concrete converter for 
//      the event header on the TDS /Event
//
//--------------------------------------------------------------------

#ifndef EF_TDR_EVENT_CNV_H
#define EF_TDR_EVENT_CNV_H

// Include files.
#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h"  

namespace Event{ class EventH; }

// Forward declarations.
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataCnv : public RawDataBaseCnv 
{ 

   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<RawDataCnv>;
protected:
   // Standard Constructor.
   RawDataCnv(ISvcLocator* svc);
public:

   StatusCode initialize(); 

   // Return the class type of this converter.
   static const CLID& classID();

   virtual long repSvcType() const {
       return PACKEDRAWEVENT_StorageType;
   }
 
   static const unsigned char storageType() {
       return PACKEDRAWEVENT_StorageType;
   }

   // Create a converted object in the Transient Data Store.
   StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 

   /// override the RawDataBaseCnv version
   virtual StatusCode updateObj(IOpaqueAddress*, DataObject*);

   virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

private:
     IRawDataInputSvc*  m_inputSvc; 
};

#endif // EF_TDR_EVENT_CNV_H
