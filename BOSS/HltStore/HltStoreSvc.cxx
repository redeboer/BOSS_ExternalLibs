#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "HltStore/HltStoreSvc.h"

#include <strstream>
#include <algorithm>

static SvcFactory<HltStoreSvc> s_factory;
//const ISvcFactory& HltStoreSvcFactory = s_factory;

HltStoreSvc::HltStoreSvc( const std::string& name, ISvcLocator* sl ) : Service(name, sl) 
{ }

HltStoreSvc::~HltStoreSvc() 
{ }

StatusCode HltStoreSvc::queryInterface (const InterfaceID& riid, void** ppvIF ) 
{
  if ( IID_IHltStoreSvc.versionMatch(riid) ) { 
//    *ppvIF = static_cast<IHltStoreSvc*> (this); 
    *ppvIF = (HltStoreSvc*)this; 
  } else { 
    return Service::queryInterface(riid, ppvIF) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode HltStoreSvc::initialize ( ) 
{
  StatusCode sc;

  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;

  sc = Service::initialize();
  if ( sc.isFailure() ) return sc;
  
  /*
  sc = service( "HltStoreSvc", m_HltStore); 
  if( sc.isFailure() ) {
    log<<MSG::WARNING<<"Unable to get service HltStore"<<endreq;
    return sc;
  }
  log << MSG::DEBUG << "(*p)" << (unsigned long)m_HltStore << endreq;
  */
  return StatusCode::SUCCESS;
}

StatusCode HltStoreSvc::finalize ( ) 
{
  ContainerType::iterator pos;
  for (pos = m_map.begin(); pos != m_map.end(); ++pos) {
//    delete (*pos).second;
  }
  m_map.erase(m_map.begin(),m_map.end());
  return StatusCode::SUCCESS;
}

  //
  // method to print out all keys in the store
  //
void HltStoreSvc::printKeys(){
  ContainerType::iterator pos;
  for (pos = m_map.begin(); pos != m_map.end(); ++pos) {
    cout << pos->first << endl;
  }
}

int HltStoreSvc::size(){
  return m_map.size();
}

int HltStoreSvc::max_size(){
  return m_map.max_size();
}

//
// returns string that contains the length of the Store+1, or, if
//that key is already taken in the Store, length+n with n>1 and n the 
//lowest value for which the key is not already taken
//
string HltStoreSvc::sListLength(){
  unsigned int iListLength = this->size();
  iListLength += 1;
  ostrstream ListLength;
  
  //convert integer into std::string
  ListLength <<  iListLength;
  ListLength << '\0'; 
  string stringListLength = ListLength.str();          
  if (this->exists(stringListLength)){
    stringListLength = "unset";
    for (int i = 0; i < this->max_size(); i++){
      iListLength += 1;
      ListLength <<  iListLength; 
      ListLength << '\0'; 
      stringListLength = ListLength.str();
      if (!(this->exists(stringListLength))) {
        ListLength.freeze(0);
        return stringListLength;
      }
    }
  }

  ListLength.freeze(0);
  return stringListLength;
}

//
// return true if named object exists
//
bool HltStoreSvc::exists(const std::string& name) {
  return (m_map.find(name) != m_map.end());
}

