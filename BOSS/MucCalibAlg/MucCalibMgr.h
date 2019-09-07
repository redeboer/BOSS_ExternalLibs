//------------------------------------------------------------------------------|
//      [File  ]:                       MucCalibMgr.h                           |
//      [Brief ]:       Header file of MucCalibMgr class for encapsulation      |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_CALIB_MGR_H
#define MUC_CALIB_MGR_H

#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAIDATupleSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TFile.h"
#include "TFolder.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TDirectory.h"

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucBoxCal.h"
#include "MucCalibAlg/MucStripCal.h"
#include "MucCalibAlg/MucMark.h"
#include "MucCalibAlg/MucIdTransform.h"

using namespace std;

class MucCalibMgr {

  public:
    MucCalibMgr( std::vector<double> jobInfo, std::vector<int> configInfo, std::string outputFile );
    ~MucCalibMgr();
		
    // for initialize
    StatusCode InitNtuple();		
    StatusCode InitHisto(); 	
    StatusCode InitArea();

    // for execute
    StatusCode DimuSelect();
    StatusCode ReadEvent();
    StatusCode CheckEvent();
    StatusCode FillEvent();
	
    // for finalize
    StatusCode AnalyseEffAndNoise();
    StatusCode AnalyseCluster();		
    StatusCode AnalyseRes();
    
    StatusCode SaveConst();	
    StatusCode EndRun();			
	
  protected:
		
    StatusCode InitOnlineHisto();    
    StatusCode InitHistoLV0();
    StatusCode InitHistoLV1();
    StatusCode InitHistoLV2();
    StatusCode Init2DEffHisto();
    StatusCode InitClusterHisto();
    StatusCode InitResHisto();
    StatusCode InitConstTree();

    StatusCode FillDigi( int part, int segment, int layer, int strip );
    StatusCode FillExpHit(  int part, int segment, int layer, int strip );
    StatusCode FillEffHit(  int part, int segment, int layer, int strip );
    StatusCode FillNosHit(  int part, int segment, int layer, int strip );
    StatusCode FillCluster(  int part, int segment, int layer, int size );
		
    StatusCode EffAndNoiseLV0();
    StatusCode EffAndNoiseLV1();
    StatusCode EffAndNoiseLV2();
    StatusCode PadEff();

    StatusCode ClearOnlineHisto();
    StatusCode ClearHistoLV0();
    StatusCode ClearHistoLV1();
    StatusCode ClearHistoLV2();
    StatusCode Clear2DEffHisto();
    StatusCode ClearClusterHisto();
    StatusCode ClearResHisto();
    StatusCode ClearConstTree();

  public:
    IMessageSvc*        msgSvc;
    INTupleSvc*         ntupleSvc;
    IDataProviderSvc*   eventSvc;

  private:
    double m_vs;              // BOSS version
    double m_hv;              // high voltage,  default = 7200V
    double m_th;              // threshold,     default = 75mV
    double m_er;              // event rate,    default = 4000Hz
    double m_tag;             // input event type 0:Anything(default), 1:Dimu, 
                              // 2:RandomTrg, 3:Cosmic ray, 4:Ppjuu, 5:Other
    int   m_recMode;          // rec mode flag, 0:ExtTrk(default), 1:SelfTrk, 2: combine
    int   m_usePad;           // calculate pad eff flag, 0: no(default), other: yes
    int   m_effWindow; 	      // eff window for tracking in eff calibration, default = 4
    int   m_clusterMode;      // flag for cluster building method, 0: no build, 1: basic(default), 2/3: other 
    int   m_clusterSave;      // flag for cluster output, 0: no save(default), 1: save 
    int   m_checkEvent;       // flag for checking event or not, 0: not check(default), 1: check 
    int   m_dimuSelect;       // flag for selecting dimu event or not, 0: no(default), 1: yes 
    int   m_dimuOnly;         // flag for using dimu event only or not, 0: no(default), 1: yes 
    
    string    m_outputFile;       // calibration constants output file name
    ofstream* m_fdata;            // cluster data output file, enable if m_clusterMode != 0     
    clock_t   m_jobStart, m_jobFinish;  // for time assumption, start at constructor, end at SaveConst()
    clock_t   m_evtBegin, m_evtEnd;  // for event processing time, start at ReadEvent(), end at FillEvent()

