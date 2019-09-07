#include "GaudiKernel/MsgStream.h"
#include "TreeTofCalibDataCnv.h"
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
//static CnvFactory<TreeTofCalibDataCnv> TofCal_factory;
//const  ICnvFactory& TreeTofCalibDataCnvFactory = TofCal_factory;

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

TreeTofCalibDataCnv::TreeTofCalibDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Calib_TofCal) { 

	}


const CLID& TreeTofCalibDataCnv::objType() const {
	return CLID_Calib_TofCal;
}

const CLID& TreeTofCalibDataCnv::classID() {
	return CLID_Calib_TofCal;
}

StatusCode TreeTofCalibDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeTofCalibDataCnv");
	log<<MSG::DEBUG<<"SetProperty"<<endreq;

	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);

	DatabaseRecord *records=add->pp();

	TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf3 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf4 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf5 = 0;
	TBufferFile *buf6 = 0;

	buf1->SetBuffer((*records)["BarTofPar"],512000,kFALSE);
	buf2->SetBuffer((*records)["EndTofPar"],512000,kFALSE);
	buf3->SetBuffer((*records)["TofConPar"],512000,kFALSE);
	buf4->SetBuffer((*records)["BarTof"],   512000,kFALSE);
	if( (*records)["EtfTofPar"]!=0 ) {
	  buf5 =  new TBufferFile(TBuffer::kRead);
	  buf5->SetBuffer((*records)["EtfTofPar"],512000,kFALSE);
	}
	if( (*records)["EtfTofBunch"]!=0 ) {
	  buf6 =  new TBufferFile(TBuffer::kRead);
	  buf6->SetBuffer((*records)["EtfTofBunch"],512000,kFALSE);
	}

	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
	std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;

	TTree *btoftree       = new TTree();
	TTree *etoftree       = new TTree();
	TTree *etftree        = new TTree();
	TTree *etfbunchtree   = new TTree();
	TTree *btofcommontree = new TTree();
	TTree *CalibInfo      = new TTree();

	btoftree->Streamer(*buf1);
	etoftree->Streamer(*buf2);
	btofcommontree->Streamer(*buf3);
	CalibInfo->Streamer(*buf4);
	if( buf5 ) {
	  etftree->Streamer(*buf5);
	}
	if( buf6 ) {
	  etfbunchtree->Streamer(*buf6);
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
	if( etftree->GetEntries() != 0 ) {

	  double cnvEtfSpeed[nEtfSpeed];
	  double cnvEtfPar[nEtfPar];
	  double cnvEtfPar1[nEtfPar];
	  double cnvEtfPar2[nEtfPar];

	  char etfname[10];
	  for( unsigned int i=0; i<nEtfSpeed; i++ ) {
	    sprintf( etfname, "Speed%i", i );
	    etftree -> SetBranchAddress( etfname, &cnvEtfSpeed[i] );
	  }
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
	    etf.setSpeed( cnvEtfSpeed );
	    etf.setP( cnvEtfPar );
	    etf.setP1( cnvEtfPar1 );
	    etf.setP2( cnvEtfPar2 );
	    tmpetf.push_back(etf);
	  }

	}


	//read etftree
	if( etfbunchtree->GetEntries() != 0 ) {
	  double cnvEtfBunchP[nEtfBunch];
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

	CalibInfo->SetBranchAddress("Run1",&m_run1);
	CalibInfo->SetBranchAddress("Run2",&m_run2);
	CalibInfo->SetBranchAddress("Version",&m_version);
	CalibInfo->SetBranchAddress("ebrId0",&m_tofidEast[0]);
	CalibInfo->SetBranchAddress("ebrId1",&m_tofidEast[1]);
	CalibInfo->SetBranchAddress("ebrId2",&m_tofidEast[2]);
	CalibInfo->SetBranchAddress("ebrId3",&m_tofidEast[3]);
	CalibInfo->SetBranchAddress("ebrId4",&m_tofidEast[4]);
	CalibInfo->SetBranchAddress("ecId0",&m_tofidEndcap[0]);
	CalibInfo->SetBranchAddress("ecId1",&m_tofidEndcap[1]);
	CalibInfo->SetBranchAddress("ecId2",&m_tofidEndcap[2]);
	CalibInfo->SetBranchAddress("ecId3",&m_tofidEndcap[3]);
	CalibInfo->SetBranchAddress("ecId4",&m_tofidEndcap[4]);
	CalibInfo->SetBranchAddress("wbrId0",&m_tofidWest[0]);
	CalibInfo->SetBranchAddress("wbrId1",&m_tofidWest[1]);
	CalibInfo->SetBranchAddress("wbrId2",&m_tofidWest[2]);
	CalibInfo->SetBranchAddress("wbrId3",&m_tofidWest[3]);
	CalibInfo->SetBranchAddress("wbrId4",&m_tofidWest[4]);
	CalibInfo->SetBranchAddress("misLable",&m_misLable);
	CalibInfo->SetBranchAddress("qCorr",&m_qCorr);
	CalibInfo->SetBranchAddress("qElec",&m_qElec);

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
	delete btoftree;
	delete etoftree;
	delete etftree;
	delete etfbunchtree;
	delete btofcommontree;
	delete CalibInfo;

	return StatusCode::SUCCESS;
}

