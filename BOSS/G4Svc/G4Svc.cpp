//------------------------------------------------------------------
//
// ClassName:   G4Svc
//  
// Description: This service provides access to Geant 4
//
// Based on Athena software
//
//------------------------------------------------------------------

//Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "G4Svc/G4Svc.h"
#include "G4Svc/G4SvcRunManager.h"
#ifdef G4VIS_USE
 #include "G4Svc/G4SvcVisManager.h"
 #include "G4VisExecutive.hh"
#endif


// GEANT
#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4RunManager.hh"
#include "G4UItcsh.hh"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <strstream>

//*************************************************************************//

G4Svc::G4Svc( const std::string& name, ISvcLocator* svc ):
      Service( name, svc ), p_runMgr(0), p_uiMgr(0), p_visMgr(0),
      _init(false), m_beamTime(0)
{
  //declareProperty( "PhysicsList", m_PhysicsList="none");
  //declareProperty( "DefaultPhysicsCut",  m_PhysicsCut=-1.);
  declareProperty( "Visualize", m_vis=false);
  //declareProperty( "VisType", m_visType="VRML1FILE");

  declareProperty( "RunVerbosity", m_runVerb=0);
  declareProperty( "EventVerbosity", m_eventVerb=0);
  declareProperty( "TrackingVerbosity", m_trackVerb=0);
  
  declareProperty( "FADSmacro",m_macroName="none" );
  declareProperty( "MdcNoiseFile", m_mdcNoiseFile="none");

  //declareProperty( "SaveHits", m_saveHits=true);
  //declareProperty( "SaveTracks", m_saveTracks=false);
  declareProperty( "LogLevel", m_logLevel=6);
  
  declareProperty( "InteractiveG4" , m_interactiveG4=false);

  declareProperty("BesGenAction", m_besGenAction=false);

  declareProperty("BeamPosX",m_beamPosX=0);
  declareProperty("BeamPosY",m_beamPosY=0);
  declareProperty("BeamPosZ",m_beamPosZ=0);

  declareProperty("BeamSizeX", m_beamSizeX=10);
  declareProperty("BeamSizeY", m_beamSizeY=20);
  declareProperty("BeamSizeZ", m_beamSizeZ=30);

  declareProperty("BeamStartTime",m_beamStartTime=0);
  declareProperty("BeamDeltaTime",m_beamDeltaTime=0);
  declareProperty("NBunch",m_nBunch=1);

  declareProperty("BunchTimeSigma",m_bunchTimeSigma=0);

  declareProperty("MdcDataInput",m_mdcDataInput=1);  
  declareProperty("MdcDedxFlag",m_mdcDedxFlag=1);  

  declareProperty("BoostLab",m_boostLab=false); 
  declareProperty("BeamAngle",m_beamAngle=11);
   
  declareProperty("SetBeamShift",m_setBeamShift= false);
  declareProperty("BeamShiftPx" ,m_beamShiftPx=39.45);  // in unit MeV
  declareProperty("BeamShiftPy" ,m_beamShiftPy=-0.98);
  declareProperty("BeamShiftPz" ,m_beamShiftPz=3.70); 
   
  declareProperty("MdcRootFlag",m_mdcRootFlag=false);
  declareProperty("TofRootFlag",m_tofRootFlag=false);
  declareProperty("EmcRootFlag",m_emcRootFlag=false);
  declareProperty("MucRootFlag",m_mucRootFlag=false);
  
  // Switch for ADC Saturation in Bhabha events
  declareProperty("TofSaturationFlag",m_tofSaturationFlag=true); 
  
  declareProperty("EmcLightOutput",m_emcLightOutput=true);
  declareProperty("EmcIncoherentNoise",m_emcIncoherentNoise=0.2);
  declareProperty("EmcCoherentNoise",m_emcCoherentNoise=0.);
  declareProperty("EmcNoiseMean",m_emcNoiseMean=1.16);
  declareProperty("EmcNoiseSigma",m_emcNoiseSigma=0.23);
  declareProperty("EmcNoiseThreshold",m_emcNoiseThreshold=0.5);
  declareProperty("EmcNoiseLevel",m_emcNoiseLevel=3);
  declareProperty("EmcTime",m_emcTime=20);
  declareProperty("EmcELecSaturation",m_emcELecSaturation=1);

  declareProperty("MucNoiseMode", m_mucNoiseMode=1);
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

G4Svc::~G4Svc() {
//  std::cout<<"G4Svc::~G4Svc()"<<std::endl;
//  if (p_runMgr != 0) {
//    delete p_runMgr;
//  }
//#ifdef G4VIS_USE
//  if (p_visMgr != 0) {
//    delete p_visMgr;
//  }
//#endif

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
G4RunManager* G4Svc::runMgr() { 
  //std::cout<<"G4Svc::runMgr()"<<std::endl;
  MsgStream log( msgSvc(), name() );

  if (p_runMgr == 0) {
    p_runMgr = new G4SvcRunManager();
    p_runMgr->SetLogLevel( m_logLevel );
    //p_runMgr->SetSaveHits( m_saveHits );
    //p_runMgr->SetSaveTracks( m_saveTracks );
  }

  return p_runMgr; 

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

StatusCode G4Svc::initialize() {

  MsgStream log( msgSvc(), name() );

  StatusCode status = Service::initialize();
  log << MSG::INFO << "Service initialized" << endreq;
  
  //get IRealizationSvc
  IRealizationSvc *tmpReal;
  status = service("RealizationSvc", tmpReal);
  if (!status.isSuccess())
  {
     log << MSG::ERROR << " Could not initialize Realization Service" << endreq;
     return status;
  } else {
    log << MSG::INFO << "got the RealizationSvc" << endreq;
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

  m_runID = m_RealizationSvc->getRunId();

  runMgr();
  return status;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

StatusCode G4Svc::finalize() {

  MsgStream log( msgSvc(), name() );
  StatusCode status = Service::finalize();

  if ( status.isSuccess() )
    log << MSG::INFO << "Service finalised successfully" << endreq;
  
  return status;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
StatusCode G4Svc::queryInterface( const InterfaceID& riid, 
					void** ppvInterface ) {
  StatusCode sc = StatusCode::FAILURE;

  if (riid == IID_IG4Svc) {
    *ppvInterface = dynamic_cast<IG4Svc*>(this);
    sc = StatusCode::SUCCESS;
  } else {
    sc = Service::queryInterface( riid, ppvInterface );
  }
  return sc;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
const G4Event* G4Svc::GetCurrentEvent() const {

  const G4Event *evt = p_runMgr->GetCurrentEvent();

  if (evt == 0) {
    MsgStream log( msgSvc(), name() );
    log << MSG::ERROR << "Could not get current G4Event" << endreq;
  }

  return ( evt );

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
const G4Run* G4Svc::GetCurrentRun() const {

  return ( p_runMgr->GetCurrentRun() );

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
G4UImanager* G4Svc::uiMgr() {
  if (p_uiMgr == 0) {
    p_uiMgr = G4UImanager::GetUIpointer();
  }
  return ( p_uiMgr );
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserInitialization(G4VUserDetectorConstruction* userInit) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserInitialization(userInit);
  }
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserInitialization(G4VUserPhysicsList* userInit) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserInitialization(userInit);
  }
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserAction(G4UserRunAction *action) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserAction(action);
  }
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserAction(G4UserEventAction *action) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserAction(action);
  }
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserAction(G4VUserPrimaryGeneratorAction *action) {
  if (p_runMgr != 0) {
    if(m_besGenAction)
      p_runMgr->SetUserAction( action );
  }
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserAction(G4UserStackingAction *action) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserAction(action);
  }
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserAction(G4UserTrackingAction *action) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserAction(action);
  }
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::SetUserAction(G4UserSteppingAction *action) {
  if (p_runMgr != 0) {
    p_runMgr->SetUserAction(action);
  }
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::StartUISession() {

  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Starting G4 terminal" << endreq;

  G4UIsession * ses = new G4UIterminal(new G4UItcsh);
  ses->SessionStart();
  
  delete ses;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

void G4Svc::G4Init() {

  MsgStream log( msgSvc(), name() );
  
  log << MSG::VERBOSE << "Attempting G4 Initialization" << endreq;
  
  if (!_init) {
    // Tracking
    //if (m_saveTracks) {
    //  log << MSG::INFO << "turning tracking on" << std::endl;
    //  uiMgr()->ApplyCommand("/tracking/storeTrajectory 1");
    //}

    // Visualization manager
#ifdef G4VIS_USE
    if (m_vis) {
      log << MSG::INFO << "Initializing G4SvcVisManager" << endreq;
      //p_visMgr = new G4SvcVisManager;
      p_visMgr = new G4VisExecutive;
      p_visMgr->Initialize();
    }
#else
    m_vis = false;
#endif
    
    if (!m_vis) {
      log << MSG::INFO << "Not including Visualization" << endreq;
    }

    // Verbosity
    log << MSG::DEBUG << "Setting verbosity levels - " 
	      << "run: " << m_runVerb << "  event: " << m_eventVerb
	      << "  track: " << m_trackVerb << endreq;
    
    {
      std::ostrstream ost;
      ost << "/run/verbose " << m_runVerb << std::ends;
      uiMgr()->ApplyCommand(ost.str());
    }
    {
      std::ostrstream ost;
      ost << "/event/verbose " << m_eventVerb << std::ends;
      uiMgr()->ApplyCommand(ost.str());
    }
    {
      std::ostrstream ost;
      ost << "/tracking/verbose " << m_trackVerb << std::ends;
      uiMgr()->ApplyCommand(ost.str());
    }
    
    log << MSG::DEBUG << "Initializing G4 kernel" << endreq;
    p_runMgr->Initialize();
    _init = true;

    if (m_macroName!="none"){
      log << MSG::INFO << "Running G4 macro "<<m_macroName<< endreq;
      uiMgr()->ApplyCommand("/control/execute "+m_macroName);
    }
    if (m_interactiveG4) {
      StartUISession();
    }
    
  }else {
    log << MSG::ERROR << "G4 kernel already initialized" << endreq;
  }
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
void G4Svc::RunInitialize()
{
	bool cond = p_runMgr->ConfirmBeamOnCondition();
 	if(cond)
 	{
		std::cout<<"########### initializing the G4Run ############"<<
		std::endl;
 		p_runMgr->SetRunIDCounter( -std::abs(m_runID) );
    p_runMgr->RunInitialization();
	}
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
void G4Svc::RunTerminate()
{
	p_runMgr->RunTermination();
}
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
void G4Svc::SimulateEvents(int i)
{
	p_runMgr->SimulateEvent(i);
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
