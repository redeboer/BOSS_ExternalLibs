// $Id: RandPoisson.h,v 1.3.4.1 2005/03/18 22:26:48 garren Exp $
// -*- C++ -*-
//
// -----------------------------------------------------------------------
//                             HEP Random
//                         --- RandPoisson ---
//                          class header file
// -----------------------------------------------------------------------
// This file is part of Geant4 (simulation toolkit for HEP).

// Class defining methods for shooting numbers according to the Poisson
// distribution, given a mean (Algorithm taken from "W.H.Press et al.,
// Numerical Recipes in C, Second Edition".
// Default mean value is set to 1, value used for operator()().

// =======================================================================
// Gabriele Cosmo - Created: 5th September 1995
//                - Added not static Shoot() method: 17th May 1996
//                - Algorithm now operates on doubles : 31st Oct 1996
//                - Added methods to shoot arrays: 28th July 1997
// J.Marraffino   - Added default mean as attribute and
//                  operator() with mean: 16th Feb 1998
// Gabriele Cosmo - Relocated static data from HepRandom: 5th Jan 1999
// M. Fischler    - Moved meanMax and defaultMean from private to protected
//		    to accomodate derived classes RandPoissonQ & RandPoissonT
// M Fischler      - put and get to/from streams 12/10/04
// =======================================================================

#ifndef RandPoisson_h
#define RandPoisson_h 1

#include "CLHEP/Random/defs.h"
#include "CLHEP/Random/Random.h"

namespace CLHEP {

/**
 * @author
 * @ingroup random
 */
class RandPoisson : public HepRandom {

public:

  inline RandPoisson ( HepRandomEngine& anEngine, double m=1.0 );
  inline RandPoisson ( HepRandomEngine* anEngine, double m=1.0 );
  // These constructors should be used to instantiate a RandPoisson
  // distribution object defining a local engine for it.
  // The static generator will be skipped using the non-static methods
  // defined below.
  // If the engine is passed by pointer the corresponding engine object
  // will be deleted by the RandPoisson destructor.
  // If the engine is passed by reference the corresponding engine object
  // will not be deleted by the RandPoisson destructor.

  virtual ~RandPoisson();
  // Destructor

  // Save and restore to/from streams
  
  std::ostream & put ( std::ostream & os ) const;
  std::istream & get ( std::istream & is );

  // Static methods to shoot random values using the static generator

  static  long shoot( double m=1.0 );

  static  void shootArray ( const int size, long* vect, double m=1.0 );

  //  Static methods to shoot random values using a given engine
  //  by-passing the static generator.

  static  long shoot( HepRandomEngine* anEngine, double m=1.0 );

  static  void shootArray ( HepRandomEngine* anEngine,
                            const int size, long* vect, double m=1.0 );

  //  Methods using the localEngine to shoot random values, by-passing
  //  the static generator.

  long  fire();
  long  fire( double m );

  void fireArray ( const int size, long* vect );
  void fireArray ( const int size, long* vect, double m);

  double operator()();
  double operator()( double m );
  
  std::string name() const;
  HepRandomEngine & engine();

  static std::string distributionName() {return "RandPoisson";}  
  // Provides the name of this distribution class

protected:

  // Protected copy constructor. Defining it here disallows user use.
  RandPoisson(const RandPoisson& d);

  double meanMax;
  double defaultMean;

  static  double getOldMean() {return oldm_st;}

  static  double getMaxMean() {return meanMax_st;}

  static  void setOldMean( double val ){oldm_st = val;}

  static  double* getPStatus() {return status_st;}

  static void setPStatus(double sq, double alxm, double g) {
    status_st[0] = sq; status_st[1] = alxm; status_st[2] = g;
  }

  inline HepRandomEngine* getLocalEngine();
  
private:

  HepRandomEngine* localEngine;
  bool deleteEngine;
  double status[3], oldm;

  // static data
  static double status_st[3];
  static double oldm_st;
  static const double meanMax_st;

};

}  // namespace CLHEP

#ifdef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
using namespace CLHEP;
#endif

#include "CLHEP/Random/RandPoisson.icc"

#endif
