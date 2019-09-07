//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibTreeCnv/src/test/checkMdc.cxx,v 1.1.1.1 2008/04/09 02:08:15 huangb Exp $
/*#include <stdio.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Mdc/MdcCalibData.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibTreeSvc.h"
using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class checkMdc : public Algorithm {

public:
  checkMdc(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibTreeSvc*    m_pTreeSvc;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<checkMdc> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& checkMdcFactory = Factory;

checkMdc::checkMdc( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}
*/
#include "checkMdc.h"
checkMdc::checkMdc( const std::string&  name, 
                        ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
    // Declare properties here.
    //   
       } 
    //   
StatusCode checkMdc::initialize() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // So far don't have any properties, but in case we do some day..
//  setProperties();

   log<<MSG::INFO << "setProperties()" << endreq;
  
   sc = service("CalibDataSvc", m_pCalibDataSvc, true);

  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not get IDataProviderSvc interface of CalibXmlCnvSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IDataProviderSvc interface of CalibXmlCnvSvc" 
	<< endreq;
  }
  
  sc = service("CalibTreeCnvSvc", m_pTreeSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not get ICalibTreeSvc interface of CalibTreeCnvSvc" 
	<< endreq;
    return sc;
  }
  // Get properties from the JobOptionsSvc
  
 sc = setProperties();

 return StatusCode::SUCCESS;

}


StatusCode checkMdc::execute( ) {
   
  MsgStream log(msgSvc(), name());

  // Cheat for now since Windows is having trouble finding definition
  // of Calibdata::Test_t
  std::string fullPath = "/Calib/MdcCal";
   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;

    SmartDataPtr<CalibData::MdcCalibData> test(m_pCalibDataSvc, fullPath);
//   if(!btof) ;
    int qtparno = test->getqtparNo();
    int xtno = test->getxtNo(); 
    int t0no = test->gett0No();
    std::cout<<"qtparno="<<qtparno<<"xtno="<<xtno<<"t0no="<<t0no<<std::endl;
    


   
   return StatusCode::SUCCESS;
}
/*
DataSvc::retrieveObject(fullPath,CalibData::CalibTest1);
*/
StatusCode checkMdc::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        checkMdc FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


