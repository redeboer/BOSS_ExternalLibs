//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/test/UseTof.cxx,v 1.13 2010/10/14 05:30:09 huangb Exp $
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
#include "UseTof.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"

UseTof::UseTof( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator ), m_pCalibDataSvc(0)
{
  // Declare properties here.

}


StatusCode UseTof::initialize() {
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


StatusCode UseTof::execute( ) {
   
  MsgStream log(msgSvc(), name());

  // Cheat for now since Windows is having trouble finding definition
  // of Calibdata::Test_t
  std::string fullPath = "/Calib/TofCal";
   log << MSG::INFO<<"execute() fullPath = "<<fullPath<< endreq;

    SmartDataPtr<CalibData::TofCalibData> btof(m_pCalibDataSvc, fullPath);

   TFile *f1 = new TFile("TofCalConst.root","read");

    TTree *btoftree = (TTree*)f1 -> Get("BarTofPar");
    double fCount[10];
     double fLeft[5];
     double fRight[5];
   btoftree ->SetBranchAddress("FCounter0",&fCount[0]);
     btoftree ->SetBranchAddress("FCounter1",&fCount[1]);
     btoftree ->SetBranchAddress("FCounter2",&fCount[2]);
     btoftree ->SetBranchAddress("FCounter3",&fCount[3]);
     btoftree ->SetBranchAddress("FCounter4",&fCount[4]);
     btoftree ->SetBranchAddress("FCounter5",&fCount[5]);
     btoftree ->SetBranchAddress("FCounter6",&fCount[6]);
     btoftree ->SetBranchAddress("FCounter7",&fCount[7]);
     btoftree ->SetBranchAddress("FCounter8",&fCount[8]);
     btoftree ->SetBranchAddress("FCounter9",&fCount[9]);
     btoftree->SetBranchAddress("FLeft0",&fLeft[0]);
      btoftree->SetBranchAddress("FLeft1",&fLeft[1]);
     btoftree->SetBranchAddress("FLeft2",&fLeft[2]);
    btoftree->SetBranchAddress("FLeft3",&fLeft[3]);
    btoftree->SetBranchAddress("FLeft4",&fLeft[4]);
   btoftree->SetBranchAddress("FRight0",&fRight[0]);
   btoftree->SetBranchAddress("FRight1",&fRight[1]);
   btoftree->SetBranchAddress("FRight2",&fRight[2]);
   btoftree->SetBranchAddress("FRight3",&fRight[3]);
   btoftree->SetBranchAddress("FRight4",&fRight[4]);

      

    for(int i =0;i<176;i++)
      {
        btoftree -> GetEntry(i);

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
    //  std::cout<<"=========================================="<<"\n";
   //   std::cout<<"cnt ="<<i<<"\n";
      double fCount1[10],fLeft1[5],fRight1[5];
      for(int j=0;j<10;j++){
        fCount1[j]=btof->getBTofFcounter(i,j);
        if(fCount1[j]!=fCount[j]) std::cout<<"error error error error"<<std::endl;  
 //      std::cout<<"
   //     std::cout<<"   fCount["<<j<<"]="<<fCount[j]<<"    fCount1["<<j<<"]="<<fCount1[j];
      }
 //  std::cout<<"\n"<<"=========================================="<<"\n";
    for(int j=0;j<5;j++){
        fLeft1[j]=btof->getBTofFleft(i,j);
     if(fLeft1[j]!=fLeft[j]) std::cout<<"error error error error"<<std::endl;
    //    std::cout<<"   fLeft["<<j<<"]="<<fLeft[j];
      }
//    std::cout<<"\n"<<"=========================================="<<"\n";

    for(int j=0;j<5;j++){
        fRight1[j]=btof->getBTofFright(i,j);
        if( fRight1[j]!= fRight[j])  std::cout<<"error error error error"<<std::endl;
  //      std::cout<<"   fRight["<<j<<"]="<<fRight[j];
      }

 
 /*     std::cout<<"Atten0="<<bAtten0<<"Atten1="<<bAtten1<<"Q="<<bQ<<"Speed0="<<bSpeed0<<"Speed1="<<bSpeed1<<"P0="<<bP0<<"P1="<<bP1<<"P2="<<bP2<<"P3="<<bP3<<"P4="<<bP4<<"P10="<<bP10;
      std::cout<<"\n";
      std::cout<<"P5="<<bP5<<"Q="<<bQ;
      std::cout<<"W1="<<bW1<<"W2="<<bW2<<"W3="<<bW3<<"W4="<<bW4;
      std::cout<<"\n";*/
      /*
      std::cout<<"Atten[1]="<<*(bAtten+1)<<"P0[1]="<<*(bP0+1)<<"P1[1]="<<*(bP1+1)<<"P2[1]="<<*(bP2+1)<<"P3[1]="<<*(bP3+1)<<"P4[1]="<<*(bP4+1);
      std::cout<<"\n";
      std::cout<<"P5[1]="<<*(bP5+1)<<"S[1]="<<*(bS+1)<<"SS[1]="<<*(bSS+1)<<"Veff[1]="<<*(bVeff+1);
      */
      }   
/*   for(int i =0;i<96;i++)
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
*/

/*double  P_b[20];
    TTree *btofinnertree = (TTree*)f1 -> Get("BarTofParInner");
    btofinnertree->SetBranchAddress("P0",&P_b[0]);
    btofinnertree->SetBranchAddress("P1",&P_b[1]);
    btofinnertree->SetBranchAddress("P2",&P_b[2]);
    btofinnertree->SetBranchAddress("P3",&P_b[3]);
    btofinnertree->SetBranchAddress("P4",&P_b[4]);
    btofinnertree->SetBranchAddress("P5",&P_b[5]);
    btofinnertree->SetBranchAddress("P6",&P_b[6]);
    btofinnertree->SetBranchAddress("P7",&P_b[7]);
    btofinnertree->SetBranchAddress("P8",&P_b[8]);
    btofinnertree->SetBranchAddress("P9",&P_b[9]);
    btofinnertree->SetBranchAddress("P10",&P_b[10]);
    btofinnertree->SetBranchAddress("P11",&P_b[11]);
    btofinnertree->SetBranchAddress("P12",&P_b[12]);
    btofinnertree->SetBranchAddress("P13",&P_b[13]);
    btofinnertree->SetBranchAddress("P14",&P_b[14]);
    btofinnertree->SetBranchAddress("P15",&P_b[15]);
    btofinnertree->SetBranchAddress("P16",&P_b[16]);
    btofinnertree->SetBranchAddress("P17",&P_b[17]);
    btofinnertree->SetBranchAddress("P18",&P_b[18]);
    btofinnertree->SetBranchAddress("P19",&P_b[19]);

   for(int i=0;i<88;i++){
    btofinnertree->GetEntry(i);
   double P[20];
   std::cout<<"\n"<<" i=================       "<<i<<std::endl;
    for(int j=0;j<10;j++){
 //      P[j]=btof->getBTofInnerPleft(i,j);
 
        std::cout<<"   P["<<j<<"]"<< P[j];
  }
   
 for(int j=0;j<10;j++){
//       P[j+10]=btof->getBTofInnerPright(i,j);
        std::cout<<"    P["<<j+10<<"]"<< P[j+10];
}
*/
/*if(P_b[16]!=P[16]){
  std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
  std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
 std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
 std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
 std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl; 
std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
 std::cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
}
 std::cout<<"\n";
if(P[16]>0){
std::cout<<"aaa"<<std::endl;
}
std::cout<<"P_b[16]===="<<P_b[16]<<"P_[16]"<<P[16]<<std::endl;
 }
 */
  double sigma[8];

   for(int i=0;i<8;i++){
    sigma[i]=btof->getBTofSigmaCorr(i);
   std::cout<<" sigma["<<i<<"]="<< sigma[i];
}
 std::cout<<"\n";

  std::cout<<"t0offset0 ="<< btof->getBTofOffset(0)<<"   t0offset1 ="<< btof->getBTofOffset(1)<<std::endl;

std::cout<<" btof->getRunBegin()"<<btof->getRunBegin();
 std::cout<<"btof->getRunEnd()"<<btof->getRunEnd();
 std::cout<<"btof->getVersion()"<<btof->getVersion();
 std::cout<<"btof->getQCorr()"<<btof->getQCorr();
 std::cout<<"btof->getMisLable()"<<btof->getMisLable();
 std::cout<<"btof->getQElec()"<<btof->getQElec();
 for(int i=0;i<5;i++){
    std::cout<<"i==="<<i<<std::endl;
    std::cout<<"  btof->getBrEast(int index)"<<btof->getBrEast(i);
    std::cout<<"  btof->getBrWest(int index)"<<btof->getBrWest(i);
    std::cout<<"  btof->getEndcap(int index)"<<btof->getEndcap(i);
}
       //maqm comment because cann't write 
   //    m_pRootSvc->writeToRoot("./Tof_test.root", fullPath);
  
  return StatusCode::SUCCESS;
}
/*
DataSvc::retrieveObject(fullPath,CalibData::CalibTest1);
*/
StatusCode UseTof::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        UseTof FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


