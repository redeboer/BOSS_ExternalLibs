//                              -*- Mode: C++ -*- 
//
// DecayMode.h - data class for a single decay mode of a particle
//
// Copyright (C)  1993  The Board of Trustees of The Leland Stanford
// 
// History:
//	Migration for BESIII MDC
//                      Junior University.  All Rights Reserved.
//
// $Id: DecayMode.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// See Also
//    AntiPdtEntry, PdtEntry, Pdt, PdtEntry

#ifndef _DECAYMODE_H_
#define _DECAYMODE_H_

#include <iostream>
#include <stdio.h>
#include <vector>
using std::vector;

class PdtEntry;

class DecayMode
{
public:
  DecayMode(float bf, vector<PdtEntry*> *l );
  
  virtual ~DecayMode();
  
  float BF() const { return _branchingFraction; }
  const vector<PdtEntry*> *childList() const { return _children; }
  
  void printOn(std::ostream& os) const; 
  bool operator==(const DecayMode& other) { return this == &other; }
  
protected:
  float  _branchingFraction;
  vector<PdtEntry*> *_children;	// list of child data
};

#endif


