//=====================================================================
//	RawDataSelector.h
//=====================================================================
//
// Description: An event selector service that steps through Event
//              Filter packed raw events by use of an event source
//              object and an iterator object called RawDataIterator. 
//              The type of event source is specified at run time. It
//              can be a single file, a set of files or the network.
//
//---------------------------------------------------------------------

#ifndef RAWDATA_SELECTOR_H
#define RAWDATA_SELECTOR_H

// Include files.
#include <string>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IProperty.h"


// Forward declarations.
class IAddressCreator;
class ISvcLocator;
class RawDataContext;
class IRawDataInputSvc;

// Class RawDataSelector.
class RawDataSelector : virtual public Service, 
                           virtual public IEvtSelector, 
                           virtual public IProperty 
{

public:

   // Standard Constructor.
   RawDataSelector(const std::string& name, ISvcLocator* svcloc);
  
   // Standard Destructor.
   ~RawDataSelector();

   // Implementation of Service base class methods.
   virtual StatusCode initialize();

   // Implementation of the IEvtSelector interface methods.
   virtual StatusCode createContext(Context*& it) const;
   virtual StatusCode next(Context& it) const;
   virtual StatusCode next(Context& it, int jump) const;
   virtual StatusCode previous(Context& it) const;
   virtual StatusCode previous(Context& it, int jump) const;

   virtual StatusCode last(Context& it) const;
   virtual StatusCode rewind(Context& it) const;

   virtual StatusCode createAddress(const Context& it,IOpaqueAddress*& iop) const;
   virtual StatusCode releaseContext(Context*& it) const;
   virtual StatusCode resetCriteria(const std::string& criteria, Context& context) const;

   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

private:

   IRawDataInputSvc*     m_eventSource;
   RawDataContext*       m_beginContext;
   RawDataContext*       m_endContext;
   IAddressCreator*      m_addressCreator; 
   std::string           m_eventSourceName;


};

#endif  // RAWDATA_SELECTOR_H
