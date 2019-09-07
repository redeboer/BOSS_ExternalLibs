//--------------------------------------------------------------------------
//
// Module: EeToeeVRandom.h
//
// Description: routines to unify random numbers using Bes random service 
//
// Modification history:
//
//   Ping RG   Feb. 16, 2009           Module created
//
//------------------------------------------------------------------------
//

#include "../EeToeeV/EeToeeVRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* EeToeeVRandom::_randomEngine=0;

void EeToeeVRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double EeToeeVRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"EeToeeVRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double EeToeeVRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in EeToeeVRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return EeToeeVRandom::random()*( max - min )+min;

} 

double EeToeeVRandom::Flat(double max){

  return max*EeToeeVRandom::random();

} 

double EeToeeVRandom::Flat(){

  return EeToeeVRandom::random();

 } 

void EeToeeVRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in EeToeeVRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



