#include "GaudiKernel/MsgStream.h"
#include "TreeDedxSimDataCnv.h"
#include "CalibData/Dedx/DedxSimData.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "CalibMySQLCnv/TreeAddress.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TBuffer.h"
#include  "TBufferFile.h"
#include  "TROOT.h"

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
//static CnvFactory<TreeDedxSimDataCnv> DedxCal_factory;
//const  ICnvFactory& TreeDedxSimDataCnvFactory = DedxCal_factory;



TreeDedxSimDataCnv::TreeDedxSimDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Dedx_Sim) { 

	}


const CLID& TreeDedxSimDataCnv::objType() const {
	return CLID_Dedx_Sim;
}

const CLID& TreeDedxSimDataCnv::classID() {
	return CLID_Dedx_Sim;
}

StatusCode TreeDedxSimDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeDedxSimDataCnv");

	CalibData::DedxSimData *tmpObject = new CalibData::DedxSimData;
	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
	DatabaseRecord *records=add->pp();

	TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);

	buf1->SetBuffer((*records)["TH1F_Col"],51200000,kFALSE);
	buf2->SetBuffer((*records)["bin"],512000,kFALSE);

	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
	std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;


	//TClass *myclass=gROOT->GetClass("TTree");
	//TTree *tree =(TTree*)buf1->ReadObject(myclass);
	TTree* tree = new TTree();
	tree->Streamer(*buf1);

	TTree* bin = new TTree(); 
	bin->Streamer(*buf2);

	TH1F *h1=new TH1F();

	std::vector<TH1F> hist;
	std::vector<double> hRange;
	// Read in the object
	int cnt;
	tree -> SetBranchAddress("TH1F_Col", &h1);
	int entries=tree->GetEntries();
	for(cnt=0; cnt<entries; cnt++){
		tree -> GetEntry(cnt);
		gROOT->cd();
		TH1F *h2=new TH1F();
		h2=(TH1F*)h1->Clone();
		hist.push_back(*h2);
	}
    int ver;
	double betagamma[5000];
	int totalNum;
	int bgNum;
    int thetaNum;
    if (bin->FindBranch("version")) bin->SetBranchAddress("version", &ver);
    else ver = 0;
    if (bin->FindBranch("thetaNum")) bin->SetBranchAddress("thetaNum", &thetaNum);
    else thetaNum = 16;
	bin-> SetBranchAddress("totalNum", &totalNum);
	bin-> SetBranchAddress("betagamma",  betagamma);
	bin-> SetBranchAddress("betagammaBounds", &bgNum);
	bin->GetEntry(0);
	//int tot=totalNum*2;
	for(cnt=0; cnt<bgNum; cnt++){
		hRange.push_back(betagamma[cnt]);
	}
    tmpObject->setVersion(ver);
	tmpObject->setRangeNo(bgNum);
	tmpObject->setRange(&hRange);
	tmpObject->setHistNo(totalNum);
    tmpObject->setThetaNo(thetaNum);
	tmpObject->setHist(&hist);
	refpObject=tmpObject;

	delete tree;
	delete bin;

	return StatusCode::SUCCESS;


}
