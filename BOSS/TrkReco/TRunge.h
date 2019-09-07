//-----------------------------------------------------------------------------
// $Id: TRunge.h,v 1.28 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TRunge.h
// Section  : Tracking
// Owner    : Kenji Inami
// Email    : inami@bmail.kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a track using Runge Kutta method
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

//#ifdef TRKRECO_DEBUG_DETAIL
//#define TRKRECO_DEBUG
//#endif
#ifndef TRUNGE_FLAG_
#define TRUNGE_FLAG_

#define Runge	64
// This must be writen in TTrackBase.h

#define HEP_SHORT_NAMES
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#ifndef CLHEP_VECTOR3D_H
#include "CLHEP/Geometry/Vector3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif
typedef HepGeom::Transform3D  HepTransform3D;
//#include "helix/Helix.h"
//#include "coil/Bfield.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"
#include "TrkReco/Bfield.h"
#include "TrkReco/TTrackBase.h"
#include "TrkReco/TRunge.h"    

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "TrkReco/RkFitMaterial.h"
#include "CLHEP/Geometry/Transform3D.h"
class TRungeFitter;
class TTrack;
class TMLink;

#define TRunge_MAXstep 10000
/// A class to represent a track in tracking.
class TRunge : public TTrackBase {

public:
  /// Constructors
  TRunge();
  TRunge(const TTrack&);
  TRunge(const Helix&);
  TRunge(const TRunge&);
  TRunge(const RecMdcTrack&);
  /// Destructor
  ~TRunge();

public:// General information
  /// returns object type
  unsigned objectType(void) const;

public:// Extractors
  /// Track parameters (at pivot)
  double dr(void) const;
  double phi0(void) const;
  double kappa(void) const;
  double dz(void) const;
  double tanl(void) const;

  /// pivot position
  const HepPoint3D& pivot(void) const;

  /// returns helix parameter
  const Vector& a(void) const;

  /// returns error matrix
  const SymMatrix& Ea(void) const;

  /// returns helix class
  Helix helix(void) const;


  /// returns NDF
  unsigned ndf(void) const;

  /// returns chi2.
  double chi2(void) const;

  /// returns reduced-chi2
  double reducedchi2(void) const;

  /// returns B field ID
  int BfieldID(void) const;

  /// returns step size
  double StepSize(void) const;

  /// return error parameters for fitting with step size control
  const double* Yscal(void) const;
  double Eps(void) const;
  double StepSizeMax(void) const;
  double StepSizeMin(void) const;

  
  /// return mass
  float Mass(void) const;

  /// return max flight length
  double MaxFlightLength(void) const;

public:// Executors

public:// Utilities
  /// calculates the closest approach to a wire in real space. 
  ///  Results are stored in TMLink. Return value is negative if error happened.
  int approach(TMLink &, const RkFitMaterial ,bool sagCorrection = true ) ;
  int approach(TMLink &, float& tof, HepVector3D& p,
	       const RkFitMaterial ,bool sagCorrection = true ) ;

  /// caluculate closest points between a line and this track
  int approach_line(TMLink &,const HepPoint3D&,const HepVector3D&,
		    HepPoint3D& onLine,HepPoint3D& onTrack,const RkFitMaterial  material) ;
  int approach_line(TMLink &,const HepPoint3D&,const HepVector3D&,
		    HepPoint3D& onLine,HepPoint3D& onTrack,
		    float& tof, HepVector3D& p,const RkFitMaterial  material) ;
  int approach_line(TMLink &,const HepPoint3D&,const HepVector3D&,
		    HepPoint3D& onLine,HepPoint3D& onTrack,
		    float& tof, HepVector3D& p, const RkFitMaterial  material,unsigned& stepNum) ;

  /// caluculate closest point between a point and this track
  int approach_point(TMLink &,const HepPoint3D&,HepPoint3D& onTrack,const RkFitMaterial  material) const;

public:// Modifiers
  /// set new pivot
  const HepPoint3D& pivot(const HepPoint3D&);

  /// set helix parameter
  const Vector& a(const Vector&);

  /// set helix error matrix
  const SymMatrix& Ea(const SymMatrix&);

  /// set B field map ID
  int BfieldID(int);

  /// set step size to calc. trajectory
  double StepSize(double);

  /// set error parameters for fitting with step size control
  const double* Yscal(const double*);
  double Eps(double);
  double StepSizeMax(double);
  double StepSizeMin(double);

  /// set mass  for tof calc.
  float Mass(float);

  // set max flight length
  double MaxFlightLength(double);

  double DisToWire(TMLink& ,double,HepPoint3D ,HepPoint3D&);
  double dEpath(double , double , double)const;
  void eloss(double path, const RkFitMaterial * material,double mass,double y[6],int index)const;

public:// utilities for local use
  /// make the trajectory in cache,   return the number of step
  unsigned Fly(const RkFitMaterial  material) const;
  unsigned Fly_SC(void) const; //fly with stepsize control
  double intersect_cylinder(double r) const;
  double intersect_yz_plane(const HepTransform3D& plane, double x) const;
  double intersect_zx_plane(const HepTransform3D& plane, double y) const;
  double intersect_xy_plane(double z) const;
  /// propagate the track using 4th-order Runge-Kutta method
  void Propagate(double y[6],const double& step,const RkFitMaterial  material) const;
  void Function(const double y[6],double f[6]) const;
  // for propagate with quality check
  void Propagate1(const double y[6], const double dydx[6],
		  const double& step, double yout[6]) const;
  void Propagate_QC(double y[6],double dydx[6],const double& steptry,
		    const double& eps, const double yscal[6],
		    double& stepdid, double& stepnext) const;
  
  /// set first point (position, momentum)  s=0, phi=0
  void SetFirst(double y[6]) const;
  /// access to trajectory cache
  unsigned Nstep(void) const;
  int GetXP(unsigned stepNum,double y[6]) const;
  // y[6] = (x,y,z,px,py,pz)  output: error=-1
  int GetStep(unsigned stepNum,double& step) const;

  /// set flight length using wire hits
  double SetFlightLength(void);

private:
  // track parameters   updated when fitted
  HepPoint3D _pivot;
  Vector _a;		//dr,phi0,kappa,dz,tanl
  SymMatrix _Ea;

  double _chi2;
  unsigned _ndf;

  static const TRungeFitter _fitter;

  friend class TRungeFitter;

  Bfield* _bfield;
  int _bfieldID;

  double _maxflightlength;
  
  double _stepSize;	// default step size  
                        //if 0, step size will be made automatically
  double _yscal[6];
  double _eps;		//used for step size control
  double _stepSizeMax;
  double _stepSizeMin;

  float _mass;
  const  RkFitMaterial FitMdcMaterial;


private:
  // caches
  float _mass2;	//=_mass*_mass;
  int _charge;

  mutable double _y[TRunge_MAXstep][6];	//(x,y,z,px,py,pz)
  mutable double _h[TRunge_MAXstep];	//step size in each step
  mutable unsigned _Nstep;		//0:not cached the trajectory
private:
    IMagneticFieldSvc* m_pmgnIMF;


};

inline unsigned TRunge::objectType(void) const{
  return Runge;
}
#endif /* TRUNGE_FLAG_ */
