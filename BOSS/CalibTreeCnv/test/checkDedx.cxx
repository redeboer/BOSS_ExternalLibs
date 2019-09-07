//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibTreeCnv/src/test/checkDedx.cxx,v 1.5 2010/05/17 06:54:24 huangb Exp $
/*#include <stdio.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibTreeSvc.h"
using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class checkDedx : public Algorithm {

public:
  checkDedx(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibTreeSvc*    m_pTreeSvc;
  // Maybe something to say which kind of data to look up?

};
*/
#include "checkDedx.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibData/Dedx/DedxSimData.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibData/Emc/EmcCalibData.h"
#include "CalibData/EsTime/EsTimeCalibData.h"
#include "CalibData/Tof/TofCalibData.h"
#include "CalibData/Muc/MucCalibData.h"
#include "CalibData/Tof/TofElecData.h"
#include "CalibData/Tof/TofSimData.h"
#include "CalibData/Mdc/MdcAlignData.h"
/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<checkDedx> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& checkDedxFactory = Factory;

checkDedx::checkDedx( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode checkDedx::initialize() {
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


StatusCode checkDedx::execute( ) {
   
  MsgStream log(msgSvc(), name());

  // Cheat for now since Windows is having trouble finding definition
  // of Calibdata::Test_t
 // std::string fullPath = "/Calib/MdcCal";
//   std::string fullPath = "/Calib/DedxCal";
//  std::string fullPath = "/Calib/DedxSim";
//   std::string fullPath = "/Calib/EmcCal";
    std::string fullPath = "/Calib/EsTimeCal";
 //  std::string fullPath = "/Calib/EstTofCal";
 //  std::string fullPath = "/Calib/MucCal";
  //   std::string fullPath = "/Calib/TofCal";
 //  std::string fullPath = "/Calib/TofQElec";
//    std::string fullPath = "/Calib/TofSim";
//   std::string fullPath = "/Calib/MdcAlign";
   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;

 //   SmartDataPtr<CalibData::MdcCalibData> test(m_pCalibDataSvc, fullPath);
//   if(!btof) ;
/*  for(int i=0;i<16;i++){
     std::cout<<"test->getdxEP(i)"<<test->getdxEP(i)<<std::endl;
  }
  for(int i=0;i<6796;i++){
  std::cout<<"test->gettension"<<test->gettension(i)<<std::endl;
  }*/
//  if(!test) ;
// SmartDataPtr<CalibData::DedxCalibData> test(m_pCalibDataSvc, fullPath);
//   SmartDataPtr<CalibData::DedxSimData> test(m_pCalibDataSvc, fullPath);
 //     SmartDataPtr<CalibData::EmcCalibData> test(m_pCalibDataSvc, fullPath);
 //   SmartDataPtr<CalibData::TofElecData> test(m_pCalibDataSvc, fullPath);
 //  SmartDataPtr<CalibData::TofSimData> test(m_pCalibDataSvc, fullPath);
//   SmartDataPtr<CalibData::MdcAlignData> test(m_pCalibDataSvc, fullPath);
SmartDataPtr<CalibData::EsTimeCalibData> test(m_pCalibDataSvc, fullPath);

   if(!test) ;
//  test->getBTof(0);
//    int qtparno = test->getgain();
//    int xtno = test->getxtNo();
 //   int t0no = test->gett0No();
 //   int qtparno =test->gethistNo();
//     int qtparno =test->getDigiCalibConstNo();
//    int qtparno =test->getToffsete();
//   int qtparno =test->getRunBegin();
 //  int qtparno =test->getUniformEff();
 //    int qtparno =test->getdyWireEast(0);
 //  std::cout<<"qtparno="<<qtparno<<std::endl;
//"xtno="<<xtno<<"t0no="<<t0no<<std::endl;
//  std::cout<<"xtno="<<xtno<<std::endl;

   return StatusCode::SUCCESS;
}
/*
DataSvc::retrieveObject(fullPath,CalibData::CalibTest1);
*/
StatusCode checkDedx::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        checkDedx FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


