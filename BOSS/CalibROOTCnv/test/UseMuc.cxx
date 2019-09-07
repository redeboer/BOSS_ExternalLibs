//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/test/UseMuc.cxx,v 1.2 2008/04/09 01:02:04 huangb Exp $
/*#include <stdio.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Muc/MucCalibData.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibRootSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include "CalibData/Muc/MucCalibConst.h"
//#include "CalibROOTCnv/MucIdTransform.h"
//#include "MucCalibAlg/MucIdTransform.h"


//using namespace std;
/// Simple algorithm to test functioning of "the other" TDS
class UseMuc : public Algorithm {

public:
  UseMuc(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:

  IDataProviderSvc* m_pCalibDataSvc;
  ICalibRootSvc*    m_pRootSvc;
  // MucIdTransform*   m_pIdTr;
  // Maybe something to say which kind of data to look up?

};

/// Instantiation of a static factory to create instances of this algorithm
//static const AlgFactory<UseMuc> Factory;
//const IAlgFactory& UseCalibFactory = Factory;
//const IAlgFactory& UseMucFactory = Factory;
*/
#include "UseMuc.h"
UseMuc::UseMuc( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode UseMuc::initialize() {
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

 // m_pIdTr = new MucIdTransform();

 return StatusCode::SUCCESS;

}


StatusCode UseMuc::execute( ) {
   
  MsgStream log(msgSvc(), name());
/*  std::vector<double> emccalib;
  CalibData::MucCalibData* emc = new CalibData::MucCalibData;
  //set emcCalibdata--------------------------------
   for(i=0;i<6000;i++)
    {  emccalib.push_back(i);
    }
  
   emc->setDigiCalibConst(&emccalib);
   emccalib.clear();
  //set EnCoeff
   for(i=0;i<43;i++)
    { emccalib.push_back(1);
    }
   emc->setEnCoeff(&emccalib);
   emccalib.clear();  
   //set PosCoeff
  for(i=0;i<15;i++)
    { emccalib.push_back(0);
    }
  emc->setPosCoeff(&emccalib);
  emc->setrunfrm(0);
  emc->setrunto(0);
 */
   //register the emcCalibData into the TCDS ; nelectable
   std::string fullPath = "/Calib/MucCal";
    log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;
  //  m_pCalibDataSvc->registerObject(fullPath,emc);

    //write the MucCalibData to the rootfile; 
//   m_pRootSvc->writeToRoot("./Muc_dat.root", fullPath);
   //another way,write the MucCalibData without TCDS
//   m_pRootSvc->writeToRoot("./Muc_dat1.root", emc);
  //retrieve DataObject from the TCDS-----------------
  SmartDataPtr<CalibData::MucCalibData> test(m_pCalibDataSvc, fullPath);
  if(!test);
   std::cout<<"m_uniformEff="<<test->getUniformEff()<<std::endl;
   std::cout<<"m_UniformNos="<<test->getUniformNos()<<std::endl;
   std::cout<<"m_UniformClst="<<test->getUniformClst()<<std::endl;
   for(int i=0;i<LAYER_MAX;i++){
     std::cout<<"m_layerEff["<<i<<"]="<< test->getLayerEff(i)<<"  m_layerNos["<<i<<"]="<<test->getLayerNos(i)<<std::endl;
     for(int j=0;j<CLST_MAX;j++){
      std::cout<<"m_layerClstPro["<<i<<"]["<<j<<"]="<<test->getLayerClstPro(i,j)<<std::endl;
     }
   }

   for(int i=0; i<PART_MAX; i++)
        {
	  for(int j=0; j<((i==1)?B_SEG_NUM:E_SEG_NUM); j++)
	  {
	    for(int k=0; k<((i==1)?B_LAY_NUM:E_LAY_NUM); k++)
	    {
		    std::cout << "Box: " << i << "\t" << j << "\t" << k << "\t"
		    << test->getBoxEff(i, j, k) << endl;
	    }
	  }	
        }
/*
	int strip_max;
        for(int i=0; i<PART_MAX; i++)
        {
	  for(int j=0; j<((i==1)?B_SEG_NUM:E_SEG_NUM); j++)
	  {
	    for(int k=0; k<((i==1)?B_LAY_NUM:E_LAY_NUM); k++)
	    {
                strip_max = m_pIdTr->GetStripMax(i, j, k);
		for(int n=0; n<strip_max; n++)
		{
			std::cout << "Strip: " << i << "\t" << j << "\t" << k << "\t" 
			    << "\t" << n << "\t" << test->getStripEff(i, j, k, n) << endl;
		}
	    }
	  }			
        }
*/

      return StatusCode::SUCCESS;
}

StatusCode UseMuc::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        UseMuc FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


