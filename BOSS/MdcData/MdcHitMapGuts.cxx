//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitMapGuts.cxx,v 1.4 2009/09/22 06:28:08 zhangy Exp $
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
#include "MdcData/MdcHitMapGuts.h"
#include "MdcData/MdcHit.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcGeom/MdcLayer.h"
#include <iostream>

MdcHitMapGuts::MdcHitMapGuts(const MdcDetector& gm)
        : _hits(gm.nLayer())
{
  unsigned n = _hits.size();
  for (unsigned l = 0; l < n; ++l) {
    const MdcLayer* lay = gm.Layer(l);
    _hits[l].resize(lay->nWires(),0);  // resizes and zeroes
  }
}

MdcHitMapGuts::~MdcHitMapGuts()
{
}

MdcHitMapGuts*
MdcHitMapGuts::instance(const MdcDetector& gm)
{
  static MdcHitMapGuts theInstance(gm);
  theInstance.clear();
  return &theInstance;
}

void
MdcHitMapGuts::clear()
{
  for (MdcHitMap_t::iterator j = _hits.begin(); j != _hits.end(); ++j) {
    std::fill(j->begin(),j->end(),static_cast<MdcHit *>(0));
  }
}

void
MdcHitMapGuts::addHit(MdcHit& theHit)
{
  _hits[theHit.layernumber()][theHit.wirenumber()] = &theHit;
}

