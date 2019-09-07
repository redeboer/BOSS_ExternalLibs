//--------------------------------------------------------------------------
//
// Module: EepipiRandom.h
//
// Description: head file for EepipiRandom.cc 
//
// Modification history:
//
//    Ping RG     Feb. 16, 2009   Module created
//
//------------------------------------------------------------------------

#ifndef BABAYAGARANDOM_H
#define BABAYAGARANDOM_H

#include "CLHEP/Random/RandomEngine.h"


class EepipiRandom{

public:

  static double Flat(double min, double max);
  static double Flat(double max);
  static double Flat();
  static void FlatArray(double* vect, const int size);

  static double random();
  
  //This class does not take ownership of the random engine;
  //the caller needs to make sure that the engine is not
  //destroyed.
  static void setRandomEngine(CLHEP::HepRandomEngine* randomEngine);

private:

  static CLHEP::HepRandomEngine* _randomEngine;

};

#endif

