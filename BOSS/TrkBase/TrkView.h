//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkView.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
//  Description:
//  Class TrkView.  Simple struct to hold view information
//  in a bit 
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Copyright Infomation;
//	Copyright (C) 2002	Lawrence Berkeley Laboratory
//
// Author(s): Dave Brown 10/25/02
//
//------------------------------------------------------------------------

#ifndef TRKVIEW_H
#define TRKVIEW_H

#include "TrkBase/TrkEnums.h"

class TrkView {
public:
// construct from a view
  TrkView(TrkEnums::TrkViewInfo view=TrkEnums::noView);
// construct from data
  TrkView(unsigned short& data);
// copy and equivalence are OK
  TrkView(const TrkView&);
  TrkView& operator =(const TrkView&);
  virtual ~TrkView();
// allow adding views
  void addView(TrkEnums::TrkViewInfo view);
// accessor
  TrkEnums::TrkViewInfo view() const;
// logical operations
  bool contains(const TrkView& other) const;
  bool contains(TrkEnums::TrkViewInfo view) const;
  bool operator == (const TrkView& other) const;
  bool operator != (const TrkView& other) const;
// direct accessor for binary comparisons
  unsigned short viewData() const { return _view; }
private:
  unsigned short _view;
// some useful statics; make them public so they can be used
public:
  static const unsigned short _xyview;
  static const unsigned short _zview;
  static const unsigned short _bothview;
};

#endif
