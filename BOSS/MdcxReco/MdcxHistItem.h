//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcxHistItem.h,v 1.15 2017/08/18 06:51:52 zhangy Exp $
//
// Description:
//     class for declare ntuple and histogram items.
//
// Environment:
//     Software developed for the BESIII Detector at the BEPCII.
// 
//
// Author(s): Zhang Yao
//
//------------------------------------------------------------------------
#ifndef MDCXHISTITEM_H
#define MDCXHISTITEM_H

#include "GaudiKernel/NTuple.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
AIDA::IHistogram2D*  g_poison;
AIDA::IHistogram1D*  g_csmax4;
AIDA::IHistogram1D*  g_csmax3;
AIDA::IHistogram1D*  g_omegag;
AIDA::IHistogram1D*  g_dPhiAU;
AIDA::IHistogram1D*  g_dPhiAU_0;
AIDA::IHistogram1D*  g_dPhiAU_1;
AIDA::IHistogram1D*  g_dPhiAU_5;
AIDA::IHistogram1D*  g_dPhiAU_7;
AIDA::IHistogram1D*  g_dPhiAV;
AIDA::IHistogram1D*  g_dPhiAV_0;
AIDA::IHistogram1D*  g_dPhiAV_1;
AIDA::IHistogram1D*  g_dPhiAV_6;
AIDA::IHistogram1D*  g_dPhiAV_8;
AIDA::IHistogram1D*  g_addSegPhi;
AIDA::IHistogram1D*  g_trkllmk;
AIDA::IHistogram1D*  g_trklcircle;
AIDA::IHistogram1D*  g_trklgood;
AIDA::IHistogram1D*  g_trklhelix;
AIDA::IHistogram1D*  g_trkldrop1;
AIDA::IHistogram1D*  g_trkldrop2;
AIDA::IHistogram1D*  g_trklappend1;
AIDA::IHistogram1D*  g_trklappend2;
AIDA::IHistogram1D*  g_trklappend3;
//AIDA::IHistogram1D*  g_fitOmega;
AIDA::IHistogram1D*  g_trklfirstProb;
AIDA::IHistogram1D*  g_trkltemp;

AIDA::IHistogram1D*  g_trklproca;
AIDA::IHistogram1D*  g_trklprocaSl;
AIDA::IHistogram1D*  g_trkld;
AIDA::IHistogram1D*  g_trkle;
AIDA::IHistogram1D*  g_trkldoca;
AIDA::IHistogram1D*  g_trkllayer;
AIDA::IHistogram2D*  g_trkldl;
AIDA::IHistogram2D*  g_trklel;
AIDA::IHistogram2D*  g_dropHitsSigma;
AIDA::IHistogram1D*  g_addHitCut;
AIDA::IHistogram2D*  g_addHitCut2d;
//AIDA::IHistogram1D*  g_addSegPhiDiff;
//-----------recon------------
NTuple::Tuple*  g_xtuple0;
NTuple::Tuple*  m_xtuple1;
NTuple::Tuple*  m_xtupleEvt;
NTuple::Tuple*  m_xtupleSeg;
NTuple::Tuple*  m_xtupleTrkl;
NTuple::Item<long>               m_xEvtNo;
NTuple::Item<double>             m_xt0;
NTuple::Item<double>             m_xtiming;
NTuple::Item<double>             m_xt0Stat;
NTuple::Item<double>             m_xt0Truth;
NTuple::Item<double>             m_xp;
NTuple::Item<double>             m_xpt;
NTuple::Item<double>             m_xpz;
NTuple::Item<double>             m_xd0;
NTuple::Item<long>               m_xnSlay;
NTuple::Item<double>             m_xphi0;
NTuple::Item<double>             m_xcpa;
NTuple::Item<double>             m_xz0;
NTuple::Item<double>             m_xtanl;
NTuple::Item<double>             m_xq;
NTuple::Item<double>             m_xpocax;
NTuple::Item<double>             m_xpocay;
NTuple::Item<double>             m_xpocaz;
NTuple::Item<double>             m_xevtNo;
NTuple::Item<double>             m_xnSt;
NTuple::Item<double>             m_xnDof;
NTuple::Item<double>             m_xchi2;
NTuple::Item<double>             m_xtkId;
NTuple::Item<long>               m_xnHit;
NTuple::Item<double>             m_xnAct;
NTuple::Item<double>             m_xlayerCount;
NTuple::Item<double>             m_xnDigi;

NTuple::Array<double>            m_xlcount;
NTuple::Array<double>            m_xresid;
NTuple::Array<double>            m_xsigma;
NTuple::Array<double>            m_xdriftD;
NTuple::Array<double>            m_xdriftT;
NTuple::Array<double>            m_xdoca;
NTuple::Array<double>            m_xentra;
//NTuple::Array<double>            m_xentraHit;
NTuple::Array<double>            m_xfltLen;
NTuple::Array<double>            m_xtof;
NTuple::Array<double>            m_xambig;
NTuple::Array<double>            m_xact;
NTuple::Array<double>            m_xtdc;
NTuple::Array<double>            m_xadc;
NTuple::Array<double>            m_xlayer;
NTuple::Array<double>            m_xwire;
NTuple::Array<double>            m_xx;
NTuple::Array<double>            m_xy;
NTuple::Array<double>            m_xz;

