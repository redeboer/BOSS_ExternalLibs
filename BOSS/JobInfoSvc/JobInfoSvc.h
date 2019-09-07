#ifndef JobInfoSvc_H
#define JobInfoSvc_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "JobInfoSvc/IJobInfoSvc.h"


class JobInfoSvc: public  Service, virtual public IJobInfoSvc,
		  virtual public IIncidentListener{

public:

  JobInfoSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~JobInfoSvc(){};
  StatusCode initialize();
  StatusCode finalize();
  StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  // Incident handler
  void handle(const Incident&);


  int xmlrpc(int evtNum);
  std::string getJobOutputFile();

private:
  long m_count;
  std::string m_outputFileName;
  std::string m_xmlrpcServer;
  int         m_xmlrpcPort;
  std::string m_xmlrpcUrl;
  std::string m_xmlrpcMethod;

};


#endif

