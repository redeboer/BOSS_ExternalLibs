#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"


#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "DataInfoSvc/DataInfoSvc.h"
#include <iostream> 
#include <fstream>

DataInfoSvc::DataInfoSvc( const string& name, ISvcLocator* svcloc) :
  Service (name, svcloc){
  // declare properties
}

DataInfoSvc::~DataInfoSvc(){
}

StatusCode DataInfoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
     if( IID_IDataInfoSvc.versionMatch(riid) ){
	  *ppvInterface = static_cast<IDataInfoSvc*> (this);
     } else{
	  return Service::queryInterface(riid, ppvInterface);
     }
     return StatusCode::SUCCESS;
}

StatusCode DataInfoSvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "DataInfoSvc::initialize()" << endreq;

  StatusCode sc = Service::initialize();
  sc = setProperties();
  return StatusCode::SUCCESS;
}

void DataInfoSvc::setDecayCard(string card)
{
  ifstream fin(card.c_str());
  string tempString;
  int i=0;
  while(getline(fin,tempString))
  {
    if(tempString.size()>0)
    {
      m_decayOptions += tempString;
      m_decayOptions += "\n";
    }
    i++;
  }
  if(i>100) m_decayOptions = "";
  MsgStream log(messageService(), name());
  log << MSG::INFO << "set decayOptions: " << endreq;
  log << MSG::INFO << endreq
      << m_decayOptions << endreq;
}

  
StatusCode DataInfoSvc::finalize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "DataInfoSvc::finalize()" << endreq;
    return StatusCode::SUCCESS;
}


