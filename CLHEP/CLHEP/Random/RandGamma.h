// $Id: RandGamma.h,v 1.3.4.1 2005/03/18 22:26:48 garren Exp $
// -*- C++ -*-
//
// -----------------------------------------------------------------------
//                             HEP Random
//                          --- RandGamma ---
//                          class header file
// -----------------------------------------------------------------------

// Class defining methods for shooting gamma distributed random values,
// given a k (default=1) and specifying also a lambda (default=1).
// Default values are used for operator()().

// Valid input values are k > 0 and lambda > 0.  When invalid values are
// presented, the code silently returns -1.0.

// =======================================================================
// John Marraffino - Created: 12th May 1998  Based on the C-Rand package
//                   by Ernst Stadlober and Franz Niederl of the Technical
//                   University of Graz, Austria.
// Gabriele Cosmo  - Removed useless methods and data: 5th Jan 1999
// M Fischler      - put and get to/from streams 12/10/04
// =======================================================================

#ifndef RandGamma_h
#define RandGamma_h 1

#include "CLHEP/Random/defs.h"
#include "CLHEP/Random/Random.h"

namespace CLHEP {

/**
 * @author
 * @ingroup random
 */
class RandGamma : public HepRandom {

public:

  inline RandGamma ( HepRandomEngine& anEngine, double k=1.0,
                                                double lambda=1.0 );
  inline RandGamma ( HepRandomEngine* anEngine, double k=1.0, 
                                                double lambda=1.0 );
  // These constructors should be used to instantiate a RandGamma
  // distribution object defining a local engine for it.
  // The static generator will be skipped using the non-static methods
  // defined below.
  // If the engine is passed by pointer the corresponding engine object
  // will be deleted by the RandGamma destructor.
  // If the engine is passed by reference the corresponding engine object
  // will not be deleted by the RandGamma destructor.

  virtual ~RandGamma();
  // Destructor

  // Static methods to shoot random values using the static generator

  static inline double shoot();

  static double shoot( double k, double lambda );

  static void shootArray ( const int size, double* vect,
                            double k=1.0, double lambda=1.0 );

  //  Static methods to shoot random values using a given engine
  //  by-passing the static generator.

  static inline double shoot( HepRandomEngine* anEngine );

  static double shoot( HepRandomEngine* anEngine, 
                                  double k, double lambda );

  static void shootArray ( HepRandomEngine* anEngine, const int size,
                            double* vect, double k=1.0,
                            double lambda=1.0 );

  //  Methods using the localEngine to shoot random values, by-passing
  //  the static generator.

  inline double fire();

  double fire( double k, double lambda );
  
  void fireArray ( const int size, double* vect);
  void fireArray ( const int size, double* vect,
                   double k, double lambda );
  inline double operator()();
  inline double operator()( double k, double lambda );

  // Save and restore to/from streams
  
  std::ostream & put ( std::ostream & os ) const;
  std::istream & get ( std::istream & is );

  std::string name() const;
  HepRandomEngine & engine();

  static std::string distributionName() {return "RandGamma";}  
  // Provides the name of this distribution class
  

private:

  // Private copy constructor. Defining it here disallows use.
  RandGamma(const RandGamma& d);

  static double genGamma( HepRandomEngine *anEngine, double k,
                                                        double lambda );

  HepRandomEngine* localEngine;
  bool deleteEngine;
  double defaultK;
  double defaultLambda;

};

}  // namespace CLHEP

#ifdef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
using namespace CLHEP;
#endif

#include "CLHEP/Random/RandGamma.icc"

#endif
