#include "McEventSelector/McIterator.h"

#include <iostream>
#include "McEventSelector/McAddress.h"

IOpaqueAddress* McIterator::operator*() const {
  return dereference();
}
IOpaqueAddress* McIterator::operator->() const {
  return dereference();
}
IEvtSelector::Iterator& McIterator::operator++() {
  if(m_eventNo>=m_eventsP){
    m_runNo++;
    m_eventNo=1;
  }
  m_eventNo++;
  return *this;
}
IEvtSelector::Iterator& McIterator::operator--() {
  m_eventNo--;
  return *this;
}
IEvtSelector::Iterator& McIterator::operator++(int){

  if(m_eventNo>=m_eventsP){
    m_runNo++;
    m_eventNo=1;
  }
  m_eventNo++;
  return *this;
}

IEvtSelector::Iterator& McIterator::operator--(int){ 
  m_eventNo--;
  return *this;
}

bool McIterator::operator==(const IEvtSelector::Iterator& it) const {
  const McIterator* zIt = dynamic_cast<const McIterator*>(&it);
  return (NULL != zIt) &&
         (m_runNo   == zIt->runNumber()) &&
         (m_eventNo == zIt->eventNumber()) ;
}  


bool McIterator::operator!=(const IEvtSelector::Iterator& it) const {
  return !operator==(it);
}

McAddress* McIterator::dereference() const {
  McAddress* zAddress(dynamic_cast<McAddress*>(m_evtSelector->reference(*this)));
  if (NULL != zAddress) {
    zAddress->setRunEvt(m_runNo, m_eventNo);
//    cerr << "McIterator::dereference: to object " 
//    	 << zAddress->objectName() << " : " << zAddress->clID()<< " : " 
//    	 << zAddress->svcType()<<endl;
  } else
    std::cerr << "McIterator::dereference() ERROR invalid opaque address type!! " << std::endl;
  return zAddress;
}
