#ifndef BES_DCHAIN_SELECTOR_H
#define BES_DCHAIN_SELECTOR_H

#include "GaudiKernel/IDataProviderSvc.h"
#include "VertexFit/IVertexDbSvc.h"
#include "DecayChain/Function/DCSelectionFunction.h"


template < class Arg >
class BesDCSelector: public DCSelectionFunction<Arg>
{

   public:

      BesDCSelector() : m_evtSvc(0) {};
      virtual ~BesDCSelector() {};


   protected:

      IDataProviderSvc* eventSvc();
      IVertexDbSvc*     vtxDbSvc();

   private:

      IDataProviderSvc* m_evtSvc;
      IVertexDbSvc*     m_vtxSvc;
};

#include "BesDChain/Template/BesDCSelector.cc"

#endif
