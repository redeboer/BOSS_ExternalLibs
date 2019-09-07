//$id$
//
//$log$

/*
 *    2003/12/26   Zhengyun You     Peking University
 * 
 */

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"
#include "EmcRecEventModel/RecEmcEventModel.h" 
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MucRawEvent/MucDigi.h"
#include "ReconEvent/ReconEvent.h"
#include "ExtEvent/RecExtTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "McTruth/McKine.h"
#include "McTruth/McParticle.h"
#include "Identifier/Identifier.h"
#include "Identifier/MucID.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "MucRecAlg/MucRecRoadFinder.h"
#include "MucRecAlg/MucRecRoadFinderParameter.h"
#include "MucGeomSvc/IMucGeomSvc.h"
#include "MucGeomSvc/MucConstant.h"
#include "MucRecEvent/MucRecHit.h"
#include "MucRecEvent/MucRecHitContainer.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRec3DRoad.h"
#include "MucRecEvent/MucRec2DRoad.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace Event;

MucRecRoadFinder::MucRecRoadFinder(const std::string &name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
  declareProperty("FittingMethod", m_fittingMethod = 2);
  declareProperty("ConfigFile",  m_configFile = "MucConfig.xml");
  declareProperty("McCosmic",    m_mccosmic = 0);
  declareProperty("OnlySeedFit",    m_onlyseedfit = 0);
  declareProperty("NtOutput",      m_NtOutput = 0);
  declareProperty("MaxHitsRec",      m_maxHitsRec = 200);
  declareProperty("United", m_united = 0);
  declareProperty("SeedType", m_seedtype = 0);
  declareProperty("MsOutput", m_MsOutput = false);
  declareProperty("FilterFile", m_filter_filename);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MucRecRoadFinder::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  m_NHitsTotal = 0;
  m_NHitsLostTotal = 0;
  for(int i = 0; i < 20; i++) m_NHitsLost.push_back(0);
  for(int i = 0; i < 10; i++) m_NHitsLostInGap.push_back(0);

  m_NEvent        = 0;
  m_NEventWithHit = 0;
  m_NEventReco    = 0;

  IMucGeomSvc* mucGeomSvc;
  StatusCode sc = service("MucGeomSvc", mucGeomSvc);
  if (sc ==  StatusCode::SUCCESS) {              
    mucGeomSvc->Dump();
    //cout<<"1st wire id:"<<mucGeomSvc->Wire(0)->Id()<<endl;
    //cout<<"2nd wire lyr id:"<<mucGeomSvc->Wire(1)->Lyr()->Id()<<endl;
    //cout<<"6860th wire lyr id:"<<mucGeomSvc->Wire(6859)->Lyr()->Id()<<endl; 
  } else {
    return StatusCode::FAILURE;
  }

  aMucRecHitContainer = 0;

  if(m_NtOutput>=1){ 
  NTuplePtr nt1(ntupleSvc(), "FILE401/T");
  
  if ( nt1 ) { m_tuple = nt1;}
  else { 
    //    m_tuple = ntupleSvc()->book ("MyTuples/1", CLID_RowWiseTuple, "MdcTrkRecon N-Tuple");
    m_tuple = ntupleSvc()->book ("FILE401/T", CLID_RowWiseTuple, "MucTrkRecon N-Tuple");
    if ( m_tuple )    {
      sc = m_tuple->addItem ("part",  m_part);
      sc = m_tuple->addItem ("seg",   m_seg);
      sc = m_tuple->addItem ("gap",   m_gap);
      sc = m_tuple->addItem ("strip", m_strip);
      sc = m_tuple->addItem ("diff",  m_diff);
      sc = m_tuple->addItem ("dist",  m_dist);
      sc = m_tuple->addItem ("run",   m_run);
      sc = m_tuple->addItem ("event", m_event);
      sc = m_tuple->addItem ("ngap",  m_ngapwithhits);
      sc = m_tuple->addItem ("nhit",  m_nhit);
      sc = m_tuple->addItem ("maxhit",  m_maxhit);
      sc = m_tuple->addItem ("multihit",m_multihit);
      sc = m_tuple->addItem ("angleCosmic",m_angle_cosmic);
      sc = m_tuple->addItem ("angleUpdown",m_angle_updown);  
      sc = m_tuple->addItem ("px",m_px);
      sc = m_tuple->addItem ("py",m_py);
      sc = m_tuple->addItem ("pz",m_pz);
      sc = m_tuple->addItem ("theta",m_theta);
      sc = m_tuple->addItem ("phi",m_phi);
      sc = m_tuple->addItem ("theta_pipe",m_theta_pipe);
      sc = m_tuple->addItem ("phi_pipe",m_phi_pipe);
      sc = m_tuple->addItem ("pxmc",m_px_mc);
      sc = m_tuple->addItem ("pymc",m_py_mc);
      sc = m_tuple->addItem ("pzmc",m_pz_mc);
      sc = m_tuple->addItem ("thetamc",m_theta_mc);
      sc = m_tuple->addItem ("phimc",m_phi_mc);
      sc = m_tuple->addItem ("thetamc_pipe",m_theta_mc_pipe);
      sc = m_tuple->addItem ("phimc_pipe",m_phi_mc_pipe);
      sc = m_tuple->addItem ("emcUp",m_emcUp);
      sc = m_tuple->addItem ("emcDown",m_emcDown);
      sc = m_tuple->addItem ("mucUp",m_mucUp);
      sc = m_tuple->addItem ("mucDown",m_mucDown);
      sc = m_tuple->addItem ("projx",m_projx);
      sc = m_tuple->addItem ("projz",m_projz);
   
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
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MucRecRoadFinder::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Part 1: Get the event header, print out event and run number
  int event, run;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::INFO << "Run: " << eventHeader->runNumber()  << "  Event: " <<  eventHeader->eventNumber() << endreq;

event = eventHeader->eventNumber();
run = eventHeader->runNumber();

//cout<<"events  run  "<<event <<"  "<<run<<endl;
  string release = getenv("BES_RELEASE");
  // if(release=="6.6.5"&&run==35896&&event==7448){return ( StatusCode::SUCCESS);}
  // if(release=="6.6.5"&&run==37241&&event==1690731){return ( StatusCode::SUCCESS);}
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

  int digiId;

  //Part 2: Retrieve MC truth 

  Hep3Vector cosmicMom;
  if(m_mccosmic==1){
    SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if (!mcParticleCol) {
      log << MSG::FATAL << "Could not find McParticle" << endreq;
      return( StatusCode::FAILURE);
    }

    McParticleCol::iterator iter_mc = mcParticleCol->begin();
    for (;iter_mc != mcParticleCol->end(); iter_mc++, digiId++) {
      log << MSG::DEBUG 
        << " particleId = " << (*iter_mc)->particleProperty()
        << endreq;   
      int pid = (*iter_mc)->particleProperty();
      //cout<<"in mucroadfinder pid = "<<pid<<endl;
      if(fabs(pid)!=13) continue;

      HepLorentzVector initialMomentum = (*iter_mc)->initialFourMomentum();
      Hep3Vector startP(initialMomentum.px(), initialMomentum.py(), initialMomentum.pz());
      Hep3Vector rotate(-initialMomentum.px(),initialMomentum.pz(),initialMomentum.py());

      if(m_NtOutput>=1){
        m_px_mc = initialMomentum.px();
        m_py_mc = initialMomentum.py();
        m_pz_mc = initialMomentum.pz();

        m_theta_mc = rotate.theta();
        m_phi_mc   = rotate.phi();

        m_theta_mc_pipe = startP.theta();
        m_phi_mc_pipe   = startP.phi();

        //m_tuple->write();  
      }

      //if(initialMomentum.py()>0)cout<<"py>0?????  "<<pid<<endl;
      cosmicMom = startP;

    }
  }
  /*
  //Part 3: Retrieve MDC digi 
  SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
  if (!mdcDigiCol) {
  log << MSG::FATAL << "Could not find MDC digi" << endreq;
  return( StatusCode::FAILURE);
  }

  MdcDigiCol::iterator iter1 = mdcDigiCol->begin();
  digiId = 0; 

  for (;iter1 != mdcDigiCol->end(); iter1++, digiId++) {
  log << MSG::INFO << "MDC digit No: " << digiId << endreq;

  log << MSG::INFO 
  << " time_channel = " << (*iter1)->getTimeChannel()
  << " charge_channel = " << (*iter1)->getChargeChannel() 
  << endreq;   
  }

  //Part 4: Retrieve TOF digi 
  SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc(),"/Event/Digi/TofDigiCol");
  if (!tofDigiCol) {
  log << MSG::FATAL << "Could not find TOF digi" << endreq;
  return( StatusCode::FAILURE);
  }

  TofDigiCol::iterator iter2 = tofDigiCol->begin();
  digiId = 0; 

  for (;iter2 != tofDigiCol->end(); iter2++, digiId++) {
  log << MSG::INFO << "TOF digit No: " << digiId << endreq;

  }

  //Part 5: Retrieve EMC digi 
  SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
  log << MSG::FATAL << "Could not find EMC digi" << endreq;
  return( StatusCode::FAILURE);
  }

  EmcDigiCol::iterator iter3 = emcDigiCol->begin();
  digiId = 0; 

  for (;iter3 != emcDigiCol->end(); iter3++, digiId++) {
  log << MSG::INFO << "Emc digit No: " << digiId << endreq;

  log << MSG::INFO 
  << " time_channel = " << (*iter3)->getTimeChannel()
  << " charge_channel = " << (*iter3)->getChargeChannel() 
  << endreq;   
  }
   */

  //Part 6: Retrieve MUC digi 
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc(),"/Event/Digi/MucDigiCol");
  if (!mucDigiCol) {
    log << MSG::FATAL << "Could not find MUC digi" << endreq;
    return( StatusCode::FAILURE);
  }

  MucDigiCol::iterator iter4 = mucDigiCol->begin();
  digiId = 0; 
  for (;iter4 != mucDigiCol->end(); iter4++, digiId++) {
  }
  log << MSG::INFO << "MUC digis:: " << digiId << endreq;
  if( digiId == 0) return( StatusCode::SUCCESS );
  
  //********************************//
  RecMucTrackCol *aMucTrackCol;
  int trackId     = -1;
  int muctrackId  = -1;

  if(m_united != 1)  // if this algorithm run individualy, we need create hitcollection and trackcollection...
  {
    Identifier mucID;
    if (!aMucRecHitContainer) {
      aMucRecHitContainer = new MucRecHitContainer();
    }
    aMucRecHitContainer->Clear();

    MucRecHitCol *aMucRecHitCol = new MucRecHitCol();
    aMucRecHitContainer->SetMucRecHitCol(aMucRecHitCol);

    
    SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
    DataObject *mucRecHitCol;
    eventSvc()->findObject("/Event/Recon/MucRecHitCol",mucRecHitCol);
    if(mucRecHitCol != NULL) {
      dataManSvc->clearSubTree("/Event/Recon/MucRecHitCol");
      eventSvc()->unregisterObject("/Event/Recon/MucRecHitCol");
    }

    StatusCode sc = eventSvc()->registerObject("/Event/Recon/MucRecHitCol", aMucRecHitContainer->GetMucRecHitCol());

    MucDigiCol::iterator iterMuc = mucDigiCol->begin();
    int mucDigiId = 0;
    for (;iterMuc != mucDigiCol->end(); iterMuc++, mucDigiId++) {
      mucID = (*iterMuc)->identify();
      int part  = MucID::barrel_ec(mucID);
      int seg   = MucID::segment(mucID);
      int gap   = MucID::layer(mucID);
      int strip = MucID::channel(mucID);
      //aMucRecHitContainer->AddHit(mucID);
      //if(part==1 && seg==2 && gap==8 && (strip==19||strip==16));//cout<<"noise hit!!!=========="<<endl;
      //else            /////this problem has been solved!
      aMucRecHitContainer->AddHit(part, seg, gap, strip);
      log << MSG::INFO <<  " digit" << mucDigiId << " : " 
        << " part "  << part
        << " seg "   << seg
        << " gap "   << gap
        << " strip " << strip
        << endreq;
    }

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

    //********************************//
    // Create track Container;
    DataObject *mucTrackCol;
    eventSvc()->findObject("/Event/Recon/RecMucTrackCol",mucTrackCol);
    if(mucTrackCol != NULL) {
      dataManSvc->clearSubTree("/Event/Recon/RecMucTrackCol");
      eventSvc()->unregisterObject("/Event/Recon/RecMucTrackCol");
    }

    aMucTrackCol = new RecMucTrackCol();
    sc = eventSvc()->registerObject("/Event/Recon/RecMucTrackCol", aMucTrackCol);
    aMucTrackCol->clear();

    // check MucTrackCol registered
    SmartDataPtr<RecMucTrackCol> findMucTrackCol(eventSvc(),"/Event/Recon/RecMucTrackCol");
    if (!findMucTrackCol) { 
      log << MSG::FATAL << "Could not find RecMucTrackCol" << endreq;
      return( StatusCode::FAILURE);
    }   
    aMucTrackCol->clear();


    log << MSG::INFO <<"MaxHitsRec : "<<m_maxHitsRec<< endreq;
    if(digiId> m_maxHitsRec) return StatusCode::SUCCESS;  //too many digit! abnormal------2008-04-17 
    //********************************//
    // Create track Container;
    
    trackId = 0;
    muctrackId = 0;
  }//// conrespond to if(m_united != 1) !!!!!!!
  else if(m_united == 1){

    Identifier mucID;
    if (!aMucRecHitContainer) {
      aMucRecHitContainer = new MucRecHitContainer();
    }
    aMucRecHitContainer->Clear();

    SmartDataPtr<MucRecHitCol> aMucRecHitCol (eventSvc(),"/Event/Recon/MucRecHitCol"); 
    if(aMucRecHitCol == NULL) {
      log << MSG::WARNING << "MucRecHitCol is NULL" << endreq;
    }

    aMucRecHitContainer->SetMucRecHitCol(aMucRecHitCol);

    SmartDataPtr<RecMucTrackCol> mucTrackCol (eventSvc(),"/Event/Recon/RecMucTrackCol");
    if(mucTrackCol == NULL) {
      log << MSG::WARNING << "aMucTrackCol is NULL" << endreq;
    }

    log << MSG::INFO <<"mucTrackCol size: "<<mucTrackCol->size()<<"  hitCol size: "<<aMucRecHitCol->size()<<endreq;
    aMucTrackCol = mucTrackCol;
    
    // Retrieve Ext track Col from TDS for setting trackId 
    SmartDataPtr<RecExtTrackCol> aExtTrackCol(eventSvc(),"/Event/Recon/RecExtTrackCol");
    if (!aExtTrackCol) {
      log << MSG::WARNING << "Can't find ExtTrackCol in TDS!" << endreq;
    }

    SmartDataPtr<RecMdcTrackCol> aMdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
    if (!aMdcTrackCol) {
      log << MSG::WARNING << "Can't find MdcTrackCol in TDS!" << endreq;
    }

    if( aExtTrackCol && aMdcTrackCol ) trackId = aMdcTrackCol->size();
    muctrackId = aMucTrackCol->size();
  }
  
  int hasEmcUp = 0;
  int hasEmcDown = 0;
  SmartDataPtr<RecEmcShowerCol> aShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if (!aShowerCol) { 
    log << MSG::WARNING << "Could not find RecEmcShowerCol" << endreq;
    //return( StatusCode::FAILURE);
  }
  else{
    RecEmcShowerCol::iterator iShowerCol;
    for(iShowerCol=aShowerCol->begin();
        iShowerCol!=aShowerCol->end();
        iShowerCol++){
      /*
         cout<<"check EmcRecShowerCol registered:"<<endl;
         cout<<"shower id: "<<(*iShowerCol)->ShowerId()<<"\t"
         <<"shower energy: "<<(*iShowerCol)->Energy()<<"\n"
         <<"shower position: "<<(*iShowerCol)->Position()<<endl;
       */
      if(((*iShowerCol)->position()).phi()>0&&((*iShowerCol)->position()).phi()<3.1415926) hasEmcUp++;
      else hasEmcDown++;
    }
  }
  if(m_NtOutput >= 1){
    m_emcUp = hasEmcUp;  m_emcDown = hasEmcDown;
  }

  //********************************//
  m_NEvent++;
  MucRec3DRoadContainer *p3DRoadC  = new MucRec3DRoadContainer();
  // Find or create the 3D road container ...
  if (!p3DRoadC) {
    p3DRoadC = new MucRec3DRoadContainer();
  }
  p3DRoadC->clear();    // make sure that the container is empty

  MucRec2DRoadContainer *p2DRoad0C = new MucRec2DRoadContainer();
  // Find or create the 2D road0 container ...
  if (!p2DRoad0C) {
    p2DRoad0C = new MucRec2DRoadContainer();
  }
  p2DRoad0C->clear();    // make sure that the container is empty

  MucRec2DRoadContainer *p2DRoad1C = new MucRec2DRoadContainer();
  // Find or create the 2D road1 container ...
  if (!p2DRoad1C) {
    p2DRoad1C = new MucRec2DRoadContainer();
  }
  p2DRoad1C->clear();    // make sure that the container is empty
  log << MSG::INFO << "Muc 2D 3D Road Container created" << endreq;

  // We have all of the input and output parameters now, so do
  // something useful with them!
  //static bool  first_call = true;
  //
  // Now find some roads!
  //

  MucRecHit *pHit0 = 0, *pHit1 = 0;
  int count0, count1, count, iGap0, iGap1;
  int orient;

  for (int iPart = 0; iPart < (int)MucID::getPartNum(); iPart++) {
    for (int iSeg = 0; iSeg < (int)MucID::getSegNum(iPart); iSeg++) {
      for (int iOrient = 0; iOrient < 2; iOrient++) {
        int nLoops = 1;
        if(m_seedtype == 1) nLoops = kNSeedLoops;
        for (int iLoop = 0; iLoop < nLoops; iLoop++) { //now only 1 loop
          // checkout the seed gap(s) from search order
          int length = kSearchLength[iLoop][iPart][iOrient];
          if(m_seedtype == 1){
            iGap0 = kSearchOrder[iLoop][iPart][iOrient][0];
            iGap1 = kSearchOrder[iLoop][iPart][iOrient][1];
          }
          else {  //new method to calc seed gaps------//
            int setgap0 = 0;
            MucDigiCol::iterator iterMuc = mucDigiCol->begin();
            int mucDigiId = 0;
            Identifier mucID;
            iGap0 = 0; iGap1 = 0;
            for (;iterMuc != mucDigiCol->end(); iterMuc++, mucDigiId++) {
              mucID = (*iterMuc)->identify();
              int part  = MucID::barrel_ec(mucID);
              int seg   = MucID::segment(mucID);
              int gap   = MucID::layer(mucID);
              int strip = MucID::channel(mucID);
              int orient = 0;
              if ( (part == 1 && gap % 2 == 0) || (part != 1 && gap % 2 == 1) ) orient = 1;

              if(part == iPart && seg == iSeg && orient == iOrient && setgap0 == 0){
                iGap0 = gap;
                setgap0 = 1;
              }//make sure that iGap0 record the 1st gap in this orient
              if(part == iPart && seg == iSeg && orient == iOrient && setgap0 == 1 && gap != iGap0 ){
                iGap1 = gap;
              }//make sure that iGap1 record the last gap in this orient
            }     
          }
          //                                                              
          if(m_MsOutput) cout <<"Find seed gap in ( "<<iPart<<"  "<<iSeg<<" ) gap0: "<<iGap0<<"  gap1: "<<iGap1<<endl;

          //new method to calc seed gaps------//
          if(iGap0 > iGap1){
            int tempGap = iGap0;
            iGap0 = iGap1;
            iGap1 = tempGap;
          } 
          if(iGap1 == iGap0) continue;

          count0 = aMucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap0);
          for (int iHit0 = 0; iHit0 < count0; iHit0++) {
            //cout << "iHit0 = " << iHit0 << endl;
            pHit0 = aMucRecHitContainer->GetHit(iPart, iSeg, iGap0, iHit0);
            if (!pHit0) {
              log << MSG::FATAL << "MucRecRoadFinder-E10 " << " part " << iPart
                << " seg " << iSeg << " gap "  << iGap0 << " null pointer"
                << endl;
            }
            else {

              //this algo use with ext and this hit has been used before; 
              if(m_united == 1 && pHit0->GetHitMode() != -1) continue;

              count1 = aMucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap1);
              if(m_MsOutput) cout << "part "  << iPart << " seg " << iSeg  << " orient " << iOrient
                << " gap0 " << iGap0 << " count0 " << count0 
                << " gap1 " << iGap1 << " count1 " << count1 << endl;
              for (int iHit1 = 0; iHit1 < count1; iHit1++) {
                //cout << "iHit1 = " << iHit1 << endl;
                pHit1 = aMucRecHitContainer->GetHit(iPart, iSeg, iGap1, iHit1);
                if (!pHit1) {
                  log << MSG::ERROR << "MucRecRoadFinder-E10 " << " part " << iPart
                    << " seg " << iSeg << " gap " << iGap1 << " null pointer"
                    << endl;
                } else {

                  //this algo use with ext and this hit has been used before;		  	
                  if(m_united == 1 && pHit1->GetHitMode() != -1) continue;

                  // Found seed hit(s), create a new 2D road object,
                  // and attach hit pHit1 and pHit0
                  MucRec2DRoad *road2D = new MucRec2DRoad(iPart, iSeg, iOrient, 0.0, 0.0, 3000.0);
                  if (!road2D) {
                    log << MSG::FATAL << "MucRecRoadFinder-E20 " << "failed to create 2D road!"  << endreq;
                    continue;
                  }
                  road2D->SetMaxNSkippedGaps(kMaxSkippedGaps);

                  if (!pHit0->GetGap()) log << MSG::ERROR << "pHit0->GetGap(), null pointer" << endreq;
                  if (pHit0->GetGap()->Orient() == iOrient) {
                    //float xStrip, yStrip, zStrip;
                    //MucRecGeometry::GetPointer()->GetStripPointer(pHit0->GetSoftID())->GetCenterPos(xStrip, yStrip, zStrip);
                    //cout << " x = " << xStrip << " y = " << yStrip << " z = " << zStrip << endl;

                    //set hit as seed then
                    bool isseed = true;
                    pHit0->SetHitSeed(true);
                    pHit1->SetHitSeed(true);

                    road2D->AttachHit(pHit0);
                    if(m_MsOutput) cout << "pHit0 attached, " << road2D->GetTotalHits() 
                      << ", hitId= "<<pHit0->Part()<<"  "<<pHit0->Seg()<<"  "<<pHit0->Gap()<<"  "<<pHit0->Strip()<<endl;
                  }

                  if (pHit1->GetGap()->Orient() == iOrient) {
                    //cout << pHit1->GetSoftID() << endl;
                    //float xStrip, yStrip, zStrip;
                    //MucRecGeometry::GetPointer()->GetStripPointer(pHit1->GetSoftID())->GetCenterPos(xStrip, yStrip, zStrip);
                    //cout << " x = " << xStrip << " y = " << yStrip << " z = " << zStrip << endl;
                    road2D->AttachHit(pHit1);
                    if(m_MsOutput) cout << "pHit1 attached, " << road2D->GetTotalHits() 
                      << ", hitId= "<<pHit1->Part()<<"  "<<pHit1->Seg()<<"  "<<pHit1->Gap()<<"  "<<pHit1->Strip()<<endl;
                  }
                  if(m_MsOutput) cout << "Hit cout " << road2D->GetTotalHits() << ", slope " << road2D->GetSlope() << endl;

                  // After first (two) hit(s) is(are) attached,
                  // calculate reference pos, direction.
                  // Project to the other planes;
                  // attach clusters within search window.

                  for (int i = 0; i < length; i++) {
                    int iGap = kSearchOrder[iLoop][iPart][iOrient][i];

                    float dXmin = kInfinity;
                    MucRecHit *pHit = 0;

                    float Win = kWindowSize[iPart][iGap];
                    //Win = road2D->GetSearchWindowSize(iGap);  

                    // following line should be removed after
                    // we have a good function classes for search window
                    //Win = WindowParGamma[iGap];

                    //search hit in iGap
                    count = aMucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap);
                    for (int iHit = 0; iHit < count; iHit++) {
                      pHit = aMucRecHitContainer->GetHit(iPart, iSeg, iGap, iHit);
                      if (!pHit) {
                        log << MSG::FATAL << "MucRecRoadFinder-E20 null cluster pointer pHit" << endreq;
                        continue;
                      }
                      else {

                        //this algo use with ext and this hit has been used before;                   
                        if(m_united == 1 && pHit->GetHitMode() != -1) continue;

                        // Get the displacement of hit pHit to road2D
                        float dX = road2D->GetHitDistance(pHit);
                        if(m_MsOutput) cout<<"dX = "<<dX<<" Win = "<<Win<<", hit: "<<pHit->Part()<<"  "<<pHit->Seg()<<"  "<<pHit->Gap()<<"  "<<pHit->Strip()<<endl;

                        if (dX < Win) {
                          // Attach the hit if it exists
                          // cout << "meet window " << pHit->GetSoftID() << endl;
                          if(iGap == iGap0 || iGap == iGap1) { //hits in these gap is used to be seed, so unvailable for calibrate effi;
                            if(pHit->GetHitMode() == -1){  
                              pHit->SetHitMode(3); //self road finder mode
                            }
                            else if(pHit->GetHitMode() == 3){
                              pHit->SetHitMode(4); //self road finder mode used!!!
                            }
                          }
                          // attach hits if we need fit or hit. OR, only attach hits not in seed layers
                          if(m_onlyseedfit == 0)road2D->AttachHit(pHit);
                          else {
                            if(iGap == iGap0 || iGap == iGap1) road2D->AttachHit(pHit);
                            else road2D->AttachHitNoFit(pHit);
                          }
                        }
                      }
                    }


                  } //  for (int i = 0; i < length; ...), Search all gaps


                  // Ok we found a road already, we need to know
                  // whether we should save it or not
                  // check road quality...
                  //

                  // 1. last gap of the road
                  bool lastGapOK = false;
                  if ( road2D->GetLastGap() >= kMinLastGap2D ) {
                    lastGapOK = true;
                  }
                  else {
                    log<<MSG::INFO << " 2D lastGap " << road2D->GetLastGap() << " < " << kMinLastGap2D << endreq;
                  }

                  // 2. positon at reference plane

                  // 3. number of gaps contain hits
                  bool firedGapsOK = false;
                  if (road2D->GetNGapsWithHits() >= kMinFiredGaps) {
                    firedGapsOK = true;
                  }
                  else {
                    log<<MSG::INFO << " 2D firedGaps " << road2D->GetNGapsWithHits() << " < " << kMinFiredGaps << endreq;
                  }

                  // 4. duplicated road check
                  bool duplicateRoad = false;
                  int maxSharedHits = 0, sharedHits = 0;
                  if (iOrient == 0) {
                    for (int index = 0; index < (int)p2DRoad0C->size(); index++) {
                      sharedHits =(*p2DRoad0C)[index]->GetNSharedHits(road2D);
                    }
                    if (maxSharedHits < sharedHits) maxSharedHits = sharedHits;
                  } 
                  else {
                    for (int index = 0; index < (int)p2DRoad1C->size(); index++) {
                      sharedHits = (*p2DRoad1C)[index]->GetNSharedHits(road2D);
                    }
                    if (maxSharedHits < sharedHits) maxSharedHits = sharedHits;
                  }

                  if(road2D->GetTotalHits() == maxSharedHits
                      || maxSharedHits >= kMinSharedHits2D ) {
                    duplicateRoad = true;
                    log<<MSG::INFO << " maxSharedHits " << maxSharedHits << " >  "  << kMinSharedHits2D << endreq;
                  }

                  // Here is our criteria for a candidate road
                  // i.e. 1) There are at least two gaps containing hits
                  //      2) LastGap of the road passes last plane cut
                  //      3) Reference position passes vertex cut
                  //      4) No existing road share same hits with the road

                  if (lastGapOK && firedGapsOK && !duplicateRoad) {
                    if (iOrient == 0) {
                      log<<MSG::INFO << "Add a road0" << endreq;
                      p2DRoad0C->add(road2D);
                    }
                    else { 
                      log<<MSG::INFO << "Add a road1" << endreq;
                      p2DRoad1C->add(road2D);
                    }
                  }
                  else {

                    // reset hit mode to be -1 if this road useless! 
                    vector<MucRecHit*> road2DHits = road2D->GetHits();
                    for(int i=0; i< road2DHits.size(); i++)
                    {
                      MucRecHit *ihit = road2DHits[i];
                      if(ihit->Gap() == iGap0 || ihit->Gap() == iGap1){
                        if(ihit->GetHitMode() == 3) ihit->SetHitMode(-1);
                        if(ihit->GetHitMode() == 4) ihit->SetHitMode(3);
                      }
                    }
                    delete road2D;
                  }
                } // else { 
              } // for ( int iHit = 0 ...) 
              } // else {
            } // for ( int iHit0 ...)
            } // for (int iLoop ...)
          } // for (int iOrient ...)
        } // for (int iSeg ...)
      } // for(iPartArm ...)

      int print2DRoadInfo = 0;
      if (print2DRoadInfo == 1) {
        // print 2DRoad container info.
        cout << "In 2DRoad container : " << endl
          << "   Num of 2DRoad0 = "   << p2DRoad0C->size() << endl 
          << endl;

        for (int iRoad = 0; iRoad < (int)p2DRoad0C->size(); iRoad++) {

          MucRec2DRoad *road = (*p2DRoad0C)[iRoad];
          cout << "      " << iRoad << "th 2DRoad0 :" << endl
            << "         Part      = " << road->GetPart()      << endl
            << "         Seg       = " << road->GetSeg()       << endl
            << "         Orient    = " << road->GetOrient()    << endl
            << "         LastGap   = " << road->GetLastGap()   << endl
            << "         TotalHits = " << road->GetTotalHits() << endl
            << "         DOF       = " << road->GetDegreesOfFreedom() << endl 
            << "         Slope     = " << road->GetSlope()     << endl
            << "         Intercept = " << road->GetIntercept() << endl
            << endl;
          //road->PrintHitsInfo();
        }

        cout << "   Num of 2DRoad1 = "   << p2DRoad1C->size() << endl 
          << endl;

        for ( int iRoad = 0; iRoad < (int)p2DRoad1C->size(); iRoad++) {

          MucRec2DRoad *road = (*p2DRoad1C)[iRoad];
          cout << "      " << iRoad << "th 2DRoad1 :" << endl
            << "         Part      = " << road->GetPart()      << endl
            << "         Seg       = " << road->GetSeg()       << endl
            << "         Orient    = " << road->GetOrient()    << endl
            << "         LastGap   = " << road->GetLastGap()   << endl
            << "         TotalHits = " << road->GetTotalHits() << endl
            << "         DOF       = " << road->GetDegreesOfFreedom() << endl 
            << "         Slope     = " << road->GetSlope()     << endl
            << "         Intercept = " << road->GetIntercept() << endl
            << endl;
          //road->PrintHitsInfo();
        }
      }

      // We found all possible 2D roads in one part and seg
      // and now it is time to construct 3D roads base on those 2D roads
      for ( int iRoad0 = 0; iRoad0 < (int)p2DRoad0C->size(); iRoad0++) {
        MucRec2DRoad *road0 = (*p2DRoad0C)[iRoad0];
        for ( int iRoad1 = 0; iRoad1 < (int)p2DRoad1C->size(); iRoad1++){
          MucRec2DRoad *road1 = (*p2DRoad1C)[iRoad1];

          // Create a new road object with road0 and road1
          if ( !(road0 &&road1) ) {
            cout << "Null pointer to road0 or road1:   "
              << "road0 = " << road0
              << "road1 = " << road1
              << endl;
            continue;
          }

          // Check that both 1D roads are in the same part and segment.
          if ( (road0->GetPart() != road1->GetPart()) 
              || (road0->GetSeg() != road1->GetSeg()) ) {
            continue;
          }

          MucRec3DRoad *road3D = new MucRec3DRoad(road0, road1);

          // What are our criteria for accepting a road as valid?
          // Check them here.
          // 1. difference of total number clusters in road0 and in road1
          // 2. difference of last gaps in road0 and road1
          // 3. InterSection OK or not (if X Y clusters in each gap are
          //                            in same panel or overlap region) 
          // 4. Reference position check
          // 5. Chisquare check
          // 6. Last Gap check
          // 7. Duplicate road check

          bool lastGapDeltaOK = false;
          if ( road3D->GetLastGapDelta() <= kMaxDeltaLastGap ) {
            lastGapDeltaOK = true;
          }
          else {
            cout << "LastGapDelta = " << road3D->GetLastGapDelta() << endl;
          }

          bool totalHitsDeltaOK = false;
          if ( road3D->GetTotalHitsDelta() <= kMaxDeltaTotalHits ) {
            totalHitsDeltaOK = true;
          }
          else {
            //cout << "TotalHitsDelta = " << road3D->GetTotalHitsDelta() << endl;
          }

          bool chiSquareCutOK = false;
          float xChiSquare = road0->GetReducedChiSquare();
          float yChiSquare = road1->GetReducedChiSquare();
          if ( xChiSquare <= kMaxXChisq 
              && yChiSquare <= kMaxYChisq ) {
            chiSquareCutOK = true;
          }
          else {
            cout << "xChiSquare = " << xChiSquare 
              << "yChiSquare = " << yChiSquare
              << endl;
          }

          bool minLastGapOK = false;
          if ( road3D->GetLastGap() >= kMinLastGap3D ) {
            minLastGapOK = true;
          }
          else {
            log<<MSG::INFO << " minLastGap = " << road3D->GetLastGap() << endreq;
          }

          bool duplicateRoad = false;
          int maxSharedHits = 0, sharedHits = 0;
          for ( int i = 0; i < (int)p3DRoadC->size(); i++){
            sharedHits =(*p3DRoadC)[i]->GetNSharedHits(road3D);
            if ( maxSharedHits < sharedHits) maxSharedHits = sharedHits;

            //cout<<"judge shared hits:  road["<<i<<"]   max = "<<maxSharedHits<<endl;
          }
          if(road3D->GetTotalHits() == maxSharedHits
              || maxSharedHits >= kMinSharedHits2D ) {
            duplicateRoad = true;
            log<<MSG::INFO << " MaxShareHits = " << maxSharedHits << endreq;
          }

          if ( lastGapDeltaOK &&
              totalHitsDeltaOK &&
              chiSquareCutOK &&
              minLastGapOK &&
              !duplicateRoad ) {
            float vx, vy, x0, y0;
            float slope1 = 100, slope0 = 100;
            if(fabs(road1->GetSlope())<100) slope1 = road1->GetSlope();
            if(fabs(road0->GetSlope())<100) slope0 = road0->GetSlope();

            if ( road3D->GetPart() == 1 ){
              road3D->TransformPhiRToXY( slope1,     slope0,     
                  road1->GetIntercept(), road0->GetIntercept(),
                  vx, vy, x0, y0);
            }
            else {
              vx = slope1;
              x0 = road1->GetIntercept();
              vy = slope0;
              y0 = road0->GetIntercept();
            }
            road3D->SetSimpleFitParams(vx, x0, vy, y0);

            log<<MSG::INFO << "Add a 3D Road ... " << endreq;

            float startx = 0.0, starty = 0.0, startz = 0.0;
            float sigmax = 0.0, sigmay = 0.0, sigmaz = 0.0;
            road3D->ProjectWithSigma(0, startx, starty, startz, sigmax, sigmay, sigmaz);//gap0

            //cout<<"slope1,0 = "<<slope1<<"  "<<slope0<<"  vx,y = "<<vx<<"  "<<vy<<endl;
            //cout<<"startxyz= "<<startx<<"  "<<starty<<"  "<<startz<<endl;
            //mom(vx,vy,1)
            float vz = 1;
            float sign = vy/fabs(vy);
            float signx = vx/fabs(vx);
            //cout<<"vxyz = "<<vx<<" "<<vy<<" "<<vz<<endl;
            if(road3D->GetPart() == 1){
              if(road3D->GetSeg() > 4){   //down segment

                vx *= -sign;
                vy *= -sign;
                vz *= -sign;
              }
              else if(road3D->GetSeg()<2){
                vx *= signx;
                vy *= signx;
                vz *= signx;
              }
              else if(road3D->GetSeg()>=3 && road3D->GetSeg()<=4){
                vx *= -signx;
                vy *= -signx;
                vz *= -signx;
              }
              else{
                vx *= sign;
                vy *= sign;
                vz *= sign;
              }
            }
            else if(road3D->GetPart() == 0){
              //fix me

              //cout<<"startxyz= "<<startx<<"  "<<starty<<"  "<<startz<<endl;
              //cout<<"vx,y,z = "<<vx<<"  "<<vy<<"  "<<vz<<endl;
              //cout<<"in road finder a endcap finded!!! -------------"<<endl;
            }
            else if(road3D->GetPart() == 2){
              //fix me

              vx *= -1;
              vy *= -1;
              vz *= -1;
              //cout<<"startxyz= "<<startx<<"  "<<starty<<"  "<<startz<<endl;
              //cout<<"vx,y,z = "<<vx<<"  "<<vy<<"  "<<vz<<endl;
              //cout<<"in road finder a endcap finded!!! ------------2-"<<endl;

            }


            Hep3Vector mom(vx, vy, vz);

            ////////////construct track
            //MucTrack *aTrack = new MucTrack(road3D);
            //cout<<"startxyz = "<<startx<<"  "<<starty<<"  "<<startz<<endl;
            //cout<<"vxyz = "<<vx<<"  "<<vy<<"  "<<vz<<endl;

            startx /= 10; starty /= 10; startz /= 10;  //mm->cm
            startx -= vx/mom.mag(); starty -= vy/mom.mag(); startz -= vz/mom.mag();  // decrease a little  

            //cout<<"startxyz = "<<startx<<"  "<<starty<<"  "<<startz<<endl;
            RecMucTrack *aTrack = new RecMucTrack();
            aTrack->SetExtMucPos(startx, starty, startz);  //mm->cm
            aTrack->SetExtMucMomentum(vx, vy, vz); 
            aTrack->SetMucPos(startx, starty, startz);
            aTrack->SetMucMomentum(vx, vy, vz);
            aTrack->SetCurrentPos( startx, starty, startz);
            aTrack->SetCurrentDir( vx, vy, vz);
            aTrack->SetRecMode(3);
            //aTrack->LineFit(1);
            //aTrack->ComputeTrackInfo(1);

            aTrack->setTrackId(trackId);
            aTrack->setId(muctrackId);
            trackId++;
            muctrackId++;

            //cout<<"find a track!!!"<<endl; 
            aMucTrackCol->add(aTrack);
            TrackFinding(aTrack);
            p3DRoadC->add(road3D);

            ////////////record strip info in this track//////////////

            vector<MucRecHit*> attachedHits = aTrack->GetHits();
            vector<MucRecHit*> expectedHits = aTrack->GetExpectedHits();
            vector<float>      distanceHits = aTrack->getDistHits();

            for(int i=0; i< expectedHits.size(); i++)
            {
              MucRecHit *ihit = expectedHits[i];
              //cout<<"expected Hits: "<<ihit->Part()<<"  "<<ihit->Seg()<<"  "<<ihit->Gap()<<"  "<<ihit->Strip()<<endl;
            }

            vector<int> multiHit;
            for(int i=0; i< attachedHits.size(); i++)
            {
              MucRecHit *ihit = attachedHits[i];
              //cout<<"attached Hits: "<<ihit->Part()<<"  "<<ihit->Seg()<<"  "<<ihit->Gap()<<"  "<<ihit->Strip()<<" hitmode: "<<ihit->GetHitMode()<<endl;
              //calc multiplicity hits;
              int hitnum = 0;
              for(int k=0; k < attachedHits.size(); k++){
                MucRecHit *khit = attachedHits[k];
                if((ihit->Part()==khit->Part())&&(ihit->Seg()==khit->Seg())&&(ihit->Gap()==khit->Gap()))
                  hitnum++;
              }
              multiHit.push_back(hitnum);
              //cout<<"multi hit: "<<hitnum<<" "<<multiHit[i]<<endl;

            }

            for(int i=0; i< expectedHits.size(); i++)
            { //calc distance between attached hits and expected hits

              MucRecHit *ihit = expectedHits[i];
              //cout<<"attached Hits: "<<ihit->Part()<<"  "<<ihit->Seg()<<"  "<<ihit->Gap()<<"  "<<ihit->Strip()<<endl;

              int diff = -999;

              for(int j=0; j< attachedHits.size(); j++){
                MucRecHit *jhit = attachedHits[j];

                //                if(attachedHits.size()!=distanceHits.size())cout<<"attached hits size no same as disthits!!!"<<endl;

                if((jhit->Part()==ihit->Part())&&(jhit->Seg()==ihit->Seg())&&(jhit->Gap()==ihit->Gap())&&attachedHits.size()==distanceHits.size())
                {    // same gap, cale distance
                  diff = ihit->Strip() - jhit->Strip();
                  //cout<<"diff = "<<diff<<endl;

                  if(m_NtOutput>=2){

                    m_part = ihit->Part();   m_seg = ihit->Seg();  m_gap = ihit->Gap();
                    m_strip = jhit->Strip();
                    m_diff = diff;
                    m_dist = distanceHits[j];
                    //cout<<"distance = "<<m_dist<<endl;

                    m_angle_cosmic = -999;
                    m_angle_updown = -999;
                    //m_px = -999; m_py = -999; m_pz = -999; m_theta = -999; m_phi = -999; m_theta_pipe = -999; m_phi_pipe = -999;
                    //m_px_mc = -999; m_py_mc = -999; m_pz_mc = -999; m_theta_mc = -999; m_phi_mc = -999;
                    //m_theta_mc_pipe = -999; m_phi_mc_pipe = -999;
                    m_ngapwithhits = aTrack->numLayers();
                    m_nhit = aTrack->numHits();
                    m_maxhit = aTrack->maxHitsInLayer();
                    m_multihit = multiHit[j];
                    m_run = eventHeader->runNumber();
                    m_event = eventHeader->eventNumber();

                    m_tuple->write();
                  }
                }


              }



              if(diff == -999) {  // to calc effi of this strip
                if(m_NtOutput>=2){
                  m_part = ihit->Part();   m_seg = ihit->Seg();  m_gap = ihit->Gap();
                  m_strip = ihit->Strip();
                  m_diff = diff;
                  m_dist = -999;
                  m_angle_updown = -999;
                  m_angle_cosmic = -999;
                  //m_px = -999; m_py = -999; m_pz = -999; m_theta = -999; m_phi = -999; m_theta_pipe = -999; m_phi_pipe = -999;
                  //m_px_mc = -999; m_py_mc = -999; m_pz_mc = -999; m_theta_mc = -999; m_phi_mc = -999;
                  //m_theta_mc_pipe = -999; m_phi_mc_pipe = -999;
                  m_ngapwithhits = aTrack->numLayers();
                  m_run = eventHeader->runNumber();
                  m_event = eventHeader->eventNumber();
                  //m_tuple->write();
                }
              }
              //if(diff != -999) cout<< "has hit in this layer"<<endl;

            }
            ////////////record strip info in this track//////////////
            /*
               m_part = -999;
               m_seg  = -999;
               m_gap  = -999;
               m_strip= -999;
               m_diff = -999;

               m_angle_updown = -999;
               m_angle_cosmic =  cosmicMom.angle(aTrack->getMucMomentum());
               if(m_angle_cosmic>1.57) m_angle_cosmic = 3.14159265 - m_angle_cosmic;
               m_run = eventHeader->runNumber();
               m_event = eventHeader->eventNumber();

               double px,py,pz,p,theta,phi;
               px = (aTrack->getMucMomentum()).x(); 
               py = (aTrack->getMucMomentum()).y();
               pz = (aTrack->getMucMomentum()).z();

               Hep3Vector rotate(-px,pz,py);
               theta = rotate.theta();
               phi   = rotate.phi();          


               m_px = px; m_py = py; m_pz = pz;
               m_theta = theta; m_phi = phi; 
               m_theta_pipe = (aTrack->getMucMomentum()).theta();
               m_phi_pipe   = (aTrack->getMucMomentum()).phi();
            //m_px_mc = -999; m_py_mc = -999; m_pz_mc = -999; m_theta_mc = -999; m_phi_mc = -999;
            //m_theta_mc_pipe = -999; m_phi_mc_pipe = -999;
            //////////if(m_py>0)m_tuple->write();

             */

          } 
          else {
            //cout << "Delete a 3D Road ... " << endl;
            delete road3D;
            // don't keep it if it's not a good condidate
          }
        } // for ( int iRoad1 ...
      } // for ( int iRoad0 .. 


      //for cosmic ray, to combine 2 track to 1
      RecMucTrack *aaTrack = 0;
      RecMucTrack *bbTrack = 0;

      int hasMucUp = 0;
      int hasMucDown = 0;
      for(int iTrack = 0; iTrack < (int)aMucTrackCol->size(); iTrack++) {
        aaTrack = (*aMucTrackCol)[iTrack];
        if((aaTrack->getMucMomentum()).phi()<3.1415926&&(aaTrack->getMucMomentum()).phi()>0) hasMucUp++;
        else hasMucDown++;


        double px,py,pz,p,theta,phi;
        px = (aaTrack->getMucMomentum()).x();
        py = (aaTrack->getMucMomentum()).y();
        pz = (aaTrack->getMucMomentum()).z();

        if(py<0)continue;

        if(m_NtOutput>=1){

          m_angle_updown = -999;
          m_angle_cosmic =  cosmicMom.angle(aaTrack->getMucMomentum());
          if(m_angle_cosmic>1.57) m_angle_cosmic = 3.14159265 - m_angle_cosmic;
          m_run = eventHeader->runNumber();
          m_event = eventHeader->eventNumber();

          Hep3Vector rotate(-px,pz,py);
          theta = rotate.theta();
          phi   = rotate.phi();

          m_px = px; m_py = py; m_pz = pz;
          m_theta = theta; m_phi = phi;
          m_theta_pipe = (aaTrack->getMucMomentum()).theta();
          m_phi_pipe   = (aaTrack->getMucMomentum()).phi();

          //if(fabs(m_phi_pipe*57.2958-90)>3)cout<<"px,y,z = "<<m_px<<"  "<<m_py<<" "<<m_pz<<endl;

          //calc proj point in y=0 plane
          Hep3Vector mucPos = aaTrack->getMucPos();
          double posx, posy, posz;
          posx = mucPos.x(); posy = mucPos.y(); posz = mucPos.z();
          //double projx, projz;
          m_projx = -999; m_projz = -999;
          if(py!=0){
            m_projx = posx - px/py*posy;
            m_projz = posz - pz/py*posy;
          }
          //cout<<"projection: "<<projx<<"  "<<projz<<endl;
        }  

      }
      if(m_NtOutput>=1){
        m_mucUp = hasMucUp; m_mucDown = hasMucDown;
        m_tuple->write();
      }

      int forCosmic = 0;
      if(aMucTrackCol->size()>=2 && forCosmic == 1){
        for(int iTrack = 0; iTrack < (int)aMucTrackCol->size(); iTrack++) {
          log << MSG::DEBUG << "iTrack " << iTrack << " / " <<(int)aMucTrackCol->size()<<endreq;
          aaTrack = (*aMucTrackCol)[iTrack];
          if(aaTrack->trackId()>=0) continue;   // this track has been used
          aaTrack->setTrackId(iTrack);
          //cout<<"atrack set index "<<iTrack<<endl;
          for(int jTrack = iTrack+1; jTrack < (int)aMucTrackCol->size(); jTrack++){
            bbTrack = (*aMucTrackCol)[jTrack];

            Hep3Vector mom_a = aaTrack->getMucMomentum();
            Hep3Vector mom_b = bbTrack->getMucMomentum();

            //cout<<"angle is : "<<mom_a.angle(mom_b)<<endl;
            if(fabs(mom_a.angle(mom_b) - 3.1415926)<0.2)
            {
              bbTrack->setTrackId(iTrack);
              //cout<<"btrack set index "<<iTrack<<endl;
              //cout<<"find one cosmic ray"<<endl;

              //cout<<"angle = "<<cosmicMom.angle(mom_a)<<"  "<<cosmicMom.angle(mom_b)<<endl;

            }

            if(m_NtOutput>=1){
              m_angle_cosmic = cosmicMom.angle(mom_a);
              if(cosmicMom.angle(mom_a)>1.57) m_angle_cosmic = 3.14159265 - cosmicMom.angle(mom_a);

              m_angle_updown = fabs(mom_a.angle(mom_b) - 3.1415926);
              m_px = -999; m_py = -999; m_pz = -999; m_theta = -999; m_phi = -999; m_theta_pipe = -999; m_phi_pipe = -999;
              m_px_mc = -999; m_py_mc = -999; m_pz_mc = -999; m_theta_mc = -999; m_phi_mc = -999;
              m_theta_mc_pipe = -999; m_phi_mc_pipe = -999;

              //m_tuple->write();
            }
          }

        }


      }

      if( p3DRoadC->size() !=0 ) {
        log<<MSG::INFO << "In 3DRoad container : " 
          << "   Num of 3DRoad = "    << p3DRoadC->size() <<endreq;

        int print2DRoadInfo = 0;
        if (print2DRoadInfo == 1) {
          for ( int iRoad = 0; iRoad < (int)p3DRoadC->size(); iRoad++) {
            MucRec3DRoad *road = (*p3DRoadC)[iRoad];
            cout << endl << "   " << iRoad << "th 3DRoad :" << endl
              << "       Part             = " << road->GetPart()    << endl
              << "       Seg              = " << road->GetSeg() << endl
              << "       NumGapsWithHits  = " << road->GetNGapsWithHits()  << endl
              << "       TotalHits        = " << road->GetTotalHits()        << endl
              << "       MaxHitsPerGap    = " << road->GetMaxHitsPerGap()    << endl
              << "       LastGapDelta     = " << road->GetLastGapDelta()     << endl
              << "       TotalHitsDelta   = " << road->GetTotalHitsDelta()   << endl
              << "       DegreesOfFreedom = " << road->GetDegreesOfFreedom() << endl 
              << "       ReducedChiSquare = " << road->GetReducedChiSquare() << endl
              << "       SlopeZX          = " << road->GetSlopeZX()          << endl
              << "       InterceptZX      = " << road->GetInterceptZX()      << endl
              << "       SlopeZY          = " << road->GetSlopeZY()          << endl
              << "       InterceptZY      = " << road->GetInterceptZY()      << endl
              << "       Hits Info : "        << endl;
            //road->PrintHitsInfo();
          }
        }

        m_NEventReco ++;
      }

      //delete p3DRoadC
      for(int i = 0 ; i < p3DRoadC->size(); i++)
        delete (*p3DRoadC)[i];
      for(int i = 0 ; i < p2DRoad0C->size(); i++)
        delete (*p2DRoad0C)[i];
      for(int i = 0 ; i < p2DRoad1C->size(); i++)
        delete (*p2DRoad1C)[i];

      p3DRoadC->clear();
      p2DRoad0C->clear();
      p2DRoad1C->clear();
      delete p3DRoadC;
      delete p2DRoad0C;
      delete p2DRoad1C;
      return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MucRecRoadFinder::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  //cout << m_NHitsLostTotal << " of " << m_NHitsTotal << " total hits" << endl; 
  //for(int i = 0; i < 20; i++) cout << "lost " << i << " hits event " << m_NHitsLost[i] << endl;
  //for(int i = 0; i < 9; i++) cout << "lost on gap " << i << " is " << m_NHitsLostInGap[i] << endl;

  return StatusCode::SUCCESS;
}

