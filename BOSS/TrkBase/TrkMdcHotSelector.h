//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkMdcHotSelector.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
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

#ifndef TRKDCHHOTSELECTOR_HH
#define TRKDCHHOTSELECTOR_HH

#include "TrkBase/TrkHotSelector.h"

class TrkMdcHotSelector : public TrkHotSelector{
public:
// only a default constructor
  TrkMdcHotSelector(bool activeOnly = true);
  virtual ~TrkMdcHotSelector();
// The Function
  virtual bool useHot(const TrkHitOnTrk& hot) const;
private:
  bool _ignoreActive;
// disallow
  TrkMdcHotSelector(const TrkMdcHotSelector&);
  TrkMdcHotSelector& operator = (const TrkMdcHotSelector&);
};
#endif
