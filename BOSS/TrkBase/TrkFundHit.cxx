//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkFundHit.cxx,v 1.4 2011/02/17 11:53:41 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//  We may want to inline some of these functions some day.
//
// Revision History:
//  20000523  M. Kelsey -- Add concrete printAll() implementation which
//		calls through to subclass print(), then dumps HOT list.
//------------------------------------------------------------------------
#include "TrkBase/TrkFundHit.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkHitOnTrkIter.h"
#include "TrkBase/TrkPredicates.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
// Remove!:
// MHK Restore, for use in printAll()
#include "TrkBase/TrkRecoTrk.h"
using std::endl;
using std::ostream;

int debug_nHit = 0;

TrkFundHit::TrkFundHit() {
  //std::cout << " @ TrkFundHit constructor: " << ++debug_nHit << " addr: " << this << std::endl;
}

TrkFundHit::~TrkFundHit()
{
  //std::cout << " @ ~TrkFundHit() : " << --debug_nHit << " addr: " << this << std::endl;
  // hitlist cleanup done in derived classes
}

TrkFundHit::TrkFundHit(const TrkFundHit& )
{
  assert(_hitList.empty());
  //std::cout << " @ TrkFundHit copy constructor : " << ++debug_nHit << " addr: " << this << std::endl;
}

TrkFundHit&
TrkFundHit::operator= (const TrkFundHit& x)
{
  //std::cout << " @ TrkFundHit operator= : " << debug_nHit << " addr: " << this << std::endl;
  assert(_hitList.empty()); 
  return *this;
}

const TrkHitOnTrk*
TrkFundHit::setUsedHit(const TrkHitOnTrk *hit)
{
  //  if (hitList->contains(hit)) {
  //    return;
  //  }
  //FIXME: check hot corresponds to this hit??
//	std::cout << "TrkFundHit setUsedHit "<<((MdcHit*)(hit->hit()))->layernumber()<<" "<<((MdcHit*)(hit->hit()))->wirenumber()<<endl;
//	this->printAll(std::cout);//yzhang debug
  _hitList.push_back(hit);
//        std::cout << "after push_bak " << std::endl;//yzhang debug
//	  printAll(std::cout);//yzhang debug
	  
  return hit;
}

// return hit if on list, return 0 if not on list...
const TrkHitOnTrk *
TrkFundHit::setUnusedHit(const TrkHitOnTrk *hit)
{
  //std::cout << " @ setUnusedHit() : " << debug_nHit << " addr: " << this << std::endl;
  if (_hitList.empty()) return 0;
  std::vector<const TrkHitOnTrk*>::iterator i=std::find(_hitList.begin(),_hitList.end(),hit);
  if (i==_hitList.end()) return 0;
  assert(*i==hit);
//  std::cout << "TrkFundHit setUnusedHit "<<((MdcHit*)(hit->hit()))->layernumber()<<" "<<((MdcHit*)(hit->hit()))->wirenumber()<<endl;
//this->printAll(std::cout);//yzhang debug
  _hitList.erase(i);
  return hit;
}

int
TrkFundHit::nUsedHits() const
{
  return _hitList.size();
}

const TrkHitOnTrk*
TrkFundHit::getHitOnTrack(const TrkRecoTrk *trk) const
{
  hot_iterator i = std::find_if(begin(), end(),
                                std::bind2nd(TrkBase::Predicates::isHotOnTrack(),trk));
  return (i==end()?0:i.get());
}

void
TrkFundHit::printAll(ostream& os) const
{
//  print(os);                            // Call through to get subclass info
  os << " used by " << nUsedHits() << " HOTs" << endl;
  if (usedHit()) {
    for (hot_iterator i=begin(); i != end(); ++i) {
        i->printAll(os);//yzhang debug 
//      os << "\ttrack " << i->parentTrack()->id() << ": ";
//      i->print(os);                // NOTE:  includes endl!
      //yzhang debug
      std::cout << " parentTrack" << std::endl;//yzhang debug
      i->parentTrack()->printAll(std::cout);
      //zhangy debug
    }
    os << endl;
  }
}
