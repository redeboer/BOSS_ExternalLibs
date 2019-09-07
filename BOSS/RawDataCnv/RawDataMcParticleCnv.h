//====================================================================
//      RawDataMcParticleCnv.h
//====================================================================
//
// Description:
//      RawDataMcParticleCnv is the concrete converter for 
//      the event on the TDS /Event/MC/McParticleCol
//
//--------------------------------------------------------------------

#ifndef RAWDATA_MCPARTICLE_CNV_H
#define RAWDATA_MCPARTICLE_CNV_H

// Include files.
#include "GaudiKernel/Converter.h"

#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h" 
#include "RawDataCnv/PackedRawDataCnvSvc.h" 
#include "RawDataCnv/EventManagement/McTruthDataStructure.h"
#include "RawDataCnv/EventManagement/McParticleBuilder.h"

// Forward declarations.
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataMcParticleCnv : public RawDataBaseCnv 
{ 

   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<RawDataMcParticleCnv>;
protected:
   // Standard Constructor.
   RawDataMcParticleCnv(ISvcLocator* svc);
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
   virtual StatusCode updateObj(IOpaqueAddress* pAddr, DataObject* pObj);

   virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);  

private:

   void addMother(McParticle* currentParticle, McParticleCol *particleCol);

   IRawDataInputSvc*  m_inputSvc;
   /// Define the interface to raw data cnv
   PackedRawDataCnvSvc* m_RawDataAccess;
   /// MdcBuilder for packing or unpacking data
   McParticleBuilder  m_mcParticleBuilder;

};

#endif // RAWDATA_MCPARTICLE_CNV_H
