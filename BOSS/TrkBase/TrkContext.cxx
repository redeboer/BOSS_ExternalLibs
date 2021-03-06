//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkContext.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
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
#include "TrkBase/TrkContext.h"

//------------------------------------------------------------------------
TrkContext::TrkContext(const BField* bf) {
//------------------------------------------------------------------------
  _bf = bf;
}

//------------------------------------------------------------------------
TrkContext::TrkContext(const TrkContext &rhs) {
//------------------------------------------------------------------------
  _bf = rhs._bf;
}

//------------------------------------------------------------------------
TrkContext&   
TrkContext::operator= (const TrkContext& rhs) {
//------------------------------------------------------------------------
  if (&rhs == this) return *this;
  _bf = rhs._bf;
  return *this;
}

//------------------------------------------------------------------------
TrkContext::~TrkContext() {
//------------------------------------------------------------------------
}

//------------------------------------------------------------------------
void
TrkContext::setBField(const BField* bf) {
//------------------------------------------------------------------------
  _bf = bf;
}

//------------------------------------------------------------------------
bool 
TrkContext::operator==(const TrkContext& rhs) const {
//------------------------------------------------------------------------
  return (_bf == rhs._bf);
}
