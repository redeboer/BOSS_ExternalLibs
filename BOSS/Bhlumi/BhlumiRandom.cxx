#include "Bhlumi/BhlumiRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* BhlumiRandom::_randomEngine=0;

void BhlumiRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}

double BhlumiRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"BhlumiRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double BhlumiRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in BhlumiRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return BhlumiRandom::random()*( max - min )+min;

} 

double BhlumiRandom::Flat(double max){

  return max*BhlumiRandom::random();

} 

double BhlumiRandom::Flat(){

  return BhlumiRandom::random();

 } 

void BhlumiRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in BhlumiRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



