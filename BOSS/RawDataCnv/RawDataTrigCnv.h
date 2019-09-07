#ifndef RAWDATA_TRIG_CNV_H
#define RAWDATA_TRIG_CNV_H

#include "GaudiKernel/Converter.h"
#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h"
#include "RawDataCnv/PackedRawDataCnvSvc.h"

// Forward declarations
class IOpaqueAddress;
class DataObject;
class StatusCode;
class IRawDataInputSvc;

template<class TYPE> class CnvFactory;

class RawDataTrigCnv : public RawDataBaseCnv
{
  // Declaration of a Converter Factory to create instances of this class.
  friend class CnvFactory<RawDataTrigCnv>;
protected:
  // Standard Constructor.
  RawDataTrigCnv(ISvcLocator* svc);

public:
  StatusCode init();

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

#endif  //RAWDATA_TRIG_CNV_H
