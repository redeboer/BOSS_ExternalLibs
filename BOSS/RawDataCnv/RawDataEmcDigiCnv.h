//====================================================================
//      RawDataEmcDigiCnv.h
//====================================================================
//
// Description:
//      RawDataEmcDigiCnv is the concrete converter for 
//      the event header on the TDS /Event
//
//--------------------------------------------------------------------

#ifndef RAWDATA_EMCDIGI_CNV_H
#define RAWDATA_EMCDIGI_CNV_H

// Include files.
#include "RawDataCnv/ClassID_temp.h" 
#include "RawDataCnvBase/RawDataBaseCnv.h"

// Forward declarations.
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;
class PackedRawDataCnvSvc;
class EmcConverter;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataEmcDigiCnv : public RawDataBaseCnv 
{ 

   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<RawDataEmcDigiCnv>;
protected:
   // Standard Constructor.
   RawDataEmcDigiCnv(ISvcLocator* svc);
   ~RawDataEmcDigiCnv();
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
     /// Define the interface to raw data cnv
     PackedRawDataCnvSvc* m_RawDataAccess;
     /// Emc converter for unpacking or packing data
     EmcConverter* m_cnv;
};

#endif // RAWDATA_EMCDIGI_CNV_H
