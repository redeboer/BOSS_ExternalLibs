#include "GaudiKernel/MsgStream.h"
#include "TreeMdcAlignDataCnv.h"
#include "CalibData/Mdc/MdcAlignData.h"
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
//static CnvFactory<TreeMdcAlignDataCnv> DedxCal_factory;
//const  ICnvFactory& TreeMdcAlignDataCnvFactory = DedxCal_factory;



TreeMdcAlignDataCnv::TreeMdcAlignDataCnv( ISvcLocator* svc) :
	TreeCalBaseCnv(svc, CLID_Calib_MdcAlign) { 

	}


const CLID& TreeMdcAlignDataCnv::objType() const {
	return CLID_Calib_MdcAlign;
}

const CLID& TreeMdcAlignDataCnv::classID() {
	return CLID_Calib_MdcAlign;
}

StatusCode TreeMdcAlignDataCnv::i_createObj(IOpaqueAddress* addr,
		DataObject*& refpObject) {

	MsgStream log(msgSvc(), "TreeMdcAlignDataCnv");
	log<<MSG::DEBUG<<"SetProperty"<<endreq;

	CalibData::MdcAlignData *tmpObject = new CalibData::MdcAlignData;
	TreeAddress* add = dynamic_cast<TreeAddress*>(addr);

	DatabaseRecord *records=add->pp(); 
	std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" CalVerSft is "<<(*records)["MdcAlignVer"];
	std::cout<<"  File name  is "<<(*records)["AlignEPFileName"]<<std::endl;

	/*char mem[1024000];
		char mem1[1024000];
		char mem2[1024000];

		memcpy(mem,par0,treesize[0]);
		memcpy(mem1,par0+1024000,treesize[1]);
		memcpy(mem2,par0+1024000*2,treesize[2]);*/
	// std::cout<<"mem="<<mem<<"   mem1:"<<mem1<<"mem2:"<<mem2<<std::endl;
	tmpObject->readPar((*records)["AlignEndPlate"],(*records)["WirePos"],(*records)["WireTension"]);
	refpObject=tmpObject;
	return StatusCode::SUCCESS;


}