    // Calibration results for total events
    // 0-efficiency, 1-eff err, 2-noise, 3-area(cm^2), 4-Cluster(number of strip), 5-track number
    double m_layerResults[6][LAYER_MAX];
    double m_boxResults[6][BOX_MAX];
    double m_stripResults[6][STRIP_MAX];

    int  m_currentRun;
    long m_currentEvent;
    int  m_eventTag;         // tag of event 0: all(default), 1: dimu 
    // Digi and hit container for total events
    // 0-digi number, 1-rec track number, 2-efficiency hit number, 
    // 3-incidental and noise hit number, 4-rec/fit hit number
    long m_record[PART_MAX][SEGMENT_MAX][LAYER_MAX][STRIP_INBOX_MAX][5];

    // Digi or hit mark collection for one event
    MucMark*  m_ptrMucMark;
    mark_col  m_digiCol;    // container of digis in an event
    mark_col  m_expHitCol;  // container of fitting hits in all rec tracks in an  event	
    mark_col  m_calHitCol;  // container of fired hits in a rec track,fresh each track
    mark_col  m_effHitCol;  // container of fired hits in all rec tracks in an event 
    mark_col  m_nosHitCol;  // container of digis not in all rec tracks in an event

    mark_col  m_segDigiCol[PART_MAX][SEGMENT_MAX];  // container of digis in segment of part
    vector< mark_col > m_clusterCol; // container of clusters in all digits of an event
    vector<float> m_lineResCol; // container of residual by line fit
    vector<float> m_quadResCol; // container of residual by quad fit
    vector<float> m_extrResCol; // container of residual by extrapolation

    // Muc id transformer
    MucIdTransform* m_ptrIdTr;
    
    // TGraph for eff with error bar
    TGraphErrors* m_geLayerEff;
    TGraphErrors* m_geBoxEff;
    TGraphErrors* m_geStripEff;
    TCanvas*      m_cv[CALIB_LV_MAX];

    // Histogram containers for efficiency and lost
    // Hit map for online check 
    TH1F* m_hHitMapBarrel_Lay[B_LAY_NUM];     // fill according to layer(map per layer)
    TH1F* m_hHitMapEndcap_Lay[2][E_LAY_NUM];
    TH1F* m_hHitMapBarrel_Seg[B_SEG_NUM];     // fill according to segmemt(map per segment)
    TH1F* m_hHitMapEndcap_Seg[2][E_SEG_NUM];

    // Nhit vs event for online check
    TH1F* m_hHitVsEvent;

    // Distance between Ext track hit and Muc track hit on the first inner layer for online check
    TH1F* m_hTrackDistance;
    
    // Diff of phi angles of two tracks(dimuon event)
    TH1F* m_hTrackPosPhiDiff;
    TH1F* m_hTrackMomPhiDiff;
    // Diff of theta angles of two tracks(dimuon event)
    TH1F* m_hTrackPosThetaDiff;
    TH1F* m_hTrackMomThetaDiff;
    
    // PhiDiff VS ThetaDiff for dimuon event
    TH2F* m_hDimuTracksPosDiff;
    TH2F* m_hDimuTracksMomDiff;    
    
    // Phi VS costheta for online check
    TH2F* m_hPhiCosTheta;

    // Spacial resolution
    TH1F* m_hBarrelResDist[B_LAY_NUM];  // barrel spacial resolution
    TH1F* m_hEndcapResDist[E_LAY_NUM];  // endcap spacial resolution
    TH1F* m_hBarrelResComp[2];             // barrel spacial resolution vs layer
    TH1F* m_hEndcapResComp[2];             // ebdcao soacuak resikytuib vs layer    

    // 2D histogram for eff map
    TH2F* m_h2DExpMap[PART_MAX][SEGMENT_MAX][LAYER_MAX];
    TH2F* m_h2DHitMap[PART_MAX][SEGMENT_MAX][LAYER_MAX];
    TH2F* m_h2DEffMap[PART_MAX][SEGMENT_MAX][LAYER_MAX];
    TObjArray* m_histArray;
    
    
    // LV0
    TH1F* m_hBrLayerFire; // only barrel
    TH1F* m_hEcLayerFire; // only endcap

