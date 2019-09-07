//	============================================================
//	PackedRawDataCnvSvc.h
//	------------------------------------------------------------
//
//	Package   :  PackedRawData Conversion Service
//
//	============================================================
#ifndef PACKEDRAWDATACNVSVC_H
#define PACKEDRAWDATACNVSVC_H 

#include "RawDataCnvBase/RawDataCnvSvc.h"
#include "RawDataCnv/EventManagement/RawEvent.h"

class IRawDataOutputSvc;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class PackedRawDataCnvSvc : public RawDataCnvSvc {
   // Declaration of a Converter Factory to create instances of this class.
   friend class CnvFactory<PackedRawDataCnvSvc>;
public:
  /// Standard Constructor
  PackedRawDataCnvSvc(const std::string& name, ISvcLocator* svc);
 
  /// Standard Destructor
  virtual ~PackedRawDataCnvSvc();

  /// Initialize the service.
  virtual StatusCode initialize();

  /// Update state of the service
  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);
  
  /// Override inherited queryInterface 
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  // Connect output source
  virtual StatusCode connectOutput(const std::string& t) ;
  // avoid warning
  virtual StatusCode connectOutput(const std::string& t,
                                   const std::string& /* mode */) {
              return connectOutput(t) ;
  }
  // Output data
  virtual StatusCode commitOutput(const std::string& t, bool b) ;


  // Get Raw Event
  virtual WriteRawEvent*& getRawEvent() {
      return m_rawEvent;
  }

protected:
  /// Add converters to the service
  virtual StatusCode addConverters();

private:
 
  std::string m_ioSvcName;
  IRawDataOutputSvc*  m_ioSvc;
  WriteRawEvent* m_rawEvent;

  uint32_t     m_status;            //fucd

  unsigned int ef_info[4];
};

#endif 
