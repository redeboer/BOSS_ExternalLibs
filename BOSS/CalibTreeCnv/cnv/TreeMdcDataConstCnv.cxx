#include "GaudiKernel/MsgStream.h"
#include "TreeMdcDataConstCnv.h"
#include "CalibData/Mdc/MdcDataConst.h"
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


TreeMdcDataConstCnv::TreeMdcDataConstCnv( ISvcLocator* svc) :
  TreeCalBaseCnv(svc, CLID_Calib_MdcDataConst) { 
}


const CLID& TreeMdcDataConstCnv::objType() const {
  return CLID_Calib_MdcDataConst;
}

const CLID& TreeMdcDataConstCnv::classID() {
  return CLID_Calib_MdcDataConst;
}

StatusCode TreeMdcDataConstCnv::i_createObj(IOpaqueAddress* addr, DataObject*& refpObject) {

  MsgStream log(msgSvc(), "TreeMdcDataConstCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;

  CalibData::MdcDataConst *tmpObject = new CalibData::MdcDataConst;
  TreeAddress* add = dynamic_cast<TreeAddress*>(addr);

  DatabaseRecord *records=add->pp(); 
  std::cout<<" SftVer is "<<(*records)["SftVer"];
	std::cout<<" MdcDataConst CalVerSft is "<<(*records)["CalParVer"];
  std::cout<<" File name is "<<(*records)["FileName"]<<std::endl;

  tmpObject->readPar((*records)["WireEff"]);
  refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

