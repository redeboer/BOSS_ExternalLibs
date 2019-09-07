// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootTofCalibDataCnv.cxx,v 1.19 2016/02/15 08:13:48 sunss Exp $
#include "GaudiKernel/MsgStream.h"
#include "RootTofCalibDataCnv.h"
#include "CalibData/Tof/TofCalibData.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/Tof/eTofCalibBase.h"
#include "CalibData/Tof/etfCalibBase.h"
#include "CalibData/Tof/etfBunchCalibBase.h"
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

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;

const unsigned int nBarAtten  = 8;
const unsigned int nBarSpeed  = 2;
const unsigned int nBarPar    = 10;
const unsigned int nBarParOff = 20;
const unsigned int nBarSigma  = 5;
const unsigned int nBarSigCnt = 10;

const unsigned int nBarSigCor = 8;
const unsigned int nBarOffset = 2;

const unsigned int nEndAtten  = 5;
const unsigned int nEndSpeed  = 4;
const unsigned int nEndPar    = 8;
const unsigned int nEndSigma  = 5;

const unsigned int nEtfSpeed  = 4;
const unsigned int nEtfPar    = 20;
const unsigned int nEtfBunch  = 4;


RootTofCalibDataCnv::RootTofCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_TofCal) { 
 }


const CLID& RootTofCalibDataCnv::objType() const {
  return CLID_Calib_TofCal;
}

const CLID& RootTofCalibDataCnv::classID() {
  return CLID_Calib_TofCal;
}

