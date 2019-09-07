//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkIdManDefault.cxx,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
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
#include "TrkFitter/TrkIdManDefault.h"

//------------------------------------------------------------------------
TrkIdManDefault::TrkIdManDefault(long firstId) {
//------------------------------------------------------------------------
  _nextId = firstId;
}

//------------------------------------------------------------------------
TrkIdManDefault::~TrkIdManDefault() {
//------------------------------------------------------------------------
}

//------------------------------------------------------------------------
long
TrkIdManDefault::nextId() {
//------------------------------------------------------------------------
  long id = _nextId;
  _nextId++;
  return id;
}

//------------------------------------------------------------------------
long
TrkIdManDefault::lastId() const {
//------------------------------------------------------------------------
  return (_nextId - 1);
}

void
TrkIdManDefault::setMax(long maxid) {
  _nextId = maxid+1;
}

