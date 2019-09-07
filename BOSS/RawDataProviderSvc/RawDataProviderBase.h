#ifndef RAWDATAPROVIDERBASE_H
#define RAWDATAPROVIDERBASE_H

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

/// Exception class
class SvcDisable {
 public:
  SvcDisable(const std::string& svcName) : m_svcName(svcName) {}
  std::string m_svcName;
};

// RawDataProvider Base Class
class RawDataProviderBase{ 
 public:
  // tianhl for mt 
  //RawDataProviderBase();
  // tianhl for mt 
  RawDataProviderBase( const char* name );
  virtual ~RawDataProviderBase(){};
  
  // initialize service point
  StatusCode initialize(ISvcLocator* svcLoc=0, IMessageSvc* pMsg=0);

  // set point of service
  void setSvcLocator(ISvcLocator* svcLoc) { m_svcLocator=svcLoc; };
  void setMsgSvc(IMessageSvc* svc) { m_msgSvc=svc; };
  void setMsgName(const char* name) { m_name = std::string(name); };

 protected:
  // tianhl for mt 
  RawDataProviderBase();
  // tianhl for mt 
  ISvcLocator* m_svcLocator;
  IMessageSvc* m_msgSvc;
  std::string  m_name;

};
#endif
