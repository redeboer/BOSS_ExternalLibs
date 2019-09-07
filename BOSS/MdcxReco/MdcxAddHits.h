//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxAddHits.h,v 1.5 2009/12/16 08:59:13 zhangy Exp $
//
// Description: Given list of hits and list of tracks, try to add unused
//              hits to tracks
//	Class Header
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
#ifndef _MDCXADDHITS_
#define _MDCXADDHITS_

#include <iostream>
#include <fstream>
#include "CLHEP/Alist/AList.h"

class MdcxHel;
class MdcxFittedHel;
class MdcxHit;

class MdcxAddHits{//:public Algorithm {
public:
  MdcxAddHits(/*const std::string& name, ISvcLocator* pSvcLocator,*/
		  HepAList<MdcxFittedHel> &f, const HepAList<MdcxHit> &h, float addit=1.5);
  MdcxAddHits(/*const std::string& name, ISvcLocator* pSvcLocator,*/
		  HepAList<MdcxHel> &f, const HepAList<MdcxHit> &h, float addit=1.5);
  virtual ~MdcxAddHits( );
/*
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
*/
  //functions 
  const HepAList<MdcxHit> &GetAssociates(int i=0);
  
protected: 
  //functions
  //data
  float addcut; 
  int ncalc,nadded;
  double sumpull;
  int kkk,kkl,thot,tuot;
  int ntracks,nhits; 
  HepAList<MdcxHel> tttt;
  HepAList<MdcxHit> hhhh;
  HepAList<MdcxHit> listoasses;
};//end of MdcxAddHits

#endif
