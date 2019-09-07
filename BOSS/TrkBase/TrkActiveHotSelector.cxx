//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkActiveHotSelector.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
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

//#include "BaBar/BaBar.hh"
#include "TrkBase/TrkActiveHotSelector.h"
#include "TrkBase/TrkHitOnTrk.h"

TrkActiveHotSelector::TrkActiveHotSelector(){}

TrkActiveHotSelector::~TrkActiveHotSelector(){}

bool
TrkActiveHotSelector::useHot(const TrkHitOnTrk& hot) const {
  return hot.isActive();
}
