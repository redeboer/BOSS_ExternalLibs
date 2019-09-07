// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootTofSimDataCnv.cxx,v 1.2 2010/02/03 01:18:23 huangb Exp $
#include "GaudiKernel/MsgStream.h"
#include "RootTofSimDataCnv.h"
#include "CalibData/Tof/TofSimData.h"
#include "CalibData/Tof/BTofSimBase.h"
#include "CalibData/Tof/ETofSimBase.h"
#include "CalibData/Tof/TofSimConstBase.h"
#include "CalibDataSvc/IInstrumentName.h"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibDataSvc/ICalibRootSvc.h"    //maybe
#include "CalibDataSvc/ICalibMetaCnvSvc.h"
//#include "CalibData/CalibTime.h"
//#include "commonRootData/idents/CalXtalId.h"
//#include "idents/CalXtalId.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;
//static CnvFactory<RootTofSimDataCnv> TofCalib_factory;
//const  ICnvFactory& RootTofSimDataCnvFactory = TofCalib_factory;



RootTofSimDataCnv::RootTofSimDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_TofSim) { 
 }


const CLID& RootTofSimDataCnv::objType() const {
  return CLID_Calib_TofSim;
}

const CLID& RootTofSimDataCnv::classID() {
  return CLID_Calib_TofSim;
}

StatusCode RootTofSimDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootTofSimDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
 // open the file
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
 
  CalibData::BTofSimBase bTof;
  CalibData::ETofSimBase eTof;
  CalibData::TofSimConstBase tofbase;
  std::vector<CalibData::BTofSimBase> tmpbTof;//; = new vector<CalibData::bTofCalibBase>;
  std::vector<CalibData::ETofSimBase> tmpeTof;
  std::vector<CalibData::TofSimConstBase> tofbaseCol;
   // Read in the object
  int cnt;
  // read btoftree ------------------------------------------------------------
     double AttenLength;
     double Gain;
     double Ratio;
     double NoiseSmear;
     TTree *btoftree = (TTree*)m_inFile -> Get("BTofSim");
     btoftree -> SetBranchAddress("AttenLength", &AttenLength);
     btoftree -> SetBranchAddress("Gain", &Gain);
     btoftree -> SetBranchAddress("Ratio", &Ratio);
     int entries=btoftree->GetEntries();
       for(cnt=0; cnt<entries; cnt++){
         btoftree -> GetEntry(cnt);
	 bTof.setGain(Gain);
         bTof.setRatio(Ratio);
         bTof.setAttenLength(AttenLength);
	 tmpbTof.push_back(bTof);
	  }
       //read etoftree
     TTree *etoftree = (TTree*)m_inFile -> Get("ETofSim");
     etoftree -> SetBranchAddress("Gain", &Gain);
     etoftree -> SetBranchAddress("AttenLength", &AttenLength);
     etoftree -> SetBranchAddress("NoiseSmear",  &NoiseSmear );
       entries=etoftree->GetEntries();
       for(cnt=0; cnt<entries; cnt++){
         etoftree->GetEntry(cnt);
         eTof.setGain(Gain);
         eTof.setAttenLength(AttenLength);
         eTof.setNoiseSmear(NoiseSmear);
	 tmpeTof.push_back(eTof);
	  }

   //read SimConstants
    double BarConstant,BarPMTGain,BarHighThres,BarLowThres,EndConstant,EndPMTGain,EndHighThres,EndLowThres,EndNoiseSwitch;
    TTree *btofcommontree = (TTree*)m_inFile -> Get("SimConstants");
    btofcommontree-> SetBranchAddress("BarConstant", &BarConstant);
    btofcommontree-> SetBranchAddress("BarPMTGain",  &BarPMTGain);
    btofcommontree-> SetBranchAddress("BarHighThres", &BarHighThres);
    btofcommontree-> SetBranchAddress("BarLowThres", &BarLowThres);
    btofcommontree-> SetBranchAddress("EndConstant", &EndConstant);
    btofcommontree-> SetBranchAddress("EndPMTGain",  &EndPMTGain);
    btofcommontree-> SetBranchAddress("EndHighThres", &EndHighThres);
    btofcommontree-> SetBranchAddress("EndLowThres", &EndLowThres);
    btofcommontree-> SetBranchAddress("EndNoiseSwitch", &EndNoiseSwitch);
       entries = btofcommontree->GetEntries();
      for(cnt=0;cnt<entries;cnt++){
	      btofcommontree->GetEntry(cnt);
	      tofbase.setBarLowThres(BarLowThres);
	      tofbase.setBarHighThres(BarHighThres);
	      tofbase.setEndLowThres(EndLowThres);
	      tofbase.setEndHighThres(EndHighThres);
	      tofbase.setBarPMTGain(BarPMTGain);
	      tofbase.setEndPMTGain(EndPMTGain); 
	      tofbase.setBarConstant(BarConstant);
	      tofbase.setEndConstant(EndConstant); 
	      tofbase.setEndNoiseSwitch(EndNoiseSwitch); 
              tofbaseCol.push_back(tofbase);   
     }

     CalibData::TofSimData *tmpObject = new CalibData::TofSimData(&tofbaseCol,&tmpbTof,&tmpeTof);

  
      refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

