#ifndef ENTRANCEANGLE_H
#define ENTRANCEANGLE_H
////////////////////////////////
//
// BESIII MDC entrance angles are in radians, and degress should inly be used
// when absolutely necessary. Automatic conversions to and from
// the radians form are provided, but you have to manually
// go to and from degrees
//
// By convention, angles are represented as [-pi/2, pi/2]
//
#include "MdcGeom/Constants.h"
#include <math.h>


class EntranceAngle
{
public:
  inline EntranceAngle();
  inline EntranceAngle(const double);
  inline ~EntranceAngle();

  inline operator double() const  { return _phi;};   // automatic conversion to double

  inline double rad() const;  
  inline double deg() const;  
  // convention : returns value in [-90, 90]

protected:
  double _phi;

  inline static double normalize(double);

};

//
// Methods for EntranceAngle
//

inline double EntranceAngle::normalize(double angle) {
  while (angle < - Constants::halfPi) {
    angle += Constants::pi;
  }
  while (angle > Constants::halfPi) {
    angle -= Constants::pi;
  }
  return angle;
}

inline EntranceAngle::EntranceAngle() : _phi(0)
{ }

inline EntranceAngle::EntranceAngle(const double phi) : _phi(normalize(phi))
{}

inline EntranceAngle::~EntranceAngle() {}

inline double EntranceAngle::rad() const
{ return _phi; }

inline double EntranceAngle::deg() const
{ return _phi *  Constants::radToDegrees; }

#endif
