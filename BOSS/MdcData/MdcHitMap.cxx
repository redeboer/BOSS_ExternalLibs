//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitMap.cxx,v 1.2 2005/07/18 03:16:11 zhangy Exp $
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
#include "MdcData/MdcHitMap.h"
#include <assert.h>

MdcHitMap::MdcHitMap(const MdcDetector& gm)
{
  _guts = MdcHitMapGuts::instance(gm);
  assert(_guts != 0);
}

MdcHitMap::~MdcHitMap()
{}
