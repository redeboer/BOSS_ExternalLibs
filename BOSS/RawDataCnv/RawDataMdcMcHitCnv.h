//====================================================================
//      RawDataMdcMcHitCnv.h
//====================================================================
//
// Description:
//      RawDataMdcMcHitCnv is the concrete converter for 
//      the event on the TDS /Event/MC/MdcMcHitCol
//
//--------------------------------------------------------------------

#ifndef RAWDATA_MDCMCHIT_CNV_H
#define RAWDATA_MDCMCHIT_CNV_H

// Include files.
#include "GaudiKernel/Converter.h"
#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h" 
#include "RawDataCnv/PackedRawDataCnvSvc.h" 

#include "RawDataCnv/EventManagement/McTruthDataStructure.h"
#include "RawDataCnv/EventManagement/MdcMcHitBuilder.h"

// Forward declarations.
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataMdcMcHitCnv : public RawDataBaseCnv 
{ 

   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<RawDataMdcMcHitCnv>;
protected:
   // Standard Constructor.
   RawDataMdcMcHitCnv(ISvcLocator* svc);
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
     /// MdcBuilder for packing or unpacking data
     MdcMcHitBuilder  m_mdcMcHitBuilder;

};

#endif // RAWDATA_MDCMCHIT_CNV_H