    TH1F* m_hLayerFire;   // all
    TH1F* m_hLayerExpHit;
    TH1F* m_hLayerEffHit;
    TH1F* m_hLayerNosHit;
    TH1F* m_hLayerEff;
    TH1F* m_hLayerNosRatio;
    TH1F* m_hLayerArea;
    TH1F* m_hLayerNos;
    TH1F* m_hLayerCnt;		
    
    // LV1
    TH1F* m_hBoxFire;
    TH1F* m_hBoxExpHit;
    TH1F* m_hBoxEffHit;
    TH1F* m_hBoxNosHit;
    TH1F* m_hBoxEff;
    TH1F* m_hBoxNosRatio;
    TH1F* m_hBoxArea;
    TH1F* m_hBoxNos;
    TH1F* m_hBoxCnt;
    
    // LV2
    TH1F* m_hStripFireMap[BOX_MAX];
    TH1F* m_hStripExpHitMap[BOX_MAX];
    TH1F* m_hStripEffHitMap[BOX_MAX];
    TH1F* m_hStripNosHitMap[BOX_MAX];
    TH1F* m_hStripEffMap[BOX_MAX];
    TH1F* m_hStripNosRatioMap[BOX_MAX];
    TH1F* m_hStripFire;
    TH1F* m_hStripExpHit;
    TH1F* m_hStripEffHit;
    TH1F* m_hStripNosHit;
    TH1F* m_hStripEff;
    TH1F* m_hStripNosRatio;
    TH1F* m_hStripArea;
    TH1F* m_hStripNos;
    TH1F* m_hStripCnt;
    
    // Histograms for cluster 
    TH1F* m_hLayerCluster[LAYER_MAX];
    TH1F* m_hLayerClusterCmp;
    TH1F* m_hBoxCluster[BOX_MAX];
    TH1F* m_hBoxClusterCmp;
    
    //Tree for constant root file 
    TTree* m_tJobLog;
    TTree* m_tStatLog;
    TTree* m_tLayConst;
    TTree* m_tBoxConst;
    TTree* m_tStrConst;
    
    // Job log tree 
    int m_fStartRun;            // the id of run start
    int m_fEndRun;              // the id of run end
    double m_fTotalDAQTime;     // total DAQ time
    double m_fTotalJobTime;     // total job time
    double m_fCalibLayerNum;    // the number of layers calibrated
    double m_fCalibBoxNum;      // the number of boxes calibrated
    double m_fCalibStripNum;    // the number of strips calibrated
    double m_fTotalEvent;       // total events
    double m_fTotalDigi;        // total MUC digis
    double m_fTotalExpHit;      // total rec hits
    double m_fTotalEffHit;      // total eff hits
    double m_fTotalNosHit;      // total incidental/noise hits
    double m_fTotalClstNum;  	  // total number of clusters
    double m_fTotalStripArea;   // total area of strips
    double m_fLayerCoverage;    // layer calibrated coverage
    double m_fBoxCoverage;      // layer calibrated coverage
    double m_fStripCoverage;    // layer calibrated coverage
    
    // Layer constants tree, level 0
    double m_fLayerId;
    double m_fLayerEff;
    double m_fLayerEffErr;
    double m_fLayerNosRatio;
    double m_fLayerDigi;
    double m_fLayerNos;
    double m_fLayerCnt;
    double m_fLayerExpHit;
    double m_fLayerEffHit;
    double m_fLayerNosHit;
    double m_fLayerCluster;
    double m_fLayerTrkNum;
    
    // Box constants tree, level 1
    double m_fBoxId;
    double m_fBoxPart;
    double m_fBoxSegment;
    double m_fBoxLayer;
    double m_fBoxEff;
    double m_fBoxEffErr;
    double m_fBoxNosRatio;
    double m_fBoxDigi;
    double m_fBoxNos;
    double m_fBoxCnt;
    double m_fBoxExpHit;
    double m_fBoxEffHit;
    double m_fBoxNosHit;				
    double m_fBoxCluster;
    double m_fBoxTrkNum;
    
    // Strip constants tree, level 2
    double m_fStripId;
    double m_fStripPart;
    double m_fStripSegment;
    double m_fStripLayer;
    double m_fStripEff;
    double m_fStripEffErr;
    double m_fStripNosRatio;
    double m_fStripDigi;
    double m_fStripNos;
    double m_fStripCnt;
    double m_fStripExpHit;
    double m_fStripEffHit;
    double m_fStripNosHit;
    double m_fStripTrkNum;
    
