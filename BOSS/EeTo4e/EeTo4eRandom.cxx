//--------------------------------------------------------------------------
//
// Module: EeTo4eRandom.h
//
// Description: routines to unify random numbers using Bes random service 
//
// Modification history:
//
//   Ping RG   Feb. 16, 2009           Module created
//
//------------------------------------------------------------------------
//

#include "../EeTo4e/EeTo4eRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* EeTo4eRandom::_randomEngine=0;

void EeTo4eRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double EeTo4eRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"EeTo4eRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double EeTo4eRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in EeTo4eRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return EeTo4eRandom::random()*( max - min )+min;

} 

double EeTo4eRandom::Flat(double max){

  return max*EeTo4eRandom::random();

} 

double EeTo4eRandom::Flat(){

  return EeTo4eRandom::random();

 } 

void EeTo4eRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in EeTo4eRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



