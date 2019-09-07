//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/test/UseEmc.cxx,v 1.5 2008/04/08 08:34:13 huangb Exp $
/*#include <stdio.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Emc/EmcCalibData.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibRootSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
//using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class UseEmc : public Algorithm {

public:
  UseEmc(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibRootSvc*    m_pRootSvc;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<UseEmc> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& UseEmcFactory = Factory;
*/
#include "UseEmc.h"
UseEmc::UseEmc( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode UseEmc::initialize() {
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


StatusCode UseEmc::execute( ) {
   
  MsgStream log(msgSvc(), name());
  int i;
  std::vector<double> emccalib;
  CalibData::EmcCalibData* emc = new CalibData::EmcCalibData;
  //set emcCalibdata--------------------------------
   for(i=0;i<6000;i++)
    {  emccalib.push_back(i);
    }
  
   emc->setDigiCalibConst(&emccalib);
  emc->setrunfrm(0);
  emc->setrunto(0);
 
   //register the emcCalibData into the TCDS ; nelectable
   std::string fullPath = "/Calib/EmcCal";
    log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;
    m_pCalibDataSvc->registerObject(fullPath,emc);

    //write the EmcCalibData to the rootfile; 
   m_pRootSvc->writeToRoot("./Emc_dat.root", fullPath);
   //another way,write the EmcCalibData without TCDS
   m_pRootSvc->writeToRoot("./Emc_dat1.root", emc);
  //retrieve DataObject from the TCDS-----------------
  SmartDataPtr<CalibData::EmcCalibData> test(m_pCalibDataSvc, fullPath);
  if(!test);
   return StatusCode::SUCCESS;
}
StatusCode UseEmc::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        UseEmc FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


