//--------------------------------------------------------------------------
// File and Version Information:
//
// tag hot channel and dead channel in the Emc Calibration constant file
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//      Chunxiu Liu
//
// Copyright Information:
//	Copyright (C) 2012-04-19               IHEP
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EmcBhaCalib/EmcCalibTagDeadOrHot.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <cstdlib>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/DataObject.h"

#include "CLHEP/Vector/ThreeVector.h"
using namespace std;

using CLHEP::Hep3Vector;

//--------------------
#include "GaudiKernel/MsgStream.h"


#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TObjArray.h"



//----------------
// Constructors --
//----------------
EmcCalibTagDeadOrHot::EmcCalibTagDeadOrHot(const std::string& name, ISvcLocator* pSvcLocator)
  :Algorithm(name, pSvcLocator),  
   m_fileExt(""),
   m_fileDir("/ihepbatch/besdata/public/liucx/Calib/"),
   m_deadIxtal(-999),
   m_hotIxtal(-999)
{

  // Declare the properties  
  declareProperty("fileExt", m_fileExt);
  declareProperty("fileDir", m_fileDir);
  declareProperty("deadIxtal", m_deadIxtal);
  declareProperty("hotIxtal", m_hotIxtal);

  m_oldConstants = new double[6240];
  m_IxtalNumber= new int[6240];
}

//--------------
// Destructor --
//--------------
EmcCalibTagDeadOrHot::~EmcCalibTagDeadOrHot() {

  if ( 0 != m_oldConstants) {
    delete [] m_oldConstants;
    m_oldConstants = 0;
  }
  if ( 0 != m_IxtalNumber) {
    delete [] m_IxtalNumber;
    m_IxtalNumber = 0;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcCalibTagDeadOrHot::initialize(){
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
 
  // use EmcCalibConstSvc
  StatusCode scCalib; 
  scCalib = Gaudi::svcLocator() -> service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if( scCalib != StatusCode::SUCCESS){
    log << MSG::ERROR << "can not use EmcCalibConstSvc" << endreq;
  } 
  else {
    std::cout << "Test EmcCalibConstSvc   DigiCalibConst(0)=  " 
  	      << m_emcCalibConstSvc -> getDigiCalibConst(0) << std::endl;
  }
  
   //init starting values for calibration constants from file or set to 1

  initCalibConst();  
 
  SetDeadOrHotChannel();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcCalibTagDeadOrHot::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;   

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcCalibTagDeadOrHot::finalize() {

  MsgStream log(msgSvc(), name());


  log << MSG::INFO << "in endRun()" << endreq;


  return StatusCode::SUCCESS;
}



//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void
EmcCalibTagDeadOrHot::initCalibConst( ) {

  
  MsgStream log(msgSvc(), name());
  
  int nConstEmc;
      
  nConstEmc= m_emcCalibConstSvc -> getDigiCalibConstNo() ;

  if ( nConstEmc!=6240) cout<<"number of calibconst="<< nConstEmc<<endl;
   
  for ( int i = 0; i< nConstEmc; i++ ) {

    m_oldConstants[i]=m_emcCalibConstSvc -> getDigiCalibConst(i);
    m_IxtalNumber[i]=m_emcCalibConstSvc -> getIxtalNumber(i);
 
  }

  ifstream In;
  In.open("/ihepbatch/bes/liucx/GrelConstIxtal2013.txt");
  ofstream out;
  out.open("thephi.dat");	
    int ixtal,ithe,iphi;
  double rel;
  while( !In.eof() ) {

    In >> ixtal>>rel;
    ithe= m_emcCalibConstSvc -> getThetaIndex(ixtal);
    iphi= m_emcCalibConstSvc -> getPhiIndex(ixtal);
    out<<ixtal<<"\t"<<ithe<<"\t"<<iphi<<"\t"<<rel<<endl;
  }
  In.close();

  out.close();

  cout<<"1,10,101"<<"\t"<<m_emcCalibConstSvc -> getIndex(1,10,101)<<endl;
  cout<<"1,30,105"<<"\t"<<m_emcCalibConstSvc -> getIndex(1,30,105)<<endl;
  cout<<"1,35,40"<<"\t"<<m_emcCalibConstSvc -> getIndex(1,35,40)<<endl;
  cout<<"0,2,40"<<"\t"<<m_emcCalibConstSvc -> getIndex(0,2,40)<<endl;
  cout<<"2,2,40"<<"\t"<<m_emcCalibConstSvc -> getIndex(2,2,40)<<endl;

}


//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void
EmcCalibTagDeadOrHot::SetDeadOrHotChannel() {

  int Nxtals=6240;

  double DigiConst[6240];
  int IxtalNumber[6240];

  for(int ind=0; ind < Nxtals; ind++ ) {

    DigiConst[ind]=m_oldConstants[ind];
    IxtalNumber[ind]=m_IxtalNumber[ind];
   if (ind==m_deadIxtal)  IxtalNumber[ind]=-9;
    if (ind==m_hotIxtal)  IxtalNumber[ind]=-99;
    // cout<<"ind="<<ind<<"\t"<< DigiConst[ind]<<endl;
  }


 TFile fconst("EmcCalibConst_DeadOrHot.root", "recreate");

 //define tree fill the absolute digicalibconsts into the root-file
  TTree* constr= new TTree("DigiCalibConst","DigiCalibConst");
  constr->Branch("DigiCalibConst",DigiConst,"DigiConst[6240]/D"); 
  constr->Branch("IxtalNumber",IxtalNumber,"IxtalNumber[6240]/I"); 

  constr->Fill();


  constr->Write();
 
  delete constr;
  
  fconst.Close();

  // end tree




   
}



