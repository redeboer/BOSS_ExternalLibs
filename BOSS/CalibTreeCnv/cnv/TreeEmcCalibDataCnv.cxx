#include "GaudiKernel/MsgStream.h"
#include "TreeEmcCalibDataCnv.h"
#include "CalibData/Emc/EmcCalibData.h"
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
//static CnvFactory<TreeEmcCalibDataCnv> EmcCal_factory;
//const  ICnvFactory& TreeEmcCalibDataCnvFactory = EmcCal_factory;



TreeEmcCalibDataCnv::TreeEmcCalibDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Calib_EmcCal) { 

	}


const CLID& TreeEmcCalibDataCnv::objType() const {
	return CLID_Calib_EmcCal;
}

const CLID& TreeEmcCalibDataCnv::classID() {
	return CLID_Calib_EmcCal;
}

StatusCode TreeEmcCalibDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeEmcCalibDataCnv");
	log<<MSG::DEBUG<<"SetProperty"<<endreq;
	//   gROOT->Reset();
	//   gDirectory = gROOT;
	//TFile("temp.root");
	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
	DatabaseRecord *records=add->pp();
	TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
	buf1->SetBuffer((*records)["DigiCalibConst"],512000,kFALSE);

	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
	std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;

	TTree* Digitree= new TTree();

	Digitree->Streamer(*buf1);

	Digitree->Print();

	CalibData::EmcCalibData *tmpObject = new CalibData::EmcCalibData ;
	// Read in our object
	int i;
	int nentries;

	// read DigiCalibConst ------------------------------------------------------------
	double emcdata;
	//  Digitree -> SetBranchAddress("DigiCalibConst", &emcdata);
	//  Digitree -> SetBranchAddress("DigiCalibConst", &emcdata);
	//  nentries = Digitree-> GetEntries();
	//  std::cout<<"nentries=="<<nentries<<std::endl;


	double EmcCalibConst[6240];
	int EmcIxtalNumber[6240];
	vector<double> DigiCalibConst;
	vector<int> IxtalNumber;
	Digitree->SetBranchAddress("DigiCalibConst", EmcCalibConst);
	Digitree -> SetBranchAddress("IxtalNumber", EmcIxtalNumber);
	Digitree -> GetEntry(0);
	for(i=0; i<6240; i++){
		DigiCalibConst.push_back(EmcCalibConst[i]);
		IxtalNumber.push_back(EmcIxtalNumber[i]); 
	}
	tmpObject -> setDigiCalibConst(&DigiCalibConst);
	tmpObject -> setIxtalNumber(&IxtalNumber);
	refpObject=tmpObject;
	//  temp->Close();
	delete Digitree;
	return StatusCode::SUCCESS;


}

