#include "BesSim.hh"
#include "BesDetectorConstruction.hh"
#include "ReadBoostRoot.hh"
#include "BesMdcGeoParameter.hh"
#include "BesPhysicsList.hh"
#include "BgsPhysicsList.hh" //caogf
#include "BesSensitiveManager.hh"
#include "BesPrimaryGeneratorAction.hh"
#include "BesRunAction.hh"
#include "BesEventAction.hh"
#include "BesSteppingAction.hh"
#include "BesTrackingAction.hh"
//#include "BesVisManager.hh"
//#include "G4VisManager.hh"
#include "QGSP.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_CHIPS.hh"
#include "QGSP_BERT_HP.hh"
#include "QBBC.hh"
#include "CHIPS.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"
#include "GaudiKernel/NTuple.h"
#include "BesKernel/IBesRndmGenSvc.h" 

#include "G4Writer/G4GDMLWriter.h"
//caogf for random seed
#include "CLHEP/Random/Random.h"
//sunss for detector version
#include "DetVerSvc/IDetVerSvc.h"




//static const AlgFactory<BesSim>    xxBAFactory;
//const IAlgFactory& BesSimFactory = xxBAFactory;

/////////////////////////////////////////////////////////////////////////////

BesSim::BesSim(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  declareProperty("Mdc",m_mdc=2);
  declareProperty("Tof",m_tof=2);
  declareProperty("Emc",m_emc=2);
  declareProperty("Muc",m_muc=2);
  declareProperty("PipeSCM",m_pipeSCM=2);
  declareProperty("Field",m_field=1);
  declareProperty("PhysicsList",m_physicsList=0);
  declareProperty("FormatAR", m_formatAR =0);
  declareProperty("Tuning",m_tuning=0);
  declareProperty("TuningFile",m_tuningFile);
  declareProperty("HitOut",m_hitOut=0);
  declareProperty("TDSFlag",m_TDSFlag=false);
  declareProperty("AsciiFlag",m_asciiFlag="00000000");
  declareProperty("AsciiFile",m_asciiFile="boost.dat");
  declareProperty("RootFlag",m_rootFlag=false);
  declareProperty("RootFile",m_rootFile="Hit.root");
  declareProperty("CutValue", newcut = 0.7);//unite is mm
  declareProperty("LogLevel",m_logLevel=5000);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode BesSim::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;
  m_nEvent = 0;
  m_nRun = 1;

  //caogf for random seed
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
  {
     log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
      return RndmStatus;
  }

  StatusCode status;
  IG4Svc *tmpSvc;
  status = service("G4Svc",tmpSvc);
  if (status.isSuccess()) {
    log << MSG::INFO << "got the G4Svc" << endreq;
    m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc);
  } else { 
    log << MSG::ERROR << "could not get the G4Svc" << endreq;
    return StatusCode::FAILURE;
  }

  IDetVerSvc*  detVerSvc;
  StatusCode sc_det = service("DetVerSvc", detVerSvc);
  if( sc_det.isFailure() ) {
    log << MSG::ERROR << "can't retrieve DetVerSvc instance" << endreq;
    return sc_det;
  }

  int phase = detVerSvc->phase();
  log << MSG::INFO << "** ~~~ZZZ~~~ ** : retrieved DetectorStage = " << phase << endreq;

  // phase 1: Endcap TOF Scintillator; phase 2: 4 Scintillator==> 2 MRPC; phase 3: Endcap TOF MRPC
  if( phase>2 ) { m_tof = 4; }

  //book Ntuple
  if(m_G4Svc->MdcRootFlag())
    bookMdcRootFile();
  
  if(m_G4Svc->TofRootFlag())
    bookTofRootFile();
  
  if(m_G4Svc->EmcRootFlag())
    bookEmcRootFile();

  if(m_G4Svc->MucRootFlag())
    bookMucRootFile();

  log << MSG::INFO << "Four sub-detector construction flags: " <<m_mdc<<" "<<m_tof<<" "<<m_emc<<" "<<m_muc<<endreq;
  
  ReadBoostRoot* readBoost =new ReadBoostRoot(m_mdc, m_tof,m_emc, m_muc, m_field,m_formatAR,m_tuning,m_hitOut);
  BesMdcGeoParameter * mdcGeoService;
  mdcGeoService = new BesMdcGeoParameter;
  
  log << MSG::INFO << "Instantiating BESIII Detector" << endreq;
 
  BesSensitiveManager* sensitiveManager = new BesSensitiveManager;
  sensitiveManager->SetLogLevel(m_G4Svc->LogLevel());

  BesDetectorConstruction* detectorConstruction = new BesDetectorConstruction();
  detectorConstruction->SetPipeSCM(m_pipeSCM);  
  m_G4Svc->SetUserInitialization(detectorConstruction);

  switch(m_physicsList)
  {
    case 1:
      m_G4Svc->SetUserInitialization(new BesPhysicsList);
      break;
    case 2:
      m_G4Svc->SetUserInitialization(new QGSP);
      break;
    case 3:
      m_G4Svc->SetUserInitialization(new QGSP_BIC);
      break;
    case 4:
      m_G4Svc->SetUserInitialization(new QGSP_BERT);
      break;
    case 5:
      m_G4Svc->SetUserInitialization(new QGSP_BERT_HP);
      break;
    case 6:
      m_G4Svc->SetUserInitialization(new BgsPhysicsList);
      break;
    case 7:
      m_G4Svc->SetUserInitialization(new CHIPS);
      break;
    case 8:
      m_G4Svc->SetUserInitialization(new QBBC);
      break;
    case 9:
      m_G4Svc->SetUserInitialization(new QGSP_BERT_CHIPS);
      break; 
    default:
      m_G4Svc->SetUserInitialization(new QGSP_BERT);
      break;
  }
  m_G4Svc->SetUserAction(new BesPrimaryGeneratorAction);
  BesRunAction* runAction = new BesRunAction;
  runAction->SetTDSFlag(m_TDSFlag);
  runAction->SetAsciiFlag(m_asciiFlag);
  runAction->SetAsciiFile(m_asciiFile);
  runAction->SetRootFlag(m_rootFlag);
  runAction->SetRootFile(m_rootFile);
  vector<string>::iterator pd;
  for (pd = m_tuningFile.begin(); pd != m_tuningFile.end(); pd++){
    std::cout << "tuning File: " << *pd << std::endl;
  }

  runAction->SetTuningFile(m_tuningFile);
  m_G4Svc->SetUserAction(runAction);
  
  BesEventAction* evtAction = new BesEventAction(runAction);
  m_G4Svc->SetUserAction(evtAction);
  
  m_G4Svc->SetUserAction(new BesSteppingAction);
  
  m_G4Svc->SetUserAction(new BesTrackingAction(runAction) );
  
