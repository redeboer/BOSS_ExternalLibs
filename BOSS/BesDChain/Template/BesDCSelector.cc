#ifndef BES_DCHAIN_SELECTOR_CC
#define BES_DCHAIN_SELECTOR_CC

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

template< class Arg >
IDataProviderSvc* BesDCSelector< Arg >::eventSvc() {

   if ( m_evtSvc == 0 ) {
      StatusCode sc = Gaudi::svcLocator()->service( "EventDataSvc", m_evtSvc, true );
      if( sc.isFailure() ) {
	 assert(false);
      }
   }

   return m_evtSvc;
}

template< class Arg >
IVertexDbSvc* BesDCSelector< Arg >::vtxDbSvc() {

   if ( m_vtxSvc == 0 ) {
      StatusCode sc = Gaudi::svcLocator()->service( "VertexDbSvc", m_vtxSvc, true );
      if( sc.isFailure() ) {
	 assert(false);
      }
   }

   return m_vtxSvc;
}

#endif
