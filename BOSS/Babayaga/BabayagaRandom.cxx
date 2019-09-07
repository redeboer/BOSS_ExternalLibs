//--------------------------------------------------------------------------
//
// Module: BabayagaRandom.h
//
// Description: routines to unify random numbers using Bes random service 
//
// Modification history:
//
//   Ping RG   Feb. 16, 2009           Module created
//
//------------------------------------------------------------------------
//

#include "../Babayaga/BabayagaRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* BabayagaRandom::_randomEngine=0;

void BabayagaRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double BabayagaRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"BabayagaRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double BabayagaRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in BabayagaRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return BabayagaRandom::random()*( max - min )+min;

} 

double BabayagaRandom::Flat(double max){

  return max*BabayagaRandom::random();

} 

double BabayagaRandom::Flat(){

  return BabayagaRandom::random();

 } 

void BabayagaRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in BabayagaRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



