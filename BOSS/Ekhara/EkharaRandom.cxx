#include "Ekhara/EkharaRandom.h"
#include "Ekhara/cfortran.h"

#include "CLHEP/Random/RanluxEngine.h"
#include <iostream>
using namespace std;
using namespace CLHEP;

HepRandomEngine* EkharaRandom::_randomEngine=0;

void EkharaRandom::setRandomEngine(CLHEP::HepRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double EkharaRandom::random(){

  if (_randomEngine==0){
    cerr <<"No random engine available in "
			   <<"EkharaRandom::random()."<<endl;
  }

    return _randomEngine->flat();
  }



double EkharaRandom::Flat( double min, double max){

  if ( min > max ) {
    cerr<< "min>max in EkharaRandom::Flat(" << min << "," << max << ")" <<endl;
  }

  return EkharaRandom::random()*( max - min )+min;

} 

double EkharaRandom::Flat(double max){

  return max*EkharaRandom::random();

} 

double EkharaRandom::Flat(){

  return EkharaRandom::random();

 } 

void EkharaRandom::FlatArray(double* vect, const int size){
  if(_randomEngine == 0) 
	  cout<<"Can not get randomEngine pointer in EkharaRandom::FlatArray"<<endl;
  else {
	  _randomEngine->flatArray(size,vect);
  }
}

FCALLSCSUB2(EkharaRandom::FlatArray,RANLXDF,ranlxdf,DOUBLEV,INT)
