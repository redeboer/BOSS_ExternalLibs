#include "GaudiKernel/MsgStream.h"
#include "TreeTofSimCnv.h"
#include "CalibData/Tof/TofSimData.h"
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
//static CnvFactory<TreeTofSimDataCnv> TofCal_factory;
//const  ICnvFactory& TreeTofSimDataCnvFactory = TofCal_factory;



TreeTofSimDataCnv::TreeTofSimDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Calib_TofSim) { 

	}


const CLID& TreeTofSimDataCnv::objType() const {
	return CLID_Calib_TofSim;
}

const CLID& TreeTofSimDataCnv::classID() {
	return CLID_Calib_TofSim;
}

StatusCode TreeTofSimDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeTofSimDataCnv");
	log<<MSG::DEBUG<<"SetProperty"<<endreq;

	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);

	DatabaseRecord *records=add->pp();

	TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf3 = new TBufferFile(TBuffer::kRead);

	buf1->SetBuffer((*records)["BTofSim"],512000,kFALSE);
	buf2->SetBuffer((*records)["ETofSim"],512000,kFALSE);
	buf3->SetBuffer((*records)["SimConstants"],512000,kFALSE);


	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["TofSimParVer"];
	std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;



	TTree *btoftree= new TTree();
	TTree *etoftree= new TTree();
	TTree *btofcommontree = new TTree();
	btoftree->Streamer(*buf1);
	etoftree->Streamer(*buf2);
	btofcommontree->Streamer(*buf3);

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
	delete btoftree;
	delete etoftree;
	delete btofcommontree;

	return StatusCode::SUCCESS;


}

