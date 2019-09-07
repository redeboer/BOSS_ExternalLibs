#include "MdcCalibAlg/MdcCalConstMgr.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TObjArray.h"

#include <iostream>
#include <iomanip>

MdcCalConstMgr::MdcCalConstMgr(){
     // constructor
}

MdcCalConstMgr::~MdcCalConstMgr(){
     // destructor
}

void MdcCalConstMgr::init(IMdcGeomSvc* mdcGeomSvc, IMdcCalibFunSvc* mdcFunSvc){
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
}

// read calibration data from TCDS
void MdcCalConstMgr::rdConstTcds(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalConstMgr");
     log << MSG::INFO << "Read calibration data from TCDS by MdcCalibFunSvc"
	 << endreq;

     int lay;
     int wid;

     int nlayer;
     int nwire;

     int key;
     double val;

     nlayer = m_mdcGeomSvc -> getLayerSize();
     nwire = m_mdcGeomSvc -> getWireSize();

// read XT data -----------------------------------------------------------
     m_mdcFunSvc -> setXtBegin();
     while( m_mdcFunSvc -> getNextXtpar(key, val) ){
	  calconst -> fillXtpar(key, val);
     }

// read T0 data -----------------------------------------------------------
     double t0;
     double delt0;
     for(wid=0; wid<nwire; wid++){
        t0 = m_mdcFunSvc -> getT0(wid);
        delt0 = m_mdcFunSvc -> getDelT0(wid);
        calconst -> fillT0(t0);
        calconst -> fillDelT0(delt0);
     }

// read QT data -----------------------------------------------------------
     double qtpar0;
     double qtpar1;
     for(lay=0; lay<nlayer; lay++){
	  qtpar0 = m_mdcFunSvc -> getQtpar(lay, 0);
	  qtpar1 = m_mdcFunSvc -> getQtpar(lay, 1);
	  calconst -> fillQtpar0(qtpar0);
	  calconst -> fillQtpar1(qtpar1);
     }

// read spatial resolution data -------------------------------------------
     m_mdcFunSvc -> setSdBegin();
     while( m_mdcFunSvc -> getNextSdpar(key, val) ){
	  calconst -> fillSdpar(key, val);
     }
}


// write calibration data from TCDS
void MdcCalConstMgr::wrtConstTcds(MdcCalibConst* calconst){

}


// read calibration data directly in run directory
void MdcCalConstMgr::rdConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalConstMgr");
     log << MSG::WARNING << "read calibration data directly from run directory" 
	 << endreq;

     TFile fconst("MdcCalibConst.root");
     log << MSG::INFO << "open file" << endreq;

// read xttree ------------------------------------------------------------
     int i;
     int key;
     double xtpar;
     int entry;
     TTree* xttree = (TTree*)fconst.Get("XtTree");
     xttree -> SetBranchAddress("xtkey", &key);
     xttree -> SetBranchAddress("xtpar", &xtpar);
     entry = (int)xttree -> GetEntries();
     for(i=0; i<entry; i++){
	  xttree -> GetEntry(i);
	  calconst -> fillXtpar(key, xtpar);
     }
     log << MSG::INFO << "read XTtree" << endreq;

// read t0tree ------------------------------------------------------------
     double t0;
     double delt0;
     TTree* t0tree = (TTree*)fconst.Get("T0Tree");
     t0tree -> SetBranchAddress("t0", &t0);
     t0tree -> SetBranchAddress("delt0", &delt0);
     entry = (int)t0tree -> GetEntries();
     for(i=0; i<entry; i++){
	  t0tree -> GetEntry(i);
	  calconst -> fillT0(t0);
	  calconst -> fillDelT0(delt0);
     }
     log << MSG::INFO << "read XTtree" << endreq;

// read qttree ------------------------------------------------------------
     double qtpar0;
     double qtpar1;
     TTree* qttree = (TTree*)fconst.Get("QtTree");
     qttree -> SetBranchAddress("qtpar0", &qtpar0);
     qttree -> SetBranchAddress("qtpar1", &qtpar1);
     entry = (int)qttree -> GetEntries();
     for(i=0; i<entry; i++){
	  qttree -> GetEntry(i);
	  calconst -> fillQtpar0(qtpar0);
	  calconst -> fillQtpar1(qtpar1);
     }

// read sdtree ---------------------------------------------------------
     double sdpar;
     TTree* sdtree = (TTree*)fconst.Get("SdTree");
     sdtree -> SetBranchAddress("sdkey", &key);
     sdtree -> SetBranchAddress("sdpar", &sdpar);
     entry = sdtree -> GetEntries();
     for(i=0; i<entry; i++){
	  sdtree -> GetEntry(i);
	  calconst -> fillSdpar(key, sdpar);
     }

     fconst.Close();

     log << MSG::INFO << "read MdcCalibConst over" << endreq;
}


// write calibration data directly in run directory
void MdcCalConstMgr::wrtConst(MdcCalibConst* calconst) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalConstMgr");
     log << MSG::INFO << "MdcCalConstMgr::wrtConst()" << endreq;

     TFile fconst("MdcCalibConst_new.root", "recreate");

// XT parameter tree -------------------------------------------------------
     int i;
     int key;
     int entry;
     double xtpar;
     TTree* xttree = new TTree("XtTree", "XtTree");
     xttree -> Branch("xtkey", &key, "key/I");
     xttree -> Branch("xtpar", &xtpar, "xtpar/D");
     calconst -> setXtBegin();
     while( calconst -> getNextXtpar(key, xtpar) ){
	  xttree -> Fill();
     }

// T0 parameter tree -------------------------------------------------------
     double t0;
     double delt0;

     TTree* t0tree = new TTree("T0Tree", "T0Tree");
     t0tree -> Branch("t0", &t0, "t0/D");
     t0tree -> Branch("delt0", &delt0, "delt0/D");
     entry = calconst -> getT0Size();
     for(i=0; i<entry; i++){
	  t0 = calconst -> getT0(i);
	  delt0 = calconst -> getDelT0(i);
	  t0tree -> Fill();
     }

// QT parameter tree -------------------------------------------------------
     double qtpar0;
     double qtpar1;

     TTree* qttree = new TTree("QtTree", "QtTree");
     qttree -> Branch("qtpar0", &qtpar0, "qtpar0/D");
     qttree -> Branch("qtpar1", &qtpar1, "qtpar1/D");
     entry = calconst -> getQtSize();
     for(i=0; i<entry; i++){
	  qtpar0 = calconst -> getQtpar0(i);
	  qtpar1 = calconst -> getQtpar1(i);
	  qttree -> Fill();
     }

// Spacial Resolution parameter tree ---------------------------------------
     double sdpar;

     TTree* sdtree = new TTree("SdTree", "SdTree");
     sdtree -> Branch("sdkey", &key, "key/I");
     sdtree -> Branch("sdpar", &sdpar, "sdpar/D");
     calconst -> setSdBegin();
     while( calconst -> getNextSdpar(key, sdpar) ){
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

     fconst.Close();

     log << MSG::INFO << "MdcCalibConst_new.root was written." << endreq;
}
