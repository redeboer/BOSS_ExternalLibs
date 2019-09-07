#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "EventModel/EventHeader.h"
#include "ReconEvent/ReconEvent.h"
#include "ExtEvent/RecExtTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "EmcRecEventModel/RecEmcEventModel.h"   //2006.11.11
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MucRawEvent/MucDigi.h"
#include "McTruth/McKine.h"
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/MucMcHit.h"
#include "McTruth/McRelTableDefs.h"
#include "Identifier/Identifier.h"
#include "Identifier/MucID.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "MucRecAlg/MucRecTrkExt.h"
#include "MucRecAlg/MucRecTrkExtParameter.h"
#include "MucGeomSvc/IMucGeomSvc.h"
#include "MucGeomSvc/MucGeoGeneral.h"
#include "MucGeomSvc/MucGeoGap.h"
#include "MucGeomSvc/MucGeoStrip.h"
#include "MucGeomSvc/MucConstant.h"
#include "MucRecEvent/MucRecHit.h"
#include "MucRecEvent/MucRecHitContainer.h"
#include "MucRecEvent/RecMucTrack.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

MucRecTrkExt::MucRecTrkExt(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
  declareProperty("ExtTrackSeedMode", m_ExtTrackSeedMode = 1); // 1: My ext from Mc track, 2: from Ext track, 3: from MUC for cosmic ray
  declareProperty("CompareWithMcHit", m_CompareWithMcHit = 0); 
  declareProperty("FittingMethod",    m_fittingMethod = 1);
  declareProperty("EmcShowerSeedMode",m_EmcShowerSeedMode = 0); // 0: Not use EmcShower as seed, 1: use...
  declareProperty("MucHitSeedMode",   m_MucHitSeedMode = 0); // 0: Not use MucHit as seed,1 use... 
  declareProperty("ConfigFile",       m_configFile = "MucConfig.xml");
  declareProperty("Blind",            m_Blind = false);
  declareProperty("NtOutput",         m_NtOutput = 0); // NTuple save to root or not
  declareProperty("MsOutput",         m_MsOutput = false); // Debug message cout or not
  declareProperty("FilterFile", m_filter_filename);
  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MucRecTrkExt::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    log << MSG::WARNING << " Could not initialize Particle Properties Service" << endreq;
    return PartPropStatus;
  }
  m_particleTable = p_PartPropSvc->PDT();
  
  m_totalEvent          = 0;
  
  m_NDigisTotal         = 0;
  m_NHitsTotal          = 0;
  m_NHitsFoundTotal     = 0;
  m_NHitsLostTotal      = 0;
  m_NHitsMisFoundTotal  = 0;
  m_NHitsLostByMdcTotal = 0;  
  m_NHitsLostByExtTotal = 0;  

  m_NTracksTotal     = 0;
  m_NTracksRecoTotal = 0;
  m_NTracksLostByMdcTotal = 0;
  m_NTracksLostByExtTotal = 0;
  m_NTracksMdcGhostTotal  = 0;

  for(int i = 0; i < 20; i++) m_NHitsLost.push_back(0);
  for(int i = 0; i < 10; i++) m_NHitsLostInGap.push_back(0);

  IMucGeomSvc* mucGeomSvc;
  StatusCode sc = service("MucGeomSvc", mucGeomSvc);
  if (sc ==  StatusCode::SUCCESS) {              
    //cout <<"dump"<<endl;
   mucGeomSvc->Dump();
   //cout<<"Hi, event routine is running"<<endl;
  } else {
    return StatusCode::FAILURE;
  }
  m_MucRecHitContainer = 0;

  //--------------book ntuple------------------
  //  NTuplePtr nt1(ntupleSvc(), "MyTuples/1");
  if(m_NtOutput>=1){
    NTuplePtr nt1(ntupleSvc(), "FILE401/T");

    if ( nt1 ) { m_tuple = nt1;}
    else {
      //    m_tuple = ntupleSvc()->book ("MyTuples/1", CLID_RowWiseTuple, "MdcTrkRecon N-Tuple");
      m_tuple = ntupleSvc()->book ("FILE401/T", CLID_RowWiseTuple, "MucTrkRecon N-Tuple");
      if ( m_tuple )    {
        sc = m_tuple->addItem ("posx",    m_posx);
        sc = m_tuple->addItem ("posy",    m_posy);
        sc = m_tuple->addItem ("posz",    m_posz);
        sc = m_tuple->addItem ("posx_ext",    m_posx_ext);
        sc = m_tuple->addItem ("posy_ext",    m_posy_ext);
        sc = m_tuple->addItem ("posz_ext",    m_posz_ext);
        sc = m_tuple->addItem ("posxsigma",   m_posx_sigma);
        sc = m_tuple->addItem ("posysigma",   m_posy_sigma);
        sc = m_tuple->addItem ("poszsigma",   m_posz_sigma);
        sc = m_tuple->addItem ("Depth",   m_depth);
        sc = m_tuple->addItem ("Distance",m_Distance);
        sc = m_tuple->addItem ("MaxHits", m_MaxHits);
        sc = m_tuple->addItem ("Chi2",    m_Chi2);
        sc = m_tuple->addItem ("Dist_x",  m_Dist_x);
        sc = m_tuple->addItem ("Dist_y",  m_Dist_y);
        sc = m_tuple->addItem ("Dist_z",  m_Dist_z);
        sc = m_tuple->addItem ("px_mc",   m_px_mc);
        sc = m_tuple->addItem ("py_mc",   m_py_mc);
        sc = m_tuple->addItem ("pz_mc",   m_pz_mc);
        sc = m_tuple->addItem ("emctrack",m_emctrack);
        sc = m_tuple->addItem ("muchasdigi",m_muc_digi);

        sc = m_tuple->addItem ("part",  m_part);
        sc = m_tuple->addItem ("seg",   m_seg);
        sc = m_tuple->addItem ("gap",   m_gap);
        sc = m_tuple->addItem ("strip", m_strip);
        sc = m_tuple->addItem ("diff",  m_diff);
        sc = m_tuple->addItem ("distance",m_distance);
        sc = m_tuple->addItem ("run",   m_run);
        sc = m_tuple->addItem ("event", m_event);

      }
      else    {   // did not manage to book the N tuple....
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple) << endmsg;
        //return StatusCode::FAILURE;
      }
    }
  }

  // load the filter event
  if ( m_filter_filename == "") {
   m_filter_filename =getenv("MUCRECALGROOT");
   m_filter_filename +="/share/filter.txt";
   }

  if (m_filter_filename.size()) {
    std::ifstream infile(m_filter_filename.c_str());

    while (!infile.eof()) {
      FilterEvent filterevt;
      infile >> filterevt.bossver >> filterevt.runid >> filterevt.eventid;
      if ((!infile.good()) || (infile.eof())) {
        break;
      }

      m_filter_event.push_back(filterevt);
//      cout << filterevt.bossver << " "
//           << filterevt.runid << " "
//           << filterevt.eventid << std::endl;
    }
  }

  //--------------end of book ntuple------------------

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MucRecTrkExt::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
  
  
  StatusCode sc = StatusCode::SUCCESS;

  // Part 1: Get the event header, print out event and run number
  int event, run;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    //return( StatusCode::FAILURE);
  }
  m_totalEvent ++;
  log << MSG::INFO << "Event: "<<m_totalEvent<<"\tcurrent run: "<<eventHeader->runNumber()<<"\tcurrent event: "<< eventHeader->eventNumber()<<endreq;

event = eventHeader->eventNumber();
run = eventHeader->runNumber();

  string release = getenv("BES_RELEASE");
