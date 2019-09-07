// 
// Description: class for ntuple items
//

#ifndef MDCHISTITEM_H
#define MDCHISTITEM_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

//histograms
//AIDA::IHistogram1D*  g_actHit;
//AIDA::IHistogram1D*  g_hitEff;
AIDA::IHistogram2D*  g_residVsLayer;
AIDA::IHistogram2D*  g_residVsDoca;
AIDA::IHistogram1D*  h_mapHit;
AIDA::IHistogram1D*  h_sfHit;

//-----------cut--------------
//AIDA::IHistogram1D*  g_segChi2;
//AIDA::IHistogram1D*  g_segChi2SlayPat3[43];
//AIDA::IHistogram1D*  g_segChi2SlayPat4[43];
AIDA::IHistogram1D*  g_maxSegChisqAx;
AIDA::IHistogram1D*  g_maxSegChisqSt;
AIDA::IHistogram1D*  g_nSigAdd;
AIDA::IHistogram1D*  g_cirTkChi2;
AIDA::IHistogram1D*  g_z0Cut;
AIDA::IHistogram1D*  g_ctCut;
AIDA::IHistogram1D*  g_delCt;
AIDA::IHistogram1D*  g_delZ0;
AIDA::IHistogram1D*  g_delPhi;
AIDA::IHistogram1D*  g_delSlope;
AIDA::IHistogram1D*  g_combChi2;
AIDA::IHistogram1D*  g_phiDiff;
AIDA::IHistogram1D*  g_slopeDiff;
AIDA::IHistogram1D*  g_fitNAct;
AIDA::IHistogram1D*  g_3dTkChi2;
AIDA::IHistogram1D*  g_pickHitWire;

//tuple of MdcTrkRecon
NTuple::Tuple*  m_tupleMc;
NTuple::Tuple*  m_tupleMcHit;
NTuple::Tuple*  m_tuple1;
NTuple::Tuple*  m_tupleSeg;
NTuple::Tuple*  m_tupleEvt;
//tuple item of mc truth
NTuple::Item<double>            m_tMcEvtNo;
NTuple::Item<long>              m_tMcNTk;
NTuple::Item<long>              m_tMcTkId;
NTuple::Item<long>              m_tMcPid;
NTuple::Item<double>            m_tMcPx;
NTuple::Item<double>            m_tMcPy;
NTuple::Item<double>            m_tMcPz;
NTuple::Item<double>            m_tMcD0;
NTuple::Item<double>            m_tMcZ0;
NTuple::Item<double>            m_tMcTheta;
NTuple::Item<double>            m_tMcFiTerm;
NTuple::Item<long>              m_tMcNHit;
NTuple::Array<double>           m_tMcLayer;
NTuple::Array<double>           m_tMcWire;
NTuple::Array<double>           m_tMcDrift;
NTuple::Array<double>           m_tMcX;
NTuple::Array<double>           m_tMcY;
NTuple::Array<double>           m_tMcZ;
NTuple::Array<double>           m_tMcLR;

//tuple item of reconstruction results
NTuple::Item<double>             m_t0;
NTuple::Item<double>             m_t0Stat;
NTuple::Item<double>             m_t0Truth;
NTuple::Item<double>             m_nTdsTk;
NTuple::Item<double>             m_nMcTk;
NTuple::Item<double>             m_p;
NTuple::Item<double>             m_pt;
NTuple::Item<double>             m_nSlay;
NTuple::Item<double>             m_pz;
NTuple::Item<double>             m_d0;
NTuple::Item<double>             m_phi0;
NTuple::Item<double>             m_cpa;
NTuple::Item<double>             m_z0;
NTuple::Item<double>             m_tanl;
NTuple::Item<double>             m_q;
NTuple::Item<double>             m_pocax;
NTuple::Item<double>             m_pocay;
NTuple::Item<double>             m_pocaz;
NTuple::Item<double>             m_evtNo;
NTuple::Item<double>             m_nSt;
NTuple::Item<double>             m_nDof;
NTuple::Item<double>             m_chi2;
NTuple::Item<double>             m_tkId;
NTuple::Item<long>               m_nHit;
NTuple::Item<double>             m_nAct;
NTuple::Item<double>             m_layerCount;
NTuple::Item<long>               m_nDigi;