NTuple::Item<long>               m_xt4EvtNo;
NTuple::Item<double>             m_xt4nRecTk;
NTuple::Item<double>             m_xt4nTdsTk;
NTuple::Item<double>             m_xt4t0;
NTuple::Item<long>               m_xt4t0Stat;
NTuple::Item<double>             m_xt4t0Truth;
NTuple::Item<long>               m_xt4nDigi;
NTuple::Item<long>              m_xt4nSeg;
NTuple::Item<double>             m_xt4time;
NTuple::Item<double>             m_xt4timeSeg;
NTuple::Item<double>             m_xt4timeTrack;
NTuple::Item<double>             m_xt4timeFit;
NTuple::Array<long>              m_xt4Layer;
NTuple::Array<double>            m_xt4Time;
NTuple::Array<double>            m_xt4Charge;
NTuple::Array<double>            m_xt4rawHit;
NTuple::Array<double>            m_xt4recHit;

NTuple::Item<long>             m_xtsSl;
NTuple::Item<double>           m_xtsD0;
NTuple::Item<double>           m_xtsOmega;
NTuple::Item<double>           m_xtsPhi0;
NTuple::Item<double>           m_xtsD0_sl_approx;
NTuple::Item<double>           m_xtsPhi0_sl_approx;
NTuple::Item<double>           m_xtsXline_bbrrf;
NTuple::Item<double>           m_xtsYline_bbrrf;
NTuple::Item<double>           m_xtsXline_slope;
NTuple::Item<double>           m_xtsYline_slope;
NTuple::Item<double>           m_xtsChisq;
NTuple::Item<long>             m_xtsPat;
NTuple::Item<long>             m_xtsNDigi;
NTuple::Array<long>            m_xtsLayer;
NTuple::Array<long>            m_xtsWire;
NTuple::Array<long>            m_xtsInSeg;
NTuple::Item<long>             m_xt5Layer;
NTuple::Item<long>             m_xt5Wire;

NTuple::Tuple*  m_xtupleCsmcSew;
NTuple::Item<double>           m_csmcD0;
NTuple::Item<double>           m_csmcPhi0;
NTuple::Item<double>           m_csmcZ0;
NTuple::Item<double>           m_csmcOmega;
NTuple::Item<double>           m_csmcPt;
NTuple::Item<double>           m_csmcTanl;

NTuple::Tuple*  m_xtupleAddSeg1;
NTuple::Item<long>             m_addSegSame;
NTuple::Item<double>           m_addSegSeedSl;
NTuple::Item<double>           m_addSegSeedPhi;
NTuple::Item<double>           m_addSegSeedPhiLay;
NTuple::Item<double>           m_addSegSeedLen;
NTuple::Item<double>           m_addSegSeedD0;
NTuple::Item<double>           m_addSegSeedPhi0;
NTuple::Item<double>           m_addSegAddSl;
NTuple::Item<double>           m_addSegAddPhi;
NTuple::Item<double>           m_addSegAddPhiLay;
NTuple::Item<double>           m_addSegAddLen;
NTuple::Item<double>           m_addSegAddD0;
NTuple::Item<double>           m_addSegAddPhi0;

NTuple::Tuple* m_xtupleAddSeg2;
NTuple::Item<long>             m_addSegEvtNo;
NTuple::Item<double>           m_addSegPoca;
NTuple::Item<long>             m_addSegSlayer;
NTuple::Item<double>           m_addSegLen;

NTuple::Tuple*  m_xtupleSegComb;
NTuple::Item<long>             m_segCombEvtNo;
NTuple::Item<double>           m_segCombOmega;
NTuple::Item<double>           m_segCombSameAU;
NTuple::Item<double>           m_segCombSameUV;
NTuple::Item<double>           m_segCombDLenAU;
NTuple::Item<double>           m_segCombDLenUV;
NTuple::Item<double>           m_segCombSlA;
NTuple::Item<double>           m_segCombSlU;
NTuple::Item<double>           m_segCombSlV;
NTuple::Item<double>           m_segCombPhiA;
NTuple::Item<double>           m_segCombPhiU;
NTuple::Item<double>           m_segCombPhiV;

NTuple::Tuple* m_xtupleDropHits;
NTuple::Item<long>             m_segDropHitsEvtNo;
NTuple::Item<long>             m_segDropHitsLayer;
NTuple::Item<long>             m_segDropHitsWire;
NTuple::Item<double>           m_segDropHitsPull;
NTuple::Item<double>           m_segDropHitsDoca;
NTuple::Item<double>           m_segDropHitsSigma;
NTuple::Item<double>           m_segDropHitsDrift;
NTuple::Item<double>           m_segDropHitsMcTkId;


int g_eventNo;

//zhangy
AIDA::IHistogram1D*  g_3dTkChi2;
#endif // MDCXHISTITEM_H
