#ifndef GLASTCNV_GLASTEVENTSELECTORITERATOR_H
#define GLASTCNV_GLASTEVENTSELECTORITERATOR_H 1

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"

#include <list>
#include <string>
#include <stdio.h>

/** @class RootEvtIterator
* @brief implements the IEvtSelector::Iterator interface
*
* $Header: /bes/bes/BossCvs/Event/RootCnvSvc/RootCnvSvc/RootEvtIterator.h,v 1.8 2008/01/09 00:05:48 codeman Exp $
*/
class RootEvtIterator: public IEvtSelector::Iterator {
 public:
  friend class RootEvtSelector;
  RootEvtIterator() {}
  RootEvtIterator( const IEvtSelector* glastEvtSel): m_glastEvtSel(glastEvtSel),
    m_recId(0) { }
  RootEvtIterator( const IEvtSelector* glastEvtSel, int recId)
    : m_glastEvtSel(glastEvtSel){
    m_recId = recId;
  }
  virtual ~RootEvtIterator() { }
  virtual IOpaqueAddress* operator*() const  {return m_glastEvtSel->reference(*this);}
  virtual IOpaqueAddress* operator->() const { return m_glastEvtSel->reference(*this);}
  virtual IEvtSelector::Iterator& operator++()    { return m_glastEvtSel->next(*this);}
  virtual IEvtSelector::Iterator& operator--()    { return m_glastEvtSel->previous(*this);}
  virtual IEvtSelector::Iterator& operator++(int) { return m_glastEvtSel->next(*this);}
  virtual IEvtSelector::Iterator& operator--(int) { return m_glastEvtSel->previous(*this);}
  virtual bool operator==(const IEvtSelector::Iterator& it) const {
    const RootEvtIterator* glastIt = dynamic_cast<const RootEvtIterator*>(&it);
    return((m_recId == glastIt->m_recId));
  }
  virtual bool operator!=(const IEvtSelector::Iterator& it) const {
    const RootEvtIterator* glastIt = dynamic_cast<const RootEvtIterator*>(&it);
    return((m_recId != glastIt->m_recId));
  }

 private:
  /// pointer to RootEvtSelector
  const IEvtSelector*   m_glastEvtSel;
  /// counts number of event
  int                   m_recId;

};

#endif  // GLASTEvtIterator_H



















