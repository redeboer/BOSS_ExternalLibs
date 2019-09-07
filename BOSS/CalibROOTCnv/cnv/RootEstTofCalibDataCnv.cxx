// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootEstTofCalibDataCnv.cxx,v 1.8 2016/02/15 08:13:48 sunss Exp $
#include "GaudiKernel/MsgStream.h"
#include "RootEstTofCalibDataCnv.h"
#include "CalibData/Tof/TofCalibData.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/Tof/eTofCalibBase.h"
#include "CalibData/Tof/etfCalibBase.h"
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
//static CnvFactory<RootEstTofCalibDataCnv> TofCalib_factory;
//const  ICnvFactory& RootEstTofCalibDataCnvFactory = TofCalib_factory;

const unsigned int nBarPar    = 10;
const unsigned int nBarParOff = 20;
const unsigned int nEndPar    = 8;
const unsigned int nEtfPar    = 20;
const unsigned int nBarOffset = 2;

RootEstTofCalibDataCnv::RootEstTofCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_EstTofCal) { 
 }


const CLID& RootEstTofCalibDataCnv::objType() const {
  return CLID_Calib_EstTofCal;
}

const CLID& RootEstTofCalibDataCnv::classID() {
  return CLID_Calib_EstTofCal;
}

StatusCode RootEstTofCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootEstTofCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
 // open the file
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
 
  CalibData::bTofCalibBase bTof;
  CalibData::eTofCalibBase eTof;
  CalibData::etfCalibBase  etf;
  CalibData::bTofCommonCalibBase bTofCommon;
  CalibData::tofCalibInfoBase tofinfo;

  std::vector<CalibData::bTofCalibBase> tmpbTof;
  std::vector<CalibData::eTofCalibBase> tmpeTof;
  std::vector<CalibData::etfCalibBase>  tmpetf;
  std::vector<CalibData::bTofCommonCalibBase> tmpbTofCommon;
  std::vector<CalibData::tofCalibInfoBase> tofinfoCol;

  // Read in the object
  int cnt;
  // read btoftree ------------------------------------------------------------
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

  TTree *btoftree = (TTree*)m_inFile -> Get("BarTofPar");

  char brname[10];
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

  for(cnt=0; cnt<176; cnt++){
    btoftree -> GetEntry(cnt);
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
    tmpbTof.push_back( bTof );
  }

  //read etoftree
  double cnvEndPar[nEndPar];

  TTree *etoftree = (TTree*)m_inFile -> Get("EndTofPar");

  char ecname[10];
  for( unsigned int i=0; i<nEndPar; i++ ) {
    sprintf( ecname, "P%i", i );
    etoftree -> SetBranchAddress( ecname, &cnvEndPar[i] );
  }
   
  for(cnt=0; cnt<96; cnt++){
    etoftree->GetEntry(cnt);
    eTof.setP( cnvEndPar );
    tmpeTof.push_back(eTof);
  }

  //read etftree
  double cnvEtfPar[nEtfPar];
  double cnvEtfPar1[nEtfPar];
  double cnvEtfPar2[nEtfPar];

  if( NULL!=m_inFile->Get("EtfTofPar") ) {
    TTree *etftree = (TTree*)m_inFile -> Get("EtfTofPar");

    char etfname[10];
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
      etf.setP( cnvEtfPar );
      etf.setP1( cnvEtfPar1 );
      etf.setP2( cnvEtfPar2 );
      tmpetf.push_back(etf);
    }
  }

  //read bTofCommonCalibBase
  double cnvBarOffset[nBarOffset];
  TTree *btofcommontree = (TTree*)m_inFile -> Get("BarTofParCommon");
  for( unsigned int i=0; i<nBarOffset; i++ ) {
    sprintf( brname, "t0offset%i", i );
    btofcommontree-> SetBranchAddress( brname, &cnvBarOffset[i]);
  }

  int entries = btofcommontree->GetEntries();
  for(cnt=0;cnt<entries;cnt++){
    btofcommontree->GetEntry(cnt);
    bTofCommon.setOffset( cnvBarOffset );
    tmpbTofCommon.push_back(bTofCommon);   
  }

  CalibData::TofCalibData *tmpObject = new CalibData::TofCalibData(&tmpbTof,&tmpbTofCommon,&tmpeTof,&tmpetf,&tofinfoCol);

  refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

StatusCode RootEstTofCalibDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
  MsgStream log(msgSvc(), "RootEstTofCalibDataCnv");

  // Open the file, create the branch
  StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
  CalibData::TofCalibData* btof = dynamic_cast<CalibData::TofCalibData*>(pTDSObj);

  // write  btoftree----------------------------------------------------------------
  double cnvBarPar1[nBarPar];
  double cnvBarPar2[nBarPar];

  char brname[8], ibrname[8];
  TTree *btoftree = new TTree("BarTofPar", "BarTofPar");
  for( unsigned int i=0; i<nBarPar; i++ ) {
    sprintf(  brname, "P%i",   i );
    sprintf( ibrname, "P%i/D", i );
    btoftree -> Branch( brname, &cnvBarPar1[i], ibrname );
  }

  for( int i=0; i<176; i++ ) {
    for( int j=0;j<static_cast<int>(nBarPar);j++){
      cnvBarPar1[j] = btof->getBTofPleft(i,j);
      cnvBarPar2[j] = btof->getBTofPright(i,j);
    }
    btoftree -> Fill();
  }
     
 //write  etoftree----------------------------------------------------------------
  double cnvEndPar[nEndPar];

  char ecname[8], iecname[8];
  TTree *etoftree = new TTree("EndTofPar", "EndTofPar");
  for( unsigned int i=0; i<nEndPar; i++ ) {
    sprintf(  ecname, "P%i",   i );
    sprintf( iecname, "P%i/D", i );
    etoftree -> Branch( ecname, &cnvEndPar[i], iecname );
  }

  for(int i=0; i<96; i++){
    for(int j=0;j<static_cast<int>(nEndPar);j++){
      cnvEndPar[j] = btof->getETofP(i,j);
    }
    etoftree->Fill();
  }

 //write  etftree----------------------------------------------------------------
  double cnvEtfPar[nEtfPar];
  double cnvEtfPar1[nEtfPar];
  double cnvEtfPar2[nEtfPar];

  char etfname[8], ietfname[8];
  TTree *etftree = new TTree("EtfTofPar", "EtfTofPar");
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
      for(int j=0;j<static_cast<int>(nEtfPar);j++){
	cnvEtfPar[j] = btof->getEtfPcombine(i,k,j);
	cnvEtfPar1[j] = btof->getEtfPleft(i,k,j);
	cnvEtfPar2[j] = btof->getEtfPright(i,k,j);
      }
      etftree -> Fill();
    }
  }

  //  write all the trees
  btoftree -> Write();
  etoftree -> Write();
  etftree -> Write();
  delete btoftree;
  delete etoftree;
  delete etftree;
  closeWrite();
  log<<MSG::INFO<<"successfully create RootFile"<<endreq;
  
  return sc;
}
