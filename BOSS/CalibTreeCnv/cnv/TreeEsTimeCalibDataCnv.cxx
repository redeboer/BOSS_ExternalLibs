#include "GaudiKernel/MsgStream.h"
#include "TreeEsTimeCalibDataCnv.h"
#include "CalibData/EsTime/EsTimeCalibData.h"
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
//static CnvFactory<TreeEsTimeCalibDataCnv> EsTimeCal_factory;
//const  ICnvFactory& TreeEsTimeCalibDataCnvFactory = EsTimeCal_factory;



TreeEsTimeCalibDataCnv::TreeEsTimeCalibDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Calib_EsTimeCal) { 

	}


const CLID& TreeEsTimeCalibDataCnv::objType() const {
	return CLID_Calib_EsTimeCal;
}

const CLID& TreeEsTimeCalibDataCnv::classID() {
	return CLID_Calib_EsTimeCal;
}

StatusCode TreeEsTimeCalibDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeEsTimeCalibDataCnv");
	log<<MSG::DEBUG<<"SetProperty"<<endreq;
	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
	DatabaseRecord *records=add->pp();  

	TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
	buf1->SetBuffer((*records)["EsTimeH2"],512000,kFALSE);
	buf2->SetBuffer((*records)["EsTimeH9"],512000,kFALSE);

	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
	std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;

	TTree* Testtree= new TTree();

	Testtree->Streamer(*buf1);
	TTree* tree2= new TTree();
	tree2->Streamer(*buf2);

	CalibData::EsTimeCalibData *tmpObject = new CalibData::EsTimeCalibData ;
	// Read in our object
	int i;

	// read TestCalibConst ------------------------------------------------------------
	double EsTimeCalibConst;
	vector<double> TestCalibConst;
	Testtree -> SetBranchAddress("meant0", &EsTimeCalibConst);
	int no= Testtree -> GetEntries();
	for(i=0; i<no; i++){
		Testtree->GetEntry(i);
		TestCalibConst.push_back(EsTimeCalibConst);
	}
	tmpObject -> setTestCalibConst(&TestCalibConst);

	double t0offsete,t0offsetb;
	int bunchtime;
	tree2->SetBranchAddress("t0offsete", &t0offsete);
	tree2->SetBranchAddress("t0offsetb", &t0offsetb);
	tree2->SetBranchAddress("bunchtime", &bunchtime);
	tree2->GetEntry(0);
	tmpObject ->setToffsetb(t0offsetb);
	tmpObject ->setToffsete(t0offsete);
	tmpObject ->setBunchTime(bunchtime);
	refpObject=tmpObject;

	delete Testtree;
	delete tree2;
	return StatusCode::SUCCESS;


}

