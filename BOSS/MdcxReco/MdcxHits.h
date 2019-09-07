//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxHits.h,v 1.6 2009/12/16 08:59:13 zhangy Exp $
//
// Description: 
//	Class Header for |MdcxHits| - container for |MdcxHit|s
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//      S. Wagner
//
// Copyright Information:
//	Copyright (C) 1994	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef MDCXHITS_H
#define MDCXHITS_H

#include <iostream>
#include <fstream>
#include "MdcxReco/MdcxHit.h"
#include "CLHEP/Alist/AList.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"

class MdcxHits {
public:
  MdcxHits();
  //MdcxHits(MdcDigiCol* digiCol, float c0=0.0, float cresol=0.0180);
  virtual ~MdcxHits();
  void reset();
  void create(MdcDigiVec digiVec, float c0=0.0, float cresol=0.0180);
  inline const HepAList<MdcxHit> &GetMdcxHitList() { return mdcxHitList; }
  void print(std::ostream &o,int pmax=10)const;
private:
  HepAList<MdcxHit> mdcxHitList; 
  void KillList() { HepAListDeleteAll(mdcxHitList); }
  static double timecut;
  int m_debug;
  bool m_hitInUse[43][288];
};
#endif

