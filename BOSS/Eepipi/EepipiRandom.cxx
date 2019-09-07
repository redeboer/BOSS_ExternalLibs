//--------------------------------------------------------------------------
//
// Module: EepipiRandom.h
//
// Description: routines to unify random numbers using Bes random service 
//
// Modification history:
//
//   Ping RG   Feb. 16, 2009           Module created
//
//------------------------------------------------------------------------
//

#include "../Eepipi/EepipiRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* EepipiRandom::_randomEngine=0;

void EepipiRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double EepipiRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"EepipiRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double EepipiRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in EepipiRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return EepipiRandom::random()*( max - min )+min;

} 

double EepipiRandom::Flat(double max){

  return max*EepipiRandom::random();

} 

double EepipiRandom::Flat(){

  return EepipiRandom::random();

 } 

void EepipiRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in EepipiRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



