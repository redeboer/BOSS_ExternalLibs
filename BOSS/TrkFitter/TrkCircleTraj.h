//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkCircleTraj.h,v 1.2 2006/03/28 01:03:35 zhangy Exp $
//
// Description:
//    Trajectory subclass that implements a circle in the x-y plane, suitable
//    for use in a track.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#ifndef TRKCIRCLETRAJ_H
#define TRKCIRCLETRAJ_H
#include "TrkBase/TrkSimpTraj.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"

class TrkVisitor;
class TrkExchangePar;

class TrkCircleTraj : public TrkSimpTraj
{

public:
  TrkCircleTraj(const HepVector&,const HepSymMatrix&, double lowlim=-99999.,
	    double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
  TrkCircleTraj(const TrkExchangePar&, double lowlim=-99999.,
	    double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
  TrkCircleTraj( const TrkCircleTraj&  );   // copy ctor
  TrkCircleTraj* clone() const;
  ~TrkCircleTraj();
  TrkCircleTraj& operator=(const TrkCircleTraj&);


  virtual HepPoint3D   position(double fltLen) const;
  virtual Hep3Vector direction(double fltLen) const;
  virtual Hep3Vector delDirect(double) const;
  virtual void       getInfo(double fltLen, HepPoint3D& pos, 
			      Hep3Vector& dir) const;
  virtual void       getInfo(double fltLen, HepPoint3D&, Hep3Vector& dir, 
			Hep3Vector& delDir) const;
  virtual void       getDFInfo(double fltLen, DifPoint&, DifVector& dir, 
			DifVector& delDir) const;
  virtual void       getDFInfo2(double fltLen, DifPoint& pos, DifVector& 
			       dir) const;
  virtual double     curvature( double fltLen) const;

  // How far can you go using given approximation before error > tolerance, 
  //   in direction pathDir?
  virtual double distTo1stError(double flt, double tol, int pathDir) const;
  virtual double distTo2ndError(double flt, double tol, int pathDir) const;

//  Real versions of the base class derivative functions
  HepMatrix derivDeflect(double fltlen,deflectDirection) const;
  HepMatrix derivDisplace(double fltlen,deflectDirection) const;
  HepMatrix derivPFract(double fltlen) const;
//  PointTraj functions
  TranslateParams paramFunction() const { return TrkCircleTraj::paramFunc; }
  // Invert the parameter.  Returns true in flags if the inversion
  // requires a change of sign in the covariance matrix.
  void invertParams(TrkParams* params, std::vector<bool>& flags) const;
  //yzhang
  int nPar() const {return NCIRPAR;}
  //zhangy
  //--------------------------------------------------
  // Visitor access
  //--------------------------------------------------

  virtual void visitAccept(TrkVisitor* vis) const;

//  Define the parameters 
private:
  enum {d0Ind=0, phi0Ind, omegaInd}; 
  enum {NCIRPAR = 3};
public:
  static int d0Index()               {return d0Ind;}
  static int phi0Index()             {return phi0Ind;}
  static int omegaIndex()            {return omegaInd;}
  static int nCirPrm()               {return NCIRPAR;}

  // circle-specific accessors
  double d0() const {return parameters()->parameter()[d0Index()];}
  double phi0() const;
  double omega() const {return parameters()->parameter()[omegaIndex()]; }


private:
//  Private functions (data members are part of the base class)

  double x( const double& ) const;
  double y( const double& ) const;
  inline double arc( const double& f) const {return f*omega();}
         double angle(const double& f) const; 
// the real point translation function
  static void paramFunc(const HepPoint3D& oldpoint,const HepPoint3D& newpoint,
			const HepVector& oldpar,const HepSymMatrix& oldcov,
			HepVector& newpar,HepSymMatrix& newcov,
			double fltlen);
};
#endif




