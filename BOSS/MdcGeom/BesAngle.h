#ifndef __BBRANGLE_H__
#define __BBRANGLE_H__
////////////////////////////////
////////////////////////////////
//
// BaBar angles are in radians, and degress should inly be used
// when absolutely necessary. Automatic conversions to and from
// the radians form are provided, but you have to manually
// go to and from degrees
//
// By convention, angles are represented as (-pi, pi]
//
#include "MdcGeom/Constants.h"
#include <math.h>


class BesAngle
{
public:
  inline BesAngle();
  inline BesAngle(const double);
  inline ~BesAngle();

  inline operator double() const  { return _phi;};   // automatic conversion to double

  inline double rad() const;  
  inline double deg() const;  
  // convention : returns value in [-180, 180]

  inline double posRad() const;
  inline double posDeg() const;
  // returns 0 to 360.  This is not the BaBar convention, and should not 
  // be used for handing off values to anyone else's code

  inline void setRad(const double);
  inline void setDeg(const double);

  inline double arc(double radius) const;

  inline void setSector(int n, int max);
  inline void setSector(int n, int max, BesAngle phi_0);

  inline BesAngle operator + (const BesAngle&) const;
  inline BesAngle operator + (const double) const;  //assumes double in radians
  inline BesAngle operator - (const BesAngle&) const;
  inline BesAngle operator - (const double) const;  //assumes double in radians
  inline BesAngle operator * (const double) const;
  inline BesAngle operator / (const double) const;
  inline friend BesAngle operator * (const double, const BesAngle&);

  inline void operator = (const BesAngle);
  inline void operator += (BesAngle);
  inline void operator -= (BesAngle);
  inline void operator += (double);  //assumes double in radians
  inline void operator -= (double);  //assumes double in radians
  inline void operator *= (double);
  inline void operator /= (double);

  // note : > and < should have no well defined meaning ?

  inline int sector(int max);
  inline int sector(int max, BesAngle phi_0); 
   // convention : returns values [1..max]
//hxt   HepString degString() const;
  inline friend double sin(const BesAngle);
  inline friend double cos(const BesAngle);
  inline friend double tan(const BesAngle);

  // class static constants defined in .cc file
  // these are generally available as Constants::pi, Constants::twoPi, etc,
  // and once the BaBar/Constants class is in a release they should be
  /// used instead.

  static const double pi;
  static const double twoPi;

  // old names, forwarded for migration BobJ May 21 97
  inline double Rad() const { return rad(); }
  inline double Deg() const { return deg(); }
//hxt   inline HepString DegString() const { return degString(); }
  inline int Sector(int max) { return sector(max); }
  inline int Sector(int max, BesAngle phi_0) { return sector(max, phi_0); }

protected:
  double _phi;

  inline static double normalize(double);

  static const double toDegrees;
//hxt   static  const HepString degChar, deg1Char, deg2Char;

};

//
// Methods for BesAngle
//

inline double BesAngle::normalize(double angle) {
  if (angle < - Constants::pi) {
    angle += Constants::twoPi;
    if (angle < - Constants::pi) angle = fmod(angle+ Constants::pi, Constants::twoPi) + Constants::pi; 
  }
  else if (angle > Constants::pi) {
    angle -= Constants::twoPi;
    if (angle > Constants::pi) angle = fmod(angle+Constants::pi, Constants::twoPi) - Constants::pi;
  }
  return angle;
}

inline BesAngle::BesAngle() : _phi(0)
{ }

inline BesAngle::BesAngle(const double phi) : _phi(normalize(phi))
{}

inline BesAngle::~BesAngle() {}

inline double BesAngle::rad() const
{ return _phi; }

inline double BesAngle::deg() const
{ return _phi *  Constants::radToDegrees; }

inline double BesAngle::posRad() const
{ 
  if (_phi >= 0.0) return _phi; 
  else return _phi + Constants::twoPi;
}

inline double BesAngle::posDeg() const
{ return posRad() *  Constants::radToDegrees; }

inline void BesAngle::setRad(const double phi)
{ _phi = normalize(phi); }

inline void BesAngle::setDeg(const double phi)
{ setRad(phi / Constants::radToDegrees); }

inline double BesAngle::arc(double radius) const
{ return radius * rad(); }

inline void BesAngle::setSector(int n, int max)
{ setRad((n + 0.5) * Constants::twoPi / max); }

inline void BesAngle::setSector(int n, int max, BesAngle phi_0)
{ setRad((n + 0.5) * Constants::twoPi / max + phi_0._phi); }

inline BesAngle BesAngle::operator + (const BesAngle &a) const
{ return BesAngle(_phi + a._phi); }

inline BesAngle BesAngle::operator + (const double a) const
{ return BesAngle(_phi + a); }

inline BesAngle BesAngle::operator - (const BesAngle &a) const
{ return BesAngle(_phi - a._phi); }

inline BesAngle BesAngle::operator - (const double a) const
{ return BesAngle(_phi - a); }

inline BesAngle BesAngle::operator * (const double x) const
{ return BesAngle(_phi * x); }

inline BesAngle BesAngle::operator / (const double x) const
{ return BesAngle(_phi / x); }

inline BesAngle operator * (const double x, const BesAngle &a)
{ return BesAngle(a * x); }

inline void BesAngle::operator = (const BesAngle a)
{ _phi = normalize(a._phi); 
}

inline void BesAngle::operator += (BesAngle a)
{ _phi = normalize(_phi + a._phi );
}

inline void BesAngle::operator += (double a)
{ _phi = normalize(_phi + a); 
}

inline void BesAngle::operator -= (BesAngle a)
{ _phi = normalize(_phi - a._phi ); 
}

inline void BesAngle::operator -= (double a)
{ _phi = normalize(_phi - a); 
}

inline void BesAngle::operator *= (double x)
{ _phi = normalize(_phi*x); 
}

inline void BesAngle::operator /= (double x)
{ _phi = normalize(_phi/x); 
}

inline int BesAngle::sector(int max)
{ 
  double phi = _phi;
  if (phi < 0) phi += Constants::twoPi;
  return ( int (phi / (Constants::twoPi / max) ) + 1 );
}

inline int BesAngle::sector(int max, BesAngle phi_0)
{ 
  BesAngle t( _phi - phi_0._phi);
  return t.sector(max);
}

inline double sin(const BesAngle a)
{ return sin(a._phi); }

inline double cos(const BesAngle a)
{ return cos(a._phi); }

inline double tan(const BesAngle a)
{ return tan(a._phi); }

#endif


