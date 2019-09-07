//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkActiveHotSelector.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//   class TrkActiveHotSelector.  Trivial implementation of TrkHotSelector for
//   selecting active hots.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Copyright Information:
//	Copyright (C) 2001	Lawrence Berkeley Laboratory
//
// Author List:
//      Dave Brown 7/17/01
//------------------------------------------------------------------------

#ifndef TRKACTIVEHOTSELECTOR_HH
#define TRKACTIVEHOTSELECTOR_HH

#include "TrkBase/TrkHotSelector.h"

class TrkActiveHotSelector : public TrkHotSelector{
public:
// only a default constructor
  TrkActiveHotSelector();
  virtual ~TrkActiveHotSelector();
// The Function
  virtual bool useHot(const TrkHitOnTrk& hot) const;
private:
  bool _ignoreActive;
// disallow
  TrkActiveHotSelector(const TrkActiveHotSelector&);
  TrkActiveHotSelector& operator = (const TrkActiveHotSelector&);
};
#endif
