//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkIdManDumb.cxx,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
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
//#include "BaBar/BaBar.hh"
#include "TrkFitter/TrkIdManDumb.h"

//------------------------------------------------------------------------
TrkIdManDumb::TrkIdManDumb(long val) {
//------------------------------------------------------------------------
  _theValue = val;
}

//------------------------------------------------------------------------
TrkIdManDumb::~TrkIdManDumb() {}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
long
TrkIdManDumb::nextId() {
//------------------------------------------------------------------------
  return theValue();
}

//------------------------------------------------------------------------
long
TrkIdManDumb::lastId() const {
//------------------------------------------------------------------------
  return theValue();
}

//------------------------------------------------------------------------
TrkIdManDumb*
TrkIdManDumb::instance() {
//------------------------------------------------------------------------
  static TrkIdManDumb theSingleton(-1);
  return &theSingleton;
}

void
TrkIdManDumb::setMax(long maxid) {
  _theValue = maxid;
}
