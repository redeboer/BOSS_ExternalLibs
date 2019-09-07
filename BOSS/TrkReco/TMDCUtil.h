//-----------------------------------------------------------------------------
// $Id: TMDCUtil.h,v 1.6 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDCUtil.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : Utilities for MDC analyses and tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDCUtil_FLAG_
#define TMDCUtil_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "CLHEP/Alist/AList.h"

class TMLink;
class TMDCWire;
class TMDCWireHit;
class TSegment;
//class Geocdc_wire;
//struct geocdc_wire;
//struct geocdc_layer;
class MdcGeoWire;
class MdcGeoLayer;

/// Constants
extern const HepPoint3D ORIGIN;

/// ALPHA = 10000. / 2.99792458 / 15.
//#define ALPHA 222.376063

/// Converter
double chisq2confLevel(int, double);

/// geocdc utilities

// conversion of wire id to sector id
float CathodeSectorId(unsigned id);



/// TMDCWireHit utilities

int sortByWireId(const TMDCWireHit ** a, const TMDCWireHit ** b);



/// Circle utilities

int intersection(const HepPoint3D & c1,
		 double r1,
		 const HepPoint3D & c2,
		 double r2,
		 double eps,
		 HepPoint3D & x1,
		 HepPoint3D & x2);

///
void bitDisplay(unsigned);
void bitDisplay(unsigned val, unsigned firstDigit, unsigned lastDigit);

//-----------------------------------------------------------------------------

#ifdef TMDCUtil_NO_INLINE
#define inline
#else
#undef inline
#define TMDCUtil_INLINE_DEFINE_HERE
#endif

#ifdef TMDCUtil_INLINE_DEFINE_HERE


#endif

#undef inline

#endif /* TMDCUtil_FLAG_ */

