//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/test/UseMdcCalibData.cxx,v 1.7 2008/04/08 08:34:13 huangb Exp $
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
#include "CalibDataSvc/ICalibRootSvc.h"
using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class UseMdcCalibData : public Algorithm {

public:
  UseMdcCalibData(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibRootSvc*    m_pRootSvc;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<UseMdcCalibData> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& UseMdcCalibDataFactory = Factory;
*/
#include "UseMdcCalibData.h"
UseMdcCalibData::UseMdcCalibData( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode UseMdcCalibData::initialize() {
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

  sc = service("CalibRootCnvSvc", m_pRootSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not get ICalibRootSvc interface of CalibRootCnvSvc" 
	<< endreq;
    return sc;
  }
  // Get properties from the JobOptionsSvc
  
 sc = setProperties();
 return StatusCode::SUCCESS;

}


StatusCode UseMdcCalibData::execute( ) {
   
  MsgStream log(msgSvc(), name());

  // Cheat for now since Windows is having trouble finding definition

  std::string fullPath = "/Calib/MdcCal";
 std::string file =m_pRootSvc->getrootfile();
 std::cout<<"file="<<file<<"\n";
// return StatusCode::SUCCESS;
   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;
   
   SmartDataPtr<CalibData::MdcCalibData> test1Copy(m_pCalibDataSvc, fullPath);
   if(!test1Copy);
   //    CalibData::MdcCalibData test;
 
   //   m_pCalibDataSvc->registerObject(FullPath,test);
    m_pRootSvc->writeToRoot("Mdc_test.root", fullPath);
//  SmartDataPtr<CalibData::MdcCalibData> test1Copy(m_pCalibDataSvc, fullPath);
   //  if(!test1Copy);
  return StatusCode::SUCCESS;
}
/*
DataSvc::retrieveObject(fullPath,CalibData::CalibTest1);
*/
StatusCode UseMdcCalibData::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        UseMdcCalibData1 FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


