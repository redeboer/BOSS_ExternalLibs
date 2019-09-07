#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

//#include "VertexFit/VertexDbSvc.h"
#include "ParticleID/ParticleID.h"
//
#include "DQAEvent/DQAEvent.h"
#include "DQA_EMC/DQA_EMC.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;


/////////////////////////////////////////////////////////////////////////////

DQA_EMC::DQA_EMC(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("NtupleOutput", m_NtupleOutput=0);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_EMC::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the sub-system name: MDC, DEDX, TOF, EMC, MUC, TRG
  // user can define more directory, such as DAQFILE/MDC/Bhabha.
  NTuplePtr nt(ntupleSvc(), "DQAFILE/EMC");
  if ( nt ) m_tuple = nt;
  else {
      m_tuple = ntupleSvc()->book("DQAFILE/EMC", CLID_ColumnWiseTuple, "EMC ntuple");
      if( m_tuple ) {
          status = m_tuple->addItem("ixtal", m_ixtal);
          status = m_tuple->addItem("npart", m_npart);
          status = m_tuple->addItem("ntheta",m_ntheta);
          status = m_tuple->addItem("nphi",  m_nphi);
          status = m_tuple->addItem("theta", m_theta);
          status = m_tuple->addItem("phi",   m_phi);
          status = m_tuple->addItem("emcX",  m_emcX);
          status = m_tuple->addItem("emcY",  m_emcY);	
          status = m_tuple->addItem("eSeed", m_eSeed);
          status = m_tuple->addItem("e5x5",  m_e5x5);
          status = m_tuple->addItem("energy",m_energy);
          status = m_tuple->addItem("time",  m_time);
	
      } else {
          log << MSG::ERROR << "Can not book N-tuple:" << long(m_tuple) << endreq;
      }
  }    

  if(service("THistSvc", m_thistsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  char name[60];
  char title[60];

  std::string HistName0, HistName;
  HistName0="/DQAHist/EMC/";

  sprintf( name, "EMC_Bhabha_ShowerEneBarrelVsEvent");      
  sprintf( title, "EMC Bhabha ShowerEneBarrel vs event");  

  m_HistEnergyB =new TH1F(name,title,200,0.,2.0);
  m_HistEnergyB->GetXaxis()->SetTitle("shower energy(GeV)");
  m_HistEnergyB->GetXaxis()->CenterTitle();
  m_HistEnergyB->GetYaxis()->SetTitle("Number of event");
  m_HistEnergyB->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName, m_HistEnergyB).isFailure()){
    log << MSG::ERROR << "Couldn't register " <<name<< endreq;
  }
  
  sprintf( name, "EMC_Bhabha_ShowerEneEastVsEvent");      
  sprintf( title, "EMC Bhabha ShowerEneEast vs event");
  m_HistEnergyEast =new TH1F(name,title,200,0.,2.0);
  m_HistEnergyEast->GetXaxis()->SetTitle("shower energy(GeV)");
  m_HistEnergyEast->GetXaxis()->CenterTitle();
  m_HistEnergyEast->GetYaxis()->SetTitle("Number of event");
  m_HistEnergyEast->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName, m_HistEnergyEast).isFailure()){
    log << MSG::ERROR << "Couldn't register "<<name << endreq;
  }
  
  
  sprintf( name, "EMC_Bhabha_ShowerEneWestVsEvent");      
  sprintf( title, "EMC Bhabha ShowerEneWest vs event");
  m_HistEnergyWest =new TH1F(name,title,200,0.,2.0);
  m_HistEnergyWest->GetXaxis()->SetTitle("shower energy(GeV)");
  m_HistEnergyWest->GetXaxis()->CenterTitle();
  m_HistEnergyWest->GetYaxis()->SetTitle("Number of event");
  m_HistEnergyWest->GetYaxis()->CenterTitle();
  
  
  HistName=HistName0+name;
  if( m_thistsvc->regHist(HistName, m_HistEnergyWest).isFailure()){
    log << MSG::ERROR << "Couldn't register "<<name << endreq;
  }
  
  
  sprintf( name, "EMC_Bhabha_ShowerThetaVsvent");      
  sprintf( title, "EMC Bhabha ShowerTheta vs event");   
  m_HistTheta =new TH1F(name,title,56, 0, 56);
  m_HistTheta->GetXaxis()->SetTitle("shower ID(theta)");
  m_HistTheta->GetXaxis()->CenterTitle();
  m_HistTheta->GetYaxis()->SetTitle("Number of event");
  m_HistTheta->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if( m_thistsvc->regHist(HistName, m_HistTheta).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<< endreq;
  }
  
  sprintf(name,"EMC_Bhabha_ShowerCosTheta");
  sprintf(title,"Emc Bhabha Costheta");
  m_HistCosTheta =new TH1F(name,title,200, -1.0, 1.0);
  m_HistCosTheta->GetXaxis()->SetTitle("shower cos(theta)");
  m_HistCosTheta->GetXaxis()->CenterTitle();
  m_HistCosTheta->GetYaxis()->SetTitle("Number of event");
  m_HistCosTheta->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName, m_HistCosTheta).isFailure()){
    log << MSG::ERROR << "Couldn't register "<<name << endreq;
  }
  
  
  sprintf( name, "EMC_Bhabha_ShowerPhiBarrelVsEvent");      
  sprintf( title, "EMC Bhabha ShowerPhiBarrel vs event");
  m_HistPhiB =new TH1F(name,title,120, 0, 120);
  m_HistPhiB->GetXaxis()->SetTitle("shower ID(phi)");
  m_HistPhiB->GetXaxis()->CenterTitle();
  m_HistPhiB->GetYaxis()->SetTitle("Number of event");
  m_HistPhiB->GetYaxis()->CenterTitle();
  
  
  HistName=HistName0+name;
  if( m_thistsvc->regHist(HistName, m_HistPhiB).isFailure()){
    log << MSG::ERROR << "Couldn't register "<<name << endreq;
  }
  
  sprintf( name, "EMC_Bhabha_ShowerPhiEastVsEvent");      
  sprintf( title, "EMC Bhabha ShowerPhiEast vs event");   
  m_HistPhiEast =new TH1F(name,title,256, -3.14, 3.14);
  m_HistPhiEast->GetXaxis()->SetTitle("shower phi(radian)");
  m_HistPhiEast->GetXaxis()->CenterTitle();
  m_HistPhiEast->GetYaxis()->SetTitle("Number of event");
  m_HistPhiEast->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_HistPhiEast).isFailure()){
    log << MSG::ERROR << "Couldn't register"<<name << endreq;
  }
  
  sprintf( name, "EMC_Bhabha_ShowerPhiWestVsEvent");      
  sprintf( title, "EMC Bhabha ShowerPhiWest vs event");
  m_HistPhiWest =new TH1F(name,title,256, -3.14, 3.14);
  m_HistPhiWest->GetXaxis()->SetTitle("shower phi(radian)");
  m_HistPhiWest->GetXaxis()->CenterTitle();
  m_HistPhiWest->GetYaxis()->SetTitle("Number of event");
  m_HistPhiWest->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_HistPhiWest).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<<endreq;
  }
  
  sprintf( name, "EMC_Bhabha_ShowerThetaPhi");      
  sprintf( title, "EMC Bhabha ShowerThetaPhi");
  m_ThetaPhi =new TH2F(name,"Theta versus Phi",
				2000, -3.15, 3.15, 2000, 0.1, 3.0);
  m_ThetaPhi->GetXaxis()->SetTitle("shower phi(radian)");
  m_ThetaPhi->GetXaxis()->CenterTitle();
  m_ThetaPhi->GetYaxis()->SetTitle("shower theta(radian)");
  m_ThetaPhi->GetYaxis()->CenterTitle();  
    
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_ThetaPhi).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<< endreq;
  }


 /////////////////////////////
  sprintf( name, "EMC_Bhabha_Time-T0");      
  sprintf( title, "EMC Bhabha Time-T0 distribution");
  m_HistTime =new TH1F(name,title,100, -40, 60);
  m_HistTime->GetXaxis()->SetTitle("EmcTime-T0 (50ns)");
  m_HistTime->GetXaxis()->CenterTitle();
  m_HistTime->GetYaxis()->SetTitle("Number of event");
  m_HistTime->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_HistTime).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<<endreq;
  }
  
  sprintf( name, "EMC_Bhabha_ixtal");      
  sprintf( title, "EMC Bhabha ixtal distribution");
  m_HistHitMap =new TH1F(name,title,6240, 0, 6240);
  m_HistHitMap->GetXaxis()->SetTitle("ixtalNumber");
  m_HistHitMap->GetXaxis()->CenterTitle();
  m_HistHitMap->GetYaxis()->SetTitle("Number of event");
  m_HistHitMap->GetYaxis()->CenterTitle();
  
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_HistHitMap).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<<endreq;
  }

  sprintf( name, "EMC_Bhabha_eSeedvsIxtal ");      
  sprintf( title, "EMC Bhabha eSeed vs ixtal");
  m_eSeedIxtal =new TH2F(name,"eSeed:Ixtal",
				6240, 0, 6240, 2000, 0, 1.8);
  m_eSeedIxtal->GetXaxis()->SetTitle("Ixtal)");
  m_eSeedIxtal->GetXaxis()->CenterTitle();
  m_eSeedIxtal->GetYaxis()->SetTitle("eSeed(GeV)");
  m_eSeedIxtal->GetYaxis()->CenterTitle();  
    
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_eSeedIxtal).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<< endreq;
  }

  sprintf( name, "EMC_Bhabha_emcX:emcYeast ");      
  sprintf( title, "EMC Bhabha emcX vs emcY of east endcap");
  m_XYeast =new TH2F(name,"emcX:emcY",
				2000, -100, 100, 2000, -100, 100);
  m_XYeast->GetXaxis()->SetTitle("emcX");
  m_XYeast->GetXaxis()->CenterTitle();
  m_XYeast->GetYaxis()->SetTitle("emcY");
  m_XYeast->GetYaxis()->CenterTitle();  
    
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_XYeast).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<< endreq;
  }

  sprintf( name, "EMC_Bhabha_emcX:emcYwest ");      
  sprintf( title, "EMC Bhabha emcX vs emcY of west endcap");
  m_XYwest =new TH2F(name,"emcX:emcY",
				2000, -100, 100, 2000, -100, 100);
  m_XYwest->GetXaxis()->SetTitle("emcX");
  m_XYwest->GetXaxis()->CenterTitle();
  m_XYwest->GetYaxis()->SetTitle("emcY");
  m_XYwest->GetYaxis()->CenterTitle();  
    
  HistName=HistName0+name;
  if(m_thistsvc->regHist(HistName,  m_XYwest).isFailure()){
    log << MSG::ERROR << "Couldn't register" <<name<< endreq;
  }

 // use EmcCalibConstSvc
  StatusCode scCalib; 
  scCalib = Gaudi::svcLocator() -> service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if( scCalib != StatusCode::SUCCESS){
    log << MSG::ERROR << "can not use EmcCalibConstSvc" << endreq;
  } 
 

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_EMC::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  m_runNo=eventHeader->runNumber();
  m_event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = "
      << m_runNo << " , "
      << m_event <<endreq;


  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);

  SmartDataPtr<DQAEvent::DQAEvent> dqaevt(eventSvc(), "/Event/DQATag");
  if ( dqaevt ) {
      log << MSG::INFO << "success get DQAEvent" << endreq;
  } else {
      log << MSG::ERROR << "Error accessing DQAEvent" << endreq;
      return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "event tag = " << dqaevt->EventTag() << endreq;

  // get the required control sample with DQA tag
  if ( dqaevt->Bhabha() ) {
      log << MSG::INFO << "Bhabha event" << endreq;
      SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);


      double eneShower,theta,phi,costheta;
      RecEmcID showerId;
      unsigned int npart;
      int ntheta,nphi;
      char Name[60];
      std::string HistName;

      for(int i = 0; i < evtRecEvent->totalCharged(); i++){
          EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;

          log << MSG::DEBUG << i << " " << (*itTrk)->partId() << " "
              << (*itTrk)->quality() << endreq;
          // get the required particle through the track's PID
          // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
//          if ( (*itTrk)->partId() != 1 ) continue; // only e+, e-
          if ( ! (*itTrk)->isElectron() ) continue;
          // if you want to do dE/dx or TOF study, select track with no bias
          // Quality: defined by whether dE/dx or TOF is used to identify particle
          // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
          // 1 - only dE/dx (can be used for TOF calibration)
          // 2 - only TOF (can be used for dE/dx calibration)
          // 3 - Both dE/dx and TOF          
      //    int qual = (*itTrk)->quality();
      //    if ( qual != 0 && qual != 2) continue; // no dE/dx PID is used in selection
//          if ( qual != 0 && qual != 1) continue; // no TOF PID is used in the selection

          RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
          if ( mdcTrk->charge() > 0 ) {
              log << MSG::DEBUG << "is electron" << endreq;
          } else {
              log << MSG::DEBUG << "is positron" << endreq;
          }

	  if((*itTrk)->isEmcShowerValid()) {  
	    
	    RecEmcShower *theShower = (*itTrk)->emcShower();
	    eneShower=theShower->energy();   //corrected energy unit GeV
	    theta = theShower->theta();
	    phi= theShower->phi();
	    costheta=cos(theta);
	    // showerId = theShower->getShowerId(); // only in the rec data file
	    showerId = RecEmcID(theShower->cellId());
	    
	    npart = EmcID::barrel_ec(showerId);
	    
	    //The theta number is defined by Endcap_east(0-5),Barrel(0-43),Endcap_west(0-5) 
	    //module is defined  by Endcap_east(0),Barrel(1),Endcap_west(2)      
	    
	    ntheta=EmcID::theta_module(showerId);
	    
	    nphi=EmcID::phi_module(showerId);  
            log << MSG::DEBUG << "test program " << npart << " " << ntheta << " " << nphi << endmsg;

	    m_npart = npart;
	    m_ntheta = ntheta;
	    m_nphi = nphi;
	    m_ixtal=m_emcCalibConstSvc->getIndex(npart,ntheta,nphi);
	    m_theta = theta;
	    m_phi = phi;
	    m_emcX = theShower->x();
	    m_emcY = theShower->y();
	    m_eSeed = theShower->eSeed();
	    m_e5x5 = theShower->e5x5();
	    m_energy = theShower->energy();
	    m_time = theShower->time();


	    TH1 *hmom(0);
  


	    sprintf( Name, "/DQAHist/EMC/EMC_Bhabha_Time-T0");      
	    HistName=Name;
	    if (m_thistsvc->getHist(HistName, hmom).isSuccess()) {
	      hmom->Fill(m_time);
	    } else {
	      log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	    }
  
	    sprintf( Name, "/DQAHist/EMC/EMC_Bhabha_ixtal");      
	    HistName=Name;
	    if (m_thistsvc->getHist(HistName, hmom).isSuccess()) {
	      hmom->Fill(m_ixtal);
	    } else {
	      log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	    }


	    m_eSeedIxtal->Fill(m_ixtal,m_eSeed);




	    if (npart==1){

	      sprintf( Name, "/DQAHist/EMC/EMC_Bhabha_ShowerEneBarrelVsEvent"); 
	      HistName=Name;
 	      if (m_thistsvc->getHist(HistName, hmom).isSuccess()) {
		hmom->Fill(eneShower);
	      } else {
                log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	      }

	      sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerPhiBarrelVsEvent");  
	      HistName=Name; 
	      if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {    
		hmom->Fill(nphi);
	      } else {
                log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	      }
	      
	    }
	    
	    if (npart==0){
	      
	      sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerEneEastVsEvent"); 
	      HistName=Name;
	      if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {    
		hmom->Fill( eneShower);
	      } else {
                log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	      }
	 
	      sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerPhiEastVsEvent"); 
	      HistName=Name;
	      if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {    
		hmom->Fill(phi);
	      } else {
                log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	      }
 
	      m_XYeast->Fill(m_emcX,m_emcY);

	    }
	    if (npart==2){
  
	      sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerEneWestVsEvent");   
	      HistName=Name;
	      if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {  
		hmom->Fill( eneShower);
	      } else {
                log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	      }

	      sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerPhiWestVsEvent");  
	      HistName=Name;
	      if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {  
		hmom->Fill(phi);
	      } else {
                log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	      }

	      m_XYwest->Fill(m_emcX,m_emcY);

	    }

	    int m_nthe=-9;
	    if (npart==0){
	      m_nthe = ntheta;
	    }
	    if (npart==2){
	      m_nthe = 55-ntheta;
	    }
	    if (npart==1){
	      m_nthe = ntheta+6;
	    }

	    sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerThetaVsvent");      
	    HistName=Name;
	    if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {  
	      hmom->Fill(m_nthe);
	    } else {
	      log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	    }

	    sprintf( Name,"/DQAHist/EMC/EMC_Bhabha_ShowerCosTheta"); 
	    HistName=Name;
	    if (m_thistsvc->getHist(HistName, hmom ).isSuccess()) {  
	      hmom->Fill(costheta);
	    } else {
	      log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
	    }

//             TH2 *hmom2(0);
// 	    sprintf(  Name, "/DQAHist/EMC/EMC_Bhabha_ShowerThetaPhi");  
// 	    HistName=Name;
// 	    if (m_thistsvc->getHist(HistName, hmom2 ).isSuccess()) {  
// 	      hmom2->Fill(phi,theta);
// 	    } else {
// 	      log << MSG::ERROR << "Couldn't retrieve" <<HistName<< endreq;
// 	    }
            m_ThetaPhi->Fill(phi, theta);

	     if(m_NtupleOutput==1)  m_tuple->write();   
            
	  }
      }
  }
 
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_EMC::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


