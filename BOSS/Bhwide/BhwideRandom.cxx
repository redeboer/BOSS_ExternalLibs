#include "Bhwide/BhwideRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* BhwideRandom::_randomEngine=0;

void BhwideRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}

double BhwideRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"BhwideRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double BhwideRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in BhwideRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return BhwideRandom::random()*( max - min )+min;

} 

double BhwideRandom::Flat(double max){

  return max*BhwideRandom::random();

} 

double BhwideRandom::Flat(){

  return BhwideRandom::random();

 } 

void BhwideRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in BhwideRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



