//------------------------------------------------------------------------------|
//      [File  ]:                       MucCalibMgr.cxx                         |
//      [Brief ]:       Manager of MUC calibration algrithom                    |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|
#include <cstdio>
#include <iostream>
#include <fstream>

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"
#include "Identifier/MucID.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "EvTimeEvent/RecEsTime.h"

#include "McTruth/McKine.h"
#include "McTruth/McParticle.h"
#include "McTruth/MucMcHit.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "TofRecEvent/RecTofTrack.h"
#include "DstEvent/TofHitStatus.h"

//#include "ReconEvent/ReconEvent.h"
#include "MucRawEvent/MucDigi.h"
#include "MucRecEvent/MucRecHit.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRecHitContainer.h"

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucCalibMgr.h"
#include "MucCalibAlg/MucMark.h"
#include "MucCalibAlg/MucBoxCal.h"
#include "MucCalibAlg/MucStripCal.h"

using namespace std;
using namespace Event;

//------------------------------------------- Constructor -----------------------------------------
MucCalibMgr::MucCalibMgr( std::vector<double> jobInfo, std::vector<int> configInfo, std::string outputFile ) 
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "-------Initializing------- " << endreq;
  
  // init Gaudi service
  log << MSG::INFO << "Initialize Gaudi service" << endreq;
  Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
  ntupleSvc = NULL;
  eventSvc  = NULL;
  
  m_jobStart = clock(); 
  
  // init parameters
  log << MSG::INFO << "Initialize parameters" << endreq;
  
  m_vs = jobInfo[0]; m_hv = jobInfo[1]; m_th = jobInfo[2];   
  
  if( jobInfo[3] <= 0 ) m_er = TRIGGER_RATE; // 4000Hz
  else                  m_er = jobInfo[3];
  
  if( jobInfo[4] <= 0 || jobInfo[4] >5 ) m_tag = 0;
  else m_tag = jobInfo[4];  
  
  if( configInfo[0] < 0 || configInfo[0] > 2 )  
    m_recMode = 0;
  else                              
    m_recMode = configInfo[0];
  
  m_usePad = configInfo[1];
    
  if( configInfo[2] < 0 || configInfo[2] > STRIP_INBOX_MAX ) 
        m_effWindow = EFF_WINDOW; // 4 strip-width
  else  m_effWindow = configInfo[2];
  
  if( configInfo[3]<0 || configInfo[3] >4 ) 
        m_clusterMode = DEFAULT_BUILD_MODE;
  else  m_clusterMode = configInfo[3];
  
  m_clusterSave = configInfo[4];
  m_checkEvent  = configInfo[5];
  m_dimuSelect  = configInfo[6];
  m_dimuOnly    = configInfo[7];

  m_outputFile  = outputFile;
 
  m_fdata = NULL; 
  if( m_clusterMode ==1 && m_clusterSave == 1 ) 
    m_fdata = new ofstream("MucCluster.dat", ios::out);
    
  m_fStartRun   = m_fEndRun       = 0;
  m_currentRun  = m_currentEvent  = 0;	
  m_eventTag    = 0;
    
  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<SEGMENT_MAX; j++ )
      for( int k=0; k<LAYER_MAX; k++ )
        for( int n=0; n<STRIP_INBOX_MAX; n++ )
        { 
          m_record[i][j][k][n][0] = 0;
          m_record[i][j][k][n][1] = 0;
          m_record[i][j][k][n][2] = 0;
          m_record[i][j][k][n][3] = 0;
          m_record[i][j][k][n][4] = 0;
        }
  
  for( int i=0; i<LAYER_MAX; i++ )
  {
    m_layerResults[0][i] = 0;
    m_layerResults[1][i] = 0;
    m_layerResults[2][i] = 0;
    m_layerResults[3][i] = 0;
    m_layerResults[4][i] = 0;
  }
  
  for( int i=0; i<BOX_MAX; i++ )
  {
    m_boxResults[0][i] = 0;
    m_boxResults[1][i] = 0;
    m_boxResults[2][i] = 0;
    m_boxResults[3][i] = 0;
    m_boxResults[4][i] = 0;
  }
  
  for( int i=0; i<STRIP_MAX; i++ )
  {
    m_stripResults[0][i] = 0;
    m_stripResults[1][i] = 0;
    m_stripResults[2][i] = 0;
    m_stripResults[3][i] = 0;
    // strip no cluster
  }
  
  m_fTotalDAQTime = 0;
  
  m_ptrMucMark  = new MucMark(0,0,0,0);
  m_ptrIdTr     = new MucIdTransform();
    
  m_digiCol.resize(0);
  m_calHitCol.resize(0);
  m_expHitCol.resize(0);
  m_effHitCol.resize(0);
  m_nosHitCol.resize(0);
  m_clusterCol.resize(0);

  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<SEGMENT_MAX; j++ ) {
      m_segDigiCol[i][j].resize(0);
    }

  // init Gaudi Ntuple
  InitNtuple();
 
  // init ROOT histograms
  InitHisto();
 
  // init Tree
  InitConstTree();    
  
  // init area of units
  InitArea();
  log << MSG::INFO << "Initialize done!" << endreq;
}

//----------------------------------------------- Destructor --------------------------------------
MucCalibMgr::~MucCalibMgr()
{
  ClearOnlineHisto();  
  
  ClearHistoLV2();
  ClearHistoLV1();
  ClearHistoLV0();
  if( m_usePad != 0 ) Clear2DEffHisto();
  
  ClearClusterHisto();
  ClearResHisto();
  ClearConstTree();  
  
  if(m_ptrMucMark)  delete m_ptrMucMark;
  if(m_ptrIdTr)     delete m_ptrIdTr;
    
  m_digiCol.clear();
  m_calHitCol.clear();
  m_effHitCol.clear();
  m_nosHitCol.clear();
  m_clusterCol.clear();
  
  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<SEGMENT_MAX; j++ ) {
      m_segDigiCol[i][j].clear();
    }

  if(m_record)        delete []m_record;
  if(m_layerResults)  delete []m_layerResults;
  if(m_boxResults)    delete []m_boxResults;
  if(m_stripResults)  delete []m_stripResults;
}

// Clear online histograms
StatusCode MucCalibMgr::ClearOnlineHisto()
{
  if(m_hHitMapBarrel_Lay) delete []m_hHitMapBarrel_Lay;
  if(m_hHitMapEndcap_Lay) delete []m_hHitMapEndcap_Lay;
  if(m_hHitMapBarrel_Seg) delete []m_hHitMapBarrel_Seg;
  if(m_hHitMapEndcap_Seg) delete []m_hHitMapEndcap_Seg;

  if(m_hHitVsEvent)       delete m_hHitVsEvent;
  if(m_hTrackDistance)    delete m_hTrackDistance;
  if(m_hTrackPosPhiDiff)  delete m_hTrackPosPhiDiff;
  if(m_hTrackPosThetaDiff)delete m_hTrackPosThetaDiff;
  if(m_hTrackMomPhiDiff)  delete m_hTrackMomPhiDiff;
  if(m_hTrackMomThetaDiff)delete m_hTrackMomThetaDiff;
  if(m_hDimuTracksPosDiff)delete m_hDimuTracksPosDiff;
  if(m_hDimuTracksMomDiff)delete m_hDimuTracksMomDiff;
  if(m_hPhiCosTheta)      delete m_hPhiCosTheta;

  return StatusCode::SUCCESS;
}

// Clear level 0 histograms
StatusCode MucCalibMgr::ClearHistoLV0()
{
  if(m_hBrLayerFire)  delete m_hBrLayerFire;
  if(m_hEcLayerFire)  delete m_hEcLayerFire;
  if(m_hLayerFire)    delete m_hLayerFire;
  if(m_hLayerExpHit)  delete m_hLayerExpHit;
  if(m_hLayerExpHit)  delete m_hLayerEffHit;
  if(m_hLayerNosHit)  delete m_hLayerNosHit;
  if(m_hLayerEff)     delete m_hLayerEff;
  if(m_hLayerNosRatio)delete m_hLayerNosRatio;
  if(m_hLayerArea)    delete m_hLayerArea;
  if(m_hLayerNos)     delete m_hLayerNos;
  if(m_hLayerCnt)     delete m_hLayerCnt;

  return StatusCode::SUCCESS;
}

// Clear level 1 histograms
StatusCode MucCalibMgr::ClearHistoLV1()
{
  if(m_hBoxFire)    delete m_hBoxFire;
  if(m_hBoxExpHit)  delete m_hBoxExpHit;
  if(m_hBoxEffHit)  delete m_hBoxEffHit;
  if(m_hBoxNosHit)  delete m_hBoxNosHit;
  if(m_hBoxEff)     delete m_hBoxEff;
  if(m_hBoxNosRatio)delete m_hBoxNosRatio;
  if(m_hBoxArea)    delete m_hBoxArea;
  if(m_hBoxNos)     delete m_hBoxNos;
  if(m_hBoxCnt)     delete m_hBoxCnt;
  
  return StatusCode::SUCCESS;
}

// Clear level 2 histograms
StatusCode MucCalibMgr::ClearHistoLV2()
{	
  for( int i=0; i< BOX_MAX; i++ )
  {
    if(m_hStripFireMap[i])    delete m_hStripFireMap[i];
    if(m_hStripExpHitMap[i])  delete m_hStripExpHitMap[i];
    if(m_hStripEffHitMap[i])  delete m_hStripEffHitMap[i];
    if(m_hStripNosHitMap[i])  delete m_hStripNosHitMap[i];
    if(m_hStripEffMap[i])     delete m_hStripEffMap[i];
    if(m_hStripNosRatioMap[i])delete m_hStripNosRatioMap[i];
  }
  
  if(m_hStripFire)    delete m_hStripFire;
  if(m_hStripExpHit)  delete m_hStripExpHit;
  if(m_hStripEffHit)  delete m_hStripEffHit;
  if(m_hStripNosHit)  delete m_hStripNosHit;
  if(m_hStripEff)     delete m_hStripEff;
  if(m_hStripNosRatio)delete m_hStripNosRatio;
  if(m_hStripArea)    delete m_hStripArea;
  if(m_hStripNos)     delete m_hStripNos;
  if(m_hStripCnt)     delete m_hStripCnt;

  return StatusCode::SUCCESS;
}

// Clear 2D eff histogrames 
StatusCode MucCalibMgr::Clear2DEffHisto()
{

  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<((i==BRID)?B_SEG_NUM:E_SEG_NUM); j++ )
      for( int k=0; k<((i==BRID)?B_LAY_NUM:E_LAY_NUM); k++ )
      {
        if(m_h2DExpMap[i][j][k]) delete m_h2DExpMap[i][j][k];
        if(m_h2DHitMap[i][j][k]) delete m_h2DHitMap[i][j][k];
        if(m_h2DEffMap[i][j][k]) delete m_h2DEffMap[i][j][k];
      }

  return StatusCode::SUCCESS;
}

// Clear cluster histograms
StatusCode MucCalibMgr::ClearClusterHisto()
{
  for( int i=0; i<LAYER_MAX; i++ ) {
    if(m_hLayerCluster[i])  delete m_hLayerCluster[i];
  }	
    
  for( int i=0; i<BOX_MAX; i++ ) {  
    if(m_hBoxCluster[i])    delete m_hBoxCluster[i];
  }	
  
  if(m_hLayerClusterCmp)  delete m_hLayerClusterCmp;
  if(m_hBoxClusterCmp)    delete m_hBoxClusterCmp;
  
  return StatusCode::SUCCESS;
}

// Clear resolution histograms
StatusCode MucCalibMgr::ClearResHisto()
{
  for( int i=0; i<B_LAY_NUM; i++ ) {
    if(m_hBarrelResDist[i]) delete m_hBarrelResDist[i];
  }
  for( int i=0; i<E_LAY_NUM; i++ ) {
    if(m_hEndcapResDist[i]) delete m_hEndcapResDist[i];
  }

  if(m_hBarrelResComp[0]) delete m_hBarrelResComp[0];
  if(m_hBarrelResComp[1]) delete m_hBarrelResComp[1];
  if(m_hEndcapResComp[0]) delete m_hEndcapResComp[0];
  if(m_hEndcapResComp[1]) delete m_hEndcapResComp[1];

  return StatusCode::SUCCESS;
}

