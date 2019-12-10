//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: EvtRanFor.cc,v 1.1.1.2 2007/10/26 05:03:14 pingrg Exp $
//
// Description:
//	subroutine emcranfor_.
//      Provides FORTRAN calable interface to EvtRandom::Flat()
//      Can be used as EVTRANFOR instead of RANLUX in FORTRAN programs
//      or as evtranfor_ instead of ranlux_ in C/C++ programs.
//      No header file is provided, as C++ programs should use EvtRandom
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Sven Menke
//
// Copyright Information: See EvtGen/COPYRIGHT
//
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "EvtGenBase/EvtRandom.hh"
extern "C" {
  void evtranfor_(float *rvec, int *len) 
  {
    for (int i=0;i<*len;i++)
      rvec[i] = EvtRandom::Flat();
  }
}

