//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxFindSegs.h,v 1.6 2009/12/16 08:59:13 zhangy Exp $
//
// Description: 
//	Class Header for |MdcxFindSegs| 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S. Wagner
//
// Copyright Information:
//	Copyright (C) 1994	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef _MDCXSEGS_
#define _MDCXSEGS_

#include <iostream>
#include <fstream>
#include "CLHEP/Alist/AList.h"
#include "MdcxReco/MdcxFittedHel.h"
#include "MdcxReco/MdcxSeg.h"
#include "MdcxReco/MdcxSegPatterns.h"

class MdcxHit;
class MdcDetector;

class MdcxFindSegs{
  public:
  MdcxFindSegs(MdcDetector* g, const HepAList<MdcxHit> &l, int m_debug = 0);
  MdcxFindSegs(const HepAList<MdcxHit> &l, int debug = 0);
  virtual ~MdcxFindSegs( );
  void initWireGroups(void);
  void process();
  //  inline const HepAList<MdcxFittedHel> &GetMdcxSeglist(){return MdcxSeglist;}
  inline const HepAList<MdcxSeg> &GetMdcxSeglist(){return MdcxSeglist;}
  void appendseg(MdcxFittedHel& fithel, int pat, int amb);
  void printseg(MdcxFittedHel& fithel, int pat,int amb,int subtry=0);
  void print(std::ostream &o,int pmax=10)const;
  inline void setDebug(bool debug=0){m_debug = debug;}
protected:
  HepAList<MdcxSeg> MdcxSeglist; 
  //  HepAList<MdcxFittedHel> MdcxSeglist; 
  MdcxFittedHel trial(int i1, int i2, int i3, int i4, int amb);
  MdcxFittedHel trial(int i1, int i2, int i3, int amb);
  void KillList(){HepAListDeleteAll(MdcxSeglist);}
  int sl;//, w0, w1, w2, w3, w4, w5;
  //int pat;
  MdcxFittedHel fithel; 
  int nseg;
  //int amb;
  const MdcDetector* gm;
  HepAList<MdcxHit> dchitlist;
  int m_debug;
  MdcxSegPatterns m_segPat;

  static int wireGroups[11][288][16];
};//endof MdcxFindSegs

#endif