// Clear Tree
StatusCode MucCalibMgr::ClearConstTree()
{

  if(m_tJobLog)     delete m_tJobLog;
  if(m_tStatLog)    delete m_tStatLog;
  if(m_tLayConst)   delete m_tLayConst;
  if(m_tBoxConst)   delete m_tBoxConst;
  if(m_tStrConst)   delete m_tStrConst;

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------- Member functions for initializing ------------------------------
//-------------------------------------------------------------------------------------------------


// init Ntuples
StatusCode MucCalibMgr::InitNtuple()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Initialize NTuples" << endreq;
  
  // Book ntuple 
  Gaudi::svcLocator() -> service("NTupleSvc", ntupleSvc);
  
  StatusCode sc;
  
  // event log	
  NTuplePtr nt1(ntupleSvc, "FILE450/EventLog");
  if ( nt1 ) { m_eventLogTuple = nt1; }
  else
  {
    m_eventLogTuple = ntupleSvc->book ("FILE450/EventLog", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_eventLogTuple ) 
    {
      sc = m_eventLogTuple->addItem ("event_id",    m_ntEventId);
      sc = m_eventLogTuple->addItem ("event_tag",   m_ntEventTag);
      sc = m_eventLogTuple->addItem ("start_time",  m_ntEsTime);
      sc = m_eventLogTuple->addItem ("digi_num",    m_ntDigiNum);
      sc = m_eventLogTuple->addItem ("track_num",   m_ntTrackNum);
      sc = m_eventLogTuple->addItem ("exphit_num",  m_ntExpHitNum);
      sc = m_eventLogTuple->addItem ("effhit_num",  m_ntEffHitNum);
      sc = m_eventLogTuple->addItem ("noshit_num",  m_ntNosHitNum);
      sc = m_eventLogTuple->addItem ("cluster_num", m_ntClusterNum);
      sc = m_eventLogTuple->addItem ("event_time",  m_ntEventTime);
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_eventLogTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  // track info
  
  NTuplePtr nt2(ntupleSvc, "FILE450/MdcTrkInfo");
  if ( nt2 ) { m_mdcTrkInfoTuple = nt2; }
  else
  {
    m_mdcTrkInfoTuple = ntupleSvc->book ("FILE450/MdcTrkInfo", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_mdcTrkInfoTuple ) 
    {
      sc = m_mdcTrkInfoTuple->addItem ("charge",        m_charge);
      sc = m_mdcTrkInfoTuple->addItem ("mdcpx",         m_mdcpx);
      sc = m_mdcTrkInfoTuple->addItem ("mdcpy",         m_mdcpy);
      sc = m_mdcTrkInfoTuple->addItem ("mdcpz",         m_mdcpz);
      sc = m_mdcTrkInfoTuple->addItem ("mdcpt",         m_mdcpt);
      sc = m_mdcTrkInfoTuple->addItem ("mdcpp",         m_mdcpp);
      sc = m_mdcTrkInfoTuple->addItem ("mdcphi",        m_mdcphi);
      sc = m_mdcTrkInfoTuple->addItem ("mdctheta",      m_mdctheta);
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_mdcTrkInfoTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
 
      
  NTuplePtr nt3(ntupleSvc, "FILE450/TrackInfo");
  if ( nt3 ) { m_trackInfoTuple = nt3; }
  else
  {
    m_trackInfoTuple = ntupleSvc->book ("FILE450/TrackInfo", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_trackInfoTuple ) 
    {
      sc = m_trackInfoTuple->addItem ("track_event",   m_ntTrackEvent);
      sc = m_trackInfoTuple->addItem ("track_tag",     m_ntTrackTag);
      sc = m_trackInfoTuple->addItem ("track_hits",    m_ntTrackHits);
      sc = m_trackInfoTuple->addItem ("segment_fly",   m_ntTrackSegFly);
      sc = m_trackInfoTuple->addItem ("layer_fly_a",   m_ntTrackLayFlyA); 
      sc = m_trackInfoTuple->addItem ("layer_fly_b",   m_ntTrackLayFlyB); 
      sc = m_trackInfoTuple->addItem ("layer_fly_c",   m_ntTrackLayFlyC); 
      sc = m_trackInfoTuple->addItem ("rec_mode",      m_trkRecMode);
      sc = m_trackInfoTuple->addItem ("chi2",            m_chi2);
      sc = m_trackInfoTuple->addItem ("px",            m_px);
      sc = m_trackInfoTuple->addItem ("py",            m_py);
      sc = m_trackInfoTuple->addItem ("pz",            m_pz);
      sc = m_trackInfoTuple->addItem ("pt",            m_pt);
      sc = m_trackInfoTuple->addItem ("pp",            m_pp);
      sc = m_trackInfoTuple->addItem ("r",             m_r );
      sc = m_trackInfoTuple->addItem ("costheta",      m_cosTheta);
      sc = m_trackInfoTuple->addItem ("theta",         m_theta);
      sc = m_trackInfoTuple->addItem ("phi",           m_phi);      
      sc = m_trackInfoTuple->addItem ("depth",         m_depth);      
      sc = m_trackInfoTuple->addItem ("br_last_lay",   m_brLastLayer);      
      sc = m_trackInfoTuple->addItem ("ec_last_lay",   m_ecLastLayer);      
      sc = m_trackInfoTuple->addItem ("total_hits",    m_totalHits);      
      sc = m_trackInfoTuple->addItem ("fired_layers",  m_totalLayers);      
      sc = m_trackInfoTuple->addItem ("maxhits_in_layer",  m_maxHitsInLayer);      
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_trackInfoTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // track collinearity 
  NTuplePtr nt4(ntupleSvc, "FILE450/TrackDiff");
  if ( nt4 ) { m_trackDiffTuple = nt4; }
  else
  {
    m_trackDiffTuple = ntupleSvc->book ("FILE450/TrackDiff", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_trackDiffTuple ) {
      sc = m_trackDiffTuple->addItem ("dimu_tag",       m_ntDimuTag);
      sc = m_trackDiffTuple->addItem ("pos_phi_diff",   m_ntPosPhiDiff);
      sc = m_trackDiffTuple->addItem ("pos_theta_diff", m_ntPosThetaDiff);
      sc = m_trackDiffTuple->addItem ("mom_phi_diff",   m_ntMomPhiDiff);
      sc = m_trackDiffTuple->addItem ("mom_theta_diff", m_ntMomThetaDiff);
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_trackDiffTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  // cluster size
  NTuplePtr nt5(ntupleSvc, "FILE450/ClusterSize");
  if ( nt5 ) { m_clusterSizeTuple = nt5; } 
  else
  { 
    m_clusterSizeTuple = ntupleSvc->book ("FILE450/ClusterSize", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_clusterSizeTuple ) {
      sc = m_clusterSizeTuple->addItem ("cluster_size",   m_ntClusterSize);
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_clusterSizeTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  // eff window
  NTuplePtr nt6(ntupleSvc, "FILE450/EffWindow");
  if ( nt6 ) { m_effWindowTuple = nt6; }
  else
  {
    m_effWindowTuple = ntupleSvc->book ("FILE450/EffWindow", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_effWindowTuple ) {
      sc = m_effWindowTuple->addItem ("hit_window",   m_ntEffWindow);
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_effWindowTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // res info
  NTuplePtr nt7(ntupleSvc, "FILE450/ResInfo");
  if ( nt7 ) { m_resInfoTuple = nt7; }
  else
  {
    m_resInfoTuple = ntupleSvc->book ("FILE450/ResInfo", CLID_RowWiseTuple, "MucCalibConst N-Tuple");
    if ( m_resInfoTuple ) {
      sc = m_resInfoTuple->addItem ("line_res",    m_lineRes);      
      sc = m_resInfoTuple->addItem ("quad_res",    m_quadRes);      
      sc = m_resInfoTuple->addItem ("extr_res",    m_extrRes);      
      sc = m_resInfoTuple->addItem ("res_part",    m_resPart);      
      sc = m_resInfoTuple->addItem ("res_segment", m_resSegment);      
      sc = m_resInfoTuple->addItem ("res_layer",   m_resLayer);      
      sc = m_resInfoTuple->addItem ("res_fired",   m_resFired);      
      sc = m_resInfoTuple->addItem ("res_mode",    m_resMode);      
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_resInfoTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }  
  
/*
  NTuplePtr nt7(ntupleSvc, "FILE450/ResInfo");
  if ( nt7 ) { m_resInfoTuple = nt7; }
  else
  {
    m_resInfoTuple = ntupleSvc->book ("FILE450/ResInfo", CLID_ColumnWiseTuple, "MucCalibConst N-Tuple");
    if ( m_resInfoTuple ) {
      sc = m_resInfoTuple->addItem ("exp_num",  m_nExpNum, 0, 30);      
      sc = m_resInfoTuple->addIndexedItem ("res",         m_nExpNum, m_res);      
      sc = m_resInfoTuple->addIndexedItem ("res_part",    m_nExpNum, m_resPart);      
      sc = m_resInfoTuple->addIndexedItem ("res_segment", m_nExpNum, m_resSegment);      
      sc = m_resInfoTuple->addIndexedItem ("res_layer",   m_nExpNum, m_resLayer);      
      sc = m_resInfoTuple->addIndexedItem ("res_fired",   m_nExpNum, m_resFired);      
    }
    else {
      log << MSG::ERROR << "Cannot book N-tuple:" << long(m_resInfoTuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }

 for(int i=0; i<24; i++ )
 {
  m_res[i] = 211; 
  m_resPart[i] = -1;
  m_resSegment[i] = -1;
  m_resLayer[i] = -1;
  m_resFired[i] = false;
 }
*/
  
  return StatusCode::SUCCESS;
}

// Initialize histogram maps of fired strips and reconstructed hits
StatusCode MucCalibMgr::InitHisto()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Initialize Histograms" << endreq;
  
  // Init online monitor histo
  InitOnlineHisto();
  
  // Init eff map and so on
  InitHistoLV2();
  InitHistoLV1();
  InitHistoLV0();
  
  // Init 2D eff map
  if( m_usePad != 0 ) Init2DEffHisto();
  
  // Init cluster histo
  InitClusterHisto();

  // Init spacial resolution histo
  InitResHisto();
  
  return  StatusCode::SUCCESS;
}

// Init histograme for online monitor 
StatusCode MucCalibMgr::InitOnlineHisto()
{
  char name[60];
  char title[60];
  int stripMax = 0;
  
  // Init hit map 
  for( int i=0; i<B_LAY_NUM; i++ )
  {
    // According to layer
    if(i%2 != 0 ) { stripMax = 96*7 + 112; }
    else          { stripMax = 48*8;       }
    sprintf( name, "HitMapBarrel_Lay_L%d", i );
    sprintf( title, "Hit map in barrel layer %d", i );
    m_hHitMapBarrel_Lay[i] = new TH1F(name,title, stripMax, 0, stripMax);
    
    if( i<E_LAY_NUM )
    {  
      stripMax = 64*4; // 256
      sprintf( name, "HitMapEastEndcap_L%d", i );
      sprintf( title, "Hit map in east-endcap layer %d", i );
      m_hHitMapEndcap_Lay[0][i] = new TH1F(name,title, stripMax, 0, stripMax);
      
      sprintf( name, "HitMapWestEndcap_L%d", i );
      sprintf( title, "Hit map in west-endcap layer %d", i );
      m_hHitMapEndcap_Lay[1][i] = new TH1F(name,title, stripMax, 0, stripMax);
    }  
      
  }

  for( int i=0; i<B_SEG_NUM; i++ )
  {
    // According to segment
    sprintf( name, "HitMapBarrel_Seg_S%d", i );
    sprintf( title, "Hit map in barrel segment %d", i );
    if(i==2 ) { stripMax = 48*5 + 112*4; }  // 688
    else      { stripMax = 48*5 + 96*4;  }  // 624
    m_hHitMapBarrel_Seg[i] = new TH1F(name,title, stripMax, 0, stripMax);

    if( i<E_SEG_NUM )
    {
      sprintf( name, "HitMapEastEndcap_S%d", i );
      sprintf( title, "Hit map in east-endcap segment %d", i );
      stripMax = 64*8;  // 512
      m_hHitMapEndcap_Seg[0][i] = new TH1F(name,title, stripMax, 0, stripMax);
      
      sprintf( name, "HitMapWestEndcap_S%d", i );
      sprintf( title, "Hit map in west-endcap segment %d", i );
      m_hHitMapEndcap_Seg[1][i] = new TH1F(name,title, stripMax, 0, stripMax);
    }
  }

  // Init histograms for online monitor  
  // 1D histograms
  m_hHitVsEvent     = new TH1F("HitVsEvent","Hit VS event",10000,0,10000);
  m_hHitVsEvent->GetXaxis()->SetTitle("Event NO.");
  m_hHitVsEvent->GetXaxis()->CenterTitle();
  m_hHitVsEvent->GetYaxis()->SetTitle("Hits");
  m_hHitVsEvent->GetYaxis()->CenterTitle();
  
  m_hTrackDistance  = new TH1F("TrackDistance","Track distance", 1000,-500,500);
  m_hTrackDistance->GetXaxis()->SetTitle("Distance of fit pos and hit pos on 1st layer [cm]");
  m_hTrackDistance->GetXaxis()->CenterTitle();
  
  m_hTrackPosPhiDiff  = new TH1F("TrackPosPhiDiff","#Delta#phi of tracks pos", 720,-2*PI,2*PI);
  m_hTrackPosPhiDiff->GetXaxis()->SetTitle("#Delta#phi [rad]");
  m_hTrackPosPhiDiff->GetXaxis()->CenterTitle();

  m_hTrackPosThetaDiff  = new TH1F("TrackPosThetaDiff","#Delta#theta of tracks pos", 720,-2*PI,2*PI);
  m_hTrackPosThetaDiff->GetXaxis()->SetTitle("#Delta#theta [rad]");
  m_hTrackPosThetaDiff->GetXaxis()->CenterTitle();
  
  m_hTrackMomPhiDiff  = new TH1F("TrackMomPhiDiff","#Delta#phi of tracks mom", 720,-2*PI,2*PI);
  m_hTrackMomPhiDiff->GetXaxis()->SetTitle("#Delta#phi [rad]");
  m_hTrackMomPhiDiff->GetXaxis()->CenterTitle();
  
  m_hTrackMomThetaDiff  = new TH1F("TrackMomThetaDiff","#Delta#theta of tracks mom", 720,-2*PI,2*PI);
  m_hTrackMomThetaDiff->GetXaxis()->SetTitle("#Delta#theta [rad]");
  m_hTrackMomThetaDiff->GetXaxis()->CenterTitle();
  
  // 2D histograms   
  m_hDimuTracksPosDiff  = new TH2F("DimuTracksPosDiff", "#Delta#phi VS #Delta#theta of dimu tracks pos", 720, -PI, PI, 720, -PI, PI);
  m_hDimuTracksPosDiff->GetXaxis()->SetTitle("#Delta#theta");
  m_hDimuTracksPosDiff->GetXaxis()->CenterTitle();
  m_hDimuTracksPosDiff->GetYaxis()->SetTitle("#Delta#phi");
  m_hDimuTracksPosDiff->GetYaxis()->CenterTitle();
  
  m_hDimuTracksMomDiff  = new TH2F("DimuTracksMomDiff", "#Delta#phi VS #Delta#theta of dimu tracks mom", 720, -PI, PI, 720, -PI, PI);
  m_hDimuTracksMomDiff->GetXaxis()->SetTitle("#Delta#theta");
  m_hDimuTracksMomDiff->GetXaxis()->CenterTitle();
  m_hDimuTracksMomDiff->GetYaxis()->SetTitle("#Delta#phi");
  m_hDimuTracksMomDiff->GetYaxis()->CenterTitle();
  
  m_hPhiCosTheta    = new TH2F("PhiVsCosTheta", "#phi VS cos(#theta)", 720, -1, 1, 720, -PI, PI);
  m_hPhiCosTheta->GetXaxis()->SetTitle("cos(#theta)");
  m_hPhiCosTheta->GetXaxis()->CenterTitle();
  m_hPhiCosTheta->GetYaxis()->SetTitle("#phi");
  m_hPhiCosTheta->GetYaxis()->CenterTitle();
  
  return StatusCode::SUCCESS;
}

// Init histograme for calibration level 0
StatusCode MucCalibMgr::InitHistoLV0()
{
  m_hBrLayerFire  = new TH1F("BrLayerFire","Fires per layer in Barrel", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hEcLayerFire  = new TH1F("EcLayerFire","Fires per layer in Endcap", 2*LAYER_MAX-1, -LAYER_MAX+1, LAYER_MAX-0.5);
  
  m_hLayerFire    = new TH1F("LayerFire","Fires per layer", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerExpHit  = new TH1F("LayerExpHit","Exp hits per layer", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerEffHit  = new TH1F("LayerEffHit","Eff hits per layer", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerNosHit  = new TH1F("LayerNosHit","Nos hits per layer", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerEff     = new TH1F("LayerEff","Layer efficiency", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerNosRatio= new TH1F("LayerNosRatio","Layer noise hit ratio", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerArea    = new TH1F("LayerArea","Layer area", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerNos     = new TH1F("LayerNos","Layer noise", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  m_hLayerCnt     = new TH1F("LayerCnt","Layer count", LAYER_MAX+1, -0.5, LAYER_MAX+0.5);
  
  return StatusCode::SUCCESS;
}

// Init histograme for calibration level 1
StatusCode MucCalibMgr::InitHistoLV1()
{
  m_hBoxFire      = new TH1F("BoxFire","Fires per box", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxExpHit    = new TH1F("BoxExpHit","Exp hits per box", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxEffHit    = new TH1F("BoxEffHit","Eff hits per box", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxNosHit    = new TH1F("BoxNosHit","Nos hits per box", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxEff       = new TH1F("BoxEff","Box efficiency", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxNosRatio  = new TH1F("BoxNosRatio","Box noise hit ratio", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxArea      = new TH1F("BoxArea","Box area", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxNos       = new TH1F("BoxNos","Box noise", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  m_hBoxCnt       = new TH1F("BoxCnt","Box count", BOX_MAX+1, -0.5, BOX_MAX+0.5);
  
  return StatusCode::SUCCESS;
}

// Init histograme for calibration level 2
StatusCode MucCalibMgr::InitHistoLV2()
{
  char name[60];
  char title[60];
  int part, segment, layer, stripMax;
      part = segment = layer = stripMax = 0;
  
  for( int i=0; i<BOX_MAX; i++ )
  {
    m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
    stripMax = m_ptrIdTr->GetStripMax( part, segment, layer );
    
    sprintf( name,  "StripFireMap_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Fires per strip in P%d_S%d_L%d Box%d",part, segment, layer, i );
    m_hStripFireMap[i] = new TH1F(name,title, stripMax, 0, stripMax);
    
    sprintf( name,  "StripExpHitMap_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Exp hits per strip in P%d_S%d_L%d Box%d",part, segment, layer, i );
    m_hStripExpHitMap[i] = new TH1F(name,title, stripMax, 0, stripMax);
    
    sprintf( name,  "StripEffHitMap_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Eff hits per strip in P%d_S%d_L%d Box%d",part, segment, layer, i );
    m_hStripEffHitMap[i] = new TH1F(name,title, stripMax, 0, stripMax);
    
    sprintf( name,  "StripNosHitMap_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Inc hits per strip in P%d_S%d_L%d Box%d",part, segment, layer, i );
    m_hStripNosHitMap[i] = new TH1F(name,title, stripMax, 0, stripMax);
    
    sprintf( name,  "StripEffMap_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Strip efficiency in P%d_S%d_L%d Box%d",part, segment, layer, i );
    m_hStripEffMap[i] = new TH1F(name,title, stripMax, 0, stripMax);
    
    sprintf( name,  "StripNosRatioMap_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Strip noise hit ratio in P%d_S%d_L%d Box%d",part, segment, layer, i );
    m_hStripNosRatioMap[i] = new TH1F(name,title, stripMax, 0, stripMax);
  }
  
  m_hStripFire    = new TH1F("StripFire", "Fires per strip", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripExpHit  = new TH1F("StripExpHit", "Exp hit per strip", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripEffHit  = new TH1F("StripEffHit", "Eff hit per strip", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripNosHit  = new TH1F("StripNoshit", "Nos hit per strip", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripEff     = new TH1F("StripEff", "Strip efficiency", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripNosRatio= new TH1F("StripNosRatio", "Strip noise hit ratio", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripArea    = new TH1F("StripArea", "Strip area", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripNos     = new TH1F("StripNos", "Strip noise", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  m_hStripCnt     = new TH1F("StripCnt", "Strip count", STRIP_MAX+1, -0.5, STRIP_MAX+0.5 );
  
  return StatusCode::SUCCESS;
}

// Init 2D eff histograme 
StatusCode MucCalibMgr::Init2DEffHisto()
{
  char name[60];
  int stripMax, boxID, stripID, xBin, yBin;
  stripMax = boxID = stripID = xBin = yBin = 0;
  double xStart, xEnd, yStart, yEnd, sWidth;
  xStart = xEnd = yStart = yEnd = sWidth = 0.;

  m_histArray = new TObjArray();
  
  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<((i==BRID)?B_SEG_NUM:E_SEG_NUM); j++ )
      for( int k=0; k<((i==BRID)?B_LAY_NUM:E_LAY_NUM); k++ )
      {    
        boxID = m_ptrIdTr->GetBoxId(i,j,k);

        if( i==BRID )
        {
          xStart = -2000, xEnd = 2000;
          sWidth = B_STR_DST[k];
          xBin = int((xEnd - xStart)/sWidth);
          yStart = -B_BOX_WT[k]/2-100, yEnd = B_BOX_WT[k]/2+100;           
          yBin = int((B_BOX_WT[k]+200)/sWidth);
        }
        else
        {
          xStart = yStart = -1250;
          xEnd = yEnd = 1250;
          sWidth = E_STR_DST;
          xBin = yBin = int((xEnd - xStart)/sWidth);
        } 
        
        sprintf(name, "ExpMap2D_P%d_S%d_L%d_Box%d", i, j, k, boxID);
        m_h2DExpMap[i][j][k] = new TH2F(name, name, xBin, xStart, xEnd, yBin, yStart, yEnd);
        sprintf(name, "HitMap2D_P%d_S%d_L%d_Box%d", i, j, k, boxID);
        m_h2DHitMap[i][j][k] = new TH2F(name, name, xBin, xStart, xEnd, yBin, yStart, yEnd);
        sprintf(name, "EffMap2D_P%d_S%d_L%d_Box%d", i, j, k, boxID);
        m_h2DEffMap[i][j][k] = new TH2F(name, name, xBin, xStart, xEnd, yBin, yStart, yEnd);

        //m_histArray->Add(m_h2DExpMap[i][j][k]);
        //m_histArray->Add(m_h2DHitMap[i][j][k]);
        m_histArray->Add(m_h2DEffMap[i][j][k]);
      }

  return StatusCode::SUCCESS;
}  

// Init histograme for cluster
StatusCode MucCalibMgr::InitClusterHisto()
{
  char name[60];
  char title[60];
  int part, segment, layer, stripMax;
      part = segment = layer = stripMax = 0;
  
  for( int i=0; i<LAYER_MAX; i++ )
  {
    sprintf( name, "LayerCluster_L%d", i );
    sprintf( title, "Clusters in L%d",i );	
    m_hLayerCluster[i] = new TH1F(name,title, CLUSTER_RANGE, 0, CLUSTER_RANGE );	
  }
  
  for( int i=0; i<BOX_MAX; i++ )
  {
    m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
    stripMax = m_ptrIdTr->GetStripMax( part, segment, layer );
    sprintf( name, "BoxCluster_P%d_S%d_L%d_Box%d", part, segment, layer, i );
    sprintf( title, "Clusters in P%d_S%d_L%d Box%d", part, segment, layer, i );
    m_hBoxCluster[i]  = new TH1F(name,title, CLUSTER_RANGE, 0, CLUSTER_RANGE );
  }
  
  m_hLayerClusterCmp  = new TH1F("LayerCluster", "LayerCluster", LAYER_MAX+1, -0.5, LAYER_MAX+0.5 );
  m_hBoxClusterCmp    = new TH1F("BoxCluster", "BoxCluster", BOX_MAX+1, -0.5, BOX_MAX+0.5 );
  
  return StatusCode::SUCCESS;
}

// Init histograms for spacial resolution
StatusCode MucCalibMgr::InitResHisto()
{
  char name[60];
  char title[60];

  for( int i=0; i<B_LAY_NUM; i++ )
  {
    sprintf( name, "BarrelRes_L%d", i );
    sprintf( title, "Barrel spacial resolution in L%d",i );
    m_hBarrelResDist[i] = new TH1F(name,title, 200, -100, 100 );
  }

  for( int i=0; i<E_LAY_NUM; i++ )
  {
    sprintf( name, "EndcapRes_L%d", i );
    sprintf( title, "Endcap spacial resolution in L%d",i );
    m_hEndcapResDist[i] = new TH1F(name,title, 200, -100, 100 );
  }

  m_hBarrelResComp[0] = new TH1F("BarrelResMean", "BarrelResMean", B_LAY_NUM+1, -0.5, B_LAY_NUM+0.5 );
  m_hBarrelResComp[1] = new TH1F("BarrelResSigma", "BarrelResSigma", B_LAY_NUM+1, -0.5, B_LAY_NUM+0.5 );
  m_hEndcapResComp[0] = new TH1F("EndcapResMean", "EndcapResMean", E_LAY_NUM+1, -0.5, E_LAY_NUM+0.5 );
  m_hEndcapResComp[1] = new TH1F("EndcapResSigma", "EndcapResSigma", E_LAY_NUM+1, -0.5, E_LAY_NUM+0.5 );

  return StatusCode::SUCCESS;
}

// Init Tree
StatusCode MucCalibMgr::InitConstTree()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Initialize Trees" << endreq;
  
  // job log tree
  m_tJobLog   = new TTree("JobLog","Job information");
  m_tJobLog->Branch("version",        &m_vs,          "m_vs/D");
  m_tJobLog->Branch("high_voltage",   &m_hv,          "m_hv/D");
  m_tJobLog->Branch("threshold",      &m_th,          "m_th/D");
  m_tJobLog->Branch("event_rate",     &m_er,          "m_er/D");
  m_tJobLog->Branch("input_tag",      &m_tag,         "m_tag/D");
  m_tJobLog->Branch("rec_mode",       &m_recMode,     "m_recMode/I");
  m_tJobLog->Branch("use_pad",        &m_usePad,      "m_usePad/I");
  m_tJobLog->Branch("eff_window",     &m_effWindow,   "m_effWindow/I");
  m_tJobLog->Branch("cluster_mode",   &m_clusterMode, "m_clusterMode/I");
  m_tJobLog->Branch("check_event",    &m_checkEvent,  "m_checkEvent/I");
  m_tJobLog->Branch("dimu_select",    &m_dimuSelect,  "m_dimuSelect/I");
  m_tJobLog->Branch("dimu_only",      &m_dimuOnly,    "m_dimuOnly/I");

  m_tJobLog->Branch("run_start",      &m_fStartRun,       "m_fStartRun/I");
  m_tJobLog->Branch("run_end",        &m_fEndRun,         "m_fEndRun/I");
  m_tJobLog->Branch("daq_time",       &m_fTotalDAQTime,   "m_fTotalDAQTime/D");
  m_tJobLog->Branch("job_time",       &m_fTotalJobTime,   "m_fTotalJobTime/D");
  m_tJobLog->Branch("calib_layer",    &m_fCalibLayerNum,  "m_fCalibLayerNum/D");
  m_tJobLog->Branch("calib_box",      &m_fCalibBoxNum,    "m_fCalibBoxNum/D");
  m_tJobLog->Branch("calib_strip",    &m_fCalibStripNum,  "m_fCalibStripNum/D");
  m_tJobLog->Branch("total_event",    &m_fTotalEvent,     "m_fTotalEvent/D");
  m_tJobLog->Branch("total_digi",     &m_fTotalDigi,      "m_fTotalDigi/D");
  m_tJobLog->Branch("total_exphit",   &m_fTotalExpHit,    "m_fTotalExpHit/D");
  m_tJobLog->Branch("total_effhit",   &m_fTotalEffHit,    "m_fTotalEffHit/D");
  m_tJobLog->Branch("total_noshit",   &m_fTotalNosHit,    "m_fTotalNosHit/D");
  m_tJobLog->Branch("total_cluster",  &m_fTotalClstNum,   "m_fTotalClstNum/D");
  m_tJobLog->Branch("total_strip_area",&m_fTotalStripArea,"m_fTotalStripArea/D");
  m_tJobLog->Branch("layer_coverage", &m_fLayerCoverage,  "m_fLayerCoverage/D");
  m_tJobLog->Branch("box_coverage",   &m_fBoxCoverage,    "m_fBoxCoverage/D");
  m_tJobLog->Branch("strip_coverage", &m_fStripCoverage,  "m_fStripCoverage/D");
    
  // statistic log tree
  m_tStatLog  = new TTree("StatLog","Statistic results");

  // layer constants tree, level 0 
  m_tLayConst = new TTree("LayConst","layer constants");
  m_tLayConst->Branch("layer_id",       &m_fLayerId,      "m_fLayerId/D");
  m_tLayConst->Branch("layer_eff",      &m_fLayerEff,     "m_fLayerEff/D");
  m_tLayConst->Branch("layer_efferr",   &m_fLayerEffErr,  "m_fLayerEffErr/D");
  m_tLayConst->Branch("layer_nosratio", &m_fLayerNosRatio,"m_fLayerNosRatio/D");
  m_tLayConst->Branch("layer_digi",     &m_fLayerDigi,    "m_fLayerDigi/D");
  m_tLayConst->Branch("layer_noise",    &m_fLayerNos,     "m_fLayerNos/D");
  m_tLayConst->Branch("layer_cnt",      &m_fLayerCnt,     "m_fLayerCnt/D");
  m_tLayConst->Branch("layer_exphit",   &m_fLayerExpHit,  "m_fLayerExpHit/D");
  m_tLayConst->Branch("layer_effHit",   &m_fLayerEffHit,  "m_fLayerEffHit/D");
  m_tLayConst->Branch("layer_nosHit",   &m_fLayerNosHit,  "m_fLayerNosHit/D");
  m_tLayConst->Branch("layer_cluster",  &m_fLayerCluster, "m_fLayerCluster/D");
  m_tLayConst->Branch("layer_trknum",   &m_fLayerTrkNum,  "m_fLayerTrkNum/D");
  
  // box constants tree, level 1 
  m_tBoxConst = new TTree("BoxConst","box constants");
  m_tBoxConst->Branch("box_id",       &m_fBoxId,      "m_fBoxId/D");
  m_tBoxConst->Branch("box_part",     &m_fBoxPart,    "m_fBoxPart/D");
  m_tBoxConst->Branch("box_segment",  &m_fBoxSegment, "m_fBoxSegment/D");
  m_tBoxConst->Branch("box_layer",    &m_fBoxLayer,   "m_fBoxLayer/D");
  m_tBoxConst->Branch("box_eff",      &m_fBoxEff,     "m_fBoxEff/D");
  m_tBoxConst->Branch("box_efferr",   &m_fBoxEffErr,  "m_fBoxEffErr/D");
  m_tBoxConst->Branch("box_nosratio", &m_fBoxNosRatio,"m_fBoxNosRatio/D");
  m_tBoxConst->Branch("box_digi",     &m_fBoxDigi,    "m_fBoxDigi/D");
  m_tBoxConst->Branch("box_noise",    &m_fBoxNos,     "m_fBoxNos/D");
  m_tBoxConst->Branch("box_cnt",      &m_fBoxCnt,     "m_fBoxCnt/D");
  m_tBoxConst->Branch("box_exphit",   &m_fBoxExpHit,  "m_fBoxExpHit/D");
  m_tBoxConst->Branch("box_effhit",   &m_fBoxEffHit,  "m_fBoxEffHit/D");
  m_tBoxConst->Branch("box_noshit",   &m_fBoxNosHit,  "m_fBoxNosHit/D");
  m_tBoxConst->Branch("box_cluster",  &m_fBoxCluster, "m_fBoxCluster/D");
  m_tBoxConst->Branch("box_trknum",   &m_fBoxTrkNum,  "m_fBoxTrkNum/D");
  
  // strip constants tree, level 2 
  m_tStrConst = new TTree("StrConst","strip constants");
  m_tStrConst->Branch("strip_id",       &m_fStripId,        "m_fStripId/D");
  m_tStrConst->Branch("strip_part",     &m_fStripPart,      "m_fStripPart/D");
  m_tStrConst->Branch("strip_segment",  &m_fStripSegment,   "m_fStripSegment/D");
  m_tStrConst->Branch("strip_layer",    &m_fStripLayer,     "m_fStripLayer/D");
  m_tStrConst->Branch("strip_eff",      &m_fStripEff,       "m_fStripEff/D");
  m_tStrConst->Branch("strip_efferr",   &m_fStripEffErr,    "m_fStripEffErr/D");
  m_tStrConst->Branch("strip_nosratio", &m_fStripNosRatio,  "m_fStripNosRatio/D");
  m_tStrConst->Branch("strip_digi",     &m_fStripDigi,      "m_fStripDigi/D");
  m_tStrConst->Branch("strip_noise",    &m_fStripNos,       "m_fStripNos/D");
  m_tStrConst->Branch("strip_cnt",      &m_fStripCnt,       "m_fStripCnt/D");
  m_tStrConst->Branch("strip_effhit",   &m_fStripEffHit,    "m_fStripEffHit/D");
  m_tStrConst->Branch("strip_exphit",   &m_fStripExpHit,    "m_fStripExpHit/D");
  m_tStrConst->Branch("strip_noshit",   &m_fStripNosHit,    "m_fStripNosHit/D");
  m_tStrConst->Branch("strip_trknum",   &m_fStripTrkNum,    "m_fStripTrkNum/D");

  return StatusCode::SUCCESS;
}

// Init area of units
StatusCode MucCalibMgr::InitArea()  
{
  int stripMax, boxID, stripID;
      stripMax = boxID = stripID = 0;
  double totalArea = 0;
  
  for( int i=0; i<PART_MAX; i++ )	
    for( int j=0; j<((i==BRID)?B_SEG_NUM:E_SEG_NUM); j++ )
      for( int k=0; k<((i==BRID)?B_LAY_NUM:E_LAY_NUM); k++ )
      {			
        MucBoxCal* aBox = new MucBoxCal( i, j, k );
        boxID = m_ptrIdTr->GetBoxId(i, j, k);
        m_boxResults[3][boxID] = aBox->GetArea();
        m_layerResults[3][k]  += aBox->GetArea();
        delete aBox;
        
        stripMax = m_ptrIdTr->GetStripMax( i, j, k );
        for( int m=0; m<stripMax; m++ )
        {
          MucStripCal* aStrip = new MucStripCal( i, j, k, m );
          stripID =m_ptrIdTr->GetStripId(i, j, k, m );
          m_stripResults[3][stripID] = aStrip->GetArea();
          totalArea +=  m_stripResults[3][stripID];
          delete aStrip;
        }
      }
  
  for( int i=0; i<LAYER_MAX; i++ ) m_hLayerArea->Fill(i, m_layerResults[3][i]);
  for( int i=0; i<BOX_MAX; i++ )   m_hBoxArea->Fill(i, m_boxResults[3][i]);
  for( int i=0; i<STRIP_MAX; i++ ) m_hStripArea->Fill(i, m_stripResults[3][i]);
  
  m_fTotalStripArea = totalArea;
  
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------- Member functions for executing --------------------------------
//-------------------------------------------------------------------------------------------------

// Dimu select
StatusCode MucCalibMgr::DimuSelect()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Dimu select" << endreq;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  
  if( m_tag > 0) { m_eventTag = (int)m_tag; return ( StatusCode::SUCCESS ); } 
   
  m_eventTag = 0;
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
  if(!eventHeader) {
    log << MSG::FATAL << "Could not find event header" << endreq;
    return( StatusCode::FAILURE );
  }
  
  // Select by MDC Info
  SmartDataPtr<RecMdcTrackCol> mdcTrackCol(eventSvc,"/Event/Recon/RecMdcTrackCol");
  if(!mdcTrackCol)  {
    log << MSG::FATAL << "Could not find mdc tracks" << endreq;
    return( StatusCode::FAILURE);
  }  
 log << MSG::INFO << mdcTrackCol->size() << endreq; 
 if( mdcTrackCol->size() != 2 ) return ( StatusCode::FAILURE );
 
 log << MSG::INFO << "r1:\t" << (*mdcTrackCol)[0]->r() << "\tz1:" << (*mdcTrackCol)[0]->z() << endreq;
 log << MSG::INFO << "r2:\t" << (*mdcTrackCol)[1]->r() << "\tz2:" << (*mdcTrackCol)[1]->z() << endreq;
 log << MSG::INFO << "p1:\t" << (*mdcTrackCol)[0]->p() << "\tp2:" << (*mdcTrackCol)[1]->p() << endreq;
 
 bool mdcFlag1 = (*mdcTrackCol)[0]->charge() + (*mdcTrackCol)[1]->charge() == 0;
 bool mdcFlag2 = fabs((*mdcTrackCol)[0]->r())<=1 && fabs((*mdcTrackCol)[0]->z())<3;
 bool mdcFlag3 = fabs((*mdcTrackCol)[1]->r())<=1 && fabs((*mdcTrackCol)[1]->z())<3;
 bool mdcFlag4 = (*mdcTrackCol)[0]->p()<2 && (*mdcTrackCol)[1]->p()<2;
 bool mdcFlag5 = fabs( (*mdcTrackCol)[0]->p() - (*mdcTrackCol)[1]->p() )/( (*mdcTrackCol)[0]->p() + (*mdcTrackCol)[1]->p() ) < 0.5;
 bool mdcPass = false;
 if( mdcFlag1 && mdcFlag2 && mdcFlag3 && mdcFlag4 && mdcFlag5 ) mdcPass = true;
  
  // Select by TOF Info
  SmartDataPtr<RecTofTrackCol> tofTrackCol(eventSvc,"/Event/Recon/RecTofTrackCol");
  if(!tofTrackCol)  {
    log << MSG::FATAL << "Could not find RecTofTrackCol!" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::INFO << "TOF tracks:\t" << tofTrackCol->size() << endreq;

  double t1, t2; 
  t1 = 0., t2 = 1000;
  // if(tofTrackCol->size() < 8 && tofTrackCol->size() > 20) 
  if(tofTrackCol->size() > 7 && tofTrackCol->size() < 21) 
  {
    int goodtof = 0;
    for(unsigned int itof = 0; itof < tofTrackCol->size(); itof++)
    {
      TofHitStatus *status = new TofHitStatus;  
      status->setStatus((*tofTrackCol)[itof]->status());

      if( !(status->is_cluster()) ) { delete status; continue; }      
      if(goodtof==0) t1 = (*tofTrackCol)[itof]->tof();
      if(goodtof==1) t2 = (*tofTrackCol)[itof]->tof();

      goodtof++;
      delete status;
    }
  }  
  bool tofPass = false;
  if( fabs( t1-t2 ) < 4) tofPass = true; // ns 
  
  // Select by EMC Info
  SmartDataPtr<RecEmcShowerCol> emcShowerCol(eventSvc,"/Event/Recon/RecEmcShowerCol");
  if (!emcShowerCol) { 
    log << MSG::FATAL << "Could not find RecEmcShowerCol!" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::INFO << "EMC showers:\t" << emcShowerCol->size() << endreq;

  if( emcShowerCol->size() < 2 ) return ( StatusCode::SUCCESS );

  double e1, e2, theta1, theta2, phi1, phi2;
  int part;

  e1      = (*emcShowerCol)[0]->energy();   e2      = (*emcShowerCol)[1]->energy();
  theta1  = (*emcShowerCol)[0]->theta();    theta2  = (*emcShowerCol)[1]->theta();
  phi1    = (*emcShowerCol)[0]->phi();      phi2    = (*emcShowerCol)[1]->phi();
  part    = (*emcShowerCol)[0]->module(); 

  log << MSG::INFO << "e1:\t" << e1 << "\te2:\t" << e2 << endreq;
  log << MSG::INFO << "theta1:\t" << theta1 << "\ttheta2:\t" << theta2 << endreq;  
  log << MSG::INFO << "phi1:\t" << phi1 << "\tphi2:\t" << phi2 << endreq;  

  bool emcFlag1 = e1 < 1.0 && e1 > 0.1;
  bool emcFlag2 = e2 < 1.0 && e2 > 0.1; 
  bool emcFlag3 = fabs(theta1 + theta2 - PI) < 0.05;
  bool emcFlag4 = fabs(phi1 - phi2) - PI < 0.4;

  bool emcPass = false;
  if( emcFlag1 && emcFlag2 && emcFlag3 && emcFlag4 ) emcPass = true; 
  
  // Select by MUC Info
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc,"/Event/Digi/MucDigiCol");
  if(!mucDigiCol)  {
    log << MSG::FATAL << "Could not find MUC digi" << endreq;
    return( StatusCode::FAILURE);
  }
  SmartDataPtr<RecMucTrackCol> mucTrackCol(eventSvc,"/Event/Recon/RecMucTrackCol");
  if (!mucTrackCol) {
    log << MSG::FATAL << "Could not find RecMucTrackCol" << endreq;
    return( StatusCode::FAILURE);
  }    
 log << MSG::INFO << "digi:\t" << mucDigiCol->size() << "tracks:\t" << mucTrackCol->size() << endreq;
 
 bool mucFlag1 = mucDigiCol->size()>6;
 bool mucFlag2 = mucTrackCol->size()>1;
 bool mucPass = false;
 if( mucFlag1 && mucFlag2 ) mucPass = true;
 
 if( mdcPass && tofPass && emcPass && mucPass ) m_eventTag = 1;
 else m_eventTag = (int)m_tag; 

  return( StatusCode::SUCCESS );
}

// Read event info from TDS
StatusCode MucCalibMgr::ReadEvent()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Read event" << endreq;
  
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  m_evtBegin = clock();
  
  // Check event head
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
  if(!eventHeader) {
    log << MSG::FATAL << "Could not find event header" << endreq;
    return( StatusCode::FAILURE );
  }
 
  m_currentRun    = eventHeader->runNumber();
  m_currentEvent  = eventHeader->eventNumber();  	
  if( m_fStartRun == 0 ) m_fStartRun = m_currentRun;
  m_fEndRun = m_currentRun;
  m_fTotalEvent++;
    
  log << MSG::INFO << "Run [ " << m_currentRun << " ]\tEvent [ " << m_currentEvent << " ]" << endreq;
  if( ((long)m_fTotalEvent)%2000 == 0 ) cout << m_fTotalEvent << "\tdone!" << endl;
  
  // Select dimu
  if( m_dimuSelect ) {
    MucCalibMgr::DimuSelect();
    log << MSG::INFO << "Event tag:\t" << m_eventTag << endreq;
    if( m_dimuOnly && m_eventTag != 1 ) return( StatusCode::FAILURE );
  }
 
  //---> Retrieve MUC digi
  log << MSG::INFO << "Retrieve digis" << endreq;
  
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc,"/Event/Digi/MucDigiCol");
  if(!mucDigiCol)  {
    log << MSG::FATAL << "Could not find MUC digi" << endreq;
    return( StatusCode::FAILURE);
  }
  
  int part, segment, layer, strip, pad;
      part = segment = layer = strip = pad = 0;
  double padX, padY, padZ;
      padX = padY = padZ = 0.;
  double resMax = 0.;
  
  Identifier mucId;
  MucDigiCol::iterator digiIter = mucDigiCol->begin();
  int eventDigi = 0;
  for ( int digiId =0; digiIter != mucDigiCol->end(); digiIter++, digiId++ )
  {
    mucId   = (*digiIter)->identify();
    part    = MucID::barrel_ec(mucId);
    segment = MucID::segment(mucId);
    layer   = MucID::layer(mucId);
    strip   = MucID::channel(mucId);
  
    log << MSG::DEBUG << "[" << part << "\t" << segment << "\t" << layer << "\t" << strip << "]\t" ;
    if( (digiId+1)%8 == 0 ) log << MSG::DEBUG << endreq;
    
    eventDigi    ++;
    
    if(abs(part)>=PART_MAX || abs(segment)>=SEGMENT_MAX || abs(layer)>=LAYER_MAX || abs(strip)>=STRIP_INBOX_MAX) {
      log << MSG::ERROR << endreq << "Digi IDs slop over!" << endreq;
      continue;
    }	
  
    // Add digi 
    MucMark *aMark = new MucMark( part, segment, layer, strip );
    m_digiCol.push_back( aMark );
    m_segDigiCol[part][segment].push_back( aMark );
  }
  log << MSG::DEBUG << endreq;
  log << MSG::INFO  << "Total digits of this event: " << eventDigi << endreq;
  if( eventDigi > 200) log << MSG::ERROR  << "Event: " << m_currentEvent << "\tdigits sharply rise:\t" << eventDigi << endreq;
  
  /*
  if( (long)m_fTotalEvent%10000 == 0 ) {
    log << MSG::INFO << "Delete HitVsEvent." << endreq;
    if(m_hHitVsEvent != NULL) delete m_hHitVsEvent;
    m_hHitVsEvent = new TH1F("HitVsEvent","Hit VS event",10000,m_fTotalEvent,m_fTotalEvent+10000);
    m_hHitVsEvent->GetXaxis()->SetTitle("Event NO.");
    log << MSG::INFO << "Recreate HitVsEvent." << endreq;
  } 
  log << MSG::INFO << "Fill HitVsEvent\t" << m_hHitVsEvent << "\t" << m_fTotalEvent << "\t" << m_currentEvent << endreq;
  //m_hHitVsEvent->Fill(m_currentEvent+m_currentEvent%10000, eventDigi);
  m_hHitVsEvent->Fill(m_fTotalEvent, eventDigi);
  log << MSG::INFO << "Fill HitVsEvent done." << endreq;
  */  
  
  // Search cluster in digis
  // Method detail in MucMark class
  int clusterNum, bigClusterNum, clusterSize;
      clusterNum = bigClusterNum = clusterSize = 0;
  if( m_clusterMode ) {
    log << MSG::INFO << "Searching clusters" << endreq;
    m_clusterCol = (*m_ptrMucMark).CreateClusterCol(m_clusterMode, m_digiCol );
  }
  
  for( unsigned int i=0; i<m_clusterCol.size(); i++ )
  {
    clusterSize = m_clusterCol[i].size();
    // real cluster, size >= 2
    if( clusterSize > CLUSTER_ALARM )
    {
      log << MSG::WARNING << "Big cluster:" << endreq;
      part    = (*m_clusterCol[i][0]).Part();
      segment = (*m_clusterCol[i][0]).Segment();
      layer   = (*m_clusterCol[i][0]).Layer();      
      
      if( m_clusterSave ) (*m_fdata) << "Event:\t" << m_currentEvent << "\tbig cluster " << bigClusterNum << endl;
      
      for( int j=0; j<clusterSize; j++ )
      {
        strip = (*m_clusterCol[i][j]).Strip();
        log << MSG::WARNING << "[" << part << "\t" << segment << "\t" << layer << "\t" << strip << "]\t";
        if( (j+1)%8 == 0 ) log << MSG::WARNING << endreq;
        if( m_clusterSave ) (*m_fdata) << part << "\t" << segment << "\t" << layer << "\t" << strip << endl;
      }
      log << MSG::WARNING << endreq;
      bigClusterNum ++;
    }
    else if( clusterSize > 1 )
    {
      log << MSG::DEBUG << "cluster: " << clusterNum << endreq;
      clusterNum ++, m_fTotalClstNum ++;
      part    = (*m_clusterCol[i][0]).Part();
      segment = (*m_clusterCol[i][0]).Segment();
      layer   = (*m_clusterCol[i][0]).Layer();
      for( int j=0; j<clusterSize; j++ )
      {        
        strip = (*m_clusterCol[i][j]).Strip();
        log << MSG::DEBUG << "[" << part << "\t" << segment << "\t" << layer << "\t" << strip << "]\t";
        if( (j+1)%8 == 0 ) log << MSG::DEBUG << endreq;
      }
      log << MSG::DEBUG << endreq;
    }
  } // End m_clusterCol.size()
  
  if( m_clusterMode) log << MSG::INFO << "Total clusters in this event: " << clusterNum << endreq;
  else                    log << MSG::INFO << "Clusters not built" << endreq;
  //<--- End retrieve digis
  
  //---> Retrieve rec tracks
  log << MSG::INFO << "Retrieve tracks" << endreq;
  // MDC tracks 
  SmartDataPtr<RecMdcTrackCol> mdcTrackCol(eventSvc,"/Event/Recon/RecMdcTrackCol");
  if(!mdcTrackCol)  {
    log << MSG::FATAL << "Could not find mdc tracks" << endreq;
    return( StatusCode::FAILURE);
  }  
  
  RecMdcTrackCol::iterator mdctrkIter = mdcTrackCol->begin();
  for (; mdctrkIter != mdcTrackCol->end(); mdctrkIter++)
  {
    m_charge = (*mdctrkIter)->charge();
    m_mdcpx = (*mdctrkIter)->px();
    m_mdcpy = (*mdctrkIter)->py();
    m_mdcpz = (*mdctrkIter)->pz();
    m_mdcpt = (*mdctrkIter)->pxy();
    m_mdcpp = (*mdctrkIter)->p();
    m_mdcphi = (*mdctrkIter)->phi();
    m_mdctheta = (*mdctrkIter)->theta();
    m_mdcTrkInfoTuple->write();   
  }
 
  // MUC tracks
  SmartDataPtr<RecMucTrackCol> mucTrackCol(eventSvc,"/Event/Recon/RecMucTrackCol");
  if (!mucTrackCol) {
    log << MSG::FATAL << "Could not find RecMucTrackCol" << endreq;
    return( StatusCode::FAILURE);
  }
  
  RecMucTrackCol  *aRecMucTrackCol = mucTrackCol;
  if (aRecMucTrackCol->size() < 1) {
    log << MSG::INFO << "No MUC tracks in this event" << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::INFO << "Total tracks of this event: " << aRecMucTrackCol->size() << endreq;
  
  // Get RecEsTimeCol
  //int esTimeflag;
  //if( m_recMode == 0 ) // only for ExtTrk
  if( 0 ) // only for ExtTrk
  {
    SmartDataPtr<RecEsTimeCol> aRecEsTimeCol(eventSvc,"/Event/Recon/RecEsTimeCol");
    if( ! aRecEsTimeCol ){
      log << MSG::ERROR << "Could not find RecEsTimeCol" << endreq;
      return StatusCode::FAILURE;    
    }else{
      RecEsTimeCol::iterator iter_evt = aRecEsTimeCol->begin();
      //tes = (*iter_evt)->getTest();
      //esTimeflag = (*iter_evt)->getStat();
      m_ntEsTime = (*iter_evt)->getStat();
      if( (*iter_evt)->getStat() != 211 ) {
        log << MSG::WARNING << "Event time not by TOF, skip!" << endreq;
        return StatusCode::SUCCESS;
      }  
    }  
  }
  
  // Phi diff of two tracks( dimuon event )
  double phi1, phi2, phiDiff, theta1, theta2, thetaDiff;
  phiDiff = thetaDiff = 0.;
  if( aRecMucTrackCol->size()==2 && (*aRecMucTrackCol)[0]->GetTotalHits() > 4 && (*aRecMucTrackCol)[1]->GetTotalHits() > 4 )
  {
    // Pos phi diff
    phi1 = (*aRecMucTrackCol)[0]->getMucPos().phi();    phi2 = (*aRecMucTrackCol)[1]->getMucPos().phi();
    if( phi1 > 0 )  phiDiff = phi1 - phi2 - PI;
    else            phiDiff = phi2 - phi1 - PI;

    // Pos theta diff    
    theta1 = (*aRecMucTrackCol)[0]->getMucPos().theta();  theta2 = (*aRecMucTrackCol)[1]->getMucPos().theta();    
    thetaDiff = theta1 + theta2 - PI;
    m_hTrackPosPhiDiff->Fill( phiDiff );
    m_hTrackPosThetaDiff->Fill( thetaDiff );
    m_hDimuTracksPosDiff->Fill( thetaDiff, phiDiff );
    m_ntPosPhiDiff    = phiDiff;
    m_ntPosThetaDiff  = thetaDiff;
    
    log << MSG::INFO << "PosPhiDiff:\t" << phiDiff << "\tPosThetaDiff:\t" << thetaDiff << endreq;

    // Mom phi diff
    phi1 = (*aRecMucTrackCol)[0]->getMucMomentum().phi(); phi2 = (*aRecMucTrackCol)[1]->getMucMomentum().phi();
    if( phi1 > 0 )  phiDiff = phi1 - phi2 - PI;
    else            phiDiff = phi2 - phi1 - PI;

    // Mom theta diff
    theta1 = (*aRecMucTrackCol)[0]->getMucMomentum().theta(); theta2 = (*aRecMucTrackCol)[1]->getMucMomentum().theta();
    thetaDiff = theta1 + theta2 - PI;

    m_hTrackMomPhiDiff->Fill( phiDiff );
    m_hTrackMomThetaDiff->Fill( thetaDiff );
    m_hDimuTracksMomDiff->Fill( thetaDiff, phiDiff );
    m_ntMomPhiDiff    = phiDiff;
    m_ntMomThetaDiff  = thetaDiff;
    
    log << MSG::INFO << "MomPhiDiff:\t" << phiDiff << "\tMomThetaDiff:\t" << thetaDiff << endreq;   
    m_ntDimuTag = m_eventTag;
    m_trackDiffTuple->write();
  }

  // Retrieve tracks for calibration
  RecMucTrackCol::iterator trackIter = mucTrackCol->begin();
  int  trackHitNum, rawHitNum, expectedHitNum, segNum, trkRecMode, lastLayerBR, lastLayerEC;
  int  layerPassNum[3], passMax[TRACK_SEG_MAX][2];
  bool firedLay[TRACK_SEG_MAX][LAYER_MAX];  
  bool seedList[PART_MAX][LAYER_MAX];
  trackHitNum = rawHitNum = expectedHitNum = segNum = trkRecMode = lastLayerBR = lastLayerEC = 0;
  layerPassNum[0] = layerPassNum[1] = layerPassNum[2] = 0;
  for( int segi=0; segi<TRACK_SEG_MAX; segi++ ) {
    passMax[segi][0] = passMax[segi][1] = 0;
    for( int layi=0; layi<LAYER_MAX; layi++ ) firedLay[segi][layi] = 0;
  }
  
  mark_col trkSeg[TRACK_SEG_MAX];
  vector<MucRecHit*> mucRawHitCol;
  vector<MucRecHit*> mucExpHitCol;
  
  for (int trackId = 0; trackIter != mucTrackCol->end(); trackIter++, trackId++)
  {
    trackHitNum = (*trackIter)->GetTotalHits();
    log << MSG::DEBUG << "Track: " << trackId << " Hits: " << trackHitNum << endreq;
  
    if( trackHitNum == 0 ) { 
      log << MSG::INFO << "Track " << trackId << " no hits" << endreq;    
      continue;
    }
  
    m_ntTrackHits     = trackHitNum;
    
    m_trkRecMode = trkRecMode = (*trackIter)->GetRecMode();
    m_chi2 = (*trackIter)->chi2();
    m_px = (*trackIter)->getMucMomentum().x();
    m_py = (*trackIter)->getMucMomentum().y();
    m_pz = (*trackIter)->getMucMomentum().z();
    m_pt = sqrt(m_px*m_px + m_py*m_py);
    m_pp = sqrt(m_px*m_px + m_py*m_py + m_pz*m_pz);
    
    // First fit position in MUC
    m_r  = (*trackIter)->getMucPos().mag();
    m_cosTheta = (*trackIter)->getMucPos().cosTheta();
    m_theta    = (*trackIter)->getMucPos().theta();
    m_phi      = (*trackIter)->getMucPos().phi();
    m_depth    = (*trackIter)->depth();
    m_brLastLayer = lastLayerBR = (*trackIter)->brLastLayer();
    m_ecLastLayer = lastLayerEC = (*trackIter)->ecLastLayer();
    m_totalHits   = (*trackIter)->numHits();
    m_totalLayers = (*trackIter)->numLayers();
    m_maxHitsInLayer = (*trackIter)->maxHitsInLayer();
    
    
    m_hPhiCosTheta->Fill(m_cosTheta, m_phi);
    log << MSG::INFO << "Fill track info" << endreq;
    
    MucRecHit* pMucRawHit;
    MucRecHit* pMucExpHit;
    if( m_calHitCol.size() != 0 ) m_calHitCol.clear(); // Fresh each track
  
    // Digis belong to this rec track
    log << MSG::DEBUG << "Reconstruction hits(digis in a track): " << endreq; 
    mucRawHitCol = (*trackIter)->GetHits(); // Get hit collection of a track
    rawHitNum += mucRawHitCol.size(); 
  
    segNum = 0;
    if( trkRecMode == 3 ) { // By SlfTrk
      for(int iPart=0; iPart<PART_MAX; iPart++)
        for(int iLayer=0; iLayer<LAYER_MAX; iLayer++) seedList[iPart][iLayer] = false;
    }
    
    for(unsigned int hitId = 0; hitId < mucRawHitCol.size(); hitId++)
    {
      pMucRawHit  = mucRawHitCol[ hitId ];
      part        = pMucRawHit->Part();
      segment     = pMucRawHit->Seg();
      layer       = pMucRawHit->Gap();
      strip       = pMucRawHit->Strip();
  
      log << MSG::DEBUG << "[" << part << "\t" << segment << "\t" << layer << "\t" << strip << "]\t";
      //if( (hitId+1)%8 == 0 ) log << MSG::DEBUG << endreq;	

      // Add hit
      MucMark *aMark = new MucMark( part, segment, layer, strip );
      m_calHitCol.push_back( aMark );

      // Set seed flag
      if( trkRecMode == 3 ) seedList[part][layer] = pMucRawHit->HitIsSeed(); // By SlfTrk
  	    
      // Find track segment
      if(hitId == 0) { trkSeg[segNum].push_back( aMark ); segNum ++; }
      else 
      {
        log << MSG::DEBUG << "segNum: " << segNum << endreq;
        bool notInSeg = true;
        for( int segi=0; segi<segNum; segi++ )
        {
          if( aMark->IsInSegWith( *(trkSeg[segi][0]) ) ) 
          {
            trkSeg[segi].push_back( aMark );
            notInSeg = false;
            break;
          }
        }
        // new track seg
        if( notInSeg == true ) 
        { 
          trkSeg[segNum].push_back( aMark ); 
          segNum ++; 
          if( segNum > TRACK_SEG_MAX ) {
            log << MSG::ERROR << "Track segment overflow: " << segNum << endreq;
            break;
          }
        }
      } // End else	
    } // End raw hits
    log << MSG::DEBUG << endreq;
  
    // Find maximal layers passed in track segments 
    layerPassNum[0] = layerPassNum[1] = layerPassNum[2] = 0;
    for( int segi=0; segi<segNum; segi++ )
    {      
      int tmpLayNum = 0; 
      passMax[segi][0] = passMax[segi][1] = trkSeg[segi][0]->Layer();
      for( unsigned int hiti=1; hiti<trkSeg[segi].size(); hiti++ )
      {
        if( trkSeg[segi][hiti]->Layer() < passMax[segi][0] )
          passMax[segi][0] = trkSeg[segi][hiti]->Layer();
        if( trkSeg[segi][hiti]->Layer() > passMax[segi][1] )
          passMax[segi][1] = trkSeg[segi][hiti]->Layer();        
        firedLay[segi][trkSeg[segi][hiti]->Layer()] = 1;
      }

      for( int layi=0; layi<LAYER_MAX; layi++ ) {
        if( firedLay[segi][layi] ) tmpLayNum ++;
      }
      
      if( segi == 0 ) layerPassNum[0] += passMax[segi][1] + 1;
      else            layerPassNum[0] += (passMax[segi][1] - passMax[segi][0] + 1);
        
      layerPassNum[1] += (passMax[segi][1] - passMax[segi][0] + 1);
      layerPassNum[2] += tmpLayNum; 
      
      trkSeg[segi].clear();
    }
    m_ntTrackEvent  = m_currentEvent;
    m_ntTrackTag    = m_eventTag;
    m_ntTrackSegFly = segNum;
    m_ntTrackLayFlyA = layerPassNum[0];
    m_ntTrackLayFlyB = layerPassNum[1];
    m_ntTrackLayFlyC = layerPassNum[2];
    m_trackInfoTuple->write();
    log << MSG::INFO << "Track\t" << trackId << "\tsegment(s):\t" << segNum 
        << "\tlayer passed:\t" << layerPassNum[0] <<"\t" << layerPassNum[1] << "\t" << layerPassNum[2] << endreq;
    //if( layerPassNum[0]>B_LAY_NUM || layerPassNum[1]>B_LAY_NUM || layerPassNum[2]>B_LAY_NUM ) 
    //  log << MSG::ERROR << "Over max layer:\t" << m_currentRun << "\t" << m_currentEvent << endreq; 
        
    // Expected hits in this rec track
    log << MSG::DEBUG << "Fitting hits(expected hits in a track): " << endreq; 
    mucExpHitCol = (*trackIter)->GetExpectedHits(); 
    expectedHitNum += mucExpHitCol.size();	
    for(unsigned int hitId = 0; hitId < mucExpHitCol.size(); hitId++)
    {
      pMucRawHit  = mucExpHitCol[ hitId ];
      part        = pMucRawHit->Part();           segment     = pMucRawHit->Seg();
      layer       = pMucRawHit->Gap();            strip       = pMucRawHit->Strip();
      
      if( m_usePad != 0 ) 
      {
        pad         = pMucRawHit->GetPadID();       padZ        = pMucRawHit->GetIntersectZ();
        padX        = pMucRawHit->GetIntersectY();  padY        = pMucRawHit->GetIntersectX(); // Note: local coordinate 
      
        if( part != BRID ) 
        {
          if(segment == 1)      { padX = -padX; }
          else if(segment == 2) { padX = -padX, padY = -padY; }
          else if(segment == 3) { padY = -padY; }
        } 
      }
      
      // Avoid bias in seed layers 
      // if( seedList[part][layer] == true ) continue;

      MucMark* currentMark = new MucMark( part, segment, layer, strip );
      m_expHitCol.push_back( currentMark );
      //log << MSG::DEBUG << "[" << part << "\t" << segment << "\t" << layer << "\t" << strip << "]\t";
      //if( (hitId+1)%8 == 0 ) log << MSG::DEBUG << endreq;
  		
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
          m_record[part][segment][layer][strip][2] ++; // Efficiency hit number
          m_record[part][segment][layer][strip][1] ++; // Rec track number
          m_effHitCol.push_back( m_segDigiCol[part][segment][isInPos] );
          
          if( m_usePad != 0 ) {
            m_h2DExpMap[part][segment][layer]->Fill(padX, padY);
            m_h2DHitMap[part][segment][layer]->Fill(padX, padY);
          }
          
        }
        else {
          m_record[part][segment][layer][strip][1] ++;
          if( m_usePad != 0 ) m_h2DExpMap[part][segment][layer]->Fill(padX, padY);
        }
        continue; // Judge next hit
      }

      // Eff calibration
      if( isInPos != -1 ) // expHit is fired
      {
        m_record[part][segment][layer][strip][2] ++; // Efficiency hit number
        m_record[part][segment][layer][strip][1] ++; // Rec track number
        m_effHitCol.push_back( m_segDigiCol[part][segment][isInPos] );

        if( m_usePad != 0 ) {
          m_h2DExpMap[part][segment][layer]->Fill(padX, padY);
          m_h2DHitMap[part][segment][layer]->Fill(padX, padY);
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
          m_record[part][segment][layer][tempStrip][2] ++; // Efficiency hit number
          m_record[part][segment][layer][tempStrip][1] ++; // Rec track number
          m_effHitCol.push_back( m_segDigiCol[part][segment][isInPos] );

          if( m_usePad != 0 ) {
            m_h2DExpMap[part][segment][layer]->Fill(padX, padY);
            m_h2DHitMap[part][segment][layer]->Fill(padX, padY);
          }
          
          m_ntEffWindow = hiti;
          m_effWindowTuple->write();
          isInEffWindow = true;
        }			
                
      } // End else 
  
      if( isInEffWindow ) { continue; } // Judge next hit
      else { // A hit should be fired but not fired and not in the EffWindow
        m_record[part][segment][layer][strip][1] ++; // Rec track number
        if( m_usePad != 0 ) m_h2DExpMap[part][segment][layer]->Fill(padX, padY);
      }

    } // End expected hits
    
    // Fill residual, and for the other way of eff calculation
    log << MSG::INFO << "Fill residual" << endreq;
    vector<float> m_lineResCol = (*trackIter)->getDistHits();
    vector<float> m_quadResCol = (*trackIter)->getQuadDistHits();
    vector<float> m_extrResCol = (*trackIter)->getExtDistHits();
    int mode = (*trackIter)->GetRecMode();
    
    for(unsigned int nres = 0; nres < m_lineResCol.size(); nres++ )
      if( fabs(m_lineResCol[nres])>resMax ) resMax = fabs(m_lineResCol[nres]);
    
    log << MSG::INFO << "Good track for res" << endreq;
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
      
      log << MSG::INFO << "Fit res" << endreq;
      for(unsigned int hitId = 0; hitId < mucRawHitCol.size(); hitId++)
      {
        pMucRawHit  = mucRawHitCol[ hitId ];
        part = pMucRawHit->Part(); segment = pMucRawHit->Seg(); layer = pMucRawHit->Gap();
        
        if( part == BRID ) m_hBarrelResDist[layer]->Fill( m_lineResCol[hitId] );        
        else               m_hEndcapResDist[layer]->Fill( m_lineResCol[hitId] );             
        
        // if exp is true and fired is true, and not filled yet
        if( firedFlag[part][layer][0] == true && firedFlag[part][layer][1] == false ) 
        {
          m_resPart     = part; 
          m_resSegment  = segment; 
          m_resLayer    = layer;
          m_lineRes     = m_lineResCol[hitId];
          m_quadRes     = m_quadResCol[hitId];
          m_extrRes     = m_extrResCol[hitId];
          m_resFired    = 1;
          m_resMode     = mode;
          m_resInfoTuple->write();
        }
        
        firedFlag[part][layer][1] = true;
      }
      
      log << MSG::INFO << "Exp res" << endreq;
      for(unsigned int hitId = 0; hitId < mucExpHitCol.size(); hitId++)
      {  
        pMucExpHit  = mucExpHitCol[ hitId ];
        part = pMucExpHit->Part(); segment = pMucExpHit->Seg(); layer = pMucExpHit->Gap();  
        
        if(firedFlag[part][layer][0] == true && firedFlag[part][layer][1] == false)
        {
          m_resPart     = part;                       
          m_resSegment  = segment;                       
          m_resLayer    = layer;
          m_lineRes     = 1000;
          m_quadRes     = 1000;
          m_extrRes     = 1000;
          m_resFired    = 0;
          m_resMode     = mode;
          m_resInfoTuple->write();
        }          
      }
      
    } // End fill residual, if track is good for res
    
    mucRawHitCol.clear();
    mucExpHitCol.clear();		

  } // End read all tracks

  if( resMax > 300 ) cout <<"Res too big!\t"<< m_fTotalEvent <<"\t"<< m_currentRun <<"\t"<< m_currentEvent <<"\t"<< resMax << endl;
  
  m_ntTrackNum = mucTrackCol->size();
  
  m_fTotalEffHit += rawHitNum;	
  log << MSG::INFO << "Total hits in this event, raw: " << rawHitNum << "\texpected: " << expectedHitNum << endreq;
  //<--- End retrieve rec tracks
  
  //---> Searching inc/noise hits  
  log << MSG::INFO << "Searching inc/noise hits" << endreq;
  bool isNosHit;
  bool hasEffHit;
  for( unsigned int i=0; i < m_digiCol.size(); i++ )
  {
    isNosHit = true;
    
    if( m_digiCol[i]->IsInCol( m_effHitCol ) !=-1) continue; // digi in effHitCol
    else
    {
      for( unsigned int j=0; j < m_clusterCol.size(); j++ )
      {
        hasEffHit = false;
        for( unsigned int k=0; k<m_clusterCol[j].size(); k++)
        {
          if( m_clusterCol[j][k]->IsInCol(m_effHitCol) != -1) // Clusters have efficiency hit
          {
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
        m_nosHitCol.push_back( m_digiCol[i] );
        m_fTotalNosHit ++;
      }
    }// End else
  } // End digi collection
  
  return StatusCode::SUCCESS;  
}

// Check event
StatusCode MucCalibMgr::CheckEvent()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Check event" << endreq;
  
  // Find over digis in digis
  // Note that only one digi relates to one strip
  log << MSG::INFO << "Searching over digi" << endreq;
  int overDigi = 0;
  for( unsigned int i=0; i < m_digiCol.size(); i++ )
    for( unsigned int j=i+1; j < m_digiCol.size(); j++ ) {
      if( (*m_digiCol[i]) == (*m_digiCol[j]) ) overDigi ++;
    }
  
  if( overDigi !=0  )
  log << MSG::ERROR << overDigi << " over digi found in digis!" << endreq;
  
  // Find over hits in reconstruction hits
  // Note that two tracks may pass thought one strip
  log << MSG::INFO << "Searching over hit" << endreq;
  int overHit = 0;
  for( unsigned int i=0; i < m_expHitCol.size(); i++ )
    for( unsigned int j=i+1; j < m_expHitCol.size(); j++ ) {
      if( (*m_expHitCol[i]) == (*m_expHitCol[j]) ) overHit ++;
    }
  
  if( overHit != 0 )
  log << MSG::WARNING << overHit << " over hits found in rec hits!" << endreq;  
  
  // Find hits of reconstruction not in MUCdigis
  log << MSG::INFO << "Searching new hit" << endreq;
  int newHit = 0;
  int num	   = 0;
  for( unsigned int i=0; i < m_expHitCol.size(); i++ )
  {
    num = m_expHitCol[i]->NumInCol( m_digiCol );
    if( num == 0 )
    {
      log << MSG::ERROR << "Exp hit not in digis!"
          << "prt: "    << (*m_expHitCol[i]).Part()
          << "\tseg: "  << (*m_expHitCol[i]).Segment()
          << "\tlay: "  << (*m_expHitCol[i]).Layer()
          << "\tstr: "  << (*m_expHitCol[i]).Strip()	<< endreq;
      
      newHit ++;
    }
  }
  
  if( newHit != 0 )
  log << MSG::WARNING << newHit << " new hit(s) found in rec hits!" << endreq;
  
  return StatusCode::SUCCESS;
}

// Fill event
StatusCode MucCalibMgr::FillEvent()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Fill event" << endreq;
  
  int part, segment, layer, strip, size;
      part = segment = layer = strip = size = 0;    
    
  // Fill digis
  log << MSG::INFO << "Fill digis" << endreq;
  for( unsigned int i=0; i<m_digiCol.size(); i++ )
  {
    part    = (*m_digiCol[i]).Part();
    segment = (*m_digiCol[i]).Segment();
    layer   = (*m_digiCol[i]).Layer();
    strip   = (*m_digiCol[i]).Strip();
    
    FillDigi( part, segment, layer, strip );
    m_record[part][segment][layer][strip][0] ++;
    m_fTotalDigi ++;
  }
  
  // Fill rec hits
  log << MSG::INFO << "Fill rec hits" << endreq;
  for( unsigned int i=0; i<m_expHitCol.size(); i++ )
  {
    part    = (*m_expHitCol[i]).Part();
    segment = (*m_expHitCol[i]).Segment();
    layer   = (*m_expHitCol[i]).Layer();
    strip   = (*m_expHitCol[i]).Strip();
  
    FillExpHit( part, segment, layer, strip );
    m_record[part][segment][layer][strip][4] ++;
    m_fTotalExpHit ++;
  }
  
  // Fill eff hits
  log << MSG::INFO << "Fill eff hits" << endreq;
  for( unsigned int i=0; i<m_effHitCol.size(); i++ )
  {
    part    = (*m_effHitCol[i]).Part();
    segment = (*m_effHitCol[i]).Segment();
    layer   = (*m_effHitCol[i]).Layer();
    strip   = (*m_effHitCol[i]).Strip();
    
    FillEffHit( part, segment, layer, strip );
    m_fTotalEffHit ++;
  }	
  
  // Fill clusters
  log << MSG::INFO << "Fill clusters" << endreq;
  for( unsigned int i=0; i<m_clusterCol.size(); i++ )
  {	
    size = m_clusterCol[i].size();
    // may include the special cluster, size = 1
    if( size > CLUSTER_CUT )
    {
      part    = (*m_clusterCol[i][0]).Part();
      segment = (*m_clusterCol[i][0]).Segment();
      layer   = (*m_clusterCol[i][0]).Layer();
      
      FillCluster( part, segment, layer, size );
      m_ntClusterSize = size;
      m_clusterSizeTuple->write();
    }
  }
 
  // Fill inc/noise hits
  log << MSG::INFO << "Fill inc/noise hits" << endreq;
  for( unsigned int i=0; i<m_nosHitCol.size(); i++ )
  {
    part    = (*m_nosHitCol[i]).Part();
    segment = (*m_nosHitCol[i]).Segment();
    layer   = (*m_nosHitCol[i]).Layer();
    strip   = (*m_nosHitCol[i]).Strip();
    
    FillNosHit( part, segment, layer, strip );
    m_record[part][segment][layer][strip][3] ++;
  }
  
  // Event log	
  m_ntEventId     = m_currentEvent;	
  m_ntEventTag    = m_eventTag;	
  m_ntDigiNum     = m_digiCol.size();
  m_ntExpHitNum   = m_expHitCol.size();
  m_ntEffHitNum   = m_effHitCol.size();
  m_ntNosHitNum   = m_nosHitCol.size();
  m_ntClusterNum  = m_clusterCol.size();
 
  // Reset mark collection
  for( unsigned int i=0; i<m_digiCol.size(); i++ ) {
    if( m_digiCol[i] != NULL )    delete m_digiCol[i];
  }
  
  for( unsigned int i=0; i<m_expHitCol.size(); i++ ) {
    if( m_expHitCol[i] != NULL )  delete m_expHitCol[i];
  }
/*  
  for( unsigned int i=0; i<m_effHitCol.size(); i++ ) {
    if( m_effHitCol[i] != NULL )  delete m_effHitCol[i];
  }
  log << MSG::INFO << m_effHitCol.size() << endreq;
*/    
  for( unsigned int i=0; i<m_calHitCol.size(); i++ ) {
    if( m_calHitCol[i] != NULL )  delete m_calHitCol[i];
  }
  
  if( m_digiCol.size() != 0 ) m_digiCol.clear();
  if( m_expHitCol.size() != 0 ) m_expHitCol.clear();
  if( m_calHitCol.size() != 0 ) m_calHitCol.clear();
  if( m_effHitCol.size() != 0 ) m_effHitCol.clear();
  if( m_nosHitCol.size() != 0 ) m_nosHitCol.clear();
  if( m_clusterCol.size() != 0 ) m_clusterCol.clear();	
 
  
  for( int i=0; i<PART_MAX; i++ ) {
    for( int j=0; j<SEGMENT_MAX; j++ ) {
      if( m_segDigiCol[i][j].size() != 0 )  m_segDigiCol[i][j].clear();
    } 
  }
  
  m_evtEnd = clock();
  
  m_ntEventTime = (double(m_evtEnd - m_evtBegin))/CLOCKS_PER_SEC;
  log << MSG::INFO << "Event time:\t" << m_ntEventTime << "s" << endreq;
  m_eventLogTuple->write();
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::FillDigi( int part, int segment, int layer, int strip )
{	
  // Hit map for online check
  int tmpId;
  
  if( (int)m_tag || m_dimuOnly==0 || (m_dimuOnly==1&&m_eventTag==1) )
  { 
    if( part == BRID )
    {
      // According to layer
      if( layer%2 == 0 ) {	
        if( segment > 4 )     tmpId = segment*48 + (47 - strip);
        else                  tmpId = segment*48 + strip;			
      }
      else if( segment < 3 )  tmpId = segment*96 + strip;
      else                    tmpId = (segment-1)*96 + 112 + strip;
      
      m_hHitMapBarrel_Lay[layer]->Fill(tmpId);
      
      // According to segment
      if( segment != B_TOP ) { 
        if( segment > 4 ) 
              tmpId = 48*((layer+1)/2) + 96*(layer/2) + ( ((layer+1)%2)*48 + (layer%2)*96 - strip - 1 );  
        else  tmpId = 48*((layer+1)/2) + 96*(layer/2) + strip;
       }
      else    tmpId = 48*((layer+1)/2) + 112*(layer/2) + strip;
      
      m_hHitMapBarrel_Seg[segment]->Fill(tmpId);
    }
    else if( part == EEID )
    {
      // According to layer
      tmpId = segment*64 + strip;
      m_hHitMapEndcap_Lay[0][layer]->Fill(tmpId);
      // According to segment
      tmpId = layer*64 + strip;
      m_hHitMapEndcap_Seg[0][segment]->Fill(tmpId);
    }
    else if( part == EWID )
    {
      // According to layer
      tmpId = segment*64 + strip;
      m_hHitMapEndcap_Lay[1][layer]->Fill(tmpId);		
      // According to segment
      tmpId = layer*64 + strip;
      m_hHitMapEndcap_Seg[1][segment]->Fill(tmpId);
    }	
  }
  
  // Total units
  int boxId = m_ptrIdTr->GetBoxId( part, segment, layer );
  int strId = m_ptrIdTr->GetStripId( part, segment, layer, strip );
  
  m_hStripFireMap[boxId]->Fill( strip );
  m_hStripFire->Fill( strId );
  m_hBoxFire->Fill( boxId );
  m_hLayerFire->Fill( layer );
  if( part==BRID )       m_hBrLayerFire->Fill( layer );
  else if( part== EEID ) m_hEcLayerFire->Fill( layer+1 );
  else                   m_hEcLayerFire->Fill( -(layer+1) );
  
  return StatusCode::SUCCESS;	
}

StatusCode MucCalibMgr::FillExpHit( int part, int segment, int layer, int strip )
{
  int boxId = m_ptrIdTr->GetBoxId( part, segment, layer );
  int strId = m_ptrIdTr->GetStripId( part, segment, layer, strip );
  
  m_hStripExpHitMap[boxId]->Fill( strip );
  m_hStripExpHit->Fill( strId );
  m_hBoxExpHit->Fill( boxId );
  m_hLayerExpHit->Fill( layer );
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::FillEffHit( int part, int segment, int layer, int strip )
{
  int boxId = m_ptrIdTr->GetBoxId( part, segment, layer );
  int strId = m_ptrIdTr->GetStripId( part, segment, layer, strip );
  
  m_hStripEffHitMap[boxId]->Fill( strip );
  m_hStripEffHit->Fill( strId );
  m_hBoxEffHit->Fill( boxId );
  m_hLayerEffHit->Fill( layer );
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::FillNosHit( int part, int segment, int layer, int strip )
{
  int boxId = m_ptrIdTr->GetBoxId( part, segment, layer );
  int strId = m_ptrIdTr->GetStripId( part, segment, layer, strip );
  
  m_hStripNosHitMap[boxId]->Fill( strip );
  m_hStripNosHit->Fill( strId );
  m_hBoxNosHit->Fill( boxId );
  m_hLayerNosHit->Fill( layer );
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::FillCluster(  int part, int segment, int layer, int size )
{
  int boxId = m_ptrIdTr->GetBoxId( part, segment, layer );	
  
  m_hLayerCluster[layer]->Fill( size );
  m_hBoxCluster[boxId]->Fill( size );
  
  return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------- Member functions for finalizing --------------------------------
//-------------------------------------------------------------------------------------------------

StatusCode MucCalibMgr::AnalyseEffAndNoise()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log<< MSG::INFO << "Start efficiency and noise calibration" << endreq;
  
  // Set time
  m_fTotalDAQTime = m_fTotalEvent/m_er;
  log<< MSG::INFO << "Total run time:\t" << m_fTotalDAQTime << endreq;
  
  EffAndNoiseLV2();
  EffAndNoiseLV1();
  EffAndNoiseLV0();
  
  if( m_usePad != 0 )  PadEff();

  return StatusCode::SUCCESS;	
}

StatusCode MucCalibMgr::EffAndNoiseLV0()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log<< MSG::INFO << "Analyse layer efficiency and noise" << endreq;
  
  int part, segment, layer, stripMax;
      part = segment = layer = stripMax = 0;
  
  double digi, effHit, trackNum, nosHit, recHit;
  double eff, effErr, noise, nosRatio, nosRatioErr, cnt, cluster;
         eff = effErr = noise = nosRatio = nosRatioErr = cnt = cluster = 0.;
 
  for( int i=0; i<LAYER_MAX; i++ )
  {
    digi = effHit = trackNum = nosHit = recHit = 0;
  
    for( int j=0; j<PART_MAX; j++ )
    {
      for( int k=0; k<SEGMENT_MAX; k++)
      {
        stripMax = m_ptrIdTr->GetStripMax( j, k, i );
        for( int n=0; n<stripMax; n++ )
        {
          digi      += m_record[j][k][i][n][0];
          trackNum  += m_record[j][k][i][n][1];
          effHit    += m_record[j][k][i][n][2];
          nosHit    += m_record[j][k][i][n][3];
          recHit    += m_record[j][k][i][n][4];
        }
      }
    }
  
    // Efficiency
    if( trackNum == 0 ) {
      eff = effErr = 0.0;
      //eff     = DEFAULT_EFF_VALUE;
      //effErr  = DEFAULT_EFF_ERR; 
    }
    else    
    {          
      eff     = ( (double)effHit )/trackNum;
      effErr  = sqrt( eff*(1-eff)/trackNum );
      m_fCalibLayerNum ++;
    }
  
    // Noise
    if( m_layerResults[3][i] < LIMIT_CUT || m_fTotalDAQTime < LIMIT_CUT ) 
      noise = DEFAULT_NOS_VALUE;
    else {
      if( m_recMode == 2 ) noise = (double)nosHit/(m_fTotalEvent*TRIGGER_WINDOW*m_layerResults[3][i]);
      else                 noise = (double)nosHit/(m_fTotalDAQTime * m_layerResults[3][i]);
    }
    
    if( digi != 0 ) {
      nosRatio  = ( (double)nosHit )/digi;
      nosRatioErr = sqrt( nosRatio*(1-nosRatio)/digi );
    }
    else  {
      nosRatio  = DEFAULT_INC_VALUE;
      nosRatioErr = 0;
    }
    
    // Counting rate
    if( m_recMode == 2 ) 
      cnt = (double)digi/(m_fTotalEvent*TRIGGER_WINDOW * m_layerResults[3][i]);
    else
      cnt = (double)digi/(m_fTotalDAQTime * m_layerResults[3][i]);
    
    // Cluster
    cluster = m_hLayerCluster[i]->GetMean();
    
    m_layerResults[0][ i ] = eff;
    m_layerResults[1][ i ] = effErr;
    m_layerResults[2][ i ] = noise;
    m_layerResults[4][ i ] = cluster;
    m_layerResults[5][ i ] = trackNum;
    
    // Fill histogram
    m_hLayerEff->Fill( i, eff );
    m_hLayerEff->SetBinError( i+1, effErr );
    m_hLayerNosRatio->Fill( i, nosRatio );
    m_hLayerNosRatio->SetBinError( i+1, nosRatioErr );
    m_hLayerNos->Fill( i, noise );
    m_hLayerCnt->Fill( i, cnt );
    
    // Add cluster histogram
    m_hLayerClusterCmp->Fill( i, cluster ); 
    
    // Fill tree
    m_fLayerId        = i;
    m_fLayerEff       = eff;
    m_fLayerEffErr    = effErr;
    m_fLayerTrkNum    = trackNum;
    m_fLayerExpHit    = recHit;
    m_fLayerEffHit    = effHit;
    m_fLayerNosRatio  = nosRatio;
    m_fLayerDigi      = digi;
    m_fLayerNosHit    = nosHit;
    m_fLayerNos       = noise;
    m_fLayerCnt       = cnt;
    m_tLayConst->Fill();
    
    // Add cluster ntuple
    m_fLayerCluster = cluster;
                 
  } // End layer
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::EffAndNoiseLV1()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log<< MSG::INFO << "Analyse box efficiency and noise" << endreq;
  
  int part, segment, layer, stripMax;
      part = segment = layer = stripMax = 0;
  
  double digi, effHit, trackNum, nosHit, recHit;  
  double eff, effErr, noise, nosRatio, nosRatioErr, cnt, cluster;
         eff = effErr = noise = nosRatio = nosRatioErr = cnt = cluster = 0.;

  for( int i=0; i<BOX_MAX; i++ )
  {
    m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
    stripMax = m_ptrIdTr->GetStripMax( part, segment, layer );
  
    digi = effHit = trackNum = nosHit = recHit = 0;
    for( int j=0; j<stripMax; j++ )
    {
      digi      += m_record[part][segment][layer][j][0];
      trackNum  += m_record[part][segment][layer][j][1];
      effHit    += m_record[part][segment][layer][j][2];
      nosHit    += m_record[part][segment][layer][j][3];
      recHit    += m_record[part][segment][layer][j][4];
    }
  
    // Efficiency
    if( trackNum == 0 ) {
      eff = effErr = 0.0;
      //eff     = DEFAULT_EFF_VALUE;
      //effErr  = DEFAULT_EFF_ERR; 
    }
    else    
    {          
      eff     = ( (double)effHit )/trackNum;
      effErr  = sqrt( eff*(1-eff)/trackNum );
      m_fCalibBoxNum ++;
    }
    
    // Noise
    if( m_boxResults[3][i] < LIMIT_CUT || m_fTotalDAQTime < LIMIT_CUT ) 
      noise = DEFAULT_NOS_VALUE;
    else {
      if( m_recMode == 2 )
        noise = (double)nosHit/(m_fTotalEvent*TRIGGER_WINDOW * m_boxResults[3][i]);
      else
        noise = (double)nosHit/(m_fTotalDAQTime * m_boxResults[3][i]);
    }

    if( digi != 0 ) {
      nosRatio  = ( (double)nosHit )/digi;
      nosRatioErr = sqrt( nosRatio*(1-nosRatio)/digi );
    }
    else {
      nosRatio  = DEFAULT_INC_VALUE;
      nosRatioErr = 0;
    }

    // Counting rate
    if( m_recMode == 2 )
      cnt = (double)digi/(m_fTotalEvent*TRIGGER_WINDOW * m_boxResults[3][i]);
    else
      cnt = (double)digi/(m_fTotalDAQTime * m_boxResults[3][i]);
    
    // Cluster
    cluster = m_hBoxCluster[i]->GetMean();
    
    m_boxResults[0][ i ] = eff;
    m_boxResults[1][ i ] = effErr;
    m_boxResults[2][ i ] = noise;
    m_boxResults[4][ i ] = cluster;
    m_boxResults[5][ i ] = trackNum;
    
    // Fill histograms
    m_hBoxEff->Fill( i, eff );
    m_hBoxEff->SetBinError( i+1, effErr );
    m_hBoxNosRatio->Fill( i, nosRatio );
    m_hBoxNosRatio->SetBinError( i+1, nosRatioErr );
    m_hBoxNos->Fill( i, noise );
    m_hBoxCnt->Fill( i, cnt );
    
    // add cluster histogram
    m_hBoxClusterCmp->Fill( i, cluster );

    // Fill tree
    m_fBoxId        = i;
    m_fBoxPart      = part;
    m_fBoxSegment   = segment;
    m_fBoxLayer     = layer;
    m_fBoxEff       = eff;
    m_fBoxEffErr    = effErr;
    m_fBoxTrkNum    = trackNum;
    m_fBoxExpHit    = recHit;
    m_fBoxEffHit    = effHit;
    m_fBoxNosRatio  = nosRatio;
    m_fBoxDigi      = digi;
    m_fBoxNosHit    = nosHit;
    m_fBoxNos       = noise;
    m_fBoxCnt       = cnt;
    m_tBoxConst->Fill();
    
    // add box cluster ntuple
    m_fBoxCluster = cluster;
    
  }// End BOX_MAX

  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::EffAndNoiseLV2()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log<< MSG::INFO << "Analyse strip efficiency and noise" << endreq;
  
  int part, segment, layer, stripMax;
      part = segment = layer = stripMax = 0;
  
  double digi, effHit, trackNum, nosHit, recHit;
  double eff, effErr, noise, nosRatio, nosRatioErr, cnt, cluster;
         eff = effErr = noise = nosRatio = nosRatioErr = cnt = cluster = 0.;

  for( int i=0; i<BOX_MAX; i++ )
  {
    m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
    stripMax = m_ptrIdTr->GetStripMax( part, segment, layer );
    
    for( int j=0; j<stripMax; j++ )
    {
      digi      = m_record[part][segment][layer][j][0];
      trackNum  = m_record[part][segment][layer][j][1];
      effHit    = m_record[part][segment][layer][j][2];
      nosHit    = m_record[part][segment][layer][j][3];
      recHit    = m_record[part][segment][layer][j][4];
      
      int stripId = m_ptrIdTr->GetStripId( part, segment, layer, j );
      
      // Efficiency
      if( trackNum == 0 ) {
        eff = effErr = 0.0;
        //eff     = DEFAULT_EFF_VALUE;
        //effErr  = DEFAULT_EFF_ERR; 
      }
      else    
      {          
        eff     = ( (double)effHit )/trackNum;
        effErr  = sqrt( eff*(1-eff)/trackNum );
        m_fCalibStripNum ++;
      }

      // Noise
      if( m_stripResults[3][stripId] < LIMIT_CUT || m_fTotalDAQTime < LIMIT_CUT ) 
        noise = DEFAULT_NOS_VALUE;
      else {
        if( m_recMode == 2 )
          noise = (double)nosHit/(m_fTotalEvent*TRIGGER_WINDOW * m_stripResults[3][stripId]);
        else
          noise = (double)nosHit/(m_fTotalDAQTime * m_stripResults[3][stripId]);
      }

      if( digi != 0 ) {
        nosRatio  = ( (double)nosHit )/digi;
        nosRatioErr = sqrt( nosRatio*(1-nosRatio)/digi );
      }
      else {
        nosRatio  = DEFAULT_INC_VALUE;
        nosRatioErr = 0.;
      }

      // Counting rate
      if( m_recMode == 2 )
        cnt = (double)digi/(m_fTotalEvent*TRIGGER_WINDOW * m_stripResults[3][stripId]);
      else
        cnt = (double)digi/(m_fTotalDAQTime * m_stripResults[3][stripId]);

      // Strip itself no clusters
      m_stripResults[0][ stripId ] = eff;
      m_stripResults[1][ stripId ] = effErr;
      m_stripResults[2][ stripId ] = noise;
      m_stripResults[5][ stripId ] = trackNum;

      // Fill histotram
      m_hStripEffMap[i]->Fill( j, eff );
      m_hStripEffMap[i]->SetBinError( j+1, effErr );
      m_hStripEff->Fill( stripId, eff );
      m_hStripEff->SetBinError( stripId+1, effErr );
      m_hStripNosRatioMap[i]->Fill( j, nosRatio );
      m_hStripNosRatioMap[i]->SetBinError( j+1, nosRatioErr );
      m_hStripNosRatio->Fill( stripId, nosRatio );
      m_hStripNosRatio->SetBinError( stripId+1, nosRatioErr );
      m_hStripNos->Fill( stripId, noise );			
      m_hStripCnt->Fill( stripId, cnt );			

      // Fill Tree
      m_fStripId        = stripId;
      m_fStripPart      = part;
      m_fStripSegment   = segment;
      m_fStripLayer     = layer;
      m_fStripEff       = eff;
      m_fStripEffErr    = effErr;
      m_fStripNosRatio  = nosRatio;
      m_fStripDigi      = digi;
      m_fStripNos       = noise;
      m_fStripCnt       = cnt;
      m_fStripEffHit    = effHit;
      m_fStripExpHit    = recHit;
      m_fStripNosHit    = nosHit;
      m_fStripTrkNum    = trackNum;
      m_tStrConst->Fill();
    
    } // End stripMax
  } // End BOX_MAX
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::PadEff()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  
  int xBinStart, xBinEnd, yBinStart, yBinEnd;
  double expHit, firedHit, eff;
  eff = expHit = firedHit = 0.;
  
  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<((i==BRID)?B_SEG_NUM:E_SEG_NUM); j++ )
      for( int k=0; k<((i==BRID)?B_LAY_NUM:E_LAY_NUM); k++ )
      {
        xBinStart = m_h2DExpMap[i][j][k]->GetXaxis()->GetFirst();
        xBinEnd   = m_h2DExpMap[i][j][k]->GetXaxis()->GetLast() + 1;
        yBinStart = m_h2DExpMap[i][j][k]->GetYaxis()->GetFirst();
        yBinEnd   = m_h2DExpMap[i][j][k]->GetYaxis()->GetLast() + 1;

        for( int xi = xBinStart; xi<xBinEnd; xi++ )
          for( int yi = yBinStart; yi<yBinEnd; yi++ )
          {
            expHit    = m_h2DExpMap[i][j][k]->GetBinContent(xi, yi);
            firedHit  = m_h2DHitMap[i][j][k]->GetBinContent(xi, yi);

            if( expHit !=0 ) eff = firedHit / expHit;
            else             eff = 0;
            
            if( eff>1.0 ) 
              cout<<"Eff error:\t["<<i<<"\t"<<j<<"\t"<<k<<",\t"<<xi<<"\t"<<yi<<"]:\t"
                  <<eff<<"\t,"<<firedHit<<" "<<expHit<<endl;

            m_h2DEffMap[i][j][k]->SetBinContent(xi, yi, eff);
          }
      }  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::AnalyseCluster()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::AnalyseRes()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Analyse spacial resolution" << endreq;

  double resMean, resMeanErr, resSigma, resSigmaErr;
  resMean = resMeanErr = resSigma = resSigmaErr = 0.;

  for( int i=0; i<B_LAY_NUM; i++ )
  {
    m_hBarrelResDist[i]->Fit("gaus");
    resMean = m_hBarrelResDist[i]->GetFunction("gaus")->GetParameter("Mean");
    resSigma = m_hBarrelResDist[i]->GetFunction("gaus")->GetParameter("Sigma");
    resMeanErr = m_hBarrelResDist[i]->GetFunction("gaus")->GetParError(1);
    resSigmaErr = m_hBarrelResDist[i]->GetFunction("gaus")->GetParError(2);

    m_hBarrelResComp[0]->Fill( i, resMean );
    m_hBarrelResComp[0]->SetBinError( i+1, resMeanErr );
    m_hBarrelResComp[1]->Fill( i, resSigma );
    m_hBarrelResComp[1]->SetBinError( i+1, resSigmaErr );
  }

  for( int i=0; i<E_LAY_NUM; i++ )
  {
    m_hEndcapResDist[i]->Fit("gaus");
    resMean = m_hEndcapResDist[i]->GetFunction("gaus")->GetParameter("Mean");
    resSigma = m_hEndcapResDist[i]->GetFunction("gaus")->GetParameter("Sigma");
    resMeanErr = m_hEndcapResDist[i]->GetFunction("gaus")->GetParError(1);
    resSigmaErr = m_hEndcapResDist[i]->GetFunction("gaus")->GetParError(2);

    m_hEndcapResComp[0]->Fill( i, resMean );
    m_hEndcapResComp[0]->SetBinError( i+1, resMeanErr );
    m_hEndcapResComp[1]->Fill( i, resSigma );
    m_hEndcapResComp[1]->SetBinError( i+1, resSigmaErr );
  }

  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::SaveConst()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << "Save calibration constants" << endreq;
  
  // Save calibrated eff in graphes
  // LV0
  double layerXY[2][LAYER_MAX];
  double layerEXY[2][LAYER_MAX];
  for( int i=0; i<LAYER_MAX; i++ )
  {
    layerXY[0][i] = i;
    layerEXY[0][i] = 0;
    if( m_layerResults[5][i] >= 100*TRACK_THRESHOLD ) {
      layerXY[1][i]   = m_layerResults[0][i];
      layerEXY[1][i]  = m_layerResults[1][i];
    } 
    else {
      layerXY[1][i]   = 0;
      layerEXY[1][i]  = 0;      
    }
  }
  m_geLayerEff = new TGraphErrors(LAYER_MAX, layerXY[0], layerXY[1], layerEXY[0], layerEXY[1]);
  m_geLayerEff->SetMarkerStyle(25);
  m_geLayerEff->SetMarkerSize(0.5);
  m_cv[0] = new TCanvas("GoodLayerEff","Layer efficiency", 50, 50, 800, 600);
  m_cv[0]->SetFillColor(0);
  m_cv[0]->SetBorderMode(0);
  m_geLayerEff->Draw("AP");
  m_cv[0]->Write();
  
  // LV1
  double boxXY[2][BOX_MAX];
  double boxEXY[2][BOX_MAX];
  for( int i=0; i<BOX_MAX; i++ )
  {
    boxXY[0][i]   = i;
    boxEXY[0][i]  = 0;
    if( m_boxResults[5][i] >= 10*TRACK_THRESHOLD ) {
      boxXY[1][i]   = m_boxResults[0][i];
      boxEXY[1][i]  = m_boxResults[1][i];
    }
    else {
      boxXY[1][i]   = 0;
      boxEXY[1][i]  = 0;
    }
  }
  m_geBoxEff = new TGraphErrors(BOX_MAX, boxXY[0], boxXY[1], boxEXY[0], boxEXY[1]);
  m_geBoxEff->SetMarkerStyle(25);
  m_geBoxEff->SetMarkerSize(0.5);
  m_cv[1] = new TCanvas("GoodBoxEff","Box efficiency", 75, 75, 800, 600);
  m_cv[1]->SetFillColor(0);
  m_cv[1]->SetBorderMode(0);
  m_geBoxEff->Draw("AP");
  m_cv[1]->Write();
  
  // LV2
  double stripXY[2][STRIP_MAX];
  double stripEXY[2][STRIP_MAX];
  for( int i=0; i<STRIP_MAX; i++ )
  {
    stripXY[0][i]   = i;
    stripEXY[0][i]  = 0;
    if( m_stripResults[5][i] >= TRACK_THRESHOLD ) {
      stripXY[1][i]   = m_stripResults[0][i];
      stripEXY[1][i]  = m_stripResults[1][i];
    }
    else {
      stripXY[1][i]   = 0;
      stripEXY[1][i]  = 0;
    }
  }
  m_geStripEff = new TGraphErrors(STRIP_MAX, stripXY[0], stripXY[1], stripEXY[0], stripEXY[1]);
  m_geStripEff->SetMarkerStyle(25);
  m_geStripEff->SetMarkerSize(0.5);
  m_cv[2] = new TCanvas("GoodStripEff","Strip efficiency", 100, 100, 800, 600);
  m_cv[2]->SetFillColor(0);
  m_cv[2]->SetBorderMode(0);
  m_geStripEff->Draw("AP");
  m_cv[2]->Write();
  
  // Save histograms
  for(int i=0; i<B_LAY_NUM; i++ )
  {
    m_hHitMapBarrel_Lay[i]->Write();

    if( i<E_LAY_NUM ) {
      m_hHitMapEndcap_Lay[0][i]->Write();
      m_hHitMapEndcap_Lay[1][i]->Write();
    }
  }

  for(int i=0; i<B_SEG_NUM; i++)
  {
    m_hHitMapBarrel_Seg[i]->Write();

    if( i< E_SEG_NUM ) {
      m_hHitMapEndcap_Seg[0][i]->Write();
      m_hHitMapEndcap_Seg[1][i]->Write();
    }
  }
  m_hTrackDistance->Fit("gaus");
  m_hTrackPosPhiDiff->Fit("gaus");
  m_hTrackPosThetaDiff->Fit("gaus");
  m_hTrackMomPhiDiff->Fit("gaus");
  m_hTrackMomThetaDiff->Fit("gaus");
  
  m_hTrackDistance->Write();
  m_hTrackPosPhiDiff->Write();
  m_hTrackPosThetaDiff->Write();
  m_hTrackMomPhiDiff->Write();
  m_hTrackMomThetaDiff->Write();  

  for( int i=0; i<B_LAY_NUM; i++ )  m_hBarrelResDist[i]->Write();
  for( int i=0; i<E_LAY_NUM; i++ )  m_hEndcapResDist[i]->Write();

  m_hBarrelResComp[0]->Write();
  m_hBarrelResComp[1]->Write();
  m_hEndcapResComp[0]->Write();
  m_hEndcapResComp[1]->Write();

  if( m_usePad != 0 ) m_histArray->Write();

  for( int i=0; i<BOX_MAX; i++ )
  {
    m_hStripFireMap[ i ]->Write();
    m_hStripExpHitMap[ i ]->Write();
    m_hStripEffHitMap[ i ]->Write();
    m_hStripNosHitMap[ i ]->Write();
    m_hStripEffMap[ i ]->Write();
    m_hStripNosRatioMap[ i ]->Write();              
  }
  m_hStripFire->Write();
  m_hStripExpHit->Write();
  m_hStripEffHit->Write();
  m_hStripNosHit->Write();
  m_hStripEff->Write();
  m_hStripArea->Write();
  m_hStripNos->Write();
  m_hStripNosRatio->Write();
  log << MSG::INFO << "Save LV2 histograms done!" << endreq;
  
  m_hBoxFire->Write(); 
  m_hBoxExpHit->Write();
  m_hBoxEffHit->Write();
  m_hBoxNosHit->Write();      
  m_hBoxEff->Write();
  m_hBoxArea->Write();
  m_hBoxNos->Write();
  m_hBoxNosRatio->Write();
  log << MSG::INFO << "Save LV1 histograms done!" << endreq;
  
  m_hBrLayerFire->Write();
  m_hEcLayerFire->Write();
  m_hLayerFire->Write();
  m_hLayerExpHit->Write();
  m_hLayerEffHit->Write();
  m_hLayerNosHit->Write();
  m_hLayerEff->Write();
  m_hLayerArea->Write();
  m_hLayerNos->Write();     
  m_hLayerNosRatio->Write();      

  for( int i=0; i<LAYER_MAX; i++ ) m_hLayerCluster[i]->Write(); 
  for( int i=0; i<BOX_MAX; i++ ) m_hBoxCluster[i]->Write();
  m_hLayerClusterCmp->Write();
  m_hBoxClusterCmp->Write();  

  log << MSG::INFO << "Save histograms done!" << endreq;
  
  // Save trees
  m_fLayerCoverage = 100*(double)m_fCalibLayerNum/LAYER_MAX;
  m_fBoxCoverage   = 100*(double)m_fCalibBoxNum/BOX_MAX;
  m_fStripCoverage = 100*(double)m_fCalibStripNum/STRIP_MAX;
  
  long digi_num, trk_num, eff_hit, nos_hit, exp_hit;
  m_tStatLog->Branch("digi_num", &digi_num, "digi_num/I");
  m_tStatLog->Branch("trk_num", &trk_num, "trk_num/I");
  m_tStatLog->Branch("eff_hit", &eff_hit, "eff_hit/I");
  m_tStatLog->Branch("nos_hit", &nos_hit, "nos_hit/I");
  m_tStatLog->Branch("exp_hit", &exp_hit, "exp_hit/I");

  int stripMax;
  for( int i=0; i<PART_MAX; i++ )
    for( int j=0; j<((i==BRID)?B_SEG_NUM:E_SEG_NUM); j++ )
      for( int k=0; k<((i==BRID)?B_LAY_NUM:E_LAY_NUM); k++ )
      {
        stripMax = m_ptrIdTr->GetStripMax( i, j, k );
        for( int n=0; n<stripMax; n++ )
        {
          digi_num = m_record[i][j][k][n][0];
          trk_num  = m_record[i][j][k][n][1];
          eff_hit  = m_record[i][j][k][n][2];
          nos_hit  = m_record[i][j][k][n][3];
          exp_hit  = m_record[i][j][k][n][4];
          m_tStatLog->Fill();
        }
      }

  m_jobFinish = clock();
  m_fTotalJobTime = (double)(m_jobFinish - m_jobStart)/CLOCKS_PER_SEC;  
  
  m_tJobLog->Fill();  
  m_tJobLog->Write();
  m_tStatLog->Write();
  m_tLayConst->Write();
  m_tBoxConst->Write();
  m_tStrConst->Write();  
  
  // Close cluster output file
  if( m_fdata != NULL ) m_fdata->close();
  
  return StatusCode::SUCCESS;
}

StatusCode MucCalibMgr::EndRun()
{
  MsgStream log(msgSvc, "MucCalibMgr");
  log << MSG::INFO << endreq;
  log << MSG::INFO << "Total events   : " << m_fTotalEvent  << endreq;
  log << MSG::INFO << "Total digis    : " << m_fTotalDigi   << endreq;
  log << MSG::INFO << "Total rec hits : " << m_fTotalExpHit << endreq;
  log << MSG::INFO << "Total eff hits : " << m_fTotalEffHit << endreq;
  log << MSG::INFO << "Total inc hits : " << m_fTotalNosHit << endreq;
  log << MSG::INFO << "Total clusters : " << m_fTotalClstNum<< endreq;
  
  log << MSG::INFO << "Strip calibrated percentage: " 
      << 100*(double)m_fCalibStripNum/STRIP_MAX << "%" << endreq;
  log << MSG::INFO << "Box   calibrated percentage: "
      << 100*(double)m_fCalibBoxNum/BOX_MAX << "%" << endreq;
  log << MSG::INFO << "Layer calibrated percentage: "
      << 100*(double)m_fCalibLayerNum/LAYER_MAX << "%" <<  endreq;
  
  log << MSG::INFO << "Calibration run successfully" << endreq << endreq;
  
  return StatusCode::SUCCESS;
}

// END
