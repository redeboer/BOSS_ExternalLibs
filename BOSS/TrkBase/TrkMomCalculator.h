//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:  TrkMomCalculator is the momentum calculation engine
//      for the TrkMomVisitor implementation of the Visitor pattern,
//      for use in calculating momenta for different types of
//      trajectories.  Important note: in calculating momentum, these 
//      functions make assumptions about how trajectories relate to mom.
//      Specifically, they assume: 1) that mom should be calculated using
//      the nominal B field, not B at the point in question -- this is valid 
//      for (today's) KalmanTrack, but is not true in general; 2) that 
//      B is parallel to the z axis.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Justin Albert, Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKMOMCALCULATOR_HH
#define TRKMOMCALCULATOR_HH
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

class TrkSimpTraj;
class BField;

using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
class BesVectorErr;

// Class interface //
class TrkMomCalculator {

public:

  TrkMomCalculator();
  virtual ~TrkMomCalculator();

  //********************************
  // The calculator functions:
  //********************************

  static double       ptMom( const TrkSimpTraj&, const BField&, double fltlen);
  static Hep3Vector   vecMom(const TrkSimpTraj&, const BField&, double fltlen);
  static BesVectorErr errMom(const TrkSimpTraj&, const BField&, double fltlen);
  static int          charge(const TrkSimpTraj&, const BField&, double fltlen);

  // Interface to vertexing algorithms (M.Bondioli 7/17/98)
  // covariance matrices of the track at fixed flight length 
  static HepMatrix    posmomCov(const TrkSimpTraj&,const BField&,
				double fltlen);
  static void         getAllCovs(const TrkSimpTraj&,const BField&,
				 double fltlen,
				 HepSymMatrix& xxCov,
				 HepSymMatrix& ppCov,
				 HepMatrix&    xpCov);
  // invert 6x6
  static bool weightToCov(const HepSymMatrix& inXX,const HepSymMatrix& inPP,const HepMatrix& inXP,
			  HepSymMatrix& outXX,     HepSymMatrix& outPP,     HepMatrix& outXP);

  // accessors to 2nd derivative of chi2 wrt x and p.
  // x0 and p0 are filled with the pos and 3mom around which expansion 
  // takes place, whilst Weights are filled with the 2nd deriv of chi2.
  // so that:
  //  dx=x-x0   dp=p-p0
  //
  //  chi2(x,p)=0.5 dx^t*Wxx*dx + dx^t*Wxp*dp + 0.5 dp^t*Wpp*dp
  // where:
  //  pos and mom are 3-dim Vectors,
  //  xxWeight ppWeight and xpWeight are 3 by 3 matrices
  //
  // takes place, whilst Weights are filled with the 2nd deriv of chi2.
  // so that:
  //  dx=x-x0   dp=p-p0
  //
  //  chi2(x,p)=0.5 dx^t*Wxx*dx + dx^t*Wxp*dp + 0.5 dp^t*Wpp*dp
  // where:
  //  pos and mom are 3-dim Vectors,
  //  xxWeight ppWeight and xpWeight are 3 by 3 matrices
  //
  static void         getAllWeights(const TrkSimpTraj&, const BField&,
				    double fltlen,
				    HepVector& pos,
				    HepVector& mom,
				    HepSymMatrix& xxWeight,
				    HepSymMatrix& ppWeight,
				    HepMatrix&    xpWeight);

private:

  static double       calcCurvPtMom(const Hep3Vector&,  
                                    double curvature, 
				    const BField&);
  static Hep3Vector   calcCurvVecMom(const Hep3Vector&, 
                                     double curvature, 
				     const BField&);
  static BesVectorErr calcCurvErrMom(const TrkSimpTraj&, 
				     const BField&,  
                                     double flt); 
  static BesVectorErr calcNeutErrMom(const TrkSimpTraj&, const BField&,  
                                     double flt);
  static int          calcCurvCharge(const Hep3Vector&, 
                                     double curvature, 
				     const BField&);        

  static HepMatrix    calcCurvPosmomCov(const TrkSimpTraj&,const BField&,
					double fltlen);
  static HepMatrix    calcNeutPosmomCov(const TrkSimpTraj&,const BField&,
					double fltlen);

  static void         calcCurvAllCovs(const TrkSimpTraj&,const BField&,
				      double fltlen,
				      HepSymMatrix& xxCov,
				      HepSymMatrix& ppCov,
				      HepMatrix&    xpCov);

  static void         calcCurvAllCovsOLD(const TrkSimpTraj&,const BField&,
				      double fltlen,
				      HepSymMatrix& xxCov,
				      HepSymMatrix& ppCov,
				      HepMatrix&    xpCov);

  static void         calcNeutAllCovs(const TrkSimpTraj&,const BField&,
				      double fltlen,
				      HepSymMatrix& xxCov,
				      HepSymMatrix& ppCov,
				      HepMatrix&    xpCov);

  static void         calcNeutAllCovs(const TrkSimpTraj&,const BField&,
				      double fltlen,
				      HepVector& x0,HepVector& p0,
				      HepSymMatrix& xxCov,
				      HepSymMatrix& ppCov,
				      HepMatrix&    xpCov);

  static void         calcCurvAllWeights(const TrkSimpTraj&,const BField&,
					 double fltlen,
					 HepVector& pos,
					 HepVector& mom,
					 HepSymMatrix& xxWeight,
					 HepSymMatrix& ppWeight,
					 HepMatrix&    xpWeight);

  static void         calcCurvAllWeightsOLD(const TrkSimpTraj&,const BField&,
					 double fltlen,
					 HepVector& pos,
					 HepVector& mom,
					 HepSymMatrix& xxWeight,
					 HepSymMatrix& ppWeight,
					 HepMatrix&    xpWeight);

  static void         calcNeutAllWeights(const TrkSimpTraj&,const BField&,
					 double fltlen,
					 HepVector& pos,
					 HepVector& mom,
					 HepSymMatrix& xxWeight,
					 HepSymMatrix& ppWeight,
					 HepMatrix&    xpWeight);

  static int          nearestInt(double);

};

#endif