//#ifdef G4VIS_USE
//visualization manager
//  G4VisManager* visManager = new BesVisManager;
//  visManager->Initialize();
//#endif

  
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode BesSim::beginRun()
{
  static int flag = -1;
  if(flag<0)
  {
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "\t start initializing G4 "<<endreq;
    //new visMgr,  StartUISession(),  runMgr->Initialize()
    m_G4Svc->G4Init();

    //to do: runMgr->RunInitialization()
    m_G4Svc->RunInitialize();
   
    //G4VPhysicalVolume* g4wv =G4TransportationManager::GetTransportationManager()-> GetNavigatorForTracking()->GetWorldVolume(); //Apply this for storing TOF GDML
    //G4GDMLWriter g4writerBes("/afs/ihep.ac.cn/bes3/offline/sw/packages/BesGDML/2.8.0/GDMLSchema/gdml.xsd", "Bes.gdml",2);
    //G4GDMLWriter g4writerMdc("/afs/ihep.ac.cn/bes3/offline/sw/packages/BesGDML/2.8.0/GDMLSchema/gdml.xsd", "Mdc.gdml",2);
    //G4GDMLWriter g4writerTof("/afs/ihep.ac.cn/bes3/offline/sw/packages/BesGDML/2.8.0/GDMLSchema/gdml.xsd", "Tof.gdml",2);//Apply this for storing TOF GDML
    //G4GDMLWriter g4writerEmc("/afs/ihep.ac.cn/bes3/offline/sw/packages/BesGDML/2.8.0/GDMLSchema/gdml.xsd", "Emc.gdml",2);
    //G4GDMLWriter g4writerMuc("/afs/ihep.ac.cn/bes3/offline/sw/packages/BesGDML/2.8.0/GDMLSchema/gdml.xsd", "Muc.gdml",2);
    //try
    //{
    //g4writerBes.DumpGeometryInfo(g4wv);
    //g4writerTof.DumpGeometryInfo(g4wv->GetLogicalVolume()->GetDaughter(0)); //Changed from 0 to 3//Apply this for storing TOF GDML

    //}
    //catch(std::logic_error &lerr)
    //{
    //  std::cout << "Caught an exception: "
    //    << lerr.what () << std::endl;
    //}

    //For random seed added by caogf. Note the position of the code, otherwise it is not available.
    CLHEP::HepRandomEngine* engine = p_BesRndmGenSvc->GetEngine("SIM");
    HepRandom::setTheEngine(engine);
    HepRandom::showEngineStatus();
  }
  return StatusCode::SUCCESS;
}

