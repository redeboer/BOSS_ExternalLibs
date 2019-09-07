#ifndef HLTCONFIGSVC_H
#define HLTCONFIGSVC_H

#include <string>
//#include <stdio.h>
//#include <iostream>
//#include <strstream>
//#include <map>

#include "GaudiKernel/Service.h"
//#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "DatabaseSvc/IDatabaseSvc.h"

#include "HltSteerData/Signature.h"
#include "HltSteerData/Sequence.h"
#include "HltConfig/MyFrame.h"

using namespace HltProcessor;

static const InterfaceID  IID_IHltConfigSvc("IHltConfigSvc", 1, 0);

class HltConfigSvc :public Service , virtual public IIncidentListener {

public:
  
  HltConfigSvc(const std::string& name, ISvcLocator* sl);
  ~HltConfigSvc();
 
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );
  virtual void handle(const Incident&);
  StatusCode GenFileFromDatabase();
  
  const std::vector<std::pair<Signature*, Sequence*> >& retrieve();

private:

  //HltConfigSvc* m_HltConfigSvc;
  IDatabaseSvc* m_dbsvc;
  IDataProviderSvc* m_eventSvc;

  bool        m_fromDatabase;
  std::string m_seqListFile;
  std::string m_sigListFile; 

  PropertyMgr m_propMgr;

public:

  static const InterfaceID& interfaceID() { return IID_IHltConfigSvc; }

};

#endif


