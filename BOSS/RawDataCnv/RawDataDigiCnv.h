//====================================================================
//      RawDataDigiCnv.h
//====================================================================
//
// Description:
//      RawDataDigiCnv is the concrete converter for 
//      the event on the TDS /Event/Digi
//
//--------------------------------------------------------------------

#ifndef RAWDATA_DIGI_CNV_H
#define RAWDATA_DIGI_CNV_H

// Include files.
#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h"

namespace Event{ class EventHeader; }

// Forward declarations.
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataDigiCnv : public RawDataBaseCnv 
{ 

   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<RawDataDigiCnv>;
protected:
   // Standard Constructor.
   RawDataDigiCnv(ISvcLocator* svc);
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
     IRawDataCnvSvc* m_RawDataAccess;

};

#endif // RAWDATA_DIGI_CNV_H
