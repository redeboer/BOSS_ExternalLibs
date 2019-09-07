#ifndef MCEVENTSELECTOR_MCITERAROR_H
#define MCEVENTSELECTOR_MCITERATOR_H 1

#include "GaudiKernel/IEvtSelector.h"

class McAddress;

class McIterator : virtual public IEvtSelector::Iterator 
{
  public:
  McIterator 	(const IEvtSelector*  selector) :  
                                          m_evtSelector(selector),
                                          m_runNo(0), m_eventNo(0), 
                                          m_eventsP(1000000) {}
  McIterator 	(const McIterator &it) :  m_evtSelector(it.m_evtSelector),
                                          m_runNo(it.m_runNo),
                                          m_eventNo(it.m_eventNo),
                                          m_eventsP(1000000) {}
  // McIterator    () :  m_evtSelector(0), m_runNo(0), m_eventNo(0) {}
  virtual ~McIterator (void) {}
  
  virtual IOpaqueAddress* operator*() const;
  virtual IOpaqueAddress* operator->() const;
  virtual Iterator& operator++();
  virtual Iterator& operator--();
  virtual Iterator& operator++(int);
  virtual Iterator& operator--(int);
  virtual bool operator==(const Iterator& it) const;
  virtual bool operator!=(const Iterator& it) const;

  unsigned int runNumber() const { return m_runNo; }
  unsigned int eventNumber() const { return m_eventNo; }

  void setRunNumber( unsigned int runNo ) { m_runNo = runNo; }
  void setEventNumber( unsigned int eventNo ) { m_eventNo = eventNo ; }
  void setEventsPerRun( unsigned int eventsP) { m_eventsP = eventsP ; }

 private:
  McAddress* dereference() const;

  const IEvtSelector*  m_evtSelector;

  unsigned int m_runNo;
  unsigned int m_eventNo;
  unsigned int m_eventsP;
};
#endif   /* MCEVENTSELECTOR_MCITERATOR_H */


