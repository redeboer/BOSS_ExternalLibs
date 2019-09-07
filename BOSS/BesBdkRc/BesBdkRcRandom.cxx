#include "BesBdkRc/BesBdkRcRandom.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* BesBdkRcRandom::_randomEngine=0;

void BesBdkRcRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}

double BesBdkRcRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"BesBdkRcRandom::random()."<<endl;
  }
    return _randomEngine->flat();
  }



double BesBdkRcRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in BesBdkRcRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return BesBdkRcRandom::random()*( max - min )+min;

} 

double BesBdkRcRandom::Flat(double max){

  return max*BesBdkRcRandom::random();

} 

double BesBdkRcRandom::Flat(){

  return BesBdkRcRandom::random();

 } 

void BesBdkRcRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) cout<<"Can not get randomEngine pointer in BesBdkRcRandom::FlatArray"<<endl;
  else _randomEngine->flatArray(size,vect);
}



