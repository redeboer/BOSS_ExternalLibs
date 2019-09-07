#include "GaudiKernel/MsgStream.h"
#include "TreeMdcCalibDataCnv.h"
#include "CalibData/Mdc/MdcCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "CalibMySQLCnv/TreeAddress.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
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
//static CnvFactory<TreeMdcCalibDataCnv> MdcCal_factory;
//const  ICnvFactory& TreeMdcCalibDataCnvFactory = MdcCal_factory;



TreeMdcCalibDataCnv::TreeMdcCalibDataCnv( ISvcLocator* svc) :
  TreeCalBaseCnv(svc, CLID_Calib_MdcCal) { 

  }


const CLID& TreeMdcCalibDataCnv::objType() const {
  return CLID_Calib_MdcCal;
}

const CLID& TreeMdcCalibDataCnv::classID() {
  return CLID_Calib_MdcCal;
}

StatusCode TreeMdcCalibDataCnv::i_createObj(IOpaqueAddress* addr,
    DataObject*& refpObject) {

  MsgStream log(msgSvc(), "TreeMdcCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
  CalibData::MdcCalibData *tmpObject = new CalibData::MdcCalibData ;  
  TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
  DatabaseRecord *records=add->pp();

  TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf3 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf4 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf5 = new TBufferFile(TBuffer::kRead);
  TBufferFile *buf6 = new TBufferFile(TBuffer::kRead);

  buf1->SetBuffer((*records)["XtTree"],512000,kFALSE);
  buf2->SetBuffer((*records)["QtTree"],512000,kFALSE);
  buf3->SetBuffer((*records)["T0Tree"],512000,kFALSE);
  buf4->SetBuffer((*records)["SdTree"],512000,kFALSE);
  buf5->SetBuffer((*records)["NewXtTrees"],51200000,kFALSE);
  buf6->SetBuffer((*records)["R2tTrees"],25600000,kFALSE);

  std::cout<<" SftVer is "<<(*records)["SftVer"];
  std::cout<<"TreeMdcCalibDataCnv: CalVerSft is "<<(*records)["CalParVer"]<<std::endl;
  std::cout<<"TreeMdcCalibDataCnv: Calib file name is "<<(*records)["FileName"]<<std::endl;

  TTree* xttree = new TTree(); 
  xttree->Streamer(*buf1); 

  TTree* qttree = new TTree(); 
  qttree->Streamer(*buf2);

  TTree* t0tree= new TTree(); 
  t0tree->Streamer(*buf3); 

  TTree* sdtree = new TTree(); 
  sdtree->Streamer(*buf4); 

  TObjArray newxttrees;
  DatabaseRecord::iterator it = (*records).find("NewXtTrees");
  if(it!=(*records).end()){
    if((*it).second!=NULL) {
      newxttrees.Streamer(*buf5); 
    }
  } 

  TObjArray r2ttrees;
  it = (*records).find("R2tTrees");
  if(it!=(*records).end()){
    if((*it).second!=NULL) {
      r2ttrees.Streamer(*buf6); 
    }
  } 
  

  // Read in the object
  int i;
  int nentries;

  //read xttree-----------------------------
  double xtpar;
  int  xtkey;
  xttree -> SetBranchAddress("xtpar", &xtpar);
  xttree -> SetBranchAddress("xtkey", &xtkey);
  nentries = xttree -> GetEntries();
  for(i=0; i<nentries; i++){
    xttree -> GetEntry(i);
    tmpObject -> setXtpar(xtkey,xtpar);
  }

  //read newxttrees-----------------------------
  if((43*18*2)==newxttrees.GetEntries()){ 
    tmpObject->setNewXtpar(&newxttrees); 
    for(int i=0;i<43*18*2;i++){
      TTree* tempTree = (TTree*) newxttrees.At(i);
      delete tempTree;
    }
  }
  //read r2ttrees-----------------------------
  if(43==r2ttrees.GetEntries()){ 
    tmpObject->setR2tpar(&r2ttrees); 
    for(int i=0;i<43;i++){
      TTree* tempTree = (TTree*) r2ttrees.At(i);
      delete tempTree;
    }
  }

  // read t0tree ------------------------------------------------------------
  double t0;
  double delt0;
  t0tree -> SetBranchAddress("t0", &t0);
  t0tree -> SetBranchAddress("delt0", &delt0);
  nentries = t0tree -> GetEntries();
  for(i=0; i<nentries; i++){
    t0tree -> GetEntry(i);
    tmpObject -> setT0(t0);
    tmpObject -> setDelT0(delt0);
  }
  // read qttree ------------------------------------------------------------
  double qtpar0;
  double qtpar1;
  qttree -> SetBranchAddress("qtpar0", &qtpar0);
  qttree -> SetBranchAddress("qtpar1", &qtpar1);
  nentries = qttree -> GetEntries();
  for(i=0; i<nentries; i++){
    qttree -> GetEntry(i);
    tmpObject -> setQtpar0(qtpar0);
    tmpObject -> setQtpar1(qtpar1);
  }

  // read Sdtree ---------------------------------------------------------
  double sdpar;
  int  sdkey;
  sdtree -> SetBranchAddress("sdpar", &sdpar);
  sdtree -> SetBranchAddress("sdkey", &sdkey);
  nentries = sdtree -> GetEntries();

  for(i=0; i<nentries; i++){
    sdtree -> GetEntry(i);
    tmpObject -> setSdpar(sdkey,sdpar);
  }

  refpObject=tmpObject;
  delete xttree;
  delete qttree;
  delete t0tree;
  delete sdtree;

  delete buf1;
  delete buf2;
  delete buf3;
  delete buf4;
  delete buf5;
  delete buf6;

  return StatusCode::SUCCESS;


}