NTuple::Array<double>            m_resid;
NTuple::Array<double>            m_sigma;
NTuple::Array<double>            m_driftD;
NTuple::Array<double>            m_driftT;
NTuple::Array<double>            m_doca;
NTuple::Array<double>            m_entra;
NTuple::Array<double>            m_fltLen;
NTuple::Array<double>            m_tof;
NTuple::Array<double>            m_ambig;
NTuple::Array<double>            m_act;
NTuple::Array<double>            m_tdc;
NTuple::Array<double>            m_adc;
NTuple::Array<double>            m_layer;
NTuple::Array<double>            m_wire;
NTuple::Array<double>            m_x;
NTuple::Array<double>            m_y;
NTuple::Array<double>            m_z;
NTuple::Array<double>            m_dx;
NTuple::Array<double>            m_dy;
NTuple::Array<double>            m_dz;
NTuple::Array<double>            m_dDriftD;
NTuple::Array<double>            m_dlr;
NTuple::Array<double>            m_cellWidth;//for MdcTrackList::pickHits() goodDriftCut

//tuple item of event
NTuple::Item<long>               m_t4EvtNo;
NTuple::Item<double>             m_t4nMcTk;
NTuple::Item<double>             m_t4nRecTk;
NTuple::Item<double>             m_t4t0;
NTuple::Item<long>               m_t4t0Stat;
NTuple::Item<double>             m_t4t0Truth;
NTuple::Item<long>               m_t4nDigi;
NTuple::Item<long>               m_t4nDigiUnmatch;
NTuple::Array<long>	         m_t4Layer;
NTuple::Array<long>	         m_t4Wire;
NTuple::Array<double>		 m_t4Time;
NTuple::Array<double>		 m_t4Charge;
NTuple::Array<double>		 m_t4recHit;
NTuple::Array<double>		 m_t4PhiMid;
NTuple::Array<double>		 m_t4Hot;

//tuple item of segment
NTuple::Item<long>               m_tsNSeg;
NTuple::Item<long>               m_tsEvtNo;
NTuple::Item<long>               m_tsNDigi;
NTuple::Array<long>	         m_tsLayer;
NTuple::Array<long>	         m_tsWire;
NTuple::Array<long>	         m_tsInSeg;
NTuple::Array<long>	         m_tsMcTkId;

//tuple item of combine ax segs
NTuple::Tuple*  g_tupleCombAx;
NTuple::Item<double>               g_combAxdPhi0;
NTuple::Item<double>               g_combAxdCurv;
NTuple::Item<double>               g_combAxSigPhi0;
NTuple::Item<double>               g_combAxSigCurv;
NTuple::Item<double>               g_combAxSlSeed;
NTuple::Item<double>               g_combAxSlTest;
NTuple::Item<double>               g_combAxQualitySeed;
NTuple::Item<double>               g_combAxQualityTest;
NTuple::Item<double>               g_combAxPatSeed;
NTuple::Item<double>               g_combAxPatTest;
NTuple::Item<double>               g_combAxNhitSeed;
NTuple::Item<double>               g_combAxNhitTest;
NTuple::Item<double>               g_combAxMc;
NTuple::Item<double>               g_combAxMcPt;
NTuple::Item<double>               g_combAxMcTheta;
NTuple::Item<double>               g_combAxMcPhi;
NTuple::Item<double>               g_combAxMcAmbigSeed;
NTuple::Item<double>               g_combAxMcAmbigTest;

NTuple::Tuple*  m_tuplet;
NTuple::Item<long>               m_tl;
NTuple::Item<long>               m_tw;
NTuple::Item<double>             m_tphi;
NTuple::Item<double>             m_tdphi;
NTuple::Item<double>             m_tdncell;

