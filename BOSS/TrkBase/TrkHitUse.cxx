//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHitUse.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#include "TrkBase/TrkHitUse.h"

//------------------------------------------------------------------------
TrkHitUse::TrkHitUse(const TrkFundHit& theHit, double flt, bool active, 
		     int usable) : _hit(theHit) {
//------------------------------------------------------------------------
   _fltLen = flt;
   _isActive = active;
   _isUsable = usable;
}

//------------------------------------------------------------------------
TrkHitUse::~TrkHitUse() {
//------------------------------------------------------------------------
}

//------------------------------------------------------------------------
const MdcHit*
TrkHitUse::mdcHit()   const {
//------------------------------------------------------------------------
  return 0;
}

//------------------------------------------------------------------------
//const SvtHit* 
//TrkHitUse::svtHit()   const {
//------------------------------------------------------------------------
//  return 0;
//}

//------------------------------------------------------------------------
bool 
TrkHitUse::operator==(const TrkHitUse& rhs) const {
//------------------------------------------------------------------------
  return ( &(hit()) == &(rhs.hit()) );
}
