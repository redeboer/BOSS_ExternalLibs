//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibTreeCnv/src/test/checkEstTof.cxx,v 1.2 2010/10/14 05:33:40 huangb Exp $
/*#include <stdio.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Tof/TofCalibData.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibTreeSvc.h"
using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class checkTof : public Algorithm {

public:
  checkTof(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibTreeSvc*    m_pTreeSvc;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<checkTof> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& checkTofFactory = Factory;
*/
#include "checkEstTof.h"
checkEstTof::checkEstTof( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode checkEstTof::initialize() {
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


StatusCode checkEstTof::execute( ) {
   
  MsgStream log(msgSvc(), name());

  // Cheat for now since Windows is having trouble finding definition
  // of Calibdata::Test_t
  std::string fullPath = "/Calib/EstTofCal";
   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;

    SmartDataPtr<CalibData::TofCalibData> btof(m_pCalibDataSvc, fullPath);

    for(int i =0;i<176;i++)
      {
        double bAtten0 = btof->getBTofAtten(i,0);
	double bAtten1 = btof->getBTofAtten(i,1);
      double bP0  = btof->getBTofPleft(i,0);
      double bP1  = btof->getBTofPleft(i,1);
      double bP2  = btof->getBTofPleft(i,2);
      double bP3 = btof->getBTofPleft(i,3);
      double bP4 = btof->getBTofPleft(i,4);
      double bP5 = btof->getBTofPleft(i,5);
      double bP10 = btof->getBTofPright(i,0);


      //      double bQ = btof->getBTofQ(i);
      double bSpeed0 = btof->getBTofSpeed(i,0);
      double bSpeed1 = btof->getBTofSpeed(i,1);
      //      double bW1 = btof->getBTofW(i,0);
      //      double bW2 = btof->getBTofW(i,1);
      //      double bW3 = btof->getBTofW(i,2);
      //      double bW4 = btof->getBTofW(i,3);
      std::cout<<"=========================================="<<"\n";
      std::cout<<"cnt ="<<i<<"\n";
      std::cout<<"Atten0="<<bAtten0<<"Atten1="<<bAtten1<<"Q="<< "bQ " <<"Speed0="<<bSpeed0<<"Speed1="<<bSpeed1<<"P0="<<bP0<<"P1="<<bP1<<"P2="<<bP2<<"P3="<<bP3<<"P4="<<bP4<<"P10="<<bP10;
      std::cout<<"\n";
      std::cout<<"P5="<<bP5<<"Q="<<"bQ";
      //      std::cout<<"W1="<<bW1<<"W2="<<bW2<<"W3="<<bW3<<"W4="<<bW4;
      std::cout<<"\n";
      
      /*
      std::cout<<"Atten[1]="<<*(bAtten+1)<<"P0[1]="<<*(bP0+1)<<"P1[1]="<<*(bP1+1)<<"P2[1]="<<*(bP2+1)<<"P3[1]="<<*(bP3+1)<<"P4[1]="<<*(bP4+1);
      std::cout<<"\n";
      std::cout<<"P5[1]="<<*(bP5+1)<<"S[1]="<<*(bS+1)<<"SS[1]="<<*(bSS+1)<<"Veff[1]="<<*(bVeff+1);
      */
      }   
   for(int i =0;i<96;i++)
      {
        double eAtten0 = btof->getETofAtten(i,0);
	double eAtten1 = btof->getETofAtten(i,1);
      	double eP0  = btof->getETofP(i,0);
	double eP1  = btof->getETofP(i,1);
	double eP2  = btof->getETofP(i,2);
	double eP3 = btof->getETofP(i,3);
	double eP4 = btof->getETofP(i,4);
	double eP5 = btof->getETofP(i,5);
	double eP6 = btof->getETofP(i,6);

    	double eSpeed0 = btof->getETofSpeed(i,0);
	double eSpeed1 = btof->getETofSpeed(i,1);

	std::cout<<"=========================================="<<"\n";
	std::cout<<"cnt ="<<i<<"\n";
	
	std::cout<<"Atten0="<<eAtten0<<"Atten1="<<eAtten1<<"Speed0="<<eSpeed0<<"Speed1="<<eSpeed1<<"P0="<<eP0<<"P1="<<eP1<<"P2="<<eP2<<"P3="<<eP3<<"P4="<<eP4<<"P6="<<eP6;
	std::cout<<"\n";
	std::cout<<"P5="<<eP5;
	std::cout<<"\n";

	}
       //maqm comment because cann't write 
      // m_pTreeSvc->writeToTree("./Tof_test.root", fullPath);
  
  return StatusCode::SUCCESS;
}
/*
DataSvc::retrieveObject(fullPath,CalibData::CalibTest1);
*/
StatusCode checkEstTof::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        checkEstTof FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


