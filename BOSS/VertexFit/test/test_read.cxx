#include "test_read.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

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
/*sc = service("VertexDbSvc", m_mdc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not get ICalibRootSvc interface of CalibRootCnvSvc" 
	<< endreq;
    return sc;
  }
*/
  // Get properties from the JobOptionsSvc
  
 return StatusCode::SUCCESS;

}


StatusCode test_read::execute( ) {
   
  MsgStream log(msgSvc(), name());
//  ReadPara* a1 = m_mdc->getReadPara();
   IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
 //  service("VertexDbSvc", vtxsvc, true);
   bool aa = vtxsvc->isVertexValid();
  std::cout<<" status: "<< aa <<std::endl;
  double* a1 = vtxsvc->PrimaryVertex();
  double* a2 = vtxsvc->SigmaPrimaryVertex();
  std::cout<<" vx: "<<a1[0]<<" vy: "<<a1[1] <<" vz: " << a1[2] <<std::endl;
  std::cout<<" vx sigma: "<<a2[0]<<" vy sigma: "<<a2[1] <<" vz sigma: " << a2[2] <<std::endl;
  return StatusCode::SUCCESS;
}

StatusCode test_read::finalize() {
return StatusCode::SUCCESS;
}
