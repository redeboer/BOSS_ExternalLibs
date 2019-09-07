#include "test_read.h"
#include "ReadPara.hh"
#include "BesMdcRes.hh"
test_read::test_read( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator )
{
  // Declare properties here.

}


StatusCode test_read::initialize() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // So far don't have any properties, but in case we do some day..
//  setProperties();
sc = service("MdcTunningSvc", m_mdc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not get ICalibRootSvc interface of CalibRootCnvSvc" 
	<< endreq;
    return sc;
  }

  // Get properties from the JobOptionsSvc
  
 return StatusCode::SUCCESS;

}


StatusCode test_read::execute( ) {
   
  MsgStream log(msgSvc(), name());
  for(int i=0;i<43;i++){
    for(int j=0;j<20;j++){
   double aa =  m_mdc->get_docaEff(i,j);
   double bb = m_mdc->get_thetaEff(i,j);
   double cc = m_mdc->get_cellEff(i,j);
   std::cout<<"i:  "<<i<<"  j: "<<j<<std::endl;
   std::cout<<" get_docaEff: "<<aa<<"   thetaEff:"<<bb<<"  cellEff:"<<cc<<std::endl;
  }
}
  
  return StatusCode::SUCCESS;
}

StatusCode test_read::finalize() {
return StatusCode::SUCCESS;


}
