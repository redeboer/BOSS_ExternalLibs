// mdcWrapAng.h
/* Handle wrap-around problems in phi: phi1 is the reference angle, and phi2 
   is adjusted, if necessary, so that it is close to phi1.  The assumption is 
   that phi1 and phi2 are in fact supposed to be close to one another. */

#ifndef MDCWRAPANG_H
#define MDCWRAPANG_H

#include "MdcGeom/Constants.h"
static const double &twpi = Constants::twoPi;

inline double mdcWrapAng( double phi1, double phi2) {

  double diff;

  diff = phi2 - phi1;
  if (diff > 0.5*twpi) phi2 -= twpi;
  else if (diff < -0.5*twpi) phi2 += twpi;

  return phi2;
}
#endif
