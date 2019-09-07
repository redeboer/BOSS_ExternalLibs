//-----------------------------------------------------------------------
// File from KalFit module
//
// Filename : KalFitWires.cc
//------------------------------------------------------------------------
// Description : 
//------------------------------------------------------------------------
// Modif :
//------------------------------------------------------------------------
#ifndef _DEFINE_WIRE_H_
#define _DEFINE_WIRE_H_

#define WireHit 1
#define WireHitInvalid 2
#define WireHitAppended 4
#define WireHitAppendedorInvalid 6
#define WireNeighbor0 8
#define WireNeighbor1 16
#define WireNeighbor2 32
#define WireNeighbor3 64
#define WireNeighbor4 128
#define WireNeighbor5 256

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "KalFitAlg/lpav/Lpav.h"

//#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
//#endif
#include "KalFitAlg/KalFitLayer_Mdc.h"

 using CLHEP::HepVector; 
 using CLHEP::Hep3Vector;
 using CLHEP::HepMatrix;
 using CLHEP::HepSymMatrix;

 using namespace CLHEP;

/// Description of a Wire class
class KalFitWire{

  enum { NREGION = 3 };
  static const double A[NREGION];
  enum { NLAYER = 50 };
  static const double F[NLAYER];

public:
  /// constructor
  KalFitWire(const int localID,
	   const KalFitLayer_Mdc & layer,
	   const HepPoint3D & fwd,
	   const HepPoint3D & bck,
	   KalFitWire * const vt,
	   unsigned int geoID,
	   unsigned int stereo);

  KalFitWire();  

  /// destructor
  ~KalFitWire(void);

  /// Extractor :
  unsigned int localId(void) const { return localId_; }
  const KalFitLayer_Mdc & layer(void) const { return layer_; }
  KalFitWire ** neighborPtr(void) {return neighbor_;}
  unsigned int state(void) const { return state_; }
  unsigned stateAND(const unsigned mask) const;
  unsigned int geoID(void) const { return geoID_; }
  unsigned int stereo(void) const { return stereo_; }

  /// returns drift distance
  double distance(void) const;

  /// returns z_distance from the center of wire by drift distance
  double distance_z(void) const;

  void state(const unsigned int i) { state_=i; }
  void stateOR(const unsigned mask);
  void stateXOR(const unsigned mask);
  void stateORXOR(const unsigned mask);
  void sagcoef(void);
  void ddl(const double d) { ddl_ = d;  }  
  void ddr(const double d) { ddr_ = d;  }  

  /// Geometry :
  HepPoint3D fwd(void) const { return fwd_; }
  HepPoint3D bck(void) const { return bck_; }
  HepPoint3D xyPosition(void) const { return xyPosition_; }
  double Acoef(void) const {  return A_; }
  double lzx(void) const {  return lzx_; }
  void chk_left_and_right(void);

  // 
  double x(void) const { return x_;}
  double y(void) const { return y_;}

  /// returns z for track la
  int z(const Lpav & la, double &  z) const;
  
  double phi(void) const;

  /// set drift distance
  double distance(const double distance);

private:  
  const KalFitWire * left(void) const;
  const KalFitWire * right(void) const;
  const KalFitWire * innerLeft(KalFitWire * const vtWire) const;
  const KalFitWire * innerRight(KalFitWire * const vtWire) const;
  const KalFitWire * outerLeft(KalFitWire * const vtWire) const;
  const KalFitWire * outerRight(KalFitWire * const vtWire) const;

private:
  int localId_;
  double A_;
  double lzx_;
  HepPoint3D fwd_;
  HepPoint3D bck_;
  const KalFitLayer_Mdc & layer_;
  HepPoint3D xyPosition_;
  unsigned int state_;
  KalFitWire * neighbor_[6];
  unsigned int geoID_;
  unsigned int stereo_;
  double ddl_, ddr_;
  double distance_;
  double x_;
  double y_;
  double dx_;
  double dy_;

};

#ifdef KalFitWire_NO_INLINE
#define inline
#else
#undef inline
#define KalFitWire_INLINE_DEFINE_HERE
#endif

#ifdef KalFitWire_INLINE_DEFINE_HERE
inline 
unsigned KalFitWire::stateAND(const unsigned mask) const
{
  return state_&mask;
}

inline
void
KalFitWire::stateOR(const unsigned mask)
{
  state_|=mask;
}

inline
void
KalFitWire::stateXOR(const unsigned mask)
{
  state_^=mask;
}

inline
void
KalFitWire::stateORXOR(const unsigned mask)
{
  state_=(state_|mask)^mask;
}

inline 
double
KalFitWire::phi(void) const
{
  return M_PI*(layer_.offset()+2*localId_)/(double)layer_.superLayer().nWire();  
}

inline
double
KalFitWire::distance(void) const
{
  return distance_;
}

inline
double
KalFitWire::distance_z(void) const
{
  return distance_ * fabs(layer_.tanSlant());
}

inline
double
KalFitWire::distance(const double distance)
{
  return distance_ = distance;
}

inline
int
KalFitWire::z(const Lpav & la, double &  z) const{
  HepVector center = la.center();
  double rho = la.radius();
  double dx2 = center(1) - x_;
  double dy2 = center(2) - y_;
  double par1 = dx_*dx_ + dy_*dy_;
  double par2 = (dx_*dx2 + dy_*dy2)/par1;
  double par3 = dx_*dy2 - dy_*dx2;
  double par4 = rho*rho*par1 -  par3*par3;
  if (par4<0.) return 0;
  par4 = sqrt(par4)/par1;
  double delta = par2 + par4;
  if (delta>=0. && delta<1.){
    z = layer_.zb()+delta*(layer_.zf()-layer_.zb());
    return 1;
  }else{
    delta = par2 - par4;
    if (delta>=0. && delta<1.){
      z = layer_.zb()+delta*(layer_.zf()-layer_.zb());
      return 1;
    }
  }
  return 0;
}

#endif
#undef inline

#endif /* _DEFINE_WIRE_H_ */
