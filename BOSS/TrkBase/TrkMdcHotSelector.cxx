//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkMdcHotSelector.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//   class TrkMdcHotSelector.  Trivial implementation of TrkHotSelector for
//   selecting Mdc hots.
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
#include "TrkBase/TrkMdcHotSelector.h"
#include "TrkBase/TrkHitOnTrk.h"

TrkMdcHotSelector::TrkMdcHotSelector(bool activeOnly) :
  _ignoreActive(!activeOnly)
{}

TrkMdcHotSelector::~TrkMdcHotSelector()
{}

bool
TrkMdcHotSelector::useHot(const TrkHitOnTrk& hot) const {
  return (hot.mdcHitOnTrack() != 0) &&
    (_ignoreActive || hot.isActive());
}

