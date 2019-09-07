//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxMergeDups.h,v 1.4 2011/12/08 06:52:29 zhangy Exp $
//
// Description: Give a list of charged tracks, try to find and merge
//	        duplicates. 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S. Wagner
//
// Copyright Information:
//	Copyright (C) 1999	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef _MDCXMERGEDUPS_
#define _MDCXMERGEDUPS_

#include <iostream>
#include <fstream>
#include "CLHEP/Alist/AList.h"
#include "MdcxReco/MdcxFittedHel.h"

class MdcxMergeDups{
    public:
  MdcxMergeDups(HepAList<MdcxFittedHel> &f, int debug);
  virtual ~MdcxMergeDups( );
  inline const HepAList<MdcxFittedHel> &GetMergedTrklist(){return CleanTrklist;}
//functions 
    protected:
  HepAList<MdcxFittedHel> NewTrklist; 
  HepAList<MdcxFittedHel> CleanTrklist; 
  int m_debug;
//functions
  void KillList(){HepAListDeleteAll(NewTrklist);}
};//endof MdcxMergeDups

#endif
