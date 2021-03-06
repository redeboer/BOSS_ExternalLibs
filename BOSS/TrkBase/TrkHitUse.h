//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHitUse.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     Carries information about how a hit is to be used on a track, and 
// is able to create the appropriate kind of HitOnTrk.  Abstract base class.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef TRKHITUSE_HH
#define TRKHITUSE_HH

class TrkFundHit;
class TrkHitOnTrk;
class TrkRep;
//class SvtHit;
class MdcHit;

// Class interface //
class TrkHitUse {

public:
  TrkHitUse(const TrkFundHit&, double fltLen, bool active=true, 
	    int usable=1);
  virtual ~TrkHitUse();

  bool                      isActive() const                {return _isActive;}
  int                       isUsable() const                {return _isUsable;}
  double                    fltLen()   const                {return _fltLen;}
  const TrkFundHit&         hit()      const                {return _hit;}
  virtual const MdcHit*     mdcHit()   const;
//  virtual const SvtHit*     svtHit()   const;
 
  virtual TrkHitOnTrk*      createHitOnTrk(const TrkRep&) const = 0;
  void                      setFltLen(double flt)           {_fltLen = flt;}

  // operator== tests on equality of contents: FundHit address and anything 
  //  relevant in the derived classes (e.g. ambiguity for Mdc).  It does 
  //  not test flight length.
  virtual bool operator==(const TrkHitUse&) const = 0;
  
private:	
  const TrkFundHit& _hit;
  bool _isActive;
  int _isUsable;
  double _fltLen;

  // Preempt 
  TrkHitUse&   operator= (const TrkHitUse&);
  TrkHitUse(const TrkHitUse &);
};

#endif
