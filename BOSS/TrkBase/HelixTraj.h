// $Id: HelixTraj.h,v 1.3 2010/03/25 09:56:26 zhangy Exp $
//
// Trajectory subclass that implements a helix simple along the z axis
// Author : Gautier Hamel de Monchenault, from TrkParms and other similar classes
//
#ifndef HELIXTRAJ_HH
#define HELIXTRAJ_HH
#include "TrkBase/TrkSimpTraj.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Geometry/Point3D.h" 
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif


#include <iosfwd>

using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;

class TrkVisitor;
class TrkExchangePar;

class HelixTraj : public TrkSimpTraj {
public:
//  Define the parameters 
  enum ParIndex {d0Index=0, phi0Index, omegaIndex, z0Index, tanDipIndex}; 
  enum {NHLXPRM = 5};
  HelixTraj(const HepVector&,const HepSymMatrix&, double lowlim=-99999.,
	    double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
  HelixTraj(const TrkExchangePar&, double lowlim=-99999.,
	    double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
  HelixTraj(const TrkParams&, double lowlim=-99999.,
	    double hilim=99999., const HepPoint3D& refpoint = _theOrigin);
  HelixTraj( const HelixTraj&  );  
  HelixTraj* clone() const;

  virtual ~HelixTraj();

// operators
  HelixTraj& operator=(const HelixTraj&);
  virtual HepPoint3D   position(double fltLen)  const;
  virtual Hep3Vector direction(double fltLen) const;
  virtual Hep3Vector delDirect(double)        const;
  virtual void       getInfo(double fltLen, HepPoint3D& pos, 
			     Hep3Vector& dir) const;
  virtual void       getInfo(double fltLen, HepPoint3D& , Hep3Vector& dir, 
			     Hep3Vector& delDir) const;
  virtual void       getDFInfo(double fltLen, DifPoint& , DifVector& dir, 
			       DifVector& delDir) const;
  virtual void       getDFInfo2(double fltLen, DifPoint& pos, DifVector& 
			       dir) const;

  // How far can you go using given approximation before error > tolerance, 
  //   in direction pathDir?
  virtual double distTo1stError(double s, double tol, int pathDir) const;
  virtual double distTo2ndError(double s, double tol, int pathDir) const;


//  Real versions of the base class derrivative functions
  double curvature( double fltLen) const;
  HepMatrix derivDeflect(double fltlen,deflectDirection) const;
  HepMatrix derivDisplace(double fltlen,deflectDirection idir) const;
  HepMatrix derivPFract(double fltlen) const;
//  PointTraj functions
  TranslateParams paramFunction() const { return HelixTraj::paramFunc; }
  // Invert the parameters.  Returns true in flags if the inversion
  // requires a change of sign in the covariance matrix.
  void invertParams(TrkParams* params, std::vector<bool>& flags) const;
  
  //yzhang
  int nPar() const { return NHLXPRM;}
  //zhangy
  
  // Helix-specific accessors
  double d0() const {return parameters()->parameter()[d0Index];}
  double phi0() const;
  double omega() const {return parameters()->parameter()[omegaIndex]; }
  double z0() const {return parameters()->parameter()[z0Index]; }
  double tanDip() const {  return parameters()->parameter()[tanDipIndex]; }

  //--------------------------------------------------
  // Visitor access
  //--------------------------------------------------

  virtual void visitAccept(TrkVisitor* vis) const;

  virtual void               print(std::ostream& os) const;
  virtual                    void printAll(std::ostream& os) const;


private:


//  Private functions (data members are part of the base class)

  //  double x( const double& ) const;
  //  double y( const double& ) const;
  double z( const double& ) const;
  double dip() const {return atan(tanDip());}
//  double cosDip() const {return 1./sqrt(1.+sqr(tanDip())); }
  double cosDip() const {return 1./sqrt(1.+tanDip()*tanDip()); }
  double sinDip() const {return tanDip()*cosDip(); }
  double translen(const double& f) const {return cosDip()*f;}
  double arc( const double& f) const {return translen(f)*omega();}
  double angle(const double& f) const;
// the real point translation function
  static void paramFunc(const HepPoint3D& oldpoint,const HepPoint3D& newpoint,
			const HepVector& oldpar,const HepSymMatrix& oldcov,
			HepVector& newpar,HepSymMatrix& newcov,
			double fltlen);
};
#endif