StatusCode RootTofCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootTofCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
 // open the file
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
 
  CalibData::bTofCalibBase bTof;
  CalibData::eTofCalibBase eTof;
  CalibData::etfCalibBase  etf;
  CalibData::etfBunchCalibBase  etfBunch;
  CalibData::bTofCommonCalibBase bTofCommon;
  CalibData::tofCalibInfoBase tofinfo;

  std::vector<CalibData::bTofCalibBase> tmpbTof;
  std::vector<CalibData::eTofCalibBase> tmpeTof;
  std::vector<CalibData::etfCalibBase>  tmpetf;
  std::vector<CalibData::etfBunchCalibBase>  tmpetfBunch;
  std::vector<CalibData::bTofCommonCalibBase> tmpbTofCommon;
  std::vector<CalibData::tofCalibInfoBase> tofinfoCol;
   // Read in the object
  int cnt;
  // read btoftree ------------------------------------------------------------

  double cnvBarAtten[nBarAtten];
  double cnvBarSpeed[nBarSpeed];
  double cnvBarPar1[nBarPar];
  double cnvBarPar2[nBarPar];
  double cnvBarParOff1_bunch0[nBarParOff];
  double cnvBarParOff2_bunch0[nBarParOff];
  double cnvBarParOff1_bunch1[nBarParOff];
  double cnvBarParOff2_bunch1[nBarParOff];
  double cnvBarParOff1_bunch2[nBarParOff];
  double cnvBarParOff2_bunch2[nBarParOff];
  double cnvBarParOff1_bunch3[nBarParOff];
  double cnvBarParOff2_bunch3[nBarParOff];
  double cnvFLeft[nBarSigma];
  double cnvFRight[nBarSigma];
  double cnvFCount[nBarSigCnt];

  TTree *btoftree = (TTree*)m_inFile -> Get("BarTofPar");

  char brname[10];
  for( unsigned int i=0; i<nBarAtten; i++ ) {
    sprintf( brname, "Atten%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarAtten[i] );    
  }
  for( unsigned int i=0; i<nBarSpeed; i++ ) {
    sprintf( brname, "Speed%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarSpeed[i] );    
  }
  for( unsigned int i=0; i<nBarPar; i++ ) {
    sprintf( brname, "P%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarPar1[i] );    
  }
  for( unsigned int i=0; i<nBarPar; i++ ) {
    sprintf( brname, "P%i", i+nBarPar );
    btoftree -> SetBranchAddress( brname, &cnvBarPar2[i] );    
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch0_Poff%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff1_bunch0[i] );    
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch0_Poff%i", i+nBarParOff );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff2_bunch0[i] );    
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch1_Poff%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff1_bunch1[i] );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch1_Poff%i", i+nBarParOff );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff2_bunch1[i] );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch2_Poff%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff1_bunch2[i] );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch2_Poff%i", i+nBarParOff );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff2_bunch2[i] );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch3_Poff%i", i );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff1_bunch3[i] );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf( brname, "Bunch3_Poff%i", i+nBarParOff );
    btoftree -> SetBranchAddress( brname, &cnvBarParOff2_bunch3[i] );
  }
  for( unsigned int i=0; i<nBarSigma; i++ ) {
    sprintf( brname, "FLeft%i", i );
    btoftree -> SetBranchAddress( brname, &cnvFLeft[i] );    
  }
  for( unsigned int i=0; i<nBarSigma; i++ ) {
    sprintf( brname, "FRight%i", i );
    btoftree -> SetBranchAddress( brname, &cnvFRight[i] );    
  }
  for( unsigned int i=0; i<nBarSigCnt; i++ ) {
    sprintf( brname, "FCounter%i", i );
    btoftree -> SetBranchAddress( brname, &cnvFCount[i] );    
  }

  for(cnt=0; cnt<176; cnt++){
    btoftree -> GetEntry(cnt);
    bTof.setAtten( cnvBarAtten );
    bTof.setSpeed( cnvBarSpeed );
    bTof.setP1( cnvBarPar1 );
    bTof.setP2( cnvBarPar2 );
    bTof.setPoff1_bunch0( cnvBarParOff1_bunch0 );
    bTof.setPoff2_bunch0( cnvBarParOff2_bunch0 );
    bTof.setPoff1_bunch1( cnvBarParOff1_bunch1 );
    bTof.setPoff2_bunch1( cnvBarParOff2_bunch1 );
    bTof.setPoff1_bunch2( cnvBarParOff1_bunch2 );
    bTof.setPoff2_bunch2( cnvBarParOff2_bunch2 );
    bTof.setPoff1_bunch3( cnvBarParOff1_bunch3 );
    bTof.setPoff2_bunch3( cnvBarParOff2_bunch3 );
    bTof.setFPleft( cnvFLeft );
    bTof.setFPright( cnvFRight );
    bTof.setFPcounter( cnvFCount );
    tmpbTof.push_back(bTof);
  }

  //read etoftree
  double cnvEndAtten[nEndAtten];
  double cnvEndSpeed[nEndSpeed];
  double cnvEndPar[nEndPar];
  double cnvEndFPar[nEndSigma];

  TTree *etoftree = (TTree*)m_inFile -> Get("EndTofPar");

  char ecname[10];
  for( unsigned int i=0; i<nEndAtten; i++ ) {
    sprintf( ecname, "Atten%i", i );
    etoftree -> SetBranchAddress( ecname, &cnvEndAtten[i] );
  }
  for( unsigned int i=0; i<nEndSpeed; i++ ) {
    sprintf( ecname, "Speed%i", i );
    etoftree -> SetBranchAddress( ecname, &cnvEndSpeed[i] );
  }
  for( unsigned int i=0; i<nEndPar; i++ ) {
    sprintf( ecname, "P%i", i );
    etoftree -> SetBranchAddress( ecname, &cnvEndPar[i] );
  }
  for( unsigned int i=0; i<nEndSigma; i++ ) {
    sprintf( ecname, "FCounter%i", i );
    etoftree -> SetBranchAddress( ecname, &cnvEndFPar[i] );
  }

  for(cnt=0; cnt<96; cnt++){
    etoftree -> GetEntry(cnt);
    eTof.setAtten( cnvEndAtten );
    eTof.setSpeed( cnvEndSpeed );
    eTof.setP( cnvEndPar );
    eTof.setFP( cnvEndFPar );
    tmpeTof.push_back(eTof);
  }

  //read etftree
  double cnvEtfSpeed[nEtfSpeed];
  double cnvEtfPar[nEtfPar];
  double cnvEtfPar1[nEtfPar];
  double cnvEtfPar2[nEtfPar];

  if( NULL!=m_inFile->Get("EtfTofPar") ) {
    TTree *etftree = (TTree*)m_inFile -> Get("EtfTofPar");

    char etfname[10];
    for( unsigned int i=0; i<nEtfSpeed; i++ ) {
      sprintf( etfname, "Speed%i", i );
      etftree -> SetBranchAddress( etfname, &cnvEtfSpeed[i] );
    }
    for( unsigned int i=0; i<nEtfPar; i++ ) {
      sprintf( etfname, "P%i", i );
      etftree -> SetBranchAddress( etfname, &cnvEtfPar[i] );    
    }
    for( unsigned int i=0; i<nEtfPar; i++ ) {
      sprintf( etfname, "P%i", i+nEtfPar );
      etftree -> SetBranchAddress( etfname, &cnvEtfPar1[i] );    
    }
    for( unsigned int i=0; i<nEtfPar; i++ ) {
      sprintf( etfname, "P%i", i+2*nEtfPar );
      etftree -> SetBranchAddress( etfname, &cnvEtfPar2[i] );    
    }

    for(cnt=0; cnt<(72*12); cnt++){
      etftree -> GetEntry(cnt);
      etf.setSpeed( cnvEtfSpeed );
      etf.setP( cnvEtfPar );
      etf.setP1( cnvEtfPar1 );
      etf.setP2( cnvEtfPar2 );
      tmpetf.push_back(etf);
    }
  }

  //read etfbunchtree
  double cnvEtfBunchP[nEtfBunch];

  if( NULL!=m_inFile->Get("EtfTofBunch") ) {
    TTree *etfbunchtree = (TTree*)m_inFile -> Get("EtfTofBunch");

    char etfbunchname[10];
    for( unsigned int i=0; i<nEtfBunch; i++ ) {
      sprintf( etfbunchname, "pbunch%i", i );
      etfbunchtree -> SetBranchAddress( etfbunchname, &cnvEtfBunchP[i] );
    }

    int entries= etfbunchtree->GetEntries();
    for(cnt=0;cnt<entries;cnt++){
      etfbunchtree -> GetEntry(cnt);
      etfBunch.setPBunch( cnvEtfBunchP );
      tmpetfBunch.push_back( etfBunch );
    }
  }

  //read bTofCommonCalibBase
  double cnvBarSigCor[nBarSigCor];
  double cnvBarOffset[nBarOffset];
  TTree *btofcommontree = (TTree*)m_inFile -> Get("BarTofParCommon");
  for( unsigned int i=0; i<nBarSigCor; i++ ) {
    sprintf( brname, "sigmaCorr%i", i );
    btofcommontree-> SetBranchAddress( brname, &cnvBarSigCor[i] );
  }
  for( unsigned int i=0; i<nBarOffset; i++ ) {
    sprintf( brname, "t0offset%i", i );
    btofcommontree-> SetBranchAddress( brname, &cnvBarOffset[i]);
  }

  int entries = btofcommontree->GetEntries();
  for(cnt=0;cnt<entries;cnt++){
    btofcommontree->GetEntry(cnt);
    bTofCommon.setSigmaCorr( cnvBarSigCor );
    bTofCommon.setOffset( cnvBarOffset );
    tmpbTofCommon.push_back(bTofCommon);   
  }
  
  int m_run1, m_run2, m_version;
  int m_qCorr, m_qElec, m_misLable;
  int m_tofidEast[5], m_tofidWest[5], m_tofidEndcap[5];

  TTree *CalibInfo = (TTree*)m_inFile -> Get("CalibInfo");
  CalibInfo->SetBranchAddress("Run1",    &m_run1           );
  CalibInfo->SetBranchAddress("Run2",    &m_run2           );
  CalibInfo->SetBranchAddress("Version", &m_version        );
  CalibInfo->SetBranchAddress("ebrId0",  &m_tofidEast[0]   );
  CalibInfo->SetBranchAddress("ebrId1",  &m_tofidEast[1]   );
  CalibInfo->SetBranchAddress("ebrId2",  &m_tofidEast[2]   );
  CalibInfo->SetBranchAddress("ebrId3",  &m_tofidEast[3]   );
  CalibInfo->SetBranchAddress("ebrId4",  &m_tofidEast[4]   );
  CalibInfo->SetBranchAddress("ecId0",   &m_tofidEndcap[0] );
  CalibInfo->SetBranchAddress("ecId1",   &m_tofidEndcap[1] );
  CalibInfo->SetBranchAddress("ecId2",   &m_tofidEndcap[2] );
  CalibInfo->SetBranchAddress("ecId3",   &m_tofidEndcap[3] );
  CalibInfo->SetBranchAddress("ecId4",   &m_tofidEndcap[4] );
  CalibInfo->SetBranchAddress("wbrId0",  &m_tofidWest[0]   );
  CalibInfo->SetBranchAddress("wbrId1",  &m_tofidWest[1]   );
  CalibInfo->SetBranchAddress("wbrId2",  &m_tofidWest[2]   );
  CalibInfo->SetBranchAddress("wbrId3",  &m_tofidWest[3]   );
  CalibInfo->SetBranchAddress("wbrId4",  &m_tofidWest[4]   );
  CalibInfo->SetBranchAddress("misLable",&m_misLable       );
  CalibInfo->SetBranchAddress("qCorr",   &m_qCorr          );
  CalibInfo->SetBranchAddress("qElec",   &m_qElec          );

  entries= CalibInfo->GetEntries();
  for(cnt=0;cnt<entries;cnt++){
    CalibInfo->GetEntry(cnt);
    tofinfo.setRunBegin(m_run1);
    tofinfo.setRunEnd(m_run2);
    tofinfo.setVersion(m_version);
    tofinfo.setQCorr(m_qCorr);
    tofinfo.setQElec(m_qElec);
    tofinfo.setMisLable(m_misLable);
    tofinfo.setBrEast(m_tofidEast);      
    tofinfo.setBrWest(m_tofidWest);
    tofinfo.setEndcap(m_tofidEndcap); 
    tofinfoCol.push_back(tofinfo);  
  }

  CalibData::TofCalibData *tmpObject = new CalibData::TofCalibData(&tmpbTof,&tmpbTofCommon,&tmpeTof,&tmpetf,&tmpetfBunch,&tofinfoCol);
  
  refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

