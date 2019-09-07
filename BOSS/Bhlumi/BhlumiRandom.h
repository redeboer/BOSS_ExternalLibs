//--------------------------------------------------------------------------
//
// Module: BhlumiRandom.h
//
//------------------------------------------------------------------------

#ifndef BHLUMIRANDOM_H
#define BHLUMIRANDOM_H

#include "CLHEP/Random/RandomEngine.h"


class BhlumiRandom{

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

