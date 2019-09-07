//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/test/UseDedx.cxx,v 1.7 2010/03/05 02:17:54 huangb Exp $
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
#include "CalibDataSvc/ICalibRootSvc.h"
using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class UseDedx : public Algorithm {

public:
  UseDedx(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  IDataProviderSvc* m_pCalibDataSvc;
  ICalibRootSvc*    m_pRootSvc;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<UseDedx> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& UseDedxFactory = Factory;
*/
#include "UseDedx.h"
UseDedx::UseDedx( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode UseDedx::initialize() {
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


StatusCode UseDedx::execute( ) {
   
  MsgStream log(msgSvc(), name());


  std::string fullPath = "/Calib/DedxCal";

   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;
   
    SmartDataPtr<CalibData::DedxCalibData> test(m_pCalibDataSvc, fullPath);
    int i;
    int j;
   double ddg;
   double ggs;
   double wireg;
   double gain;
   double zdep;
   double resol;
   double layerg;
   double m_id_doca;
   double m_iner_chi;
   double m_iner_gain;
   double m_iner_hits;
   double m_ip_eangle;
   double m_out_chi;
   double m_out_gain;
   double m_out_hits;

    for(i=0;i<4;i++){
      for(j=0;j<43;j++){
	ddg = test->getddg(i,j);
        ggs = test->getggs(i,j);
        zdep = test->getzdep(i,j);
//	std::cout<<"ddg["<<i<<"]["<<j<<"]="<<ddg;
//        std::cout<<"         ggs["<<i<<"]["<<j<<"]="<<ggs;
//        std::cout<<"        zdep["<<i<<"]["<<j<<"]="<<zdep;
//	std::cout<<"\n";
      }
    }
    
     for(int i=0;i<1600;i++){
      m_id_doca = test->get_id_doca(i);
      m_iner_chi= test->get_iner_chi(i);
      m_iner_gain= test->get_iner_gain(i);
      m_iner_hits= test->get_iner_hits(i);
      m_ip_eangle= test->get_ip_eangle(i);
      m_out_chi= test->get_out_chi(i);
      m_out_gain= test->get_out_gain(i);
      m_out_hits= test->get_out_hits(i);
     std::cout<<"m_id_doca:  "<<m_id_doca<<" m_iner_chi: "<<m_iner_chi<<" m_iner_gain: "<<m_iner_gain<<" m_iner_hits:"<<m_iner_hits<<" m_ip_eangle:"<<m_ip_eangle<<"  m_out_chi="<<m_out_chi<<" m_out_gain:"<<m_out_gain<<" m_out_hits="<<m_out_hits<<std::endl;
      }
    gain = test->getgain();
//    std::cout<<"gain="<<gain<<"\n";
     double m_costheta;
    for(int i=0;i<80;i++){
     m_costheta = test->get_costheta(i);
      std::cout<<" m_costheta="<<m_costheta;
      if(i%5==0) std::cout<<"\n"<<std::endl;
    }
    double t0,dedx;
    for(int i=0;i<35;i++){
     t0=test->get_t0(i);
     dedx=test->get_dedx(i);
     std::cout<<"t0  ="<<t0<<"  dedx="<<dedx;
     if(i%5==0) std::cout<<"\n"<<std::endl;
    }

    resol = test->getresol();
  //  std::cout<<"resol="<<resol<<"\n";

    for(i=0;i<43;i++)
     { layerg = test -> getlayerg(i);
  //   std::cout<<"layerg["<<i<<"]="<<layerg;
     }

    for(i=0;i<60;i++)
     { wireg = test -> getwireg(i);
    // std::cout<<"wireg["<<i<<"]="<<wireg<<"\n";
     }
    
    for(i=0;i<20;i++){
    std::cout<<"hadron "<<i<<" = "<<test->get_hadron(i);
     }   

     //for pah is error
     //m_pRootSvc->writeToRoot("/home/huangb/2.root", fullPath);
   return StatusCode::SUCCESS;
}

StatusCode UseDedx::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        UseDedx FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


