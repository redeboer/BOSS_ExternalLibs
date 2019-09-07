//*********************************************
//
// Environment:
//      This software is part of models developed at BES collaboration.
//      If you use all or part  of it, please give an appropriate acknowledgement.
//
// Copyright Information: 
//
// Module:  PseuMarc.cc
//
// Description: to unify the random engines of :
//    Rlu in lund_crm1_evtgen.F as EvtGen 
//    Pyr in Pythia
//    by replace the random number in PseuMar_makeVec(rved,lenv)(see PseuMar.F),
//    RANMAR(RVEC,LENV) in tauola.F
// Modification history:
//
//    Ping R.-G.       Nov., 2007       Module created
//
//------------------------------------------------------------------------

#include "KKMC/KKMCRandom.h"
#include <iostream>
using namespace std;

extern "C" {
  extern void flat_(float*);
}

static int nr=0;

void flat_(float *rndm)
{
  *rndm= (float)KKMCRandom::random();
  nr++;
  //  cout<<nr<<": "<<"KKMCRandom= "<<*rndm<<endl;
  return;
} 


