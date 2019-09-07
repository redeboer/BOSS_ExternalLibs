#include "GaudiKernel/MsgStream.h"
#include "TreeEstTofCalibDataCnv.h"
#include "CalibData/Tof/TofCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "CalibMySQLCnv/TreeAddress.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TBuffer.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibDataSvc/ICalibTreeSvc.h"    //maybe
#include "CalibDataSvc/ICalibMetaCnvSvc.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;
//static CnvFactory<TreeEstTofCalibDataCnv> TofCal_factory;
//const  ICnvFactory& TreeEstTofCalibDataCnvFactory = TofCal_factory;

const unsigned int nBarPar    = 10;
const unsigned int nBarParOff = 20;
const unsigned int nEndPar    = 8;
const unsigned int nEtfPar    = 20;
const unsigned int nBarOffset = 2;

TreeEstTofCalibDataCnv::TreeEstTofCalibDataCnv( ISvcLocator* svc) :
  TreeCalBaseCnv(svc, CLID_Calib_EstTofCal) { 
      
}


const CLID& TreeEstTofCalibDataCnv::objType() const {
  return CLID_Calib_EstTofCal;
}

const CLID& TreeEstTofCalibDataCnv::classID() {
  return CLID_Calib_EstTofCal;
}

StatusCode TreeEstTofCalibDataCnv::i_createObj(IOpaqueAddress* addr,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "TreeEstTofCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
  
  TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
  DatabaseRecord *records=add->pp();

  TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf3 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf4 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf5 = 0;

  buf1->SetBuffer((*records)["BarTofPar"],512000,kFALSE);
  buf2->SetBuffer((*records)["EndTofPar"],512000,kFALSE);
  buf3->SetBuffer((*records)["TofConPar"],512000,kFALSE);
  buf4->SetBuffer((*records)["BarTof"],512000,kFALSE);

  if( (*records)["EtfTofPar"]!=0 ) {
    buf5 = new TBufferFile(TBuffer::kRead);
    buf5->SetBuffer((*records)["EtfTofPar"],512000,kFALSE);
  }

  std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
  std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;

  TTree *btoftree       = new TTree();
  TTree *etoftree       = new TTree();
  TTree *etftree        = new TTree();
  TTree *btofcommontree = new TTree();
  TTree *CalibInfo      = new TTree();

  btoftree->Streamer(*buf1);
  etoftree->Streamer(*buf2);
  btofcommontree->Streamer(*buf3);
  CalibInfo->Streamer(*buf4);
  if( buf5 ) {
    etftree->Streamer(*buf5);
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
    btoftree -> GetEntry( cnt );
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
    tmpbTof.push_back(bTof);
  }


  //read etoftree
  double cnvEndPar[nEndPar];

  char ecname[10];
  for( unsigned int i=0; i<nEndPar; i++ ) {
    sprintf( ecname, "P%i", i );
    etoftree -> SetBranchAddress( ecname, &cnvEndPar[i] );
  }

  for( cnt=0; cnt<96; cnt++ ) {
    etoftree -> GetEntry( cnt );
    eTof.setP( cnvEndPar );
    tmpeTof.push_back( eTof );
  }


  //read etftree
  if( etftree->GetEntries() != 0 ) {

    double cnvEtfPar[nEtfPar];
    double cnvEtfPar1[nEtfPar];
    double cnvEtfPar2[nEtfPar];

    char etfname[10];
    for( unsigned int i=0; i<nEtfPar; i++ ) {
      sprintf( etfname, "P%i", i );
      etftree -> SetBranchAddress( etfname, &cnvEtfPar[i] );
      sprintf( etfname, "P%i", nEtfPar+i );
      etftree -> SetBranchAddress( etfname, &cnvEtfPar1[i] );
      sprintf( etfname, "P%i", 2*nEtfPar+i );
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
  delete btoftree;
  delete etoftree;
  delete etftree;
  delete btofcommontree;
  delete CalibInfo;

  return StatusCode::SUCCESS;
}

