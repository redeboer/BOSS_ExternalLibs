//====================================================================
//	McEventSelector.h
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//  Description: The McEventSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria".
//
//	Author     : P. Calafiura
//  Created    : 2/28/00
//	Changes    : 
//
//
//====================================================================

#ifndef MCEVENTSELECTOR_MCEVENTSELECTOR_H
#define MCEVENTSELECTOR_MCEVENTSELECTOR_H 1

// Include files
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/

// Forward declarations
class ISvcLocator;

#ifdef WIN32
// Disable warning C4786: identifier was truncated to '255' characters in the debug information
#pragma warning ( disable : 4786 )
#endif


//--------------------------------------------------------------------
// Event Selector 
//--------------------------------------------------------------------

class McEventSelector : virtual public Service, 
                        virtual public IEvtSelector {
public:
   // Standard Constructor.
   McEventSelector(const std::string& name, ISvcLocator* svcloc);
  
   // Standard Destructor.
   ~McEventSelector();

   // Implementation of Service base class methods.
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   virtual StatusCode queryInterface(const InterfaceID& riid, 
				    void** ppvInterface);
   virtual StatusCode createContext(Context*& refpCtxt) const;

   virtual StatusCode last(Context& refContext) const;
   virtual StatusCode next(Context& refCtxt) const;
   virtual StatusCode next(Context& refCtxt,int jump) const;
   virtual StatusCode previous(Context& refCtxt) const;
   virtual StatusCode previous(Context& refCtxt,int jump) const;
   virtual StatusCode rewind(Context& refCtxt) const;

   virtual StatusCode createAddress(const Context& refCtxt, 
				    IOpaqueAddress*&) const;
   virtual StatusCode releaseContext(Context*& refCtxt) const;
   virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;
  
private:
   IntegerProperty m_runNo;
   IntegerProperty m_firstEventNo;
   IntegerProperty m_eventsPerRun;

  /*
  static IService* createClassObject( const std::string& svcname, ISvcLocator* svcloc );
  // IEvtSelector implementation
  virtual StatusCode initialize();
  virtual StatusCode setCriteria( const std::string& criteria );
  virtual StatusCode setCriteria( const SelectionCriteria& criteria );  
  virtual Iterator* begin() const;
  virtual Iterator* end() const;
  virtual Iterator& next(Iterator& it) const; 
  virtual Iterator& previous(Iterator& it) const; 
  IOpaqueAddress* reference(const Iterator& it) const;

  // IInterface implementation
  virtual StatusCode queryInterface(const IID& riid, void** ppvInterface);

  // Constructors & Destructors
  McEventSelector(  const std::string& name, ISvcLocator* svcloc );
  ~McEventSelector();

private:
  //  EventSource* p_eventSource;
  McIterator* p_beginIter;
  McIterator* p_endIter;
  IntegerProperty m_runNo;
  IntegerProperty m_firstEventNo;
  IntegerProperty m_eventsPerRun;
  */

};

#endif  // MCEVENTCNV_MCEVENTSELECTOR_H