//  if(release=="6.6.5"&&run==35896&&event==7448){return ( StatusCode::SUCCESS);}
//  if(release=="6.6.5"&&run==37241&&event==1690731){return ( StatusCode::SUCCESS);}
  // filter the event
  for (std::vector<FilterEvent>::iterator it = m_filter_event.begin();
         it != m_filter_event.end(); ++it) {
    const FilterEvent& fe = (*it);
    if (release == fe.bossver && run == fe.runid && event == fe.eventid) {
      cout << "SKIP: " << fe.bossver << " "
           << fe.runid << " "
           << fe.eventid << std::endl;
      return StatusCode::SUCCESS;
    }
  }

  //Part 2: Retrieve MC truth 
  if(m_CompareWithMcHit==1){
    SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if (!mcParticleCol) {
      log << MSG::FATAL << "Could not find McParticle" << endreq;
      //return( StatusCode::FAILURE);
    }

    McParticleCol::iterator iter_mc = mcParticleCol->begin();
    //do not loop; just get first particle info

    //if(!(*iter_mc)->primaryParticle()) continue;
    int pid = (*iter_mc)->particleProperty();
    int charge = 0;
    double mass = -1.0; 

    if( pid >0 ) 
    { 
      if(m_particleTable->particle( pid )){
        charge = (int)m_particleTable->particle( pid )->charge();
        mass = m_particleTable->particle( pid )->mass();
      }
      else log << MSG::ERROR << "no this particle id in particle table, please check data" <<endreq;
    } else if ( pid <0 ) {
      if(m_particleTable->particle( -pid )){
        charge = (int)m_particleTable->particle( -pid )->charge();
        charge *= -1;
        mass = m_particleTable->particle( -pid )->mass();
      }
      else log << MSG::ERROR << "no this particle id in particle table, please check data" <<endreq;
    } else {
      log << MSG::WARNING << "wrong particle id, please check data" <<endreq;
    }

    //  if(!charge) {
    // 	  log << MSG::WARNING
    // 	      << "neutral particle charge = 0!!! ...just skip it ! " << endreq;
    // 	  continue;
    //  }

    HepLorentzVector initialMomentum = (*iter_mc)->initialFourMomentum();
    HepLorentzVector initialPos = (*iter_mc)->initialPosition();
    if(m_NtOutput>=1){
      m_px_mc = initialMomentum.px();
      m_py_mc = initialMomentum.py();
      m_pz_mc = initialMomentum.pz();
    }
    //cout<<"mc mom: "<<m_px_mc<<endl;

    log << MSG::INFO << " particleId = " << (*iter_mc)->particleProperty() << endreq;   
  }

  //Part 6: Retrieve MUC digi 
  int digiId = 0;
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc(),"/Event/Digi/MucDigiCol");
  if (!mucDigiCol) {
    log << MSG::FATAL << "Could not find MUC digi" << endreq;
    return( StatusCode::FAILURE);
  }
  MucDigiCol::iterator iter4 = mucDigiCol->begin();
  for (;iter4 != mucDigiCol->end(); iter4++, digiId++) {
  }
  log << MSG::INFO << "Total MUC digis:\t" << digiId << endreq;
  if( digiId == 0 ) return ( StatusCode::SUCCESS);

  // Retrieve MdcMcHit
  /*
     SmartDataPtr<Event::MdcMcHitCol> aMdcMcHitCol(eventSvc(),"/Event/MC/MdcMcHitCol");
     if (!aMdcMcHitCol) {
     log << MSG::WARNING << "Could not find MdcMcHitCol" << endreq;
  //return( StatusCode::FAILURE);
  }
  //log << MSG::DEBUG << "MdcMcHitCol contains " << aMdcMcHitCol->size() << " Hits " << endreq;

  // Retrieve TofMcHit 
  SmartDataPtr<Event::TofMcHitCol> aTofMcHitCol(eventSvc(),"/Event/MC/TofMcHitCol");
  if (!aTofMcHitCol) {
  log << MSG::WARNING << "Could not find TofMcHitCol" << endreq;
  //return( StatusCode::FAILURE);
  }
  //log << MSG::DEBUG << "TofMcHitCol contains " << aTofMcHitCol->size() << " Hits " << endreq;

  // Retrieve EmcMcHit 
  SmartDataPtr<Event::EmcMcHitCol> aEmcMcHitCol(eventSvc(),"/Event/MC/EmcMcHitCol");
  if (!aEmcMcHitCol) {
  log << MSG::WARNING << "Could not find EmcMcHitCol" << endreq;
  //return( StatusCode::FAILURE);
  }
  //log << MSG::DEBUG << "EmcMcHitCol contains " << aEmcMcHitCol->size() << " Hits " << endreq;
   */

  Identifier mucID;

  //McPartToMucHitTab assocMcMuc;
  //assocMcMuc.init();

  if (m_CompareWithMcHit) {
    McPartToMucHitTab assocMcMuc;
    assocMcMuc.init();

    SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if (!mcParticleCol) {
      log << MSG::FATAL << "Could not find McParticle" << endreq;
      //return( StatusCode::FAILURE);
    }
    McParticleCol::iterator iter_mc = mcParticleCol->begin();

    // Retrieve MucMcHit 
    SmartDataPtr<Event::MucMcHitCol> aMucMcHitCol(eventSvc(),"/Event/MC/MucMcHitCol");
    if (!aMucMcHitCol) {
      log << MSG::WARNING << "Could not find MucMcHitCol" << endreq;
      //return( StatusCode::FAILURE);
    }

    log << MSG::DEBUG << "MucMcHitCol contains " << aMucMcHitCol->size() << " Hits " << endreq;
    MucMcHitCol::iterator iter_MucMcHit = aMucMcHitCol->begin();
    for (; iter_MucMcHit != aMucMcHitCol->end(); iter_MucMcHit++) {
      mucID = (*iter_MucMcHit)->identify();

      log << MSG::DEBUG << " MucMcHit " << " : " 
        << " part " << MucID::barrel_ec(mucID)
        << " seg "  << MucID::segment(mucID)
        << " gap "  << MucID::layer(mucID)
        << " strip " << MucID::channel(mucID)
        << " Track Id " << (*iter_MucMcHit)->getTrackIndex() 
        << " pos x " << (*iter_MucMcHit)->getPositionX()
        << " pos y " << (*iter_MucMcHit)->getPositionY()
        << " pos z " << (*iter_MucMcHit)->getPositionZ()
        << endreq;

      McParticle *assocMcPart = 0;
      for (iter_mc = mcParticleCol->begin(); iter_mc != mcParticleCol->end(); iter_mc++) {
        if ( (*iter_mc)->trackIndex() == (*iter_MucMcHit)->getTrackIndex() ) {
          assocMcPart = *iter_mc;
          break;
        }
      }
      if (assocMcPart == 0) {
        log << MSG::WARNING << "No Corresponding Mc Particle found for this MucMcHit" << endreq;
      }

      MucMcHit *assocMucMcHit = *iter_MucMcHit;
      McPartToMucHitRel *relMcMuc = 0;
      relMcMuc = new McPartToMucHitRel( assocMcPart, assocMucMcHit );
      if (relMcMuc == 0) log << MSG::DEBUG << "relMcMuc not created " << endreq;
      else {
        bool addstat = assocMcMuc.addRelation( relMcMuc );
        if(!addstat) delete relMcMuc;
      }
    }

    log << MSG::DEBUG << " Fill McPartToMucHitTab, size " << assocMcMuc.size() << endreq;
    iter_mc = mcParticleCol->begin();
    for (;iter_mc != mcParticleCol->end(); iter_mc++) {
      log << MSG::DEBUG << " Track index " << (*iter_mc)->trackIndex() 
        << " particleId = " << (*iter_mc)->particleProperty()
        << endreq;   

      vector< McPartToMucHitRel* > vecMucMcHit = assocMcMuc.getRelByFirst(*iter_mc);
      vector< McPartToMucHitRel* >::iterator iter_MucMcHit = vecMucMcHit.begin();
      for (; iter_MucMcHit != vecMucMcHit.end(); iter_MucMcHit++) {
        mucID = (*iter_MucMcHit)->getSecond()->identify();

        log << MSG::DEBUG 
          //<< " McPartToMucHitTab " << iter_assocMcMuc
          << " MC Particle index " 
          << (*iter_MucMcHit)->getFirst()->trackIndex()
          << " contains " << " MucMcHit " 
          << " part "  << MucID::barrel_ec(mucID)
          << " seg "   << MucID::segment(mucID)
          << " gap "   << MucID::layer(mucID)
          << " strip " << MucID::channel(mucID)
          //<< " posX " << (*iter_MucMcHit)->getSecond()->getPositionX()
          //<< " posY " << (*iter_MucMcHit)->getSecond()->getPositionY()
          //<< " posZ " << (*iter_MucMcHit)->getSecond()->getPositionZ()
          << endreq;
      }
    }

    //assocMcPart = new McParticle(**iter_mc);
    //MucMcHit *assocMucMcHit = new MucMcHit(mucID, (*iter_MucMcHit)->getTrackIndex(), (*iter_MucMcHit)->getPositionX(),
    //				 (*iter_MucMcHit)->getPositionY(), (*iter_MucMcHit)->getPositionZ(),
    //				 (*iter_MucMcHit)->getPx(), (*iter_MucMcHit)->getPy(), (*iter_MucMcHit)->getPz() );

    // Retrieve McPartToMucHitTab 
    //SmartDataPtr<McPartToMucHitTab> aMcPartToMucHitTab(eventSvc(),"/Event/MC/McPartToMucHitTab");
    //if (!aMcPartToMucHitTab) {
    //log << MSG::ERROR << "Could not find McPartToMucHitTab" << endreq;
    //  return( StatusCode::FAILURE);
    //}
  }

  //
  // Read in Muc Digi;
  if (!m_MucRecHitContainer) {
    m_MucRecHitContainer = new MucRecHitContainer();
  }
  m_MucRecHitContainer->Clear();
  MucRecHitCol *aMucRecHitCol = new MucRecHitCol();
  m_MucRecHitContainer->SetMucRecHitCol(aMucRecHitCol);

  SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
  DataObject *mucRecHitCol;
  eventSvc()->findObject("/Event/Recon/MucRecHitCol",mucRecHitCol);
  if(mucRecHitCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/MucRecHitCol");
    eventSvc()->unregisterObject("/Event/Recon/MucRecHitCol");
  }

  sc = eventSvc()->registerObject("/Event/Recon/MucRecHitCol", aMucRecHitCol); 
  if (!sc) {
    log << MSG::ERROR << "/Event/Recon/MucRecHitCol not registerd!" << endreq;
    return( StatusCode::FAILURE);
  }

  log << MSG::INFO << "Add digis" << endreq; 
  MucDigiCol::iterator iter_Muc = mucDigiCol->begin();
  int mucDigiId = 0;
  for (;iter_Muc != mucDigiCol->end(); iter_Muc++, mucDigiId++) {
    mucID = (*iter_Muc)->identify();
    int part  = MucID::barrel_ec(mucID);
    int seg   = MucID::segment(mucID);
    int gap   = MucID::layer(mucID);
    int strip = MucID::channel(mucID);
    //m_MucRecHitContainer->AddHit(mucID);
    m_MucRecHitContainer->AddHit(part, seg, gap, strip);

    log << MSG::DEBUG <<  " digit" << mucDigiId << " : " 
      << " part "  << part
      << " seg "   << seg
      << " gap "   << gap
      << " strip " << strip
      << endreq;
  }

  //
  // Create track Container;
  RecMucTrackCol *aRecMucTrackCol = new RecMucTrackCol();

  //Register RecMucTrackCol to TDS
  DataObject *aReconEvent ;
  eventSvc()->findObject("/Event/Recon",aReconEvent);
  if(aReconEvent==NULL) {
    //then register Recon
    aReconEvent = new ReconEvent();
    StatusCode sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return( StatusCode::FAILURE);
    }
  }
  StatusCode fr = eventSvc()->findObject("/Event/Recon",aReconEvent);
  if(fr!=StatusCode::SUCCESS) {
    log << MSG::WARNING << "Could not find register ReconEvent, will create it" <<endreq;          
    StatusCode sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return( StatusCode::FAILURE);
    }
  }

  DataObject *mucTrackCol;
  eventSvc()->findObject("/Event/Recon/RecMucTrackCol",mucTrackCol);
  if(mucTrackCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMucTrackCol");
    eventSvc()->unregisterObject("/Event/Recon/RecMucTrackCol");
  }

  sc = eventSvc()->registerObject("/Event/Recon/RecMucTrackCol", aRecMucTrackCol);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register MUC track collection" << endreq;
    return( StatusCode::FAILURE);
  }

  // check RecMucTrackCol registered
  SmartDataPtr<RecMucTrackCol> findRecMucTrackCol(eventSvc(),"/Event/Recon/RecMucTrackCol");
  if (!findRecMucTrackCol) { 
    log << MSG::FATAL << "Could not find RecMucTrackCol" << endreq;
    return( StatusCode::FAILURE);
  }
  aRecMucTrackCol->clear();

  // m_ExtTrackSeedMode  1: ext from MC track, 2: use Ext track

  // Retrieve Ext track Col from TDS 
  //Check ExtTrackCol in TDS.
  SmartDataPtr<RecExtTrackCol> aExtTrackCol(eventSvc(),"/Event/Recon/RecExtTrackCol");
  if (!aExtTrackCol) {
    log << MSG::WARNING << "Can't find ExtTrackCol in TDS!" << endreq;
    //return( StatusCode::FAILURE);
  }

  SmartDataPtr<RecMdcTrackCol> aMdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
  if (!aMdcTrackCol) {
    log << MSG::WARNING << "Can't find MdcTrackCol in TDS!" << endreq;
    //return( StatusCode::FAILURE);
  }

  //Retrieve Emc track col from TDS   2006.11.11 liangyt
  //Check EmcRecShowerCol in TDS.   
  SmartDataPtr<RecEmcShowerCol> aShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if (!aShowerCol) { 
    log << MSG::WARNING << "Could not find RecEmcShowerCol" << endreq;
    //return( StatusCode::FAILURE);
  } 

  //    EmcRecShowerCol::iterator iShowerCol;
  //    for(iShowerCol=aShowerCol->begin();
  //        iShowerCol!=aShowerCol->end();
  //        iShowerCol++){
  //      cout<<"check EmcRecShowerCol registered:"<<endl;
  //          <<"shower position: "<<(*iShowerCol)->Position()<<endl;
  //    }

  int muctrackId = 0;

  log << MSG::INFO << "Add tracks info, ExtTrackSeedMode = " << m_ExtTrackSeedMode << ", EmcShowerSeedMode = " << m_EmcShowerSeedMode << endreq;
  //  if (m_ExtTrackSeedMode == 1 || !aExtTrackCol) {
  if (m_ExtTrackSeedMode == 1) 
  {  
    SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if (!mcParticleCol) {
      log << MSG::FATAL << "Could not find McParticle" << endreq;
      //return( StatusCode::FAILURE);
      return( StatusCode::SUCCESS);
    }
    McParticleCol::iterator iter_mc = mcParticleCol->begin();

    int trackIndex = -99; 
    for (int iTrack = 0;iter_mc != mcParticleCol->end(); iter_mc++, iTrack++) {
      if(!(*iter_mc)->primaryParticle()) continue;
      int pid = (*iter_mc)->particleProperty();
      int charge = 0;
      double mass = -1.0;
      if( pid >0 ) 
      { 
        if(m_particleTable->particle( pid )){
          charge = (int)m_particleTable->particle( pid )->charge();
          mass = m_particleTable->particle( pid )->mass();
        }
        else log << MSG::ERROR << "no this particle id in particle table, please check data" <<endreq;
      } else if ( pid <0 ) 
      {
        if(m_particleTable->particle( -pid )){
          charge = (int)m_particleTable->particle( -pid )->charge();
          charge *= -1;
          mass = m_particleTable->particle( -pid )->mass();
        }
        else log << MSG::ERROR << "no this particle id in particle table, please check data" <<endreq;
      } else {
        log << MSG::WARNING << "wrong particle id, please check data" <<endreq;
      }

      if(!charge) {
        log << MSG::WARNING
          << " neutral particle charge = 0!!! ...just skip it !" 
          << endreq;
        continue;
      }
      
      trackIndex = (*iter_mc)->trackIndex();
      log << MSG::DEBUG << "iTrack " << iTrack << " index " << trackIndex
	        << " particleId = " << (*iter_mc)->particleProperty() 
	        << endreq;   
      
      RecMucTrack *aTrack = new RecMucTrack();
      aTrack->setTrackId(trackIndex);
      aTrack->setId(muctrackId);
      
      HepLorentzVector initialMomentum = (*iter_mc)->initialFourMomentum();
      HepLorentzVector initialPos = (*iter_mc)->initialPosition();
      float theta0 = initialMomentum.theta();
      float phi0   = initialMomentum.phi();
      //float dirX = sin(theta0) * cos(phi0);
      //float dirY = sin(theta0) * sin(phi0);
      //float dirZ = cos(theta0);
      float x0 = initialPos.x();
      float y0 = initialPos.y();
      float z0 = initialPos.z();
      
      Hep3Vector startPos(x0, y0, z0);
      Hep3Vector startP(initialMomentum.px(), initialMomentum.py(), initialMomentum.pz());
      log << MSG::DEBUG << "startP " << startP <<"  startPos "<<startPos<< endreq;
      Hep3Vector endPos(0, 0, 0), endP;
      
      aTrack->GetMdcExtTrack(startPos, startP, charge, endPos, endP);
      aTrack->SetMdcPos( x0, y0, z0);
      aTrack->SetMdcMomentum( startP.x(), startP.y(), startP.z() );
      aTrack->SetExtTrackID(trackIndex);
      aTrack->SetExtMucPos( endPos.x(), endPos.y(), endPos.z() );
      aTrack->SetExtMucMomentum( endP.x(), endP.y(), endP.z() );
      aTrack->SetMucPos( endPos.x(), endPos.y(), endPos.z() );
      aTrack->SetMucMomentum( endP.x(), endP.y(), endP.z() );
      aTrack->SetCurrentPos( endPos.x(), endPos.y(), endPos.z());
      aTrack->SetCurrentDir( endP.x(), endP.y(), endP.z() );
      //aTrack->SetMucVertexPos( x0, y0, z0);
      //aTrack->SetMucVertexDir( dirX, dirY, dirZ );
      //aTrack->SetCurrentPos( x0, y0, z0);
      //aTrack->SetCurrentDir( dirX, dirY, dirZ );
      
      //log << MSG::DEBUG 
      //<< "  pdg " << (*iter_mc)->particleProperty()
      //<< "  mucPos " << aTrack->GetMucVertexPos() 
      //<< "  mucDir " << aTrack->GetMucVertexDir() 
      //<< endreq; 
      log << MSG::INFO << "Add track, trkIndex: " << trackIndex << "\tmucTrkId: " << muctrackId << endreq; 
      aRecMucTrackCol->add(aTrack);
      muctrackId ++ ;
    }
  }
  else if (m_ExtTrackSeedMode == 2) 
  {
    if (!aExtTrackCol || !aMdcTrackCol) {
      log << MSG::WARNING << "Can't find ExtTrackCol or MdcTrackCol in TDS!" << endreq;
      return StatusCode::SUCCESS;
    }

    int trackIndex = -99;
    double kdep = -99;
    double krechi = 0.;
    int kdof  = 0;
    int kbrLay = -1;
    int kecLay = -1;
    //log << MSG::INFO << "MdcTrackCol:\t " << aMdcTrackCol->size() << "\tExtTrackCol:\t " << aExtTrackCol->size() << endreq; 
    RecExtTrackCol::iterator iter_ExtTrack = aExtTrackCol->begin();
    RecMdcTrackCol::iterator iter_MdcTrack = aMdcTrackCol->begin();
    int iExtTrack = 0;
    for (;iter_ExtTrack != aExtTrackCol->end() && iter_MdcTrack != aMdcTrackCol->end(); iter_ExtTrack++, iter_MdcTrack++, iExtTrack++) 
    { 
      trackIndex = (*iter_ExtTrack)->GetTrackId();
    log << MSG::DEBUG << "iExtTrack " << iExtTrack << " index " << trackIndex << " MucPos "      
        << iExtTrack << (*iter_ExtTrack)->mucPosition().x() << " " 
        << (*iter_ExtTrack)->mucPosition().y() << " " 
        << (*iter_ExtTrack)->mucPosition().z() << " r " 
        << (*iter_ExtTrack)->mucPosition().r() << endreq;

      if((*iter_ExtTrack)->mucPosition().x() == -99 &&
          (*iter_ExtTrack)->mucPosition().y() == -99 &&
          (*iter_ExtTrack)->mucPosition().z() == -99)
      {
        log << MSG::INFO <<"Bad ExtTrack, trackIndex: " << trackIndex << ", skip" << endreq;
        continue;
      }
      
      //added by LI Chunhua 2013/02/01
      krechi = (*iter_ExtTrack)->MucKalchi2();
      kdof= (*iter_ExtTrack)->MucKaldof();
      kdep = (*iter_ExtTrack)->MucKaldepth();
      kbrLay = (*iter_ExtTrack)->MucKalbrLastLayer();
      kecLay = (*iter_ExtTrack)->MucKalecLastLayer();
      if(kdof<=0)krechi = 0.;
      else krechi = krechi/kdof;
      kdep = kdep/10.;
      //*********************************
      RecMucTrack *aTrack = new RecMucTrack();
      aTrack->setTrackId(trackIndex);
      aTrack->setId(muctrackId);

      aTrack->setType(0); //0 for use seed from mdc ext;
      
      aTrack->SetExtTrack(*iter_ExtTrack);

      //added by LI Chunhua 2013/02/01
      aTrack->setkalRechi2(krechi);
      aTrack->setkalDof(kdof);
      aTrack->setkalDepth(kdep);
      aTrack->setkalbrLastLayer(kbrLay);
      aTrack->setkalecLastLayer(kecLay);
      //******************************
      //Hep3Vector mdcPos      = (*iter_ExtTrack)->GetMdcPosition();
      Hep3Vector mdcMomentum = (*iter_MdcTrack)->p3();
      Hep3Vector mucPos      = (*iter_ExtTrack)->mucPosition();
      Hep3Vector mucMomentum = (*iter_ExtTrack)->mucMomentum();
    
      //aTrack->SetMdcPos( mdcPos.x(), mdcPos.y(), mdcPos.z() );
      aTrack->SetMdcMomentum( mdcMomentum.x(), mdcMomentum.y(), mdcMomentum.z() );
      aTrack->SetExtMucPos( mucPos.x(), mucPos.y(), mucPos.z() );
      //cout << "ExtTrack MucPos " << aTrack->GetExtMucPos() << endl;
      aTrack->SetExtMucMomentum( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
      aTrack->SetMucPos( mucPos.x(), mucPos.y(), mucPos.z() );
      aTrack->SetMucMomentum( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
      aTrack->SetCurrentPos( mucPos.x(), mucPos.y(), mucPos.z());
      aTrack->SetCurrentDir( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
      aTrack->SetRecMode(0);  //mdc ext mode
      
      log << MSG::INFO << "Add track, trkIndex: " << trackIndex << "\tmucTrkId: " << muctrackId << endreq; 
      aRecMucTrackCol->add(aTrack);
      muctrackId ++ ;
    }
  }
  else if(m_ExtTrackSeedMode == 3) 
  { //cosmic ray

    if (!aMdcTrackCol) {
      log << MSG::WARNING << "Can't find MdcTrackCol in TDS!" << endreq;
      return StatusCode::SUCCESS;
      //return( StatusCode::FAILURE);
    }
    log<< MSG::INFO << "Mdc track size: "<<aMdcTrackCol->size()<<endreq;

    int trackIndex = -99; 
    for(RecMdcTrackCol::iterator iter_mdc1 = aMdcTrackCol->begin(); iter_mdc1 != aMdcTrackCol->end(); iter_mdc1++){
      //if((*iter_mdc1)->getFi0() > 0.5*pi && (*iter_mdc1)->getFi0() < 1.5*pi){
      //  cout<<"this is down track"<<endl;
      //}
    
      trackIndex = (*iter_mdc1)->trackId();
      HepVector helix = (*iter_mdc1)->helix();
      //cout<<"helix: "<<helix<<endl;

      float x0 = helix[0] * cos(helix[1]);
      float y0 = helix[0] * sin(helix[1]);
      float z0 = helix[3];

      //float dx = 1/(sqrt(1+helix[4]*helix[4])) * (-1* sin(helix[1]));
      //float dy = 1/(sqrt(1+helix[4]*helix[4])) * cos(helix[1]);
      //float dz = 1/(sqrt(1+helix[4]*helix[4])) * helix[4];
  
      float dx =  -1* sin(helix[1]);
      float dy =  cos(helix[1]);
      float dz =  helix[4];

      //cout<<"pos: "<<x0<<"  "<<y0<<"  "<<z0<<"  dir: "<<dx<<"  "<<dy<<"  "<<dz<<endl;

      RecMucTrack *aTrack = new RecMucTrack();
      aTrack->setTrackId(trackIndex);
      aTrack->setId(muctrackId);
      muctrackId ++ ;

      aTrack->setType(3); //3 for use seed from mdc without magnet field;

    
      Hep3Vector mucPos(x0,y0,z0);
      Hep3Vector mucMomentum(dx,dy,dz);
    
      aTrack->SetExtMucPos(x0,y0,z0);
      aTrack->SetExtMucMomentum(dx,dy,dz);
    
      aTrack->SetMucPos(x0,y0,z0);
      aTrack->SetMucMomentum(dx,dy,dz);
      aTrack->SetCurrentPos(x0,y0,z0);
      aTrack->SetCurrentDir(dx,dy,dz);
      aTrack->SetRecMode(0);  //mdc ext mode
      
      log << MSG::INFO << "Add track, trkIndex: " << trackIndex << "\tmucTrkId: " << muctrackId << endreq; 
      aRecMucTrackCol->add(aTrack);
      muctrackId ++ ;
    }
  }
  else {log << MSG::INFO <<"ExtTrackSeedMode error!"<<endreq; }

  //Rec from Emc extend track
  if(m_EmcShowerSeedMode == 1)
  {
    int trackIndex = 999;
    RecEmcShowerCol::iterator iShowerCol;
    for(iShowerCol=aShowerCol->begin();  iShowerCol!=aShowerCol->end(); iShowerCol++)
    {
      //       cout<<"shower id: "<<(*iShowerCol)->ShowerId()<<" "
      // 	  <<"shower energy: "<<(*iShowerCol)->Energy()<<" "
      // 	  <<"shower position: "<<(*iShowerCol)->Position()<<endl;

      RecMucTrack *aTrack = new RecMucTrack();
      aTrack->setTrackId(trackIndex);
      aTrack->setId(muctrackId);
      aTrack->setType(1); //1 for use seed from emc hits;

      Hep3Vector mucPos      = (*iShowerCol)->position();
      Hep3Vector mucMomentum = (*iShowerCol)->position();

      aTrack->SetExtMucPos( mucPos.x(), mucPos.y(), mucPos.z() );
      //cout << "EmcTrack MucPos " << aTrack->GetExtMucPos() << endl;
      aTrack->SetExtMucMomentum( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
      aTrack->SetMucPos( mucPos.x(), mucPos.y(), mucPos.z() );
      aTrack->SetMucMomentum( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
      aTrack->SetCurrentPos( mucPos.x(), mucPos.y(), mucPos.z());
      aTrack->SetCurrentDir( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
      aTrack->SetRecMode(1);  //emc ext mode
      //cout<<" MucRecTrkExt 111 trackIndex :"<<aTrack->GetIndex()<<endl;
      
      log << MSG::INFO << "Add track, trkIndex: " << trackIndex << "\tmucTrkId: " << muctrackId << endreq; 
      aRecMucTrackCol->add(aTrack);
      muctrackId ++ ;

      m_emcrec = 1;
    }
  }
  log << MSG::DEBUG << " track container filled " << endreq;

  // All input filled, begin track finding;
  log << MSG::INFO << "Start tracking..." << endreq;
  int runVerbose = 1;
  RecMucTrack *aTrack = 0;
  for(int iTrack = 0; iTrack < (int)aRecMucTrackCol->size(); iTrack++) 
  {
    log << MSG::DEBUG << "iTrack " << iTrack << endreq;
    aTrack = (*aRecMucTrackCol)[iTrack]; 
    //cout<<"in MucRecTrkExt trackIndex :"<<aTrack->GetIndex()<<endl;
    
    Hep3Vector currentPos = aTrack->GetCurrentPos();
    Hep3Vector currentDir = aTrack->GetCurrentDir();
    if(currentPos.mag() < kMinor) {
      log << MSG::WARNING << "No MUC intersection in track " << iTrack << endreq;
      continue;
    }
    //log << MSG::INFO << " pos " << currentPos << " dir " << currentDir << endreq;
    
    int firstHitFound[2] = { 0,  0}; // Has the fist position in this orient determined? if so, could CorrectDirection()
    int firstHitGap[2]   = {-1, -1}; // When the fist position in this orient determined, the gap it is on
    for(int partSeq = 0; partSeq < (int)MucID::getPartNum(); partSeq++) 
    {
      int iPart = kPartSeq[partSeq];
      for(int iGap = 0; iGap < (int)MucID::getGapNum(iPart); iGap++) 
      {
        //log << MSG::INFO << iPart << iGap << endreq;
        int seg = -1;
        int orient = MucGeoGeneral::Instance()->GetGap(iPart, 0, iGap)->Orient();;

        float xInsct, yInsct, zInsct;
        aTrack->Project(iPart, iGap, xInsct, yInsct, zInsct, seg);
        if(m_MsOutput) cout << "part " << iPart << " gap " << iGap << " x " << xInsct << " y " << yInsct << " z " << zInsct << " seg " << seg << endl; 
        
        if(seg == -1) continue;

        aTrack->SetCurrentInsct(xInsct, yInsct, zInsct);

        for(int iDeltaSeg = 0; iDeltaSeg < kNSegSearch; iDeltaSeg++) 
        {
          int iSeg = seg + kDeltaSeg[iDeltaSeg];  // also find on neighbor seg;
          if(iSeg < 0) iSeg += MucID::getSegNum(iPart);
          if(iSeg > (int)MucID::getSegNum(iPart) - 1) iSeg -= MucID::getSegNum(iPart);

          //log << MSG::DEBUG << iPart << iSeg << iGap 
          //  << "NHits " << m_MucRecHitContainer->GetGapHitCount(iPart, seg, iGap) << endreq;

          //----------now change window size(depond on mom)
          //float window = kWindowSize[iPart][iGap];
          Hep3Vector mom_mdc = aTrack->getMdcMomentum();
          float window = getWindowSize(mom_mdc, iPart, iSeg, iGap);

          if (firstHitFound[orient] != 1) window *= kFirstHitWindowRatio;   // if no hits have been found on this orient, expand the window.
          for (int iHit = 0; iHit < m_MucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap); iHit++) 
          {
            log << MSG::DEBUG << "iSeg " << iSeg << " iHit " << iHit << endreq;
            MucRecHit* pHit = m_MucRecHitContainer->GetHit(iPart, iSeg, iGap, iHit);
            //cout<< "strip " << pHit->Strip() << " center " << pHit->GetCenterPos() << endl;

            if (!pHit) {
              log << MSG::WARNING << "MucRecTrkExt: null pointer to pHit" << endreq;
              continue;
            }
            else 
            {
              //cout<<"in MucRecTrkExt: pHit exist : "<<iPart<<" "<<iSeg<<" "<<iGap<<" "<<iHit<<" mode:"<<pHit->GetHitMode()<<endl;
              // Get the displacement of hit pHit to intersection
              //float dX  = aTrack->GetHitDistance(pHit);
              float dX  = aTrack->GetHitDistance2(pHit);  //not abs value now!
              log << MSG::DEBUG << "distance = " << setw(8) << dX << " size " << setw(4) << window << endreq;

              if(m_NtOutput >= 2){ //too many info
                m_distance = dX;
                m_part = iPart; m_seg = iSeg; m_gap = iGap; m_strip = pHit->Strip();
                m_diff = -99; m_tuple->write();
              }

              if (fabs(dX) < window) 
              {
                // Attach the hit if it exists
                // cout << "meet window " << pHit->GetSoftID() << endl;
                //****************if this if emc track, we abondon used hit in mdc*******************
                //if(m_emcrec == 1 )
                if(aTrack->GetRecMode() == 0) {
                  pHit->SetHitMode(1); //mdc ext
                  aTrack->AttachHit(pHit);
                  //cout<<"in MucRecTrkExt: trackmode==0 so mdc ext "<<iPart<<" "<<iSeg<<" "<<iGap<<" "<<iHit<<endl;
                }
                if(aTrack->GetRecMode() == 1) {
                  //cout<<"in MucRecTrkExt: HitMode = "<<pHit->GetHitMode()<<" part:  "<<iPart<<" "<<iSeg<<" "<<iGap<<" "<<iHit<<endl;
                  if(pHit->GetHitMode()!=1) {
                    aTrack->AttachHit(pHit);  //this hit has not been used by mdc ext
                    pHit->SetHitMode(2); //emc ext
                  }
                }

                //push back distance between ext and hits
                aTrack->pushExtDistHits(dX);   //2009-03-12

                if (firstHitGap[orient] == -1) firstHitGap[orient] = iGap;
                firstHitFound[orient] = 1;
                //cout << "You could correct directon in orient " << orient << endl;

                log << MSG::DEBUG << " part " << iPart << " seg " << iSeg << " gap " << iGap 
                    << " strip " << setw(2) << pHit->Strip() << " attatched" << endreq;
                log << MSG::DEBUG << "current total hits " << aTrack->GetTotalHits() << endreq;
              }
              else {
                m_NHitsLostInGap[iGap]++;
                //log << MSG::DEBUG cout << " part " << iPart << " seg " << iSeg << " gap " << iGap 
                //       << " strip " << setw(2) << pHit->GetSoftID().GetStrip() 
                //     << " not attached !" << endreq;
              }
            } // end pHit else 
          } // end for iHit
          aTrack->CalculateInsct(iPart, iSeg, iGap);
        } // end for iDeltaSeg
        
        // When correct dir in the orient is permitted and this gap is not the gap first hit locates.
        if(m_ExtTrackSeedMode != 3 && !m_Blind) { //for cosmic ray, we do not correct dir and pos...
          if (firstHitFound[orient] && firstHitGap[orient] != iGap) aTrack->CorrectDir();
          aTrack->CorrectPos();
        }
        //cout << "Current Intersection " << aTrack->GetCurrentInsct() << endl;
        //cout << "Current Direction    " << aTrack->GetCurrentDir() << endl;
        aTrack->AttachInsct(aTrack->GetCurrentInsct());
      } // end for iGap
    } // end for iSeg
    aTrack->LineFit(m_fittingMethod);
    aTrack->ComputeTrackInfo(m_fittingMethod);
    log << MSG::INFO <<"Fit track done! trackIndex: " << aTrack->trackId() << ", mucId: " << aTrack->id() << ", RecMode: " << aTrack->GetRecMode() << endreq;
 
    if(m_NtOutput>=1)
    {
      m_depth = aTrack->depth();
      m_Distance = aTrack->distance();
      m_MaxHits = aTrack->maxHitsInLayer();
      m_Chi2  = aTrack->chi2();
      m_Dist_x = aTrack->GetExtMucDistance().x();
      m_Dist_y = aTrack->GetExtMucDistance().y();
      m_Dist_z = aTrack->GetExtMucDistance().z();
      m_posx_ext = aTrack->GetMucStripPos().x();
      m_posy_ext = aTrack->GetMucStripPos().y();
      m_posz_ext = aTrack->GetMucStripPos().z();

      m_emctrack = m_emcrec;
    }

    //test distance of line/quad fitting
    if(m_NtOutput>=2)
    {
	    vector<MucRecHit*> attachedHits = aTrack->GetHits();
	    vector<MucRecHit*> expectedHits = aTrack->GetExpectedHits();
	    vector<float>      distanceHits = aTrack->getDistHits();
	    vector<float>      distanceHits_quad =  aTrack->getQuadDistHits();
	    vector<float>      distanceHits_ext =  aTrack->getExtDistHits();

	    for(int i=0; i< expectedHits.size(); i++)
	    {
		    MucRecHit *ihit = expectedHits[i];
		    //cout<<"expected Hits: "<<ihit->Part()<<"  "<<ihit->Seg()<<"  "<<ihit->Gap()<<"  "<<ihit->Strip()<<endl;
	    }

	    for(int j=0; j< attachedHits.size(); j++)
      {
		    MucRecHit *jhit = attachedHits[j];
		    //                if(attachedHits.size()!=distanceHits.size())cout<<"attached hits size no same as disthits!!!"<<endl;
		    if(attachedHits.size()==distanceHits.size())
		    {    // same gap, cale distance
			    m_part = jhit->Part();   m_seg = jhit->Seg();  m_gap = jhit->Gap();
			    m_strip = jhit->Strip();
			    m_distance = distanceHits[j];
			    m_Distance = distanceHits_quad[j];
			    m_diff = -9999;
			    //cout<<"distance = "<<m_dist<<endl;
			    //if(distanceHits.size() == distanceHits_ext.size())
			    //cout<<"ext dist: "<<distanceHits_ext[j]<<endl;
			    m_tuple->write();
		    }
	    }
    } // end if output

    int nHitsAttached = aTrack->GetTotalHits();
    //m_NHitsAttachedTotal += nHitsAttached;
    //if(mucDigiCol->size() - recHitNum != 0) 
    log << MSG::DEBUG << "track Index " << aTrack->trackId()
	      << setw(2) << mucDigiCol->size() - nHitsAttached << " of "
	      << setw(2) << mucDigiCol->size() << " lost " << endreq;
    //m_NHitsLost[int(mucDigiCol->size() - nHitsAttached)]++;
  } // end for iTrack

  
  // if (aRecMucTrackCol->size() == 0) m_NHitsLost[0]++;
  //m_NHitsTotal         += mucDigiCol->size();
    
  //****************** look up unrec hits to do recon with MUC info ***************??

  if(m_MucHitSeedMode == 1)
  {
    MucRecHit *pHit = 0 ,*pHit0 = 0, *pHit1 = 0;
    int count, orient;

    for (int iPart = 0; iPart < (int)MucID::getPartNum(); iPart++) 
    {
      for (int iSeg = 0; iSeg < (int)MucID::getSegNum(iPart); iSeg++) 
      {
        bool hit0 = false, hit1 = false;  int firstgap0 = -1, firstgap1 = -1; int nStrip0 = 0, nStrip1 = 0;
        Hep3Vector posHit0, posHit1;
        for(int iGap = 0; iGap < (int)MucID::getGapNum(iPart); iGap++) 
        {
          count = m_MucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap);
          for (int iHit0 = 0; iHit0 < count; iHit0++) 
          {
            //cout << "iHit0 = " << iHit0 << endl;
            pHit = m_MucRecHitContainer->GetHit(iPart, iSeg, iGap, iHit0);
            if (!pHit) {
              log << MSG::FATAL << "MucRecRoadFinder-E10 " << " part " << iPart
                << " seg " << iSeg << " gap "  << iGap << " null pointer"
                << endl;
            }
            else 
            {
              //cout<< "pHit Mode is : " << pHit->GetHitMode() << endl;
              if(pHit->GetHitMode() == -1)
              {
                orient = MucGeoGeneral::Instance()->GetGap(iPart, 0, iGap)->Orient();
                if(orient == 1 && hit0 == false){
                  hit0 = true;
                  firstgap0 = iGap;
                }
                if(iGap == firstgap0){
                  posHit0 += pHit->GetCenterPos();
                  nStrip0++;
                }

                if(orient == 0 && hit1 == false){
                  hit1 = true;
                  firstgap1 = iGap;
                }
                if(iGap == firstgap1){
                  posHit1 += pHit->GetCenterPos();
                  nStrip1++;
                }
                // 	      if(orient == 1 && hit0 == false){   //orient = 1
                // 		hit0 = true;
                // 		pHit0 = pHit;  //pHit0 is the first hit of first gap in this segment with orient 1
                // 	      }
                // 	      if(orient == 0 && hit1 == false){
                // 		hit1 = true;
                // 		pHit1 = pHit;  //pHit0 is the first hit of first gap in this segment with orient 0
                // 	      }
              }
            }// pHit0 exist;
          } // iHit0
        }  //iGap

        //if hit0 hit1 exist, make a seed
        if(hit0 && hit1)
        {
          posHit0 /= nStrip0;
          posHit1 /= nStrip1;
          //cout<< "pHit0 position is " << posHit0  <<" "<<nStrip0<<endl;
          //cout<< "pHit1 position is " << posHit1  <<" "<<nStrip1<<endl;

          int trackIndex = 999;
          RecMucTrack *aTrack = new RecMucTrack();
          aTrack->setTrackId(trackIndex);
          aTrack->setId(muctrackId);
          muctrackId ++ ;

          aTrack->setType(2); //2 for use seed from Muc Information

          Hep3Vector mucPos, mucMomentum;    
          if(iPart==1) {
            mucMomentum.set(posHit0.x(),posHit0.y(),posHit1.z());
            mucPos = mucMomentum * 0.9;  //shorten mucPos, otherwise maybe no intersection point found for the track and this gap
          }
          else{
            mucMomentum.set(posHit0.x(),posHit1.y(),posHit0.z()*0.5+posHit1.z()*0.5);
            mucPos = mucMomentum * 0.9;  //shorten mucPos, otherwise maybe no intersection point found for the track and this gap
          }

          aTrack->SetExtMucPos( mucPos.x(), mucPos.y(), mucPos.z() );
          //cout << "EmcTrack MucPos " << aTrack->GetExtMucPos() << endl;
          aTrack->SetExtMucMomentum( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
          aTrack->SetMucPos( mucPos.x(), mucPos.y(), mucPos.z() );
          aTrack->SetMucMomentum( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
          aTrack->SetCurrentPos( mucPos.x(), mucPos.y(), mucPos.z());
          aTrack->SetCurrentDir( mucMomentum.x(), mucMomentum.y(), mucMomentum.z() );
          //cout<<" MucRecTrkExt 111 trackIndex :"<<aTrack->GetIndex()<<endl;
          aTrack->SetRecMode(2); 
          aRecMucTrackCol->add(aTrack);

          TrackFinding(aTrack);
        }

        if(m_NtOutput>=1){
          m_depth = aTrack->depth();
          m_Distance = aTrack->distance();
          m_MaxHits = aTrack->maxHitsInLayer();
          m_Chi2  = aTrack->chi2();
          m_Dist_x = aTrack->GetExtMucDistance().x();
          m_Dist_y = aTrack->GetExtMucDistance().y();
          m_Dist_z = aTrack->GetExtMucDistance().z();
          m_posx_ext = aTrack->GetMucStripPos().x();
          m_posy_ext = aTrack->GetMucStripPos().y();
          m_posz_ext = aTrack->GetMucStripPos().z();

          m_emctrack = 2;
        }

      }  //iSeg
    }  //iPart
  } // end if SeedMode == 1

  //************** end of look up unrec hits to do recon with MUC info ************??
  int nTracksTotal = 0;//mcParticleCol->size();
  int nTracksFound = 0;
  int nTracksLost  = 0;
  int nTracksLostByExt = 0;
  int nTracksMisFound = 0;
  
  int nDigisTotal   = 0;//mucDigiCol->size();
  int nHitsTotal    = 0;//assocMcMuc.size();
  int nHitsFound    = 0;
  int nHitsLost     = 0;
  int nHitsMisFound = 0;
  
/*
  if (m_CompareWithMcHit) {
    // 
    // Compare RecMucTracks with McPartToMucHitTab;

    log << MSG::DEBUG << " *******************************" << endreq;
    log << MSG::DEBUG << " Compare Mc Info with rec tracks" << endreq;
    log << MSG::DEBUG << " McParticleCol     size " << mcParticleCol->size() << endreq;
    log << MSG::DEBUG << " McPartToMcuHitTab size " << assocMcMuc.size() << endreq;

    iter_mc = mcParticleCol->begin();
    for (;iter_mc != mcParticleCol->end(); iter_mc++) {
      log << MSG::DEBUG << " McParticle index " << (*iter_mc)->getTrackIndex() 
        << " particleId = " << (*iter_mc)->particleProperty()
        << endreq;   

      bool foundRecoTrack = false;
      log << MSG::DEBUG << " Reconed RecMucTrackCol size " << aRecMucTrackCol->size() << endreq;
      aTrack = 0;
      for(int iTrack = 0; iTrack < (int)aRecMucTrackCol->size(); iTrack++) {
        log << MSG::DEBUG << "iTrack " << iTrack << endreq;
        aTrack = (*aRecMucTrackCol)[iTrack]; 
        if (aTrack->GetExtTrackID() == (*iter_mc)->getTrackIndex()) {
          log << MSG::DEBUG << " found iTrack " << iTrack 
            << " index " << aTrack->GetExtTrackID() 
            << " equals to " << " McParticle index " << (*iter_mc)->getTrackIndex()
            << endreq;
          foundRecoTrack = true;
          break;
        }
      }
      if (foundRecoTrack == false) {
        nTracksLost++;
        m_TrackLostByMdc.push_back(eventHeader->eventNumber());
        log << MSG::DEBUG << " this McParticle is not found in RecMucTrackCol,"
          << "->not intialized from ExtTrack-> not constructed in Mdc" << endreq;
      }
      else {
        nTracksFound++;
      }

      int nHitsFoundInTrack   = 0; 
      int nHitsLostInTrack    = 0; 

      vector< McPartToMucHitRel* > vecMucMcHit = assocMcMuc.getRelByFirst(*iter_mc);
      log << MSG::DEBUG << " McParticle " << (*iter_mc)->getTrackIndex() 
        << " contains " << vecMucMcHit.size() << " MucMcHit " << endreq;
      if (!aTrack) {
        nHitsLostInTrack = vecMucMcHit.size();
        m_NHitsLostByMdcTotal += nHitsLostInTrack;
        log << MSG::DEBUG << " This Mc particle has no corresponding Reco Track, "
          << " all of its MucMcHits lost" << endreq;  
      }
      else if ((aTrack->GetExtMucPos()).mag() < 0.1) {
        nHitsLostInTrack = vecMucMcHit.size();
        m_NHitsLostByExtTotal += nHitsLostInTrack;
        nTracksLostByExt++;
        m_TrackLostByExt.push_back(eventHeader->eventNumber());
        log << MSG::DEBUG << " This Mc particle 's Ext track's intersection with Muc is zero," 
          << " all of its MucMcHits lost" << endreq;
      }
      else {
        vector< McPartToMucHitRel* >::iterator iter_MucMcHit = vecMucMcHit.begin();
        for (; iter_MucMcHit != vecMucMcHit.end(); iter_MucMcHit++) {
          mucID = (*iter_MucMcHit)->getSecond()->identify();
          int part  = MucID::barrel_ec(mucID);
          int seg   = MucID::segment(mucID);
          int gap   = MucID::layer(mucID);
          int strip = MucID::channel(mucID);

          log << MSG::DEBUG 
            //<< " McPartToMucHitTab " << iter_assocMcMuc
            << " McParticle index " 
            << (*iter_MucMcHit)->getFirst()->getTrackIndex()
            << " contains " << " MucMcHit " 
            << " part "  << part
            << " seg "   << seg
            << " gap "   << gap
            << " strip " << strip
            //<< " posX " << (*iter_MucMcHit)->getSecond()->getPositionX()
            // << " posY " << (*iter_MucMcHit)->getSecond()->getPositionY()
            //<< " posZ " << (*iter_MucMcHit)->getSecond()->getPositionZ()
            << endreq;

          if (aTrack->HasHit(part, seg, gap, strip)) {
            nHitsFoundInTrack++;
            log << MSG::DEBUG << " This MucMchit found on this Reco track" << endreq;
          }
          else { 
            nHitsLostInTrack++;
            log << MSG::DEBUG << " This MucMchit not found on this Reco track, it is lost " << endreq;
          }
        }
      }

      nHitsFound += nHitsFoundInTrack;
      nHitsLost  += nHitsLostInTrack;

      m_NHitsLost[nHitsLost]++;
      if (nHitsLost > 0) {
        m_TrackLostHit.push_back(eventHeader->eventNumber());
        m_TrackLostHitNumber.push_back(nHitsLost);
      }

      log << MSG::DEBUG << " In " << vecMucMcHit.size() << " MucMcHit : "
        << nHitsFoundInTrack << " found in Reco track, " 
        << nHitsLostInTrack << " lost " << endreq;
    }

    // Reversely, Compare McPartToMucHitTab with RecMucTracks;
    log << MSG::DEBUG << " *******************************" << endreq;
    log << MSG::DEBUG << " Compare rec tracks with Mc Info" << endreq;
    log << MSG::DEBUG << " Reconed RecMucTrackCol size " << aRecMucTrackCol->size() << endreq;
    for(int iTrack = 0; iTrack < (int)aRecMucTrackCol->size(); iTrack++) {
      log << MSG::DEBUG << "iTrack " << iTrack << endreq;
      aTrack = (*aRecMucTrackCol)[iTrack]; 
      //cout << "MucPos " << aTrack->GetMucPos() << " MucMomentum " << aTrack->GetMucMomentum() << endl;

      bool foundMcParticle = false;
      iter_mc = mcParticleCol->begin();
      for (;iter_mc != mcParticleCol->end(); iter_mc++) {
        if ((*iter_mc)->getTrackIndex() == aTrack->GetExtTrackID()) {
          log << MSG::DEBUG << " found McParticle index " << (*iter_mc)->getTrackIndex()
            << " equals to " << " Reconed track " << iTrack 
            << " index " << aTrack->GetExtTrackID() 
            << endreq;
          foundMcParticle = true;
          break;
        }
      }
      if (foundMcParticle == false) {
        nTracksMisFound++;
        log << MSG::DEBUG << " This Reconed track has no corresponding McParticle, where is it from? " << endreq; 
      }

      int nHitsFoundInMcParticle    = 0; 
      int nHitsMisFoundInMcParticle = 0; 
      vector< MucRecHit* > vecMucRecHit = aTrack->GetHits();
      log << MSG::DEBUG << " Reconed Track " << iTrack 
        << " index " << aTrack->GetExtTrackID() 
        << " contains " << aTrack->GetTotalHits() << " MucRecMcHit " << endreq;
      vector< MucRecHit* >::iterator iter_MucRecHit = vecMucRecHit.begin();
      for (; iter_MucRecHit != vecMucRecHit.end(); iter_MucRecHit++) {
        int part  = (*iter_MucRecHit)->Part();
        int seg   = (*iter_MucRecHit)->Seg();
        int gap   = (*iter_MucRecHit)->Gap();
        int strip = (*iter_MucRecHit)->Strip();

        log << MSG::DEBUG 
          << " Reconed track index " 
          << aTrack->GetExtTrackID() 
          << " contains " << " MucRecHit " 
          << " part "  << part
          << " seg "   << seg
          << " gap "   << gap
          << " strip " << strip;

        bool foundMcHit = false;
        vector< McPartToMucHitRel* > vecMucMcHit = assocMcMuc.getRelByFirst(*iter_mc);
        vector< McPartToMucHitRel* >::iterator iter_MucMcHit = vecMucMcHit.begin();
        for (; iter_MucMcHit != vecMucMcHit.end(); iter_MucMcHit++) {
          mucID = (*iter_MucMcHit)->getSecond()->identify();
          if (part  == MucID::barrel_ec(mucID) &&
              seg   == MucID::segment(mucID) &&
              gap   == MucID::layer(mucID) &&
              strip == MucID::channel(mucID)) {
            foundMcHit = true;
            break;
          }
        }

        if (foundMcHit == true) {
          nHitsFoundInMcParticle++;
          log << MSG::DEBUG << " This MucRecHit belongs to this Mc Particle " << endreq;
        }
        else { 
          nHitsMisFoundInMcParticle++;
          log << MSG::DEBUG << " This MucRecHit does not belong to this Reco track, it should not be attached " << endreq;
        }
      }

      //nHitsFound += nHitsFoundInMcParticle;
      nHitsMisFound  += nHitsMisFoundInMcParticle;

      log << MSG::DEBUG << " In " << vecMucRecHit.size() << " MucRecHit : "
        << nHitsFoundInMcParticle << " found in Mc Particle, " 
        << nHitsMisFoundInMcParticle << " not found in Mc Particle, mis attached " 
        << endreq;
    }
  }
*/
/*
  log << MSG::INFO << " This event contains " << nTracksTotal << " Mc Particle, " 
    << nTracksFound << " tracks found, " 
    << nTracksLost  << " tracks lost, " 
    << nTracksMisFound << " tracks mis found " 
    << endreq;
  log << MSG::INFO << " This event contains " << nDigisTotal << " Muc Digis " << endreq;
  log << MSG::INFO << " This event contains " << nHitsTotal  << " MucMcHits, " 
    << nHitsFound << " mc hits found, "
    << nHitsLost  << " mc hits lost, " 
    << nHitsMisFound << " mc hits mis found "
    << endreq;
*/
  m_NDigisTotal        += nDigisTotal;
  m_NHitsTotal         += nHitsTotal;
  m_NHitsFoundTotal    += nHitsFound;
  m_NHitsLostTotal     += nHitsLost;
  m_NHitsMisFoundTotal += nHitsMisFound;
  //m_NHitsLost[nHitsLost]++;

  m_NTracksTotal          += nTracksTotal;
  m_NTracksRecoTotal      += nTracksFound;
  m_NTracksLostByMdcTotal += nTracksLost;
  m_NTracksLostByExtTotal += nTracksLostByExt;
  m_NTracksMdcGhostTotal  += nTracksMisFound;
  if (aRecMucTrackCol->size() > 0) 
  {
    RecMucTrack *aRecMucTrack = (*aRecMucTrackCol)[0]; 
    //m_posx = 0.0; m_posy = 0.0; m_posz = 0.0; 
    if(m_NtOutput>=1){
      m_posx = aRecMucTrack->getMucPos().x();
      m_posy = aRecMucTrack->getMucPos().y();
      m_posz = aRecMucTrack->getMucPos().z();

      m_posx_sigma = aRecMucTrack->getMucPosSigma().x();
      m_posy_sigma = aRecMucTrack->getMucPosSigma().y();
      m_posz_sigma = aRecMucTrack->getMucPosSigma().z();
    }
    //cout << m_posx << " " << m_posy << " " << m_posz << endl;
    //    m_tuple->write();
  }
  
  if(m_NtOutput>=1) m_tuple->write(); 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MucRecTrkExt::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  log << MSG::INFO << " In " << m_NDigisTotal << " total MucDigis " << endreq;
  log << MSG::INFO << " In " << m_NHitsTotal  << " total MucMcHits " << endreq;
  log << MSG::INFO << m_NHitsFoundTotal    << " hits found in Reco tracks, " << endreq;
  log << MSG::INFO << m_NHitsLostTotal     << " hits lost (not found), of which " 
      << m_NHitsLostByMdcTotal << " lost because Mdc track not constructed " 
      << m_NHitsLostByExtTotal << " lost because Ext track not intersect with muc " << endreq;
  log << MSG::INFO << m_NHitsMisFoundTotal << " hits mis found (not belong to this track, but mis attached)" << endreq; 
  
  log << MSG::INFO << " In " << m_NTracksTotal << " total Mc tracks " << endreq;
  log << MSG::INFO << m_NTracksRecoTotal << " tracks found " << endreq;
  log << MSG::INFO << m_NTracksLostByMdcTotal << " tracks lost because Mdc track not constructed " << endreq;
  log << MSG::INFO << m_NTracksLostByExtTotal << " tracks lost because Ext track not intersect with muc " << endreq;
  log << MSG::INFO << m_NTracksMdcGhostTotal  << " tracks are Mdc ghost tracks " << endreq;
 
  /*
  for(int i = 0; i < 20; i++) log << MSG::DEBUG << "lost " << i << " hits track " << m_NHitsLost[i] << endreq;
  for(int i = 0; i < 9; i++)  log << MSG::DEBUG << "lost on gap " << i << " is " << m_NHitsLostInGap[i] << endreq;
  cout << m_TrackLostHit.size() << " track has hit lost, their event id : " << endl;
  for (int i = 0; i < m_TrackLostHit.size(); i++) {
    cout << m_TrackLostHit[i] << " : " << m_TrackLostHitNumber[i] << endl;
  }
  cout << m_TrackLostByMdc.size() << " tracks lost by Mdc, their event id : " << endl;
  for (int i = 0; i < m_TrackLostByMdc.size(); i++) {
    cout << m_TrackLostByMdc[i] << endl;
  }
  cout << m_TrackLostByExt.size() << " tracks lost by Ext no intersection with muc, their event id : " << endl;
  for (int i = 0; i < m_TrackLostByExt.size(); i++) {
    cout << m_TrackLostByExt[i] << endl;
  }
  */

  return StatusCode::SUCCESS;
}

void 
MucRecTrkExt::TrackFinding(RecMucTrack *aTrack)
{
  MsgStream log(msgSvc(), name());

  Hep3Vector currentPos = aTrack->GetCurrentPos();
  Hep3Vector currentDir = aTrack->GetCurrentDir();
//   if(currentPos.mag() < kMinor) {
//     log << MSG::WARNING << "No MUC intersection in track " << endreq;
//     continue;
//   }
   
  int firstHitFound[2] = { 0,  0}; // Has the fist position in this orient determined? if so, could CorrectDirection()
  int firstHitGap[2]   = {-1, -1}; // When the fist position in this orient determined, the gap it is on
  for(int partSeq = 0; partSeq < (int)MucID::getPartNum(); partSeq++) 
  {
    int iPart = kPartSeq[partSeq];
    for(int iGap = 0; iGap < (int)MucID::getGapNum(iPart); iGap++) 
    {
      int seg = -1;
      int orient = MucGeoGeneral::Instance()->GetGap(iPart, 0, iGap)->Orient();;
      float xInsct, yInsct, zInsct;
      aTrack->Project(iPart, iGap, xInsct, yInsct, zInsct, seg);
      log << MSG::INFO << "part " << iPart << " gap " << iGap << " x " << xInsct << " y " << yInsct << " z " << zInsct << " seg " << seg << endreq; 
      //cout<<"project: gap="<<iGap<<"  seg="<<seg<<"  "<<xInsct<<" "<<yInsct<<" "<<zInsct<<endl;

      if(seg == -1) {
        //log << MSG::DEBUG << "no intersection with part " << iPart
        //  << " gap " << iGap << " in this track !" << endl;
        continue;
      }
      aTrack->SetCurrentInsct(xInsct, yInsct, zInsct);
      
      for(int iDeltaSeg = 0; iDeltaSeg < kNSegSearch; iDeltaSeg++) 
      {
        int iSeg = seg + kDeltaSeg[iDeltaSeg];  // also find on neighbor seg;
        if(iSeg < 0) iSeg += MucID::getSegNum(iPart);
        if(iSeg > (int)MucID::getSegNum(iPart) - 1) iSeg -= MucID::getSegNum(iPart);

        //float window = kWindowSize[iPart][iGap];
        Hep3Vector mom_mdc = aTrack->getMdcMomentum();
        float window = getWindowSize(mom_mdc, iPart, iSeg, iGap);

        if (firstHitFound[orient] != 1) window *= kFirstHitWindowRatio;   // if no hits have been found on this orient, expand the window.

        for (int iHit = 0; iHit < m_MucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap); iHit++) 
        {
          log << MSG::DEBUG << "iSeg " << iSeg << " iHit " << iHit << endreq;
          MucRecHit* pHit = m_MucRecHitContainer->GetHit(iPart, iSeg, iGap, iHit);
          //cout<< "strip " << pHit->Strip() << " center " << pHit->GetCenterPos() << endl;

          if (!pHit) {
            log << MSG::WARNING << "MucRecTrkExt: null pointer to pHit" << endreq;
            continue;
          }
          else 
          {
            // Get the displacement of hit pHit to intersection
            float dX  = aTrack->GetHitDistance2(pHit);
            log << MSG::DEBUG << "distance = " << setw(8) << dX << " size " << setw(4) << window << endreq;

            //cout<<"dX= "<<dX<<"  window="<<window<<endl;
            if (fabs(dX) < window) 
            {
              // Attach the hit if it exists
              //cout << "meet window " << pHit->GetSoftID() << endl;
              //****************if this if emc track, we abondon used hit in mdc*******************
              //if(m_emcrec == 1 )
              if(aTrack->GetRecMode() == 0) {
                pHit->SetHitMode(1); //mdc ext
                aTrack->AttachHit(pHit);
                //cout<<"in MucRecTrkExt: trackmode==0 so mdc ext "<<iPart<<" "<<iSeg<<" "<<iGap<<" "<<iHit<<endl;
              }
              if(aTrack->GetRecMode() == 1) {
                //cout<<"in MucRecTrkExt: HitMode = "<<pHit->GetHitMode()<<" part:  "<<iPart<<" "<<iSeg<<" "<<iGap<<" "<<iHit<<endl;
                if(pHit->GetHitMode()!=1) {
                  aTrack->AttachHit(pHit);  //this hit has not been used by mdc ext
                  pHit->SetHitMode(2); //emc ext
                }
              }
              if(aTrack->GetRecMode() == 2) {
                //cout<<"in MucRecTrkExt: HitMode = "<<pHit->GetHitMode()<<" part:  "<<iPart<<" "<<iSeg<<" "<<iGap<<" "<<iHit<<endl;
                if(pHit->GetHitMode()==-1) {
                  aTrack->AttachHit(pHit);  //this hit has not been used by mdc ext
                  pHit->SetHitMode(2); //emc ext
                }
              }

              if (firstHitGap[orient] == -1) firstHitGap[orient] = iGap;
              firstHitFound[orient] = 1;
              //cout << "You could correct directon in orient " << orient << endl;
              //cout<< " part " << iPart << " seg " << iSeg << " gap " << iGap 
              //	  << " strip " << setw(2) << pHit->Strip() << " attatched" << endl;
              log << MSG::DEBUG << " part " << iPart << " seg " << iSeg << " gap " << iGap 
                << " strip " << setw(2) << pHit->Strip() << " attatched" << endreq;
              log << MSG::DEBUG << "current total hits " << aTrack->GetTotalHits() << endreq;
            }
            else 
            {
              m_NHitsLostInGap[iGap]++;
              //log << MSG::DEBUG cout << " part " << iPart << " seg " << iSeg << " gap " << iGap 
              //       << " strip " << setw(2) << pHit->GetSoftID().GetStrip() 
              //     << " not attached !" << endreq;
            }
          } // end pHit else
        } // end iHit
        
        aTrack->CalculateInsct(iPart, iSeg, iGap);
      } // end DeltaSeg
      
      // When correct dir in the orient is permitted and this gap is not the gap first hit locates. 
      if (firstHitFound[orient] && firstHitGap[orient] != iGap) aTrack->CorrectDir();
      aTrack->CorrectPos();
      //cout << "Current Intersection " << aTrack->GetCurrentInsct() << endl;
      //cout << "Current Direction    " << aTrack->GetCurrentDir() << endl;
      aTrack->AttachInsct(aTrack->GetCurrentInsct());
    } // end iGap
  } // end iPart
  
  aTrack->LineFit(m_fittingMethod);
  aTrack->ComputeTrackInfo(m_fittingMethod);

  //cout<<" in TrackFinding: depth= "<<aTrack->GetDepth()<<endl;
} // End TrackFinding()

//the function to get window size with (mom, part, seg, gap); getMdcMomentum
float
MucRecTrkExt::getWindowSize(Hep3Vector mom, int part, int seg, int gap)
{
  int mom_id;
  if(mom.mag()<0.6) mom_id = 0;
  else if(mom.mag()<0.8) mom_id = 1;
  else if(mom.mag()<1) mom_id = 2;
  else mom_id = 3;

  return kWindowSize_Mom_Gap[mom_id][gap]; 
}

// END
