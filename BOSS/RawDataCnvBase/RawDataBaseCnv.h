#ifndef _RawDataBaseCnv_H
#define _RawDataBaseCnv_H 1

#include <vector>
#include <string>
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/CnvFactory.h"
#include "RawDataCnvBase/IRawDataCnvSvc.h"

#include "EventModel/EventModel.h"
extern const CLID& CLID_EmcDigiCol;

#ifndef _RawDataBaseCnv_StorageType 
#define _RawDataBaseCnv_StorageType 1
  static long Test_StorageType = 0x09;
#endif

/** class RawDataBaseCnv
*   brief Base class for all GLAST EBF Converters.
*
*   Based on GLAST's EbfBaseCnv 
*
*/
 
class RawDataBaseCnv : public Converter {
protected:
  /// Pointer to event conversion service interface
  IRawDataCnvSvc*  m_CnvSvc;
  /// Leaf objects giving the locations of the objects in the data store
  std::vector<IRawDataCnvSvc::Leaf>   m_leaves;

  
public:
  RawDataBaseCnv(ISvcLocator* svc);
  RawDataBaseCnv(unsigned char storageType, const CLID& clid, ISvcLocator* svc);

  virtual ~RawDataBaseCnv() { };

  virtual long repSvcType() const { return Test_StorageType; }

  static const long storageType()     { return Test_StorageType; }

  static const CLID& classID() {return CLID_EmcDigiCol;};

  
  StatusCode initialize();
 
  virtual StatusCode finalize();

  //virtual StatusCode addConverters () {return StatusCode::FAILURE;};
 
  /// Store TDS path to link a particular converter to an object on the TDS
  void declareObject(const std::string& fullPath, const CLID& clid, const std::string& bank, long par=0);
    
  /// Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
    
  /// Resolve the references of the converted object. 
  virtual StatusCode fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject);
    
  /// Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
    
  /// Update the references of an already converted object.
  virtual StatusCode updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);
  
};

#endif // _RawDataBaseCnv_H

