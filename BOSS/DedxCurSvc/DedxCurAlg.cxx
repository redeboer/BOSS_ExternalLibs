#include "DedxCurAlg.h"
#include "DedxCurSvc/IDedxCurSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"

DedxCurAlg::DedxCurAlg( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode DedxCurAlg::initialize() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // So far don't have any properties, but in case we do some day..
//  setProperties();

 return StatusCode::SUCCESS;

}


StatusCode DedxCurAlg::execute( ) {
   
  MsgStream log(msgSvc(), name());

    IDedxCurSvc*  vtxsvc;
  Gaudi::svcLocator()->service("DedxCurSvc", vtxsvc);
  for(int i=0;i<5;i++){
  std::cout<<"vtxsvc->getSigma(i)"<<vtxsvc->getSigma(i)<<std::endl;
  std::cout<<"vtxsvc->getCurve(i)"<<vtxsvc->getCurve(i)<<std::endl;
}

   return StatusCode::SUCCESS;
}

StatusCode DedxCurAlg::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        DedxCurAlg FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


