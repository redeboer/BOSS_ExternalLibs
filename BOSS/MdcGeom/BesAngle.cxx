//--------------------------------------------------------------------------
//
// Description:
//	Class BesAngle
//
//------------------------------------------------------------------------
#include "MdcGeom/BesAngle.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MdcGeom/Constants.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

const double BesAngle::pi        = Constants::pi;
const double BesAngle::twoPi     = Constants::twoPi;
const double BesAngle::toDegrees = Constants::radToDegrees;
/* hxt 
// The followings characters are used in DegString() 
// for printout in degrees
#ifdef HPUX
const HepString BesAngle::degChar = "°";
#else
const HepString BesAngle::degChar = "^";
#endif

const HepString BesAngle::deg1Char =  "'";
const HepString BesAngle::deg2Char = "\"";

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

HepString BesAngle::degString() const
{
  float tmp;
  int deg, deg_, deg__;
  HepString sign = "";
  
  if ((tmp = this->deg()) < 0) 
  { sign = "-"; tmp = -tmp; };
  deg   = int(tmp);
  deg_  = int(tmp = 60*(tmp - deg));
  deg__ = int(60*(tmp - deg_));
  
  return 
  (
    sign + 
    HepString(deg)+degChar+
    HepString(deg_)+deg1Char+
    HepString(deg__)+deg2Char
  );
}

hxt */
