//====================================================================
//      RawDataMucMcHitCnv.h
//====================================================================
//
// Description:
//      RawDataMucMcHitCnv is the concrete converter for 
//      the event on the TDS /Event/MC/MucMcHitCol
//
//--------------------------------------------------------------------

#ifndef RAWDATA_MUCMCHIT_CNV_H
#define RAWDATA_MUCMCHIT_CNV_H

// Include files.
#include "GaudiKernel/Converter.h"
#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h" 
#include "RawDataCnv/PackedRawDataCnvSvc.h" 

#include "RawDataCnv/EventManagement/MucMcHitBuilder.h"
#include "RawDataCnv/EventManagement/McTruthDataStructure.h"

// Forward declarations.
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataMucMcHitCnv : public RawDataBaseCnv 
{ 

   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<RawDataMucMcHitCnv>;
protected:
   // Standard Constructor.
   RawDataMucMcHitCnv(ISvcLocator* svc);
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
     /// MucMcHitder for packing or unpacking data
     MucMcHitBuilder  m_mucMcHitBuilder;

};

#endif // RAWDATA_MUCMCHIT_CNV_H