StatusCode RootTofCalibDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
  MsgStream log(msgSvc(), "RootTofCalibDataCnv");

  // Open the file, create the branch
  StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
  int j;
  CalibData::TofCalibData* btof = dynamic_cast<CalibData::TofCalibData*>(pTDSObj);

  // write  btoftree----------------------------------------------------------------
  double cnvBarAtten[nBarAtten];
  double cnvBarSpeed[nBarSpeed];
  double cnvBarPar1[nBarPar];
  double cnvBarPar2[nBarPar];
  double cnvBarParOff1_bunch0[nBarParOff];
  double cnvBarParOff2_bunch0[nBarParOff];
  double cnvBarParOff1_bunch1[nBarParOff];
  double cnvBarParOff2_bunch1[nBarParOff];
  double cnvBarParOff1_bunch2[nBarParOff];
  double cnvBarParOff2_bunch2[nBarParOff];
  double cnvBarParOff1_bunch3[nBarParOff];
  double cnvBarParOff2_bunch3[nBarParOff];
  double cnvFLeft[nBarSigma];
  double cnvFRight[nBarSigma];
  double cnvFCount[nBarSigCnt];

  char brname[8], ibrname[8];
  TTree *btoftree = new TTree("BarTofPar", "BarTofPar");
  for( unsigned int i=0; i<nBarAtten; i++ ) {
    sprintf(  brname, "Atten%i",   i );
    sprintf( ibrname, "Atten%i/D", i );
    btoftree -> Branch( brname, &cnvBarAtten[i], ibrname );
  }
  for( unsigned int i=0; i<nBarSpeed; i++ ) {
    sprintf(  brname, "Speed%i",   i );
    sprintf( ibrname, "Speed%i/D", i );
    btoftree -> Branch( brname, &cnvBarSpeed[i], ibrname );
  }
  for( unsigned int i=0; i<nBarPar; i++ ) {
    sprintf(  brname, "P%i",   i );
    sprintf( ibrname, "P%i/D", i );
    btoftree -> Branch( brname, &cnvBarPar1[i], ibrname );
  }
  for( unsigned int i=0; i<nBarPar; i++ ) {
    sprintf(  brname, "P%i",   i+nBarPar );
    sprintf( ibrname, "P%i/D", i+nBarPar );
    btoftree -> Branch( brname, &cnvBarPar2[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch0_Poff%i",   i );
    sprintf( ibrname, "Bunch0_Poff%i/D", i );
    btoftree -> Branch( brname, &cnvBarParOff1_bunch0[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch0_Poff%i",   i+nBarParOff );
    sprintf( ibrname, "Bunch0_Poff%i/D", i+nBarParOff );
    btoftree -> Branch( brname, &cnvBarParOff2_bunch0[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch1_Poff%i",   i );
    sprintf( ibrname, "Bunch1_Poff%i/D", i );
    btoftree -> Branch( brname, &cnvBarParOff1_bunch1[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch1_Poff%i",   i+nBarParOff );
    sprintf( ibrname, "Bunch1_Poff%i/D", i+nBarParOff );
    btoftree -> Branch( brname, &cnvBarParOff2_bunch1[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch2_Poff%i",   i );
    sprintf( ibrname, "Bunch2_Poff%i/D", i );
    btoftree -> Branch( brname, &cnvBarParOff1_bunch2[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch2_Poff%i",   i+nBarParOff );
    sprintf( ibrname, "Bunch2_Poff%i/D", i+nBarParOff );
    btoftree -> Branch( brname, &cnvBarParOff2_bunch2[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch3_Poff%i",   i );
    sprintf( ibrname, "Bunch3_Poff%i/D", i );
    btoftree -> Branch( brname, &cnvBarParOff1_bunch3[i], ibrname );
  }
  for( unsigned int i=0; i<nBarParOff; i++ ) {
    sprintf(  brname, "Bunch3_Poff%i",   i+nBarParOff );
    sprintf( ibrname, "Bunch3_Poff%i/D", i+nBarParOff );
    btoftree -> Branch( brname, &cnvBarParOff2_bunch3[i], ibrname );
  }
  for( unsigned int i=0; i<nBarSigma; i++ ) {
    sprintf(  brname, "FLeft%i",   i );
    sprintf( ibrname, "FLeft%i/D", i );
    btoftree -> Branch( brname, &cnvFLeft[i], ibrname );
  }
  for( unsigned int i=0; i<nBarSigma; i++ ) {
    sprintf(  brname, "FRight%i",   i );
    sprintf( ibrname, "FRight%i/D", i );
    btoftree -> Branch( brname, &cnvFRight[i], ibrname );
  }
  for( unsigned int i=0; i<nBarSigCnt; i++ ) {
    sprintf(  brname, "FCounter%i",   i );
    sprintf( ibrname, "FCounter%i/D", i );
    btoftree -> Branch( brname, &cnvFCount[i], ibrname );
  }

  for( int i=0; i<176; i++ ) {
    for(j=0;j<static_cast<int>(nBarAtten);j++) {
      cnvBarAtten[j] = btof->getBTofAtten(i,j);
    }
    for(j=0;j<static_cast<int>(nBarSpeed);j++) {
      cnvBarSpeed[j] = btof->getBTofSpeed(i,j);
    }
    for(j=0;j<static_cast<int>(nBarPar);j++){
      cnvBarPar1[j] = btof->getBTofPleft(i,j);
      cnvBarPar2[j] = btof->getBTofPright(i,j);
    }
    for(j=0;j<static_cast<int>(nBarParOff);j++){
      cnvBarParOff1_bunch0[j] = btof->getBTofPoffleft_bunch0(i,j);
      cnvBarParOff2_bunch0[j] = btof->getBTofPoffright_bunch0(i,j);
    }
    for(j=0;j<static_cast<int>(nBarParOff);j++){
      cnvBarParOff1_bunch1[j] = btof->getBTofPoffleft_bunch1(i,j);
      cnvBarParOff2_bunch1[j] = btof->getBTofPoffright_bunch1(i,j);
    }
    for(j=0;j<static_cast<int>(nBarParOff);j++){
      cnvBarParOff1_bunch2[j] = btof->getBTofPoffleft_bunch2(i,j);
      cnvBarParOff2_bunch2[j] = btof->getBTofPoffright_bunch2(i,j);
    }
    for(j=0;j<static_cast<int>(nBarParOff);j++){
      cnvBarParOff1_bunch3[j] = btof->getBTofPoffleft_bunch3(i,j);
      cnvBarParOff2_bunch3[j] = btof->getBTofPoffright_bunch3(i,j);
    }
    for(j=0;j<static_cast<int>(nBarSigma);j++){
      cnvFLeft[j]  = btof->getBTofFleft(i,j);
      cnvFRight[j] = btof->getBTofFright(i,j);
    }
    for(j=0;j<static_cast<int>(nBarSigCnt);j++){
      cnvFCount[j] = btof->getBTofFcounter(i,j);
    }
    btoftree -> Fill();
  }
     
 //write  etoftree----------------------------------------------------------------
  double cnvEndAtten[nEndAtten];
  double cnvEndSpeed[nEndSpeed];
  double cnvEndPar[nEndPar];
  double cnvEndFPar[nEndSigma];

  char ecname[8], iecname[8];
  TTree *etoftree = new TTree("EndTofPar", "EndTofPar");
  for( unsigned int i=0; i<nEndAtten; i++ ) {
    sprintf(  ecname, "Atten%i",   i );
    sprintf( iecname, "Atten%i/D", i );
    etoftree -> Branch( ecname, &cnvEndAtten[i], iecname );
  }
  for( unsigned int i=0; i<nEndSpeed; i++ ) {
    sprintf(  ecname, "Speed%i",   i );
    sprintf( iecname, "Speed%i/D", i );
    etoftree -> Branch( ecname, &cnvEndSpeed[i], iecname );
  }
  for( unsigned int i=0; i<nEndPar; i++ ) {
    sprintf(  ecname, "P%i",   i );
    sprintf( iecname, "P%i/D", i );
    etoftree -> Branch( ecname, &cnvEndPar[i], iecname );
  }
  for( unsigned int i=0; i<nEndSigma; i++ ) {
    sprintf(  ecname, "FCounter%i",   i );
    sprintf( iecname, "FCounter%i/D", i );
    etoftree -> Branch( ecname, &cnvEndFPar[i], iecname );
  }
    
  for(int i=0; i<96; i++){
    for(j=0;j<static_cast<int>(nEndAtten);j++) {
      cnvEndAtten[j] = btof->getETofAtten(i,j);
    }
    for(j=0;j<static_cast<int>(nEndSpeed);j++) {
      cnvEndSpeed[j] = btof->getETofSpeed(i,j);
    }
    for(j=0;j<static_cast<int>(nEndPar);j++){
      cnvEndPar[j] = btof->getETofP(i,j);
    }
    for(j=0;j<static_cast<int>(nEndSigma);j++){
      cnvEndFPar[j] = btof->getETofFP(i,j);
    }
    etoftree -> Fill();
  }

 //write  etftree----------------------------------------------------------------
  double cnvEtfSpeed[nEtfSpeed];
  double cnvEtfPar[nEtfPar];
  double cnvEtfPar1[nEtfPar];
  double cnvEtfPar2[nEtfPar];

  char etfname[8], ietfname[8];
  TTree *etftree = new TTree("EtfTofPar", "EtfTofPar");
  for( unsigned int i=0; i<nEtfSpeed; i++ ) {
    sprintf(  etfname, "Speed%i",   i );
    sprintf( ietfname, "Speed%i/D", i );
    etftree -> Branch( etfname, &cnvEtfSpeed[i], ietfname );
  }
  for( unsigned int i=0; i<nEtfPar; i++ ) {
    sprintf(  etfname, "P%i",   i );
    sprintf( ietfname, "P%i/D", i );
    etftree -> Branch( etfname, &cnvEtfPar[i], ietfname );
  }
  for( unsigned int i=0; i<nEtfPar; i++ ) {
    sprintf(  etfname, "P%i",   i+nEtfPar );
    sprintf( ietfname, "P%i/D", i+nEtfPar );
    etftree -> Branch( etfname, &cnvEtfPar1[i], ietfname );
  }
  for( unsigned int i=0; i<nEtfPar; i++ ) {
    sprintf(  etfname, "P%i",   i+2*nEtfPar );
    sprintf( ietfname, "P%i/D", i+2*nEtfPar );
    etftree -> Branch( etfname, &cnvEtfPar2[i], ietfname );
  }

  for( int i=0; i<72; i++ ) {
    for( int k=0; k<12; k++ ) {
      for(j=0;j<static_cast<int>(nEtfSpeed);j++) {
	cnvEtfSpeed[j] = btof->getEtfSpeed(i,k,j);
      }
      for(j=0;j<static_cast<int>(nEtfPar);j++){
	cnvEtfPar[j] = btof->getEtfPcombine(i,k,j);
	cnvEtfPar1[j] = btof->getEtfPleft(i,k,j);
	cnvEtfPar2[j] = btof->getEtfPright(i,k,j);
      }
      etftree -> Fill();
    }
  }

 //write  etftree----------------------------------------------------------------
  double cnvEtfBunchP[nEtfBunch];

  char etfbunchname[8], ietfbunchname[8];
  TTree *etfbunchtree = new TTree("EtfTofBunch", "EtfTofBunch");
  for( unsigned int i=0; i<nEtfBunch; i++ ) {
    sprintf(  etfbunchname, "pbunch%i",   i );
    sprintf( ietfbunchname, "pbunch%i/D", i );
    etfbunchtree -> Branch( etfbunchname, &cnvEtfBunchP[i], ietfbunchname );
  }

  for(j=0;j<static_cast<int>(nEtfBunch);j++) {
    cnvEtfBunchP[j] = btof->getEtfPBunch(j);
  }
  etfbunchtree -> Fill();


  //  write all the trees
  btoftree -> Write();
  etoftree -> Write();
  etftree -> Write();
  etfbunchtree -> Write();
  delete btoftree;
  delete etoftree;
  delete etftree;
  delete etfbunchtree;
  closeWrite();
  log<<MSG::INFO<<"successfully create RootFile"<<endreq;
  
  return sc;
}
