//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxCosmicSewer.h,v 1.8 2012/07/20 05:48:16 zhangy Exp $
//
// Description:
//	Class MdcxCosmicSewer. This is a simple example of a user module. It
//	just prints out each entrypoint (operation) as it is accessed.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Zhang Yao(zhangyao@ihep.ac.cn)
//
// Copyright Information:
//	Copyright (C) 1994		Lawrence Berkeley Laboratory
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef MDCXCOSMICSEWER_H
#define MDCXCOSMICSEWER_H

//----------------------
// Base Class Headers --
//----------------------
#include "GaudiKernel/Algorithm.h"
#include "CLHEP/Alist/AList.h"
#include "MdcGeom/MdcDetector.h"
#include "BField/BField.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkFitter/TrkContextEv.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "GaudiKernel/NTuple.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"
#include "MdcPrintSvc/MdcPrintSvc.h"

class MdcxCosmicSewer : public Algorithm {

public: 
  // Constructors
  MdcxCosmicSewer(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  virtual ~MdcxCosmicSewer();

  // Operations
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(); 
  StatusCode beginRun();  

  void MdcxHitsToHots(HepVector& helix, TrkRecoTrk* trk, HitRefVec& hits, HitRefVec& skiped);
  void store(TrkRecoTrk* tk, HitRefVec& skip);
  void clearRecMdcTrackHit();
  void dumpTdsTrack(RecMdcTrackCol* trackList);

private:
  void getInfo(HepVector helix, double fltLen, HepPoint3D& pos, Hep3Vector & dir);
  int patAmbig(int bes3FlagLR);
  int bes3FlagLR(int patAmbig);
  int 			m_debug;
  bool	            	m_hist;
  bool	            	m_doSag;

  std::vector<float> 	m_cosmicSewPar;
  bool 			m_cosmicSewSkip;
  bool 			m_countPropTime; 
  bool		    	m_lineFit;

  MdcUtilitySvc*   	m_mdcUtilitySvc;
  MdcPrintSvc*              m_mdcPrintSvc;
  const MdcDetector* 	m_gm;
  IMagneticFieldSvc*   	m_pIMF;
  BField*	    	m_bfield;
  TrkContextEv* 	m_context;
  const MdcCalibFunSvc* m_mdcCalibFunSvc; 
  RawDataProviderSvc* 	m_rawDataProviderSvc;
  double 		m_bunchT0; 
  long			i_evt;
  bool                  m_test;
  int 			m_nSewed;

  NTuple::Tuple*  	m_xtupleCsmcSew;
  NTuple::Item<double>           m_csmcD0;
  NTuple::Item<double>           m_csmcPhi0;
  NTuple::Item<double>           m_csmcZ0;
  NTuple::Item<double>           m_csmcOmega;
  NTuple::Item<double>           m_csmcPt;
  NTuple::Item<double>           m_csmcTanl;
};
#endif
