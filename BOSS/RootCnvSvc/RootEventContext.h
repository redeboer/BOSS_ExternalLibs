#ifndef ROOTEVENTCONTEX_H
#define ROOTEVENTCONTEX_H

#include "GaudiKernel/IEvtSelector.h"
#include "RootCnvSvc/RootEvtSelector.h"

class RootEventSelector;
class IOpaqueAddress;

/** @class RootEventContext
 *  @brief This class provides the Context for RootEventSelector
 **/

class RootEventContext : virtual public IEvtSelector::Context {

public:
   /// Constructor
   RootEventContext(const RootEvtSelector* selector);

   /// Copy constructor
   RootEventContext(const RootEventContext& ctxt);

   /// Destructor
   virtual ~RootEventContext();

   /// Inequality operator.
   virtual void* identifier() const;
   

private:
   const RootEvtSelector* m_evtSelector;
};
#endif
