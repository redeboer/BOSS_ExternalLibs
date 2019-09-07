//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkFit.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     Implements a few basic TrkAbsFit functions (most are left for TrkRep and
//     subclasses).
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//            modified by Justin Albert
//------------------------------------------------------------------------

#ifndef TRKFIT_HH
#define TRKFIT_HH

#include "TrkBase/TrkAbsFit.h"
class ChisqConsistency;

// Class interface //
class TrkFit : public TrkAbsFit {

public:
  virtual ChisqConsistency    chisqConsistency() const = 0;
  virtual bool validFlightLength(double fltL,double tolerance=0.0)      const;
  void printType(std::ostream& ostr) const;

  virtual int                 nActive()                   const = 0;
  virtual int                 nSvt()                      const = 0;
  virtual int                 nMdc()                      const = 0;
  virtual PdtPid::PidType     particleType()              const = 0;
  virtual TrkExchangePar    helix(double fltL)            const = 0;
  virtual double            arrivalTime(double fltL)      const = 0;
  virtual double            startFoundRange()             const = 0;
  virtual double            endFoundRange()               const = 0;

protected:
  TrkFit();
  virtual ~TrkFit();
private:
  // Preempt
  TrkFit&   operator= (const TrkFit&);
  TrkFit(const TrkFit &);
};
#endif