StatusCode BesSim::bookMdcRootFile()
{
  MsgStream log(msgSvc(), name());
  NTuplePtr nt(ntupleSvc(), "FILE801/n1");
  NTuple::Tuple* tupleMdc;
  if(nt) tupleMdc = nt;
  else {
    tupleMdc = ntupleSvc()->book("FILE801/n1",CLID_ColumnWiseTuple,"BesSim");
    m_G4Svc->SetTupleMdc(tupleMdc);
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode BesSim::bookTofRootFile()
{
  MsgStream log(msgSvc(), name());
  NTuplePtr nt1(ntupleSvc(), "FILE802/n1");
  NTuple::Tuple* tupleTof1;
  if(nt1) tupleTof1 = nt1;
  else {
    tupleTof1 = ntupleSvc()->book("FILE802/n1",CLID_ColumnWiseTuple,"BesSim");       
    m_G4Svc->SetTupleTof1(tupleTof1);
  }
  NTuplePtr nt2(ntupleSvc(), "FILE802/n2");
  NTuple::Tuple* tupleTof2;
  if(nt2) tupleTof2 = nt2;
  else {
    tupleTof2 = ntupleSvc()->book("FILE802/n2",CLID_ColumnWiseTuple,"BesSim");       
    m_G4Svc->SetTupleTof2(tupleTof2);
  }
  NTuplePtr nt3(ntupleSvc(), "FILE802/n3");
  NTuple::Tuple* tupleTof3;
  if(nt3) tupleTof3 = nt3;
  else {
    tupleTof3 = ntupleSvc()->book("FILE802/n3",CLID_ColumnWiseTuple,"BesSim");       
    m_G4Svc->SetTupleTof3(tupleTof3);
  }
  /*
  NTuplePtr nt4(ntupleSvc(), "FILE802/n4");
  NTuple::Tuple* tupleTof4;
  if(nt4) tupleTof4 = nt4;
  else {
    tupleTof4 = ntupleSvc()->book("FILE802/n4",CLID_ColumnWiseTuple,"BesSim");
    m_G4Svc->SetTupleTof4(tupleTof4);
  }
  */
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode BesSim::bookEmcRootFile()
{
  NTuplePtr nt1(ntupleSvc(), "FILE803/n1");
  NTuple::Tuple* tupleEmc1;
  if(nt1) tupleEmc1 = nt1;
  else {
    tupleEmc1 = ntupleSvc()->book("FILE803/n1",CLID_ColumnWiseTuple,"BesSim");       
    m_G4Svc->SetTupleEmc1(tupleEmc1);
  }
  
  NTuplePtr nt2(ntupleSvc(), "FILE803/n2");
  NTuple::Tuple* tupleEmc2;
  if(nt2) tupleEmc2 = nt2;
  else {
    tupleEmc2 = ntupleSvc()->book("FILE803/n2",CLID_ColumnWiseTuple,"BesSim");       
    m_G4Svc->SetTupleEmc2(tupleEmc2);
  }

  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
StatusCode BesSim::bookMucRootFile()
{
  MsgStream log(msgSvc(), name());
  NTuplePtr nt(ntupleSvc(), "FILE804/n1");
  NTuple::Tuple* tupleMuc;
  if(nt) tupleMuc = nt;
  else {
    tupleMuc = ntupleSvc()->book("FILE804/n1",CLID_ColumnWiseTuple,"BesSim");
    m_G4Svc->SetTupleMuc(tupleMuc);
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode BesSim::execute() {

  MsgStream log(msgSvc(), name());
  
  //log << MSG::INFO << "event ID:"<<m_nEvent<<endreq;
  if(m_nEvent%m_logLevel == 0)
    std::cout<<" BesSim Begin of Event: "<<m_nEvent<<std::endl;
  
  m_G4Svc->SimulateEvents(m_nEvent);
  m_nEvent++;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode BesSim::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;
  log << MSG::DEBUG  << "\t terminating the current G4 run"<<endreq;
  m_G4Svc->RunTerminate();
  
  std::cout<<"BesSim::finalize(), total events in this run: "<<m_nEvent<<std::endl;

  if(m_G4Svc->runMgr() != 0) 
    delete m_G4Svc->runMgr();

#ifdef G4VIS_USE
  if(m_G4Svc->visMgr() != 0)
    delete m_G4Svc->visMgr();
#endif


  return StatusCode::SUCCESS;
}