    // N-tuple
    NTuple::Tuple*  m_eventLogTuple;      // Event info
    NTuple::Item<double> m_ntEventId;
    NTuple::Item<double> m_ntEventTag;
    NTuple::Item<double> m_ntEsTime;
    NTuple::Item<double> m_ntDigiNum;
    NTuple::Item<double> m_ntTrackNum;
    NTuple::Item<double> m_ntExpHitNum;
    NTuple::Item<double> m_ntEffHitNum;
    NTuple::Item<double> m_ntNosHitNum;
    NTuple::Item<double> m_ntClusterNum;
    NTuple::Item<double> m_ntEventTime;   // run time of a event
    
    NTuple::Tuple*  m_mdcTrkInfoTuple;    // Mdc track info     
    NTuple::Item<long>   m_charge;
    NTuple::Item<double> m_mdcpx;
    NTuple::Item<double> m_mdcpy;
    NTuple::Item<double> m_mdcpz;
    NTuple::Item<double> m_mdcpt;
    NTuple::Item<double> m_mdcpp;
    NTuple::Item<double> m_mdcphi;
    NTuple::Item<double> m_mdctheta;
    
    NTuple::Tuple*  m_trackInfoTuple;     // Track info     
    NTuple::Item<double> m_ntTrackEvent;
    NTuple::Item<double> m_ntTrackTag;
    NTuple::Item<double> m_ntTrackHits;
    NTuple::Item<double> m_ntTrackSegFly;
    NTuple::Item<double> m_ntTrackLayFlyA;
    NTuple::Item<double> m_ntTrackLayFlyB;
    NTuple::Item<double> m_ntTrackLayFlyC;
    NTuple::Item<double> m_trkRecMode;		
    NTuple::Item<double> m_chi2;		
    NTuple::Item<double> m_px;		
    NTuple::Item<double> m_py;
    NTuple::Item<double> m_pz;
    NTuple::Item<double> m_pt;
    NTuple::Item<double> m_pp;
    NTuple::Item<double> m_r;
    NTuple::Item<double> m_cosTheta;
    NTuple::Item<double> m_theta;
    NTuple::Item<double> m_phi;
    NTuple::Item<double> m_depth;
    NTuple::Item<double> m_brLastLayer;
    NTuple::Item<double> m_ecLastLayer;
    NTuple::Item<double> m_totalHits;
    NTuple::Item<double> m_totalLayers;
    NTuple::Item<double> m_maxHitsInLayer;    
    
    NTuple::Tuple*  m_trackDiffTuple;     // Track collinearity 
    NTuple::Item<double> m_ntDimuTag;
    NTuple::Item<double> m_ntPosPhiDiff;
    NTuple::Item<double> m_ntPosThetaDiff;
    NTuple::Item<double> m_ntMomPhiDiff;
    NTuple::Item<double> m_ntMomThetaDiff;   
    
    NTuple::Tuple*  m_clusterSizeTuple;   // Cluster size total
    NTuple::Item<double> m_ntClusterSize;
            
    NTuple::Tuple*  m_effWindowTuple;     // EFF_WINDOW
    NTuple::Item<double> m_ntEffWindow;

/*
    NTuple::Tuple*  m_resInfoTuple;       // Res info
    NTuple::Item<long>     m_nExpNum;
    NTuple::Array<double>  m_res;
    NTuple::Array<long>    m_resPart;
    NTuple::Array<long>    m_resSegment;
    NTuple::Array<long>    m_resLayer;
    NTuple::Array<bool>    m_resFired;        
*/    

    NTuple::Tuple*  m_resInfoTuple;       // Res info
    NTuple::Item<double>  m_lineRes;
    NTuple::Item<double>  m_quadRes;
    NTuple::Item<double>  m_extrRes;
    NTuple::Item<long>    m_resPart;
    NTuple::Item<long>    m_resSegment;
    NTuple::Item<long>    m_resLayer;
    NTuple::Item<long>    m_resFired;    
    NTuple::Item<long>    m_resMode;    
				
};			

#endif