#ifdef MDCPATREC_TIMETEST
NTuple::Tuple*  m_tupleTime;
NTuple::Item<double>             m_eventTime;
NTuple::Item<long>             m_recTkNum;
NTuple::Item<long>             m_mcTkNum;
NTuple::Item<long>             m_t5EvtNo;
NTuple::Item<long>             m_t5nHit;
NTuple::Item<long>             m_t5nDigi;
NTuple::Item<long>             m_t5nSt;
NTuple::Item<double>             m_t5fitTime;
NTuple::Item<double>             m_t5p;
NTuple::Item<double>             m_t5pt;
#endif

#ifdef MDCPATREC_RESLAYER
int m_resLayer;
NTuple::Tuple*  g_tupleres;
NTuple::Item<double>             g_resLayer;
NTuple::Item<double>             g_t6Layer;
#endif

NTuple::Tuple*  g_tupleFindSeg;
NTuple::Item<double>            g_findSegChi2;
NTuple::Item<double>            g_findSegIntercept;
NTuple::Item<double>            g_findSegSlope;
NTuple::Item<double>            g_findSegChi2Refit;
NTuple::Item<double>            g_findSegChi2Add;
NTuple::Item<int>               g_findSegPat;
NTuple::Item<int>               g_findSegNhit;
NTuple::Item<int>               g_findSegPat34;
NTuple::Item<int>               g_findSegSl;
NTuple::Item<double>            g_findSegMc;
NTuple::Item<double>            g_findSegAmbig;

NTuple::Tuple*  m_tuplePick;
NTuple::Item<long>               m_pickLayer;
NTuple::Item<long>               m_pickNCell;
NTuple::Item<long>               m_pickNCellWithDigi;
NTuple::Array<long>              m_pickWire;
NTuple::Array<double>            m_pickPredDrift;
NTuple::Array<double>            m_pickDrift;
NTuple::Array<double>            m_pickDriftTruth;
NTuple::Array<int>               m_pickPhizOk;
NTuple::Array<double>            m_pickPhiMaxDiff;
NTuple::Array<double>            m_pickPhiMinDiff;
NTuple::Array<double>            m_pickZ;
NTuple::Array<double>            m_pickResid;
NTuple::Array<double>            m_pickSigma;
NTuple::Array<double>            m_pickPhiLowCut;
NTuple::Array<double>            m_pickPhiHighCut;
NTuple::Array<double>            m_pickDriftCut;
NTuple::Array<long>              m_pickMcTk;
NTuple::Array<long>              m_pickIs2D;
NTuple::Array<double>            m_pickPt;
NTuple::Array<double>            m_pickCurv;

NTuple::Tuple*  m_tupleWireEff;
NTuple::Item<long>               m_we_tkId;
NTuple::Item<long>               m_we_pdg;
NTuple::Item<long>               m_we_primary;
NTuple::Item<long>               m_we_layer;
NTuple::Item<long>               m_we_wire;
NTuple::Item<long>               m_we_gwire;
NTuple::Item<double>             m_we_pt;
NTuple::Item<double>             m_we_theta;
NTuple::Item<double>             m_we_phi;
//NTuple::Item<double>             m_we_tdc;
NTuple::Item<long>               m_we_poisoned;
NTuple::Item<long>               m_we_seg;
NTuple::Item<long>               m_we_track;

NTuple::Tuple*  m_tupleWireEffPoi;
NTuple::Item<long>               m_we_poi_tkId;
NTuple::Item<long>               m_we_poi_pdg;
NTuple::Item<long>               m_we_poi_primary;
NTuple::Item<long>               m_we_poi_layer;
NTuple::Item<long>               m_we_poi_wire;

int haveDigiTk[43][288];
double haveDigiPt[43][288];
double haveDigiTheta[43][288];
double haveDigiPhi[43][288];
double haveDigiDrift[43][288];
int haveDigiAmbig[43][288];
int recHitMap[43][288];
#endif 
