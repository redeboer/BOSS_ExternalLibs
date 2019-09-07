#include "GaudiKernel/MsgStream.h"
#include "TreeMucCalibDataCnv.h"
#include "CalibData/Muc/MucCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "CalibMySQLCnv/TreeAddress.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TBuffer.h"
#include "TH1F.h"

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
//using namespace CalibData;
//static CnvFactory<TreeMucCalibDataCnv> MucCal_factory;
//const  ICnvFactory& TreeMucCalibDataCnvFactory = MucCal_factory;

using CalibData::MucCalibData;
using CalibData::MucIdTransform;

TreeMucCalibDataCnv::TreeMucCalibDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Calib_MucCal) { 
		m_ptrIdTr = new MucIdTransform();
	}


const CLID& TreeMucCalibDataCnv::objType() const {
	return CLID_Calib_MucCal;
}

const CLID& TreeMucCalibDataCnv::classID() {
	return CLID_Calib_MucCal;
}

StatusCode TreeMucCalibDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeMucCalibDataCnv");
	log<<MSG::DEBUG<<"SetProperty"<<endreq;
	CalibData::MucCalibData *tmpObject = new CalibData::MucCalibData ;  
	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
	DatabaseRecord *records=add->pp();

	TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
	TBufferFile *buf3 = new TBufferFile(TBuffer::kRead);
	buf1->SetBuffer((*records)["LayTree"],512000,kFALSE);
	buf2->SetBuffer((*records)["BoxTree"],512000,kFALSE);
	buf3->SetBuffer((*records)["StrTree"],512000,kFALSE);
	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
	std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;


	TTree* laytree = new TTree(); 
	laytree->Streamer(*buf1); 
	log<<MSG::DEBUG<< "laytree name:\t" << laytree->GetName() << endreq;
	TTree* boxtree = new TTree(); 
	boxtree->Streamer(*buf2);
	log<<MSG::DEBUG<< "boxtree name:\t" << boxtree->GetName() << endreq;
	TTree* strtree= new TTree(); 
	strtree->Streamer(*buf3); 
	log<<MSG::DEBUG<< "strtree name:\t" << strtree->GetName() << endreq;

	// Read in the object
	//read layerConst-----------------------------
	Double_t lay_eff,box_eff,str_eff;
	Double_t lay_cnt, box_cnt, str_cnt;
	Double_t lay_nos, box_nos, str_nos;
	Double_t lay_nos_ratio, box_nos_ratio, str_nos_ratio;
	lay_eff = box_eff = str_eff = 0.0;
	lay_cnt = box_cnt = str_cnt = 0.0;
	lay_nos = box_nos = str_nos = 0.0;
	lay_nos_ratio = box_nos_ratio = str_nos_ratio = 0.0;

	char name[60];
	laytree->SetBranchAddress("layer_eff", &lay_eff);
	laytree->SetBranchAddress("layer_cnt", &lay_cnt);
	laytree->SetBranchAddress("layer_noise", &lay_nos);
	laytree->SetBranchAddress("layer_nosratio",&lay_nos_ratio);

	boxtree->SetBranchAddress("box_eff", &box_eff);
	boxtree->SetBranchAddress("box_cnt", &box_cnt);
	boxtree->SetBranchAddress("box_noise", &box_nos);
	boxtree->SetBranchAddress("box_nosratio", &box_nos_ratio); 

	strtree->SetBranchAddress("strip_eff", &str_eff);
	strtree->SetBranchAddress("strip_cnt", &str_cnt);
	strtree->SetBranchAddress("strip_noise", &str_nos);
	strtree->SetBranchAddress("strip_nosratio", &str_nos_ratio);

	int part, segment, layer, strip;
	part = segment = layer = strip = 0;
	for(int i=0; i<LAYER_MAX; i++)
	{
		laytree->GetEntry(i);
		tmpObject->setLayerEff(lay_eff,i);
		tmpObject->setLayerCnt(lay_cnt, i);
		tmpObject->setLayerNos(lay_nos, i);
		tmpObject->setLayerNosRatio(lay_nos_ratio, i);
		sprintf(name,"LayClstPro");
		for(int j=0; j<CLST_MAX; j++) {
			//		  if( ht_Pro[0] != NULL ) tmpObject->setLayerClstPro(ht_Pro[0]->GetBinContent(j),i,j);
			//          else tmpObject->setLayerClstPro(DEFAULT_CLST_PRO[j],i,j);
			tmpObject->setLayerClstPro(DEFAULT_CLST_PRO[j],i,j);
		}
		log<<MSG::DEBUG<<"layer: " << i << "\t" << lay_eff <<endreq;		
	}

	for(int i=0; i<BOX_MAX; i++)
	{
		m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
		boxtree->GetEntry(i);		
		tmpObject->setBoxEff(box_eff, part, segment, layer);
		tmpObject->setBoxCnt(box_cnt, part, segment, layer);
		tmpObject->setBoxNos(box_nos, part, segment, layer);
		tmpObject->setBoxNosRatio(box_nos_ratio, part, segment, layer);

		sprintf(name,"BoxClstPro_B%d",i);
		for(int j=0; j<CLST_MAX; j++) {
			//		  if( ht_Pro[1] != NULL ) tmpObject->setBoxClstPro(ht_Pro[1]->GetBinContent(j),part,segment,layer,j);
			//	  else
			tmpObject->setBoxClstPro(DEFAULT_CLST_PRO[j],part,segment,layer,j);
		}
		log<<MSG::DEBUG<<"box: " << i << "\t" << box_eff <<endreq;               
	}

	log<<MSG::DEBUG<<"strtree entries:\t" << strtree->GetEntries() << endreq;
	for(int i=0; i<STRIP_MAX; i++)
	{       
		m_ptrIdTr->SetStripPos( i, &part, &segment, &layer, &strip );
		strtree->GetEntry(i);
		tmpObject->setStripEff(str_eff, part, segment, layer, strip);
		tmpObject->setStripCnt(str_cnt, part, segment, layer, strip);
		tmpObject->setStripNos(str_nos, part, segment, layer, strip);
		tmpObject->setStripNosRatio(str_nos_ratio, part, segment, layer, strip);
		log<<MSG::DEBUG<<"strip: " << i << "\t" << str_eff <<endreq;               

	}  

	refpObject=tmpObject;
	delete laytree;
	delete boxtree;
	delete strtree;
	return StatusCode::SUCCESS;


}

