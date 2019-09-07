//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitUse.cxx,v 1.2 2011/09/26 00:55:50 zhangy Exp $
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

//#include "BaBar/BaBar.h"
#include "MdcData/MdcHitUse.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcData/MdcHit.h"


MdcHitUse::MdcHitUse(const MdcHit& thehit, double flt, int amb,
                     bool active, int usable) :
  TrkHitUse(thehit, flt, active, usable) 
{
    _ambig = amb;
}

MdcHitUse::~MdcHitUse() 
{ }

TrkHitOnTrk* 
MdcHitUse::createHitOnTrk(const TrkRep& rep) const 
//MdcHitUse::createHitOnTrk(const TrkRep& rep, bool maintainAmbiguity) const 
{
  // This is a kludge -- fix once MdcHitOnTrack gets more cleaned up
  //   (time should not be used for anything (I hope)
  // Also note that I temporarily have to cast off const from TrkRep
  const MdcHit *h=mdcHit(); assert(h!=0);
  MdcRecoHitOnTrack tempHot(*h, ambig(), 0.);
  TrkHitOnTrk* newHot = tempHot.clone(&const_cast<TrkRep&>(rep));
  newHot->setFltLen( fltLen() );
  updateMeasurement(*newHot);
  /*
  //yzhang changed 2011-05-04 
  std::cout<< __FILE__ << "   " << __LINE__ << "  maintainAmbiguity "<<maintainAmbiguity<<std::endl;
  if( maintainAmbiguity )
  {
    std::cout<< __FILE__ << "   " << __LINE__ << " 2d  "<<false<<std::endl;
    updateMeasurement(*newHot,0, false); //2d maintainAmbiguity = false
  }else{
    std::cout<< __FILE__ << "   " << __LINE__ << " 3d  "<<true<<std::endl;
    updateMeasurement(*newHot,0, true); //3d maintainAmbiguity = true 
  }
  */
  //zhangy
  return newHot;
}

bool 
MdcHitUse::operator==(const TrkHitUse& rhs) const 
{
  // This is not going to win any design prizes:
  if (mdcHit() == 0 || rhs.mdcHit() ==0) return false;
  const MdcHitUse& x = static_cast<const MdcHitUse&>(rhs);
  return ( ambig() == x.ambig() && TrkHitUse::operator==(x) );
}

const MdcHit* 
MdcHitUse::mdcHit() const 
{
  return static_cast<const MdcHit*>(&(hit()));
}