void 
MucRecRoadFinder::TrackFinding(RecMucTrack *aTrack)
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
  for(int partSeq = 0; partSeq < (int)MucID::getPartNum(); partSeq++) {
    int iPart = kPartSeq[partSeq];
    for(int iGap = 0; iGap < (int)MucID::getGapNum(iPart); iGap++) {
      int seg = -1;
      int orient = MucGeoGeneral::Instance()->GetGap(iPart, 0, iGap)->Orient();;

      float xInsct, yInsct, zInsct;
      aTrack->Project(iPart, iGap, xInsct, yInsct, zInsct, seg);
      if(m_MsOutput) cout<<"part "<<iPart<<" gap "<<iGap<<" x "<<xInsct<<" y "<<yInsct<<" z "<<zInsct<<" seg "<<seg<<endl; 

      if(seg == -1) {
        //log << MSG::DEBUG << "no intersection with part " << iPart
        //  << " gap " << iGap << " in this track !" << endl;
        continue;
      }

      aTrack->SetCurrentInsct(xInsct, yInsct, zInsct);

      for(int iDeltaSeg = 0; iDeltaSeg < kNSegSearch; iDeltaSeg++) {
        int iSeg = seg + kDeltaSeg[iDeltaSeg];  // also find on neighbor seg;
        if(iSeg < 0) iSeg += MucID::getSegNum(iPart);
        if(iSeg > (int)MucID::getSegNum(iPart) - 1) iSeg -= MucID::getSegNum(iPart);

        float window = kWindowSize[iPart][iGap];

        for (int iHit = 0; iHit < aMucRecHitContainer->GetGapHitCount(iPart, iSeg, iGap); iHit++) {
          log << MSG::DEBUG << "iSeg " << iSeg << " iHit " << iHit << endreq;
          MucRecHit* pHit = aMucRecHitContainer->GetHit(iPart, iSeg, iGap, iHit);
          //cout<< "strip " << pHit->Strip() << " center " << pHit->GetCenterPos() << endl;

          if (!pHit) {
            log << MSG::WARNING << "MucRecTrkExt: null pointer to pHit" << endreq;
            continue;
          }
          else {

            //this algo use with ext and this hit has been used before;                   
            if(pHit->GetHitMode() != -1 && pHit->GetHitMode() != 3) continue;

            // Get the displacement of hit pHit to intersection
            float dX  = aTrack->GetHitDistance(pHit);
            log << MSG::DEBUG << "distance = " << setw(8) << dX << " size " << setw(4) << window << endreq;

            //cout<<"dX= "<<dX<<"  window="<<window<<endl;

            if (dX < window) {
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
                aTrack->AttachHit(pHit);  //this hit has not been used by mdc ext
                //pHit->SetHitMode(2); //emc ext
              }
              if(aTrack->GetRecMode() == 3) { 
                if(pHit->GetHitMode()!=1) {     
                  aTrack->AttachHit(pHit);  //this hit has not been used by mdc ext
                  pHit->SetHitMode(3); //emc ext
                }
              }

              if (firstHitGap[orient] == -1) firstHitGap[orient] = iGap;
              firstHitFound[orient] = 1;
              //cout << "You could correct directon in orient " << orient << endl;

              //cout<< " part " << iPart << " seg " << iSeg << " gap " << iGap 
              //          << " strip " << setw(2) << pHit->Strip() << " attatched" << endl;

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
          }
        }
        aTrack->CalculateInsct(iPart, iSeg, iGap);
      }

      if(m_onlyseedfit == 0) {//
        // When correct dir in the orient is permitted and this gap is not the gap first hit locates. 
        if (firstHitFound[orient] && firstHitGap[orient] != iGap) aTrack->CorrectDir();
        aTrack->CorrectPos();
        //cout << "Current Intersection " << aTrack->GetCurrentInsct() << endl;
        //cout << "Current Direction    " << aTrack->GetCurrentDir() << endl;
        aTrack->AttachInsct(aTrack->GetCurrentInsct());
      }// else; not correct pos & dir.

    }
  }
  aTrack->LineFit(1);
  aTrack->ComputeTrackInfo(1);
  log << MSG::INFO << "Fit track done! trackIndex: " << aTrack->trackId() << ", mucId: " << aTrack->id() << ", RecMode: " << aTrack->GetRecMode() << endreq; 
  //cout<<" in TrackFinding: depth= "<<aTrack->GetDepth()<<endl;
}

// END



