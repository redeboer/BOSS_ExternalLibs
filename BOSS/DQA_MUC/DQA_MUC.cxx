#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Algorithm.h"

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

#include "ReconEvent/ReconEvent.h"
#include "MucRawEvent/MucDigi.h"
#include "MucRecEvent/MucRecHit.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRecHitContainer.h"

#include "DQAEvent/DQAEvent.h"
#include "DQA_MUC/DQA_MUC.h"

using CLHEP::Hep3Vector;
/////////////////////////////////////////////////////////////////////////////

DQA_MUC::DQA_MUC(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("EffWindow",    m_effWindow = 4);  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_MUC::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  // Call Histogram service
  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  
  char name[60];
  char title[60];

  // Resolution histograms
  for( int i=0; i<B_LAY_NUM; i++ )
  {
    sprintf( name, "BrResDist_All_L%d", i );
    sprintf( title, "Barrel spacial resolution in L%d",i );
    m_hBrResDist[i][0] = new TH1F(name,title, 200, -100, 100 );
    sprintf( name, "/DQAHist/MUC/BrResDist_All_L%d", i );
    if(m_thsvc->regHist(name, m_hBrResDist[i][0]).isFailure()) 
    { log << MSG::ERROR << "Couldn't register " << name << endreq; }
    
    sprintf( name, "BrResDist_Dimu_L%d", i );
    m_hBrResDist[i][1] = new TH1F(name,title, 200, -100, 100 );
    sprintf( name, "/DQAHist/MUC/BrResDist_Dimu_L%d", i );
    if(m_thsvc->regHist(name, m_hBrResDist[i][1]).isFailure()) 
    { log << MSG::ERROR << "Couldn't register " << name << endreq; }
  }

  for( int i=0; i<E_LAY_NUM; i++ )
  {
    sprintf( name, "EcResDist_All_L%d", i );
    sprintf( title, "Endcap spacial resolution in L%d",i );
    m_hEcResDist[i][0] = new TH1F(name,title, 200, -100, 100 );
    sprintf( name, "/DQAHist/MUC/EcResDist_All_L%d", i );
    if(m_thsvc->regHist(name, m_hEcResDist[i][0]).isFailure()) 
    { log << MSG::ERROR << "Couldn't register " << name << endreq; }
    
    sprintf( name, "EcResDist_Dimu_L%d", i );
    m_hEcResDist[i][1] = new TH1F(name,title, 200, -100, 100 );
    sprintf( name, "/DQAHist/MUC/EcResDist_Dimu_L%d", i );
    if(m_thsvc->regHist(name, m_hEcResDist[i][1]).isFailure()) 
    { log << MSG::ERROR << "Couldn't register " << name << endreq; }
  }

  //m_hBrRes[0] = new TH1F("BrRes_All", "BrResAll", B_LAY_NUM+1, -0.5, B_LAY_NUM+0.5 );
  //m_hBrRes[0]->GetXaxis()->SetTitle("Layer id"); 
  //m_hBrRes[0]->GetXaxis()->CenterTitle();
  //m_hBrRes[0]->SetStats(0);
  //m_hBrRes[1] = new TH1F("BrRes_Dimu", "BrResDimu", B_LAY_NUM+1, -0.5, B_LAY_NUM+0.5 );
  //m_hBrRes[1]->GetXaxis()->SetTitle("Layer id"); 
  //m_hBrRes[1]->GetXaxis()->CenterTitle();
  //m_hBrRes[1]->SetStats(0);
  //
  //m_hEcRes[0] = new TH1F("EcRes_All", "EcResAll", E_LAY_NUM+1, -0.5, E_LAY_NUM+0.5 );
  //m_hEcRes[0]->GetXaxis()->SetTitle("Layer id"); 
  //m_hEcRes[0]->GetXaxis()->CenterTitle();
  //m_hEcRes[0]->SetStats(0);
  //m_hEcRes[1] = new TH1F("EcRes_Dimu", "EcResDimu", E_LAY_NUM+1, -0.5, E_LAY_NUM+0.5 );
  //m_hEcRes[1]->GetXaxis()->SetTitle("Layer id"); 
  //m_hEcRes[1]->GetXaxis()->CenterTitle();
  //m_hEcRes[1]->SetStats(0);
  
  //if(m_thsvc->regHist("/DQAHist/MUC/BrRes_All", m_hBrRes[0]).isFailure()) 
  //{ log << MSG::ERROR << "Couldn't register BrRes_All" << endreq; }
  //if(m_thsvc->regHist("/DQAHist/MUC/BrRes_Dimu", m_hBrRes[1]).isFailure()) 
  //{ log << MSG::ERROR << "Couldn't register BrRes_Dimu" << endreq; }
  //if(m_thsvc->regHist("/DQAHist/MUC/EcRes_All", m_hEcRes[0]).isFailure()) 
  //{ log << MSG::ERROR << "Couldn't register EcRes_All" << endreq; }
  //if(m_thsvc->regHist("/DQAHist/MUC/EcRes_Dimu", m_hEcRes[1]).isFailure()) 
  //{ log << MSG::ERROR << "Couldn't register BrRes_Dimu" << endreq; }

  // Efficiency histograms, j=0: numberator, j=1; denominator
  for(int i=0; i<TAGN; i++)
  {
    for(int j=0; j<2; j++)
    {     
      for(int k=0; k<LVLN; k++)
      { 
        sprintf( name, "%sEff_%s_%s", LNAME[k], ENAME[i], HTYPE[j] );
        sprintf( title, "%s efficiency",LNAME[k] );

        if(k==0) {
          m_hEff[k][i][j] = new TH1F(name,title, LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
          m_hEff[k][i][j]->GetXaxis()->SetTitle("Layer id"); 
        }else if(k==1) {
          m_hEff[k][i][j] = new TH1F(name,title, BOX_MAX+1, -0.5, BOX_MAX+0.5);  
          m_hEff[k][i][j]->GetXaxis()->SetTitle("Box id [EE:0~31, BR:32~103, WE:104~135]");
        }else {
          m_hEff[k][i][j] = new TH1F(name,title, STRIP_MAX+1, -0.5, STRIP_MAX+0.5);  
          m_hEff[k][i][j]->GetXaxis()->SetTitle("Strip id");
        }
        
        m_hEff[k][i][j]->GetXaxis()->CenterTitle();
        //m_hEff[k][i][j]->SetStats(0);
        
        sprintf( name, "/DQAHist/MUC/%sEff_%s_%s", LNAME[k], ENAME[i], HTYPE[j] );
        if(m_thsvc->regHist(name, m_hEff[k][i][j]).isFailure()) 
        { log << MSG::ERROR << "Couldn't register " << name << endreq; }
      }
      
      sprintf( name, "%sNosRatio_%s_%s", LNAME[1], ENAME[i], HTYPE[j] );
      sprintf( title, "%s noise ratio",LNAME[1] );
      m_hNosRatio[i][j]  = new TH1F(name, title, BOX_MAX+1, -0.5, BOX_MAX+0.5);  
      m_hNosRatio[i][j]->GetXaxis()->SetTitle("Box id [EE:0~31, BR:32~103, WE:104~135]"); 
      m_hNosRatio[i][j]->GetXaxis()->CenterTitle();
      //m_hNosRatio[i][j]->GetYaxis()->SetRangeUser(0., 0.3);
      //m_hNosRatio[i][j]->SetStats(0);

      sprintf( name, "/DQAHist/MUC/%sNosRatio_%s_%s", LNAME[1], ENAME[i], HTYPE[j] );
      if(m_thsvc->regHist(name, m_hNosRatio[i][j]).isFailure()) 
      { log << MSG::ERROR << "Couldn't register " << name << endreq; }
    }
    
    // Costheta and phi 
    sprintf( name, "Costheta_%s", ENAME[i]);
    sprintf( title, "Costheta");
    m_hCostheta[i]  = new TH1F(name, title, 360, -1.0, 1.0);
    m_hCostheta[i]->GetXaxis()->SetTitle("cos#theta"); 
    m_hCostheta[i]->GetXaxis()->CenterTitle();

    sprintf( name, "Phi_%s", ENAME[i]);
    sprintf( title, "Phi");
    m_hPhi[i]  = new TH1F(name, title, 360, -PI, PI);
    m_hPhi[i]->GetXaxis()->SetTitle("#phi"); 
    m_hPhi[i]->GetXaxis()->CenterTitle();
    
    sprintf( name, "/DQAHist/MUC/Costheta_%s", ENAME[i] );
    if(m_thsvc->regHist(name, m_hCostheta[i]).isFailure()) 
    { log << MSG::ERROR << "Couldn't register " << name << endreq; }
    sprintf( name, "/DQAHist/MUC/Phi_%s", ENAME[i] );
    if(m_thsvc->regHist(name, m_hPhi[i]).isFailure()) 
    { log << MSG::ERROR << "Couldn't register Phi_All" << endreq; }
  }
  
  //m_hStripEff[0] = new TH1F("StripEff_All", "Strip efficiency", 101, -0.005, 1.005 );
  //m_hStripEff[0]->GetXaxis()->SetTitle("Efficiency"); 
  //m_hStripEff[0]->GetXaxis()->CenterTitle();
  //m_hStripEff[1] = new TH1F("StripEff_Dimu", "Strip efficiency", 101, -0.005, 1.005 );
  //m_hStripEff[1]->GetXaxis()->SetTitle("Efficiency"); 
  //m_hStripEff[1]->GetXaxis()->CenterTitle();
  
  // Parameters 
  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<SEGMENT_MAX; j++ )
      for( int k=0; k<LAYER_MAX; k++ )
        for( int n=0; n<STRIP_INBOX_MAX; n++ )
        {
          m_recordAll[i][j][k][n][0] = 0;
          m_recordAll[i][j][k][n][1] = 0;
          m_recordAll[i][j][k][n][2] = 0;
          m_recordAll[i][j][k][n][3] = 0;
          m_recordAll[i][j][k][n][4] = 0;

          m_recordDimu[i][j][k][n][0] = 0;
          m_recordDimu[i][j][k][n][1] = 0;
          m_recordDimu[i][j][k][n][2] = 0;
          m_recordDimu[i][j][k][n][3] = 0;
          m_recordDimu[i][j][k][n][4] = 0;
        }
  
  m_ptrMucMark  = new MucMark(0,0,0,0);
  m_ptrIdTr     = new MucIdTransform();

  m_digiCol.resize(0);
  m_expHitCol.resize(0);
  m_effHitCol.resize(0);
  m_nosHitCol.resize(0);
  m_clusterCol.resize(0);

  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<SEGMENT_MAX; j++ ) {
      m_segDigiCol[i][j].resize(0);
    }

  log << MSG::INFO << "Initialize done!" <<endmsg;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_MUC::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  m_run   = eventHeader->runNumber();
  m_event = eventHeader->eventNumber();
  log << MSG::DEBUG <<"Run " << m_run << "\tEvent "  << m_event <<endreq;

  SmartDataPtr<DQAEvent::DQAEvent> dqaevt(eventSvc(), "/Event/DQATag");
  if ( dqaevt ) {
      log << MSG::INFO << "success get DQAEvent" << endreq;
  } else {
      log << MSG::ERROR << "Error accessing DQAEvent" << endreq;
      return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "DQA event tag = " << dqaevt->EventTag() << endreq;

  int part, segment, layer, strip;
  char name[100];
  bool isDimu = false;
  if ( dqaevt->Dimu() )  isDimu = true;
  log << MSG::INFO << "DQADimuTag:\t" << dqaevt->Dimu() << endreq;

  //---> Retrieve MUC digi
  log << MSG::INFO << "Retrieve digis" << endreq;
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc(),"/Event/Digi/MucDigiCol");
  if(!mucDigiCol)  {
    log << MSG::FATAL << "Could not find MUC digi" << endreq;
    return( StatusCode::FAILURE);
  }

  Identifier mucId;
  MucDigiCol::iterator digiIter = mucDigiCol->begin();
  int eventDigi = 0;
  for ( int digiId =0; digiIter != mucDigiCol->end(); digiIter++, digiId++ )
  {
    mucId   = (*digiIter)->identify();
    part    = MucID::barrel_ec(mucId);    segment = MucID::segment(mucId);    
    layer   = MucID::layer(mucId);        strip   = MucID::channel(mucId);
    eventDigi    ++;
    
    // Add digi 
    MucMark *aMark = new MucMark( part, segment, layer, strip );
    m_digiCol.push_back( aMark );
    m_segDigiCol[part][segment].push_back( aMark );
    m_recordAll[part][segment][layer][strip][0] ++;
    if( isDimu ) m_recordDimu[part][segment][layer][strip][0] ++; 
  }
  log << MSG::INFO << "Total digis in this event: " << eventDigi << endreq; 
  if( eventDigi > 500) cout <<"Run:\t"<< m_run <<"\tEvent:\t"<< m_event << "\tdigits inflation:\t" << eventDigi << endreq;

  // Search cluster in digis
  m_clusterCol = (*m_ptrMucMark).CreateClusterCol(1, m_digiCol );
  log << MSG::INFO << "Total clusters in this event: " << m_clusterCol.size() << endreq; 
  //<--- End retrieve digis

  //---> Retrieve rec tracks
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<RecMucTrackCol> mucTrackCol(eventSvc(),"/Event/Recon/RecMucTrackCol");
  if (!mucTrackCol) {
//    log << MSG::FATAL << "Could not find RecMucTrackCol" << endreq;
    log << MSG::WARNING <<  "Could not find RecMucTrackCol" << endreq; // until BOSS 7.0.3, if not MUC track, no RecMucTrackCol either.
    return( StatusCode::SUCCESS);
  }

  RecMucTrackCol  *aRecMucTrackCol = mucTrackCol;
  if (aRecMucTrackCol->size() < 1) {
    log << MSG::INFO << "No MUC tracks in this event" << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::INFO << "Total tracks of this event: " << aRecMucTrackCol->size() << endreq;
  
  int  trackHitNum, expectedHitNum, segNum, lastLayerBR, lastLayerEC;
  int  layerPassNum[3], passMax[TRACK_SEG_MAX][2];
  bool firedLay[TRACK_SEG_MAX][LAYER_MAX];
  double costheta, phi;
  TH1* htmp(0);
  trackHitNum = expectedHitNum = segNum = lastLayerBR = lastLayerEC = 0;
  layerPassNum[0] = layerPassNum[1] = layerPassNum[2] = 0;
  for( int segi=0; segi<TRACK_SEG_MAX; segi++ ) {
    passMax[segi][0] = passMax[segi][1] = 0;
    for( int layi=0; layi<LAYER_MAX; layi++ ) firedLay[segi][layi] = 0;
  }

  vector<MucRecHit*> mucRawHitCol;
  vector<MucRecHit*> mucExpHitCol;
  RecMucTrackCol::iterator trackIter = mucTrackCol->begin();
  for (int trackId = 0; trackIter != mucTrackCol->end(); trackIter++, trackId++)
  {
    trackHitNum = (*trackIter)->numHits();

    if( trackHitNum == 0 ) {
      log << MSG::INFO << "Track " << trackId << " no hits" << endreq;
      continue;
    }

    lastLayerBR = (*trackIter)->brLastLayer();
    lastLayerEC = (*trackIter)->ecLastLayer();    
    // First fit position in MUC
    CLHEP::Hep3Vector a3Vector((*trackIter)->xPos(),(*trackIter)->yPos(),(*trackIter)->zPos());
    costheta = a3Vector.cosTheta();
    phi      = a3Vector.phi();    
    if(m_thsvc->getHist("/DQAHist/MUC/Costheta_All", htmp).isSuccess()) {
      htmp->Fill( costheta );
    } else {
      log << MSG::ERROR << "Fail to retrieve Costheta_All" << endreq;      
    }
    if(m_thsvc->getHist("/DQAHist/MUC/Phi_All", htmp).isSuccess()) {
      htmp->Fill( phi );
    } else {
      log << MSG::ERROR << "Fail to retrieve Phi_All" << endreq;      
    }

    if( isDimu ) 
    {
      if(m_thsvc->getHist("/DQAHist/MUC/Costheta_Dimu", htmp).isSuccess()) {
        htmp->Fill( costheta );
      } else {
        log << MSG::ERROR << "Fail to retrieve Costheta_Dimu" << endreq;      
      }
      if(m_thsvc->getHist("/DQAHist/MUC/Phi_Dimu", htmp).isSuccess()) {
        htmp->Fill( phi );
      } else {
        log << MSG::ERROR << "Fail to retrieve Phi_Dimu" << endreq;      
      }
    }       
    log << MSG::INFO << "Fill costheta and phi:\t" << costheta << "\t" << phi << endreq;

    MucRecHit* pMucRawHit;
    MucRecHit* pMucExpHit;


    // Expected hits in this rec track
    mucExpHitCol = (*trackIter)->GetExpectedHits();
    //mucExpHitCol = (*trackIter)->getExpHits();
    expectedHitNum += mucExpHitCol.size();
    for(unsigned int hitId = 0; hitId < mucExpHitCol.size(); hitId++)
    {
      pMucRawHit  = mucExpHitCol[ hitId ];
      part        = pMucRawHit->Part();     segment     = pMucRawHit->Seg();
      layer       = pMucRawHit->Gap();      strip       = pMucRawHit->Strip();

      MucMark* currentMark = new MucMark( part, segment, layer, strip );
      m_expHitCol.push_back( currentMark );

      // Judge efficiency hit
      int  isInPos       = -1;
      bool isInEffWindow = false;
      isInPos = currentMark->IsInCol( m_segDigiCol[part][segment] );

      // Avoid bias in outer layers caused by low momentum tracks
      if( part == BRID && (layer-lastLayerBR>1) ) continue;
      if( part != BRID && (layer-lastLayerEC>1) ) continue;

      // Avoid bias in both sides of the innermost layer of Barrel 
      if( part==BRID && layer==0 && (strip<2 || strip>45) )
      {
        if( isInPos != -1) // expHit is fired
        {
          m_recordAll[part][segment][layer][strip][2] ++; // Efficiency hit number
          m_recordAll[part][segment][layer][strip][1] ++; // Rec track number
          m_effHitCol.push_back( m_segDigiCol[part][segment][isInPos] );

          if(isDimu) {
            m_recordDimu[part][segment][layer][strip][2] ++; // Efficiency hit number
            m_recordDimu[part][segment][layer][strip][1] ++; // Rec track number
          }
        }
        else {
          m_recordAll[part][segment][layer][strip][1] ++;
          if(isDimu) m_recordDimu[part][segment][layer][strip][1] ++;
        }
        continue; // Judge next hit
      } 
      // Eff calibration
      if( isInPos != -1 ) // expHit is fired
      {
        m_recordAll[part][segment][layer][strip][2] ++; // Efficiency hit number
        m_recordAll[part][segment][layer][strip][1] ++; // Rec track number
        m_effHitCol.push_back( m_segDigiCol[part][segment][isInPos] );

        if(isDimu) {
          m_recordDimu[part][segment][layer][strip][2] ++; // Efficiency hit number
          m_recordDimu[part][segment][layer][strip][1] ++; // Rec track number
        }

        continue; // Judge next hit
      }
      else for(int tempStrip=0, hiti=-m_effWindow; hiti<=m_effWindow; hiti++ )
      {
        if( hiti == 0 ) continue;
        tempStrip = strip + hiti;
        if( tempStrip < 0 || tempStrip > m_ptrIdTr->GetStripMax(part,segment,layer) ) continue;

        isInPos = m_ptrMucMark->IsInCol( part, segment, layer, tempStrip, m_segDigiCol[part][segment] );
        if( isInPos != -1 )
        {
          m_recordAll[part][segment][layer][tempStrip][2] ++; // Efficiency hit number
          m_recordAll[part][segment][layer][tempStrip][1] ++; // Rec track number
          m_effHitCol.push_back( m_segDigiCol[part][segment][isInPos] );

          if(isDimu) {
            m_recordDimu[part][segment][layer][tempStrip][2] ++; // Efficiency hit number
            m_recordDimu[part][segment][layer][tempStrip][1] ++; // Rec track number
          }

          isInEffWindow = true;
        }
      } // End else 

      if( isInEffWindow ) { continue; } // Judge next hit
      else { // A hit should be fired but not fired and not in the EffWindow
        m_recordAll[part][segment][layer][strip][1] ++; // Rec track number
        if(isDimu) m_recordDimu[part][segment][layer][strip][1] ++;
      }

    } // End expected hits

    // Fill residual, and for the other way of eff calculation
    log << MSG::INFO << "Fill residual" << endreq;
    vector<float> m_lineResCol = (*trackIter)->getDistHits();
    // Digis belong to this rec track
    mucRawHitCol = (*trackIter)->GetHits(); // Get hit collection of a track
    //mucRawHitCol = (*trackIter)->getVecHits(); // Get hit collection of a track
    
    if( trackHitNum > 4 && m_lineResCol[0] != -99) // track is good for res 
    {
      // Fill res histograms
      bool firedFlag[PART_MAX][LAYER_MAX][2];
      for(int iprt=0; iprt<PART_MAX; iprt++)
        for(int jlay=0; jlay<LAYER_MAX; jlay++)
          firedFlag[iprt][jlay][0] = firedFlag[iprt][jlay][1] = false;

      for(unsigned int hitId = 0; hitId < mucExpHitCol.size(); hitId++)
      {
        pMucExpHit  = mucExpHitCol[ hitId ];
        part = pMucExpHit->Part(); segment = pMucExpHit->Seg(); layer = pMucExpHit->Gap();
        firedFlag[part][layer][0] = true;
      }

      for(unsigned int hitId = 0; hitId < mucRawHitCol.size(); hitId++)
      {
        pMucRawHit  = mucRawHitCol[ hitId ];
        part = pMucRawHit->Part(); segment = pMucRawHit->Seg(); layer = pMucRawHit->Gap();

        // if exp is true and fired is true, and not filled yet
        if( firedFlag[part][layer][0] == true && firedFlag[part][layer][1] == false )
        {
          if( part == BRID ) 
          {
            sprintf( name, "/DQAHist/MUC/BrResDist_All_L%d", layer );
            if(m_thsvc->getHist(name, htmp).isSuccess()) {
              htmp->Fill( m_lineResCol[hitId] );
            } else {
              log << MSG::ERROR << "Fail to retrieve " << name << endreq;      
            }
            //m_hBrResDist[layer][0]->Fill( m_lineResCol[hitId] );
            
            if( isDimu ) 
            { 
              sprintf( name, "/DQAHist/MUC/BrResDist_Dimu_L%d", layer );
              if(m_thsvc->getHist(name, htmp).isSuccess()) {
                htmp->Fill( m_lineResCol[hitId] );
              } else { 
                log << MSG::ERROR << "Fail to retrieve " << name << endreq;          
              }              
              //m_hBrResDist[layer][1]->Fill( m_lineResCol[hitId] );
            }
          } else {
            sprintf( name, "/DQAHist/MUC/EcResDist_All_L%d", layer );
            if(m_thsvc->getHist(name, htmp).isSuccess()) {
              htmp->Fill( m_lineResCol[hitId] );
            } else { 
              log << MSG::ERROR << "Fail to retrieve " << name << endreq;          
            }            
            //m_hEcResDist[layer][0]->Fill( m_lineResCol[hitId] );
            
            if( isDimu ) 
            {
              sprintf( name, "/DQAHist/MUC/EcResDist_Dimu_L%d", layer );
              if(m_thsvc->getHist(name, htmp).isSuccess()) {
                htmp->Fill( m_lineResCol[hitId] );
              } else { 
                log << MSG::ERROR << "Fail to retrieve " << name << endreq;          
              }              
             //m_hEcResDist[layer][1]->Fill( m_lineResCol[hitId] );
            }
          }
        }

        firedFlag[part][layer][1] = true;
      }
    } // End fill residual, if track is good for res

    mucRawHitCol.clear();
    mucExpHitCol.clear();

  } // End read all tracks
  //<--- End retrieve rec tracks

  //---> Searching inc/noise hits  
  log << MSG::INFO << "Searching inc/noise hits" << endreq;
  bool isNosHit;
  bool hasEffHit;
  for( unsigned int i=0; i < m_digiCol.size(); i++ )
  {
    isNosHit = true;

    if( m_digiCol[i]->IsInCol( m_effHitCol )!=-1 ) continue; // digi in effHitCol
    else
    {
      for( unsigned int j=0; j < m_clusterCol.size(); j++ )
      {
        hasEffHit = false;
        for( unsigned int k=0; k<m_clusterCol[j].size(); k++)
        {
          if( m_clusterCol[j][k]->IsInCol(m_effHitCol) != -1)
	  {  // Clusters have efficiency hit
            hasEffHit = true;
          break; // Out a cluster
          }
	}

        if ( hasEffHit && (m_digiCol[i]->IsInCol( m_clusterCol[j] ) != -1) ) {
          isNosHit = false;
          break; // Out cluster collection
        }
      } // End cluster col

      if( isNosHit ) {          
        part    = (*m_digiCol[i]).Part();     segment = (*m_digiCol[i]).Segment();
        layer   = (*m_digiCol[i]).Layer();    strip   = (*m_digiCol[i]).Strip();
        
        //log << MSG::INFO << "Noise hit:\t" << part<<"\t"<<segment<<"\t"<<layer<<"\t"<<strip << endreq;
        m_recordAll[part][segment][layer][strip][3] ++;
        if( isDimu ) m_recordDimu[part][segment][layer][strip][3] ++;
        m_nosHitCol.push_back(m_digiCol[i]);
      }
    }// End else
  } // End digi collection
  //<--- End searching inc/noise hits

  DQA_MUC::FillHistograms( isDimu );
  
  // Reset mark collection
  for( unsigned int i=0; i<m_digiCol.size(); i++ ) {
    if( m_digiCol[i] != NULL )    delete m_digiCol[i];
  }

  for( unsigned int i=0; i<m_expHitCol.size(); i++ ) {
    if( m_expHitCol[i] != NULL )  delete m_expHitCol[i];
  }
  
  if( m_effHitCol.size() != 0 ) m_effHitCol.clear();
  if( m_expHitCol.size() != 0 ) m_expHitCol.clear();
  if( m_nosHitCol.size() != 0 ) m_nosHitCol.clear();
  if( m_digiCol.size() != 0 ) m_digiCol.clear();
  if( m_clusterCol.size() != 0 ) m_clusterCol.clear();  

  for( int i=0; i<PART_MAX; i++ ) {
    for( int j=0; j<SEGMENT_MAX; j++ ) {
      if( m_segDigiCol[i][j].size() != 0 )  m_segDigiCol[i][j].clear();
    } 
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_MUC::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  
/*  
  TH1 *htmp(0);
  
  // Fit spacial resolution
  log << MSG::INFO << "Spacial resolution fitting" << endreq;
  double resSigma, resSigmaErr;
  resSigma = resSigmaErr = 0.;

  for( int i=0; i<B_LAY_NUM; i++ )
  {
    m_hBrResDist[i][0]->Fit("gaus");
    resSigma = m_hBrResDist[i][0]->GetFunction("gaus")->GetParameter("Sigma");
    resSigmaErr = m_hBrResDist[i][0]->GetFunction("gaus")->GetParError(2);
    if(m_thsvc->getHist("/DQAHist/MUC/BrRes_All", htmp).isSuccess()) {
      htmp->Fill( i, resSigma );
      htmp->SetBinError( i+1, resSigmaErr );
    } else {
      log << MSG::ERROR << "Fail to retrieve BrRes_All" << endreq;
    }
    //log << MSG::INFO << "Barrle layer:\t" << i << "\t" << resSigma << "\t" << resSigmaErr << endreq;
    
    m_hBrResDist[i][1]->Fit("gaus");
    resSigma = m_hBrResDist[i][1]->GetFunction("gaus")->GetParameter("Sigma");
    resSigmaErr = m_hBrResDist[i][1]->GetFunction("gaus")->GetParError(2);
    if(m_thsvc->getHist("/DQAHist/MUC/BrRes_Dimu", htmp).isSuccess()) {
    htmp->Fill( i, resSigma );
    htmp->SetBinError( i+1, resSigmaErr );
    } else {
      log << MSG::ERROR << "Fail to retrieve BrRes_Dimu" << endreq;
    }
    
  }

  for( int i=0; i<E_LAY_NUM; i++ )
  { 
    m_hEcResDist[i][0]->Fit("gaus");
    resSigma = m_hEcResDist[i][0]->GetFunction("gaus")->GetParameter("Sigma");
    resSigmaErr = m_hEcResDist[i][0]->GetFunction("gaus")->GetParError(2);
    if(m_thsvc->getHist("/DQAHist/MUC/EcRes_All", htmp).isSuccess()) {
      htmp->Fill( i, resSigma );
      htmp->SetBinError( i+1, resSigmaErr );
    } else {
      log << MSG::ERROR << "Fail to retrieve EcRes_All" << endreq;
    }
    //log << MSG::INFO << "Endcap layer:\t" << i << "\t" << resSigma << "\t" << resSigmaErr << endreq;
    
    m_hEcResDist[i][1]->Fit("gaus");
    resSigma = m_hEcResDist[i][1]->GetFunction("gaus")->GetParameter("Sigma");
    resSigmaErr = m_hEcResDist[i][0]->GetFunction("gaus")->GetParError(2);
    if(m_thsvc->getHist("/DQAHist/MUC/EcRes_Dimu", htmp).isSuccess()) {
      htmp->Fill( i, resSigma );
      htmp->SetBinError( i+1, resSigmaErr );
    } else {
      log << MSG::ERROR << "Fail to retrieve EcRes_Dimu" << endreq;
    }
  }

  
  // Efficiency and noise ratio  
  int part, segment, layer, stripMax;
  part = segment = layer = stripMax = 0;
  double digi[TAGN][3], effHit[TAGN][3], trackNum[TAGN][3], nosHit[TAGN][3];
  double eff[TAGN], effErr[TAGN], nos[TAGN], nosErr[TAGN];
  for(int i=0; i<TAGN; i++) {
    for( int j=0; j<3; j++) {
      digi[i][j] = effHit[i][j] = trackNum[i][j] = nosHit[i][j] = 0.;
    }
    
    eff[i] = effErr[i] = nos[i] = nosErr[i] = 0.;
  }

  // Layer efficiency
  for( int i=0; i<LAYER_MAX; i++ )
  {
    effHit[0][0] = trackNum[0][0] = 0;
    effHit[1][0] = trackNum[1][0] = 0;

    for( int j=0; j<PART_MAX; j++ ) {
      for( int k=0; k<SEGMENT_MAX; k++) {
        stripMax = m_ptrIdTr->GetStripMax( j, k, i );
        for( int n=0; n<stripMax; n++ ) {
          trackNum[0][0]  += m_recordAll[j][k][i][n][1];
          effHit[0][0]    += m_recordAll[j][k][i][n][2];
          
          trackNum[1][0]  += m_recordDimu[j][k][i][n][1];
          effHit[1][0]    += m_recordDimu[j][k][i][n][2];
        }
      }
    }

    if( trackNum[0][0] == 0 ) {
      eff[0] = effErr[0] = 0.0;
    } else {
      eff[0]     = ( (double)effHit[0][0] )/trackNum[0][0];
      effErr[0]  = sqrt( eff[0]*(1-eff[0])/trackNum[0][0] );
    }  
    if(m_thsvc->getHist("/DQAHist/MUC/LayerEff_All", htmp).isSuccess()) {
      htmp->Fill( i, eff[0] );
      htmp->SetBinError( i+1, effErr[0] );
    } else {
      log << MSG::ERROR << "Fail to retrieve LayerEff_All" << endreq;
    }
    log << MSG::INFO << "LayerEff:\t" << i << "\t" << eff[0] << "\t" << effErr[0] << endreq;

    if( trackNum[1][0] == 0 ) {
      eff[1] = effErr[1] = 0.0;
    } else { 
      eff[1]     = ( (double)effHit[1][0] )/trackNum[1][0];
      effErr[1]  = sqrt( eff[1]*(1-eff[1])/trackNum[1][0] );
    }                
    if(m_thsvc->getHist("/DQAHist/MUC/LayerEff_Dimu", htmp).isSuccess()) {
      htmp->Fill( i, eff[1] );
      htmp->SetBinError( i+1, effErr[1] );
    } else { 
      log << MSG::ERROR << "Fail to retrieve LayerEff_Dimu" << endreq;
    }    
    
  } // End layer    

  // Box efficiency and noise ratio, strip efficiency
  for( int i=0; i<BOX_MAX; i++ )
  {
    m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
    stripMax = m_ptrIdTr->GetStripMax( part, segment, layer );

    digi[0][1] = effHit[0][1] = trackNum[0][1] = nosHit[0][1] = 0;
    digi[1][1] = effHit[1][1] = trackNum[1][1] = nosHit[1][1] = 0;
    for( int j=0; j<stripMax; j++ )
    {
      // For box
      digi[0][1]      += m_recordAll[part][segment][layer][j][0];
      trackNum[0][1]  += m_recordAll[part][segment][layer][j][1];
      effHit[0][1]    += m_recordAll[part][segment][layer][j][2];
      nosHit[0][1]    += m_recordAll[part][segment][layer][j][3];

      digi[1][1]      += m_recordDimu[part][segment][layer][j][0];
      trackNum[1][1]  += m_recordDimu[part][segment][layer][j][1];
      effHit[1][1]    += m_recordDimu[part][segment][layer][j][2];
      nosHit[1][1]    += m_recordDimu[part][segment][layer][j][3];
      
      // For strip
      trackNum[0][2]  = m_recordAll[part][segment][layer][j][1];
      effHit[0][2]    = m_recordAll[part][segment][layer][j][2];
      trackNum[1][2]  = m_recordDimu[part][segment][layer][j][1];
      effHit[1][2]    = m_recordDimu[part][segment][layer][j][2];

      if( trackNum[0][2] == 0 ) {
        eff[0] = 0.0;
      } else {
        eff[0]     = ( (double)effHit[0][2] )/trackNum[0][2];
      }  
      if(m_thsvc->getHist("/DQAHist/MUC/StripEff_All", htmp).isSuccess()) {
        htmp->Fill( eff[0] );
      } else {
        log << MSG::ERROR << "Fail to retrieve StripEff_All" << endreq;
      }

      if( trackNum[1][2] == 0 ) {
        eff[1] = 0.0;
      } else { 
        eff[1]     = ( (double)effHit[1][2] )/trackNum[1][2];
      }                
      if(m_thsvc->getHist("/DQAHist/MUC/StripEff_Dimu", htmp).isSuccess()) {
        htmp->Fill( eff[1] );
      } else { 
        log << MSG::ERROR << "Fail to retrieve StripEff_Dimu" << endreq;
      }    
    } // End StripMax
    
    // Box efficiency
    if( trackNum[0][1] == 0 ) {
      eff[0] = effErr[0] = 0.0;
    } else {
      eff[0]     = ( (double)effHit[0][1] )/trackNum[0][1];
      effErr[0]  = sqrt( eff[0]*(1-eff[0])/trackNum[0][1] );
    }  
    if(m_thsvc->getHist("/DQAHist/MUC/BoxEff_All", htmp).isSuccess()) {
      htmp->Fill( i, eff[0] );
      htmp->SetBinError( i+1, effErr[0] );
    } else {
      log << MSG::ERROR << "Fail to retrieve BoxEff_All" << endreq;
    }
    log << MSG::INFO << "BoxEff:\t" << i << "\t" << eff[0] << "\t" << effErr[0] << endreq;

    if( trackNum[1][1] == 0 ) {
      eff[1] = effErr[1] = 0.0;
    } else { 
      eff[1]     = ( (double)effHit[1][1] )/trackNum[1][1];
      effErr[1]  = sqrt( eff[1]*(1-eff[1])/trackNum[1][1] );
    }                
    if(m_thsvc->getHist("/DQAHist/MUC/BoxEff_Dimu", htmp).isSuccess()) {
      htmp->Fill( i, eff[1] );
      htmp->SetBinError( i+1, effErr[1] );
    } else { 
      log << MSG::ERROR << "Fail to retrieve BoxEff_Dimu" << endreq;
    }    

    // Box noise ratio
    if( digi[0][1] == 0 ) {
      nos[0] = nosErr[0] = 0.0;
    } else {
      nos[0]     = ( (double)nosHit[0][1] )/digi[0][1];
      nosErr[0]  = sqrt( nos[0]*(1-nos[0])/digi[0][1] );
    }  
    if(m_thsvc->getHist("/DQAHist/MUC/BoxNosRatio_All", htmp).isSuccess()) {
      htmp->Fill( i, nos[0] );
      htmp->SetBinError( i+1, nosErr[0] );
    } else {
      log << MSG::ERROR << "Fail to retrieve BoxNosRatio_All" << endreq;
    }
    log << MSG::INFO << "BoxNosRatio:\t" << i << "\t" << nos[0] << "\t" << nosErr[0] << endreq;

    if( digi[1][1] == 0 ) {
      nos[1] = nosErr[1] = 0.0;
    } else {
      nos[1]     = ( (double)nosHit[1][1] )/digi[1][1];
      nosErr[1]  = sqrt( nos[1]*(1-nos[1])/digi[1][1] );
    }  
    if(m_thsvc->getHist("/DQAHist/MUC/BoxNosRatio_Dimu", htmp).isSuccess()) {
      htmp->Fill( i, nos[1] );
      htmp->SetBinError( i+1, nosErr[1] );
    } else {
      log << MSG::ERROR << "Fail to retrieve BoxNosRatio_Dimu" << endreq;
    }
  }// End BOX_MAX
*/  

  log << MSG::INFO << "Finalize successfully! " << endmsg;
  
  return StatusCode::SUCCESS;
}

StatusCode DQA_MUC::FillHistograms(bool isDimu) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Filling histograms" << endmsg;
  char name[100];
  int part, segment, layer, strip, boxId, strId;
      part = segment = layer = strip = boxId = strId = 0;
  TH1 *hEff[2][2][3];
  
  for(int i=0; i<TAGN; i++) {
    for(int j=0; j<2; j++)  {
      for(int k=0; k<LVLN; k++)
      {    
        sprintf( name, "/DQAHist/MUC/%sEff_%s_%s", LNAME[k], ENAME[i], HTYPE[j] );
        if(!m_thsvc->getHist(name, hEff[i][j][k]).isSuccess()) 
          log << MSG::ERROR << "Fail to retrieve " << name << endreq;
      }
    }
  }

  // Numberator of eff
  for(int i=0; i<m_effHitCol.size(); i++)
  {
    part    =  m_effHitCol[i]->Part();
    segment =  m_effHitCol[i]->Segment();
    layer   =  m_effHitCol[i]->Layer();
    strip   =  m_effHitCol[i]->Strip();
    boxId = m_ptrIdTr->GetBoxId(part, segment, layer); 
    strId = m_ptrIdTr->GetStripId(part, segment, layer, strip); 
    hEff[0][0][0]->Fill(layer);
    hEff[0][0][1]->Fill(boxId);
    hEff[0][0][2]->Fill(strId);

    if( isDimu ) {
      hEff[1][0][0]->Fill(layer);
      hEff[1][0][1]->Fill(boxId);
      hEff[1][0][2]->Fill(strId);
    }
  }  

  // Denominator of eff          
  for(int i=0; i<m_expHitCol.size(); i++)
  {
    part    =  m_expHitCol[i]->Part();
    segment =  m_expHitCol[i]->Segment();
    layer   =  m_expHitCol[i]->Layer();
    strip   =  m_expHitCol[i]->Strip();
    //cout<<"ExpHit:\t"<<part<<"\t"<<segment<<"\t"<<layer<<"\t"<<strip<<endl;
    boxId = m_ptrIdTr->GetBoxId(part, segment, layer);
    strId = m_ptrIdTr->GetStripId(part, segment, layer, strip);
    hEff[0][1][0]->Fill(layer);
    hEff[0][1][1]->Fill(boxId);
    hEff[0][1][2]->Fill(strId);           

    if( isDimu ) {
      hEff[1][1][0]->Fill(layer);
      hEff[1][1][1]->Fill(boxId);
      hEff[1][1][2]->Fill(strId);
    }
    
  }

  TH1 *hNosRatio[2][2];
  for(int i=0; i<TAGN; i++) {
    for(int j=0; j<2; j++) {
      sprintf( name, "/DQAHist/MUC/%sNosRatio_%s_%s", LNAME[1], ENAME[i], HTYPE[j] );
      if(!m_thsvc->getHist(name, hNosRatio[i][j]).isSuccess()) 
      log << MSG::ERROR << "Fail to retrieve " << name << endreq;
    }
  }
    
  // Numberator of box noise ratio 
  for(int i=0; i<m_nosHitCol.size(); i++)
  {
    part    =  m_nosHitCol[i]->Part();
    segment =  m_nosHitCol[i]->Segment();
    layer   =  m_nosHitCol[i]->Layer();
    strip   =  m_nosHitCol[i]->Strip();
    boxId = m_ptrIdTr->GetBoxId(part, segment, layer); 
    strId = m_ptrIdTr->GetStripId(part, segment, layer, strip); 
    hNosRatio[0][0]->Fill(boxId);

    if( isDimu )  hNosRatio[1][0]->Fill(boxId);
  }

  // Denominator of box noise ratio 
  for(int i=0; i<m_digiCol.size(); i++)
  {
    part    =  m_digiCol[i]->Part();
    segment =  m_digiCol[i]->Segment();
    layer   =  m_digiCol[i]->Layer();
    strip   =  m_digiCol[i]->Strip();
    boxId = m_ptrIdTr->GetBoxId(part, segment, layer); 
    hNosRatio[0][1]->Fill(boxId);

    if( isDimu )  hNosRatio[1][1]->Fill(boxId);
  }
  
  return StatusCode::SUCCESS;
}

//END

