//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/test/UseEstTof.cxx,v 1.2 2009/09/22 07:40:45 huangb Exp $
/*#include <stdio.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Tof/EstTofCalibData.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibRootSvc.h"

using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class UseTof : public Algorithm {

public:
  UseTof(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibRootSvc*    m_pRootSvc;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<UseTof> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& UseTofFactory = Factory;
*/
#include "UseEstTof.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "CalibData/Tof/TofElecData.h"
#include "CalibData/Tof/BarrelElec.h"
#include "CalibData/Tof/EndcapElec.h"

UseEstTof::UseEstTof( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode UseEstTof::initialize() {
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


StatusCode UseEstTof::execute( ) {
   
  MsgStream log(msgSvc(), name());

  // Cheat for now since Windows is having trouble finding definition
  // of Calibdata::Test_t
  std::string fullPath = "/Calib/TofQElec";
   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;

    SmartDataPtr<CalibData::TofElecData> btof(m_pCalibDataSvc, fullPath);
  
   for(int i =0;i<96;i++)
      {
        CalibData::BarrelElec aa = btof->getBTof(i);
        CalibData::EndcapElec bb = btof->getETof(i);
       std::cout<<"i==========="<<i<<std::endl;
       for(int j=0;j<4;j++){
       std::cout<<"getNumEast=="<<aa.getNumEast(j)<<"   getNumWest=="<<aa.getNumWest(j)<<std::endl;
        std::cout<<"getEast=="<<bb.getNum(j)<<std::endl;
        }
       for(int j=0;j<10;j++){ 
        std::cout<<"j==="<<j<<std::endl;
        std::cout<<"  getP1=="<<aa.getP1(j)<<std::endl;
        std::cout<<"  getP2=="<<aa.getP2(j)<<std::endl;
         std::cout<<"  getSimP1=="<<aa.getSimP1(j)<<std::endl;   
         std::cout<<"  getSimP2=="<<aa.getSimP2(j)<<std::endl;       
         std::cout<<"  getP=="<<bb.getP(j)<<std::endl;
          std::cout<<"  getSimP=="<<bb.getSimP(j)<<std::endl;
   }
      }


   
 

       //maqm comment because cann't write 
   //    m_pRootSvc->writeToRoot("./Tof_test.root", fullPath);
  
  return StatusCode::SUCCESS;
}
/*
DataSvc::retrieveObject(fullPath,CalibData::CalibTest1);
*/
StatusCode UseEstTof::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        UseTof FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}

