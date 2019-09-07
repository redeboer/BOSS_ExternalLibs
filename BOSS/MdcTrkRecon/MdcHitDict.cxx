//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitDict.cxx,v 1.1.1.1 2005/04/21 06:23:43 maqm Exp $
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
#include "MdcTrkRecon/MdcHitDict.h"
#include "MdcData/MdcHit.h"
#include "MdcTrkRecon/MdcSegUsage.h"

//------------------------------------------------------------------------
MdcHitDict::MdcHitDict() //: m_segUsage(hashFun, 1009)
//------------------------------------------------------------------------
{}

//------------------------------------------------------------------------
MdcHitDict::~MdcHitDict()
//------------------------------------------------------------------------
{}


//------------------------------------------------------------------------
MdcSegUsage* 
MdcHitDict::get(const MdcHit* theHit) const {
//------------------------------------------------------------------------
//  return m_segUsage.findValue(theHit);
  return (*m_segUsage.find(theHit)).second;
 
}

//------------------------------------------------------------------------
void 
MdcHitDict::put(MdcHit* theHit, MdcSegUsage* theAux) {
//------------------------------------------------------------------------
//  m_segUsage.insertKeyAndValue(theHit, theAux);
  m_segUsage[theHit] = theAux;
}

//------------------------------------------------------------------------
void 
MdcHitDict::clear() {
//------------------------------------------------------------------------
  m_segUsage.clear();
}