StatusCode RootTofSimDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
   MsgStream log(msgSvc(), "RootTofSimDataCnv");

 // Open the file, create the branch
   StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
 /*    int i;
     int j;
     CalibData::TofCalibData* btof = dynamic_cast<CalibData::TofCalibData*>(pTDSObj);

  // write  btoftree----------------------------------------------------------------
     double cnvP1[10];
     double cnvP2[10];
     double cnvW[4];
     double cnvAtten[8];
     double cnvQ;
     double cnvSpeed[2];
     TTree *btoftree = new TTree("BarTofPar", "BarTofPar");
      btoftree -> Branch("Atten0",&cnvAtten[0], "Atten0/D");
      btoftree -> Branch("Atten1",&cnvAtten[1], "Atten1/D");
      btoftree -> Branch("Atten2",&cnvAtten[2], "Atten2/D");
      btoftree -> Branch("Atten3",&cnvAtten[3], "Atten3/D");
      btoftree -> Branch("Atten4",&cnvAtten[4], "Atten4/D");
      btoftree -> Branch("Atten5",&cnvAtten[5], "Atten5/D");
      btoftree -> Branch("Atten6",&cnvAtten[6], "Atten6/D");
      btoftree -> Branch("Atten7",&cnvAtten[7], "Atten7/D");
      btoftree -> Branch("Q0",&cnvQ, "Q0/D");
      btoftree -> Branch("Speed0",&cnvSpeed[0], "Speed0/D");
      btoftree -> Branch("Speed1",&cnvSpeed[1], "Speed1/D");
      btoftree -> Branch("P0",&cnvP1[0], "P0/D");
      btoftree -> Branch("P1",&cnvP1[1], "P1/D");
      btoftree -> Branch("P2",&cnvP1[2], "P2/D");
      btoftree -> Branch("P3",&cnvP1[3], "P3/D");
      btoftree -> Branch("P4",&cnvP1[4], "P4/D");
      btoftree -> Branch("P5",&cnvP1[5], "P5/D");
      btoftree -> Branch("P6",&cnvP1[6], "P6/D");
      btoftree -> Branch("P7",&cnvP1[7], "P7/D");
      btoftree -> Branch("P8",&cnvP1[8], "P8/D");
      btoftree -> Branch("P9",&cnvP1[9], "P9/D");
      btoftree -> Branch("P10",&cnvP2[0], "P10/D");
      btoftree -> Branch("P11",&cnvP2[1], "P11/D");
      btoftree -> Branch("P12",&cnvP2[2], "P12/D");
      btoftree -> Branch("P13",&cnvP2[3], "P13/D");
      btoftree -> Branch("P14",&cnvP2[4], "P14/D");
      btoftree -> Branch("P15",&cnvP2[5], "P15/D");
      btoftree -> Branch("P16",&cnvP2[6], "P16/D");
      btoftree -> Branch("P17",&cnvP2[7], "P17/D");
      btoftree -> Branch("P18",&cnvP2[8], "P17/D");
      btoftree -> Branch("P19",&cnvP2[9], "P17/D");
      btoftree -> Branch("W0",&cnvW[0], "W0/D");
      btoftree -> Branch("W1",&cnvW[1], "W1/D");
      btoftree -> Branch("W2",&cnvW[2], "W2/D");
      btoftree -> Branch("W3",&cnvW[3], "W3/D");

       for(i=0; i<176; i++){
	 cnvAtten[0] = btof->getBTofAtten(i,0);
	 cnvAtten[1] = btof->getBTofAtten(i,1);
	 cnvAtten[2] = btof->getBTofAtten(i,2);
	 cnvQ = btof->getBTofQ(i);
         cnvSpeed[0] = btof->getBTofSpeed(i,0);
	 cnvSpeed[1] = btof->getBTofSpeed(i,1);
	 for(j=0;j<10;j++){
	   cnvP1[j] = btof->getBTofPleft(i,j);
	   cnvP2[j] = btof->getBTofPright(i,j);
	 }
         for(j=0;j<4;j++){
	   cnvW[j]=btof->getBTofW(i,j);
	 }
	 btoftree -> Fill();
       }
     
 //write  etoftree----------------------------------------------------------------
     double ecnvP[8];
     double ecnvAtten[5];
     double ecnvSpeed[3];
     TTree *etoftree = new TTree("EndTofPar", "EndTofPar");
      etoftree -> Branch("Atten0",&ecnvAtten[0], "Atten0/D");
      etoftree -> Branch("Atten1",&ecnvAtten[1], "Atten1/D");
      etoftree -> Branch("Atten2",&ecnvAtten[2], "Atten2/D");
      etoftree -> Branch("Atten3",&ecnvAtten[3], "Atten3/D");
      etoftree -> Branch("Atten4",&ecnvAtten[4], "Atten4/D");
      etoftree -> Branch("Speed0",&ecnvSpeed[0], "Speed0/D");
      etoftree -> Branch("Speed1",&ecnvSpeed[1], "Speed1/D");
      etoftree -> Branch("P0",&ecnvP[0], "P0/D");
      etoftree -> Branch("P1",&ecnvP[1], "P1/D");
      etoftree -> Branch("P2",&ecnvP[2], "P2/D");
      etoftree -> Branch("P3",&ecnvP[3], "P3/D");
      etoftree -> Branch("P4",&ecnvP[4], "P4/D");
      etoftree -> Branch("P5",&ecnvP[5], "P5/D");
      etoftree -> Branch("P6",&ecnvP[6], "P6/D");
      etoftree -> Branch("P7",&ecnvP[7], "P7/D");
    
       for(i=0; i<96; i++){
	 ecnvAtten[0] = btof->getETofAtten(i,0);
	 ecnvAtten[1] = btof->getETofAtten(i,1);
	 ecnvAtten[2] = btof->getETofAtten(i,2);
         ecnvSpeed[0] = btof->getETofSpeed(i,0);
	 ecnvSpeed[1] = btof->getETofSpeed(i,1);
	 for(j=0;j<8;j++){
	   ecnvP[j] = btof->getETofP(i,j);
	 }
	 etoftree -> Fill();
       }
//  write all the trees
     btoftree -> Write();
     etoftree -> Write();
     delete btoftree;
     delete etoftree;
     closeWrite();
     log<<MSG::INFO<<"successfully create RootFile"<<endreq;
*/  
     return sc;

}
