//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHotListFull.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
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

//#include "BaBar/BaBar.h"
#include "TrkBase/TrkHotListFull.h"
#include "TrkBase/TrkPredicates.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkView.h"
//#include "ErrLogger/ErrLog.h"
#include "MdcRecoUtil/BesCollectionUtils.h"
#include <assert.h>
#include <algorithm>
#include <functional>

// Default ctor
// Special case of ctor below:

TrkHotListFull::TrkHotListFull()
{
}


TrkHotListFull::TrkHotListFull(const TrkHotList& inHots,TrkBase::Functors::cloneHot f)
{
	//TrkHotListFull::clone come here
    _hotlist.reserve(dfltCapac());
//Clones Hots, and makes each point at the new track.
    for (TrkHotList::hot_iterator i = inHots.begin();i!=inHots.end();++i) {
//	    i->printAll(std::cout);//yzhang debug
         _hotlist.push_back(f(*i));
    }
}


TrkHotListFull::TrkHotListFull(TrkHotList& inHots, TrkBase::Functors::setParent f)
{
    _hotlist.reserve(dfltCapac());
// shallow copy the hots and remove from input hotList
  for (TrkHotList::nc_hot_iterator i = inHots.begin();i!=inHots.end();++i) {
      _hotlist.push_back(f(*i));
  }
  inHots.hotlist().clear();
}


TrkHotList*
TrkHotListFull::clone(TrkBase::Functors::cloneHot f) const
{
  return new TrkHotListFull(*this, f);
}

TrkHotListFull::~TrkHotListFull()
{
//	std::cout << " ~TrkHotListFull" << std::endl;//yzhang debug
// turn the parents off before deleting hots.  This avoids a cyclic delete error
// when deleting a track
//   std::for_each(begin(),end(),setParent(0));
  std::for_each(_hotlist.begin(),_hotlist.end(),bes::Collection::DeleteObject());
}

size_t
TrkHotListFull::dfltCapac()
{
  static size_t _dfltCapac = 75;
  return _dfltCapac;
}

void
TrkHotListFull::updateHots()
{
  std::for_each(begin(),end(),updateMeasurement(0,false));
  sort();
}

void
TrkHotListFull::append(TrkHitOnTrk* newHot)
{
  _hotlist.push_back(newHot);
}

void
TrkHotListFull::remove(TrkHitOnTrk* deadHot)
{
  typedef std::vector<TrkHitOnTrk*>::iterator iter_t;
  iter_t i = std::find(_hotlist.begin(),_hotlist.end(),deadHot);
  if (i!=_hotlist.end()) {
    delete *i;
    _hotlist.erase(i);
  } else
    std::cout<<"ErrMsg(error) "<< " you asked to remove a hit which I don't have! " << std::endl;
}

TrkHitOnTrk*
TrkHotListFull::findHot(const TrkFundHit* theHit) const
{
  TrkBase::Predicates::hotMatchesFundHit match(theHit);
  TrkHotList::hot_iterator i = std::find_if(begin(),end(),match);
  return i==end()?0:const_cast<TrkHitOnTrk*>( i.get() );  // FIXME: return (non)const TrkHitOnTrk from (non)const
}

int
TrkHotListFull::nActive(TrkEnums::TrkViewInfo view) const
{
  int nAct = 0;
  for (TrkHotList::hot_iterator i = begin();i!=end();++i) {
    if (i->isActive())
      if(view == TrkEnums::bothView || i->whatView() == view)++nAct;
  }
  return nAct;
}

int
TrkHotListFull::nHit(TrkEnums::TrkViewInfo view) const
{
  if(view == TrkEnums::bothView)
    return end()-begin();
  else {
    int nAct = 0;
    for (TrkHotList::hot_iterator i = begin();i!=end();++i) {
      if(i->whatView() == view)++nAct;
    }
    return nAct;
  }
}

bool
TrkHotListFull::hitCapable() const
{
  return true;
}

int
TrkHotListFull::nMdc(TrkEnums::TrkViewInfo view) const
{
  bool activeOnly(true);
  TrkBase::Predicates::isMdcHitOnTrack mdc(activeOnly);
  TrkBase::Predicates::hasView v(view);
// FIXME: W6U1 doesn't have std::count (at least, not in the config used by BaBar!)
// FIXME: std::compose2 is an SGI extension...
//  return std::count(begin(),end(),std::compose2(std::logical_and<bool>(),mdc,v);
  int n = 0;
  for (TrkHotList::hot_iterator i = begin();i!=end();++i)
      if(mdc(*i)&&v(*i)) ++n;
  return n;
}

int
TrkHotListFull::nSvt(TrkEnums::TrkViewInfo view) const
{
  bool activeOnly(true);
  TrkBase::Predicates::isSvtHitOnTrack svt(activeOnly);
  TrkBase::Predicates::hasView v(view);
// FIXME: W6U1 doesn't have std::count (at least, not in the config used by BaBar!)
// FIXME: std::compose2 is an SGI extension...
//  return std::count(begin(),end(),std::compose2(std::logical_and<bool>(),svt,v);
  int n = 0;
  for (TrkHotList::hot_iterator i = begin();i!=end();++i)
          if (svt(*i)&&v(*i)) ++n;
  return n;
}

double
TrkHotListFull::startFoundRange() const
{
  TrkBase::Predicates::isHotActive active;
  TrkHotList::hot_iterator i = std::find_if(begin(),end(),active);
  return i == end() ? 9999 : i->fltLen();
}

double
TrkHotListFull::endFoundRange() const
{
  double maxFlt = -9999;
  TrkBase::Predicates::isHotActive predicate;
  TrkHotList::hot_iterator i = end();
  TrkHotList::hot_iterator b = begin();
  while (i-- != b) {
    if (predicate(*i)) {
      maxFlt = i->fltLen();
      break;
    }
  }
  return maxFlt;
}


TrkView
TrkHotListFull::svtView(int layer) const
{
  TrkView retval;
  bool activeOnly(true);
  TrkBase::Predicates::isSvtHitOnTrack svt(activeOnly);
  TrkBase::Predicates::isLayer l(layer);
  // FIXME: std::compose2 is an SGI extension...
  for(TrkHotList::hot_iterator i=begin();i!=end();++i)
        if (svt(*i)&&l(*i)) retval.addView(i->whatView());
  return retval;
}

unsigned
TrkHotListFull::firstMdcLayer() const
{
  unsigned firstlay(50);
  bool activeOnly(true);
  TrkBase::Predicates::isMdcHitOnTrack mdc(activeOnly);
  for (TrkHotList::hot_iterator i = begin();i!=end();++i) {
    if (mdc(*i)) firstlay = std::min(firstlay,i->layerNumber());
  }
  return firstlay<50?firstlay:0;
}

unsigned
TrkHotListFull::lastMdcLayer() const
{
  unsigned lastlay(0);
  bool activeOnly(true);
  TrkBase::Predicates::isMdcHitOnTrack mdc(activeOnly);
  for (TrkHotList::hot_iterator i = begin();i!=end();++i) {
    if (mdc(*i)) lastlay = std::max(lastlay,i->layerNumber());
  }
  return lastlay;
}

const std::vector<TrkHitOnTrk*>&
TrkHotListFull::hotlist() const
{
  return _hotlist;
}

std::vector<TrkHitOnTrk*>&
TrkHotListFull::hotlist()
{
  return _hotlist;
}

bool
TrkHotListFull::isActive(unsigned ihot) const {
  if(ihot<_hotlist.size())
    return _hotlist[ihot]->isActive();
  else
    return false;
}
	    
