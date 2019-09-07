//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitUse.h,v 1.2 2011/09/26 00:55:50 zhangy Exp $
//
// Description:
//     Minimal description of how a hit is to be used on a track; used for 
//     adding hits to tracks.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef MDCHITUSE_HH
#define MDCHITUSE_HH

#include "TrkBase/TrkHitUse.h"
#include "TrkBase/TrkHitOnTrkUpdater.h"

class MdcHit;

// Class interface //
class MdcHitUse : public TrkHitUse, TrkHitOnTrkUpdater {

public:
  MdcHitUse(const MdcHit&, double fltLen, int ambig,
            bool active=true, int usable=1);
  virtual ~MdcHitUse();
  virtual bool operator==(const TrkHitUse&) const;

  int ambig() const                                          {return _ambig;}
  virtual TrkHitOnTrk* createHitOnTrk(const TrkRep&) const;
  //virtual TrkHitOnTrk* createHitOnTrk(const TrkRep&, bool maintainAmbiguity=false) const;//2011-05-04 

  // Cast (safe) of base hit
  const MdcHit* mdcHit() const;

private:
  int _ambig;
  double _t0;

  // Preempt 
  MdcHitUse& operator= (const MdcHitUse&);
  MdcHitUse(const MdcHitUse &);
};

#endif
