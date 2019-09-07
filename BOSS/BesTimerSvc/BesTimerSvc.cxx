// this is -*- c++ -*-
// $Id: BesTimerSvc.cxx,v 1.4 2011/02/18 04:23:58 maqm Exp $
// $Name: BesTimerSvc-00-00-12 $
#include <algorithm>
#include <string>
#include <vector>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "BesTimerSvc/BesTimerSvc.h"


BesTimerSvc::BesTimerSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {}

BesTimerSvc::~BesTimerSvc() {
  std::vector < BesTimer* >::iterator it =  m_itemList.begin();
  for (; it != m_itemList.end(); it++) {
    delete (*it);
  }  
}

StatusCode BesTimerSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ) {

  if ( IID_IBesTimerSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<IBesTimerSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}
StatusCode BesTimerSvc::initialize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;

  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;
  // Set my own properties
  setProperties();  
  
  return StatusCode::SUCCESS;
}


StatusCode BesTimerSvc::finalize ( ) {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": End of Run Statistics" << endreq;
  this->print();
  return StatusCode::SUCCESS;
}



BesTimer* BesTimerSvc::addItem(const std::string &itemName) {

  // Create a BesTimer and return a pointer to it
  // If an item of that name already exists, flag error
  // by returning NULL
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << name() << " adding " << itemName  << endreq;
  
  BesTimer* item = new BesTimer(itemName);
  m_itemList.push_back(item);
  return item;
}

void BesTimerSvc::print( ) {
  MsgStream log(messageService(), name());

  log << MSG::INFO << "==============================BesTimerSvc::Print===============================" << endreq;
  unsigned int maxname=0;
  std::vector < BesTimer* >::iterator it =  m_itemList.begin();
  for (; it != m_itemList.end(); it++) {
    if ((*it)->name().length() > maxname) maxname = static_cast<int>((*it)->name().length());
    if (!(*it)->propName().empty()) {
      if ((*it)->propName().length() > maxname) maxname = static_cast<int>((*it)->propName().length());
    }
  }

  it =  m_itemList.begin();
  for (; it != m_itemList.end(); it++) {
    std::string nam = (*it)->name();
    for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
    log << MSG::INFO << nam << "  mean(t) : " << std::setw(12) << (*it)->mean()<< 
      " ms      rms : " << std::setw(12) << (*it)->rms() << " ms " << endreq;
  }
  log << MSG::INFO << "------------------------------------------------------------------------------" << endreq;
  it =  m_itemList.begin();
  for (; it != m_itemList.end(); it++) {
    if (!(*it)->propName().empty()) {
      std::string nam = (*it)->name();
      for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
      log << MSG::INFO << nam << "  mean(t) : " << std::setw(12) << (*it)->mean()<< 
	" ms      rms : " << std::setw(12) << (*it)->rms() << " ms " << endreq;

      nam = (*it)->propName();
      for (unsigned int i = static_cast<int>(nam.length()); i<maxname; i++) nam.insert(i," ");
      if ((*it)->meanPropVal() == 0) {
	log << MSG::INFO << nam << "  mean(N) : " << std::setw(12) << endreq;
      } else {
	log << MSG::INFO << nam << "  mean(N) : " << std::setw(12) << (*it)->meanPropVal() << 
	  "   mean(t/N) : " 
	    << 1000.*(*it)->meanTimePerObject() << " us " << endreq; 
      }
    }
  }

  log << MSG::INFO << "==============================BesTimerSvc::Print===============================" << endreq;
  }

std::vector < BesTimer* >::iterator BesTimerSvc::begin( void ) {
  return m_itemList.begin();
}
std::vector < BesTimer* >::iterator BesTimerSvc::end( void ){
  return m_itemList.end();
}

