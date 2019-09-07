// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootMdcCalibDataCnv.cxx,v 1.9 2011/12/20 22:33:05 zhangy Exp $
#include "GaudiKernel/MsgStream.h"
#include "RootMdcCalibDataCnv.h"
#include "CalibData/Mdc/MdcCalibData.h"
#include "CalibData/Mdc/MdcCalStruct.h"
#include "CalibDataSvc/IInstrumentName.h"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TBufferFile.h"
#include "TObjArray.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibDataSvc/ICalibRootSvc.h"    //maybe
#include "CalibDataSvc/ICalibMetaCnvSvc.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;
//static CnvFactory<RootMdcCalibDataCnv> MdcCal_factory;
//const  ICnvFactory& RootMdcCalibDataCnvFactory = MdcCal_factory;



RootMdcCalibDataCnv::RootMdcCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_MdcCal) { 

}


const CLID& RootMdcCalibDataCnv::objType() const {
  return CLID_Calib_MdcCal;
}

const CLID& RootMdcCalibDataCnv::classID() {
  return CLID_Calib_MdcCal;
}

StatusCode RootMdcCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootMdcCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
 
  CalibData::MdcCalibData *tmpObject = new CalibData::MdcCalibData ;
  // Read in our object
  int i;
  int nentries;
  // read xttree ------------------------------------------------------------
  double xtpar;
  int  xtkey;
  TTree *xttree = (TTree*)m_inFile -> Get("XtTree");
  xttree -> SetBranchAddress("xtpar", &xtpar);
  xttree -> SetBranchAddress("xtkey", &xtkey);
  nentries = xttree -> GetEntries();
  for(i=0; i<nentries; i++){
    xttree -> GetEntry(i);
    tmpObject -> setXtpar(xtkey,xtpar);
  }

  // read newxttree ------------------------------------------------------------
  TObjArray newXtTrees;
  if(NULL!=m_inFile->Get("trNewXt00_00_0")){
    for(int layid=0; layid<43; layid++){
      for(int entr=0; entr<18; entr++){
	for(int lr=0; lr<2; lr++){
	  char newXtTreeName[20];
	  sprintf(newXtTreeName,"trNewXt%02d_%02d_%d",layid,entr,lr);
	  TTree* newXtTree = ((TTree*)m_inFile->Get(newXtTreeName));
	  if(!newXtTree) break;
	  newXtTrees.Add(newXtTree->CloneTree());
	}//end lr
      }//end entr
    }//end layid
    if((43*18*2)==newXtTrees.GetEntries())tmpObject->setNewXtpar(&newXtTrees);
  }


  // read r2ttree ------------------------------------------------------------
  TObjArray r2tTrees;
  if(NULL!=m_inFile->Get("r2t00")){
    for(int layid=0; layid<43; layid++){
      char r2tTreeName[20];
      sprintf(r2tTreeName,"r2t%02d",layid);
      TTree* r2tTree = ((TTree*)m_inFile->Get(r2tTreeName));
      if(!r2tTree) break;
      r2tTrees.Add(r2tTree->CloneTree());
    }
    if(43==r2tTrees.GetEntries()) tmpObject->setR2tpar(&r2tTrees);
  }

  // read t0tree ------------------------------------------------------------
  double t0;
  double delt0;
  TTree *t0tree = (TTree*)m_inFile -> Get("T0Tree");
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
  TTree *qttree = (TTree*)m_inFile -> Get("QtTree");
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
  TTree *sdtree = (TTree*)m_inFile -> Get("SdTree");
  sdtree -> SetBranchAddress("sdpar", &sdpar);
  sdtree -> SetBranchAddress("sdkey", &sdkey);
  nentries = sdtree -> GetEntries();

  for(i=0; i<nentries; i++){
    sdtree -> GetEntry(i);
    tmpObject -> setSdpar(sdkey,sdpar);
  }

  refpObject=tmpObject;
  return StatusCode::SUCCESS;
}

StatusCode RootMdcCalibDataCnv::createRoot(const std::string& fname, 
    CalibData::CalibBase1* pTDSObj) {

  MsgStream log(msgSvc(), "RootMdcCalibDataCnv");
  // Open the file, create the branch
  StatusCode sc = openWrite(fname);
  if(!sc)
  { log<<MSG::ERROR<<"unable to open files"<<endreq;
  }
  // write the Data in the TCDS to RootFile
  CalibData::MdcCalibData* tmpObject = dynamic_cast<CalibData::MdcCalibData*>(pTDSObj);
  int tmpNo;
  int key;
  double xtpar;
  double t0;
  double delt0;
  double qtpar[2];
  double sdpar;
  int i;

  //xttree------------------------------------------------------------------
  TTree *xttree = new TTree("XtTree", "XtTree");
  xttree -> Branch("xtkey", &key, "key/I");
  xttree -> Branch("xtpar", &xtpar, "xtpar/D");
  tmpObject -> setXtBegin();
  while( tmpObject -> getNextXtpar(key, xtpar) ){
    xttree -> Fill();
  }
  //newxttree------------------------------------------------------------------
  //FIXME
  /*
     TObjArray newxttrees_1;
  //TBufferFile newxttrees_buf(TBuffer::kWrite);
  int nTree=0;
  for(int t_layer=0; t_layer<43; t_layer++){
  for(int t_bin=0; t_bin<18; t_bin++){
  for(int t_lr=0; t_lr<2; t_lr++){
  char newXtTreeName[20];
  sprintf(newXtTreeName,"trNewXt%02d_%02d_%d",t_layer,t_bin,t_lr);
  TTree *newXttree =  ((TTree*)f->Get(newXtTreeName))->CloneTree();
  newxttrees_1.Add(newXttree);
  nTree++;
  }
  }
  }
  newxttrees_1.Streamer(newxttrees_buf);
  */


  //t0tree-------------------------------------------------------------------
  TTree *t0tree = new TTree("T0Tree", "T0Tree");
  t0tree -> Branch("t0", &t0, "t0/D");
  t0tree -> Branch("delt0", &delt0, "delt0/D");
  tmpNo = tmpObject -> gett0No();
  for(i=0; i<tmpNo; i++){
    t0 = tmpObject -> getT0(i);
    delt0 = tmpObject -> getDelT0(i);
    t0tree -> Fill();
  }

  //qttree--------------------------------------------------------------------
  TTree *qttree = new TTree("QtTree", "QtTree");
  qttree -> Branch("qtpar0", &(qtpar[0]), "qtpar0/D");
  qttree -> Branch("qtpar1", &(qtpar[1]), "qtpar1/D");
  tmpNo = tmpObject -> getqtparNo();
  for(i=0; i<tmpNo; i++){
    qtpar[0] =  tmpObject -> getQtpar0(i);
    qtpar[1] =  tmpObject -> getQtpar1(i);
    qttree -> Fill();
  }

  //sdtree--------------------------------------------------------------------
  TTree *sdtree = new TTree("SdTree", "SdTree");
  sdtree -> Branch("sdkey", &key, "key/I");
  sdtree -> Branch("sdpar", &sdpar, "sdpar/D");
  tmpObject -> setSdBegin();
  while( tmpObject -> getNextSdpar(key, sdpar) ){
    sdtree -> Fill();
  }

  xttree -> Write();
  t0tree -> Write();
  qttree -> Write();
  sdtree -> Write();

  delete xttree;
  delete t0tree;
  delete qttree;
  delete sdtree;

  closeWrite();
  log<<MSG::INFO<<"successfully create RootFile"<<endreq;
  return sc;
}
