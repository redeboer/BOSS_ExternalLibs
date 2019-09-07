//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtRandom.cc
//
// Description: routines to get random numbers from 
//              random number generator.
//
// Modification history:
//
//    DJL/RYD   September 25, 1996           Module created
//
//------------------------------------------------------------------------
//

#include "KKMC/KKMCRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* KKMCRandom::_randomEngine=0;

void KKMCRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double KKMCRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"KKMCRandom::random()."<<endl;
  }

    return _randomEngine->flat();
  }



double KKMCRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in KKMCRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return KKMCRandom::random()*( max - min )+min;

} 

double KKMCRandom::Flat(double max){

  return max*KKMCRandom::random();

} 

double KKMCRandom::Flat(){

  return KKMCRandom::random();

 } 

void KKMCRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in KKMCRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}


//----------------
extern "C"{
  extern double rlu_(int *);      //jetset74
}


double  rlu_(int *){
  double rdm= KKMCRandom::Flat();
  // std::cout<<"rlu_ = "<<rdm<<std::endl;
  return rdm;
 }  
