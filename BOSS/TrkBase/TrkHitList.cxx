//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHitList.cxx,v 1.5 2010/09/26 00:31:59 zhangy Exp $
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
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkHotList.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkHitOnTrkIter.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkBase/TrkFundHit.h"
#include "TrkBase/TrkRep.h"
#include "TrkBase/TrkHitUse.h"
#include "TrkBase/TrkRepIter.h"
#include "MdcRecoUtil/PdtPid.h"//yzhang debug
#include <iostream>
using namespace std;

TrkHitList::TrkHitList(TrkRecoTrk* trk, PdtPid::PidType hypo)
        : _theTrack(trk)
        , _myHypo(hypo)
{
}

TrkHitList::~TrkHitList()
{
}

TrkRep*
TrkHitList::theRep()
{
  return _theTrack->getRep(_myHypo);
}

const TrkRep*
TrkHitList::theRep() const
{
  return _theTrack->getRep(_myHypo);
}

const TrkHotList&
TrkHitList::hotList() const
{
  const TrkHotList* x = theRep()->hotList();
  assert(x!=0);
  return *x;
}

TrkErrCode
TrkHitList::fit()
{
  _theTrack->_fitNumber[_myHypo]++;

  TrkErrCode err = theRep()->fit();
  theRep()->hotList()->sort();
  return err;
}

bool
TrkHitList::removeHit(const TrkFundHit *theHit)
{
  // This would be more efficient if the Rep did the finding of the Hot
  //   (save one search through each hotlist).
  if (theHit == 0) return false;
  if (!theHit->usedOnTrack(_theTrack)) {
#ifdef MDCPATREC_WARNING
    cout <<"ErrMsg(warning) "<<
      "TrkHitList: you just deleted a hit that was not on the track." << endl;
#endif
    return false;
  }
  std::pair<TrkRepIter,TrkRepIter> reps = _theTrack->uniqueReps();
  for (TrkRepIter i= reps.first; i != reps.second; ++i) {
    // Find the Hot and ask the Rep to remove it
    TrkHitOnTrk* h = i->hotList()->findHot(theHit);
    if (h != 0) i->removeHot(h);
  }
  return true;
}

TrkHitOnTrk*
TrkHitList::appendHot(const TrkHitOnTrk *theHot)
{
  if (theHot == 0) return 0;
  // Test whether hit already on this track
  if (theHot->hit() != 0 && theHot->hit()->usedOnTrack(_theTrack)) {
#ifdef MDCPATREC_WARNING
      cout <<"ErrMsg(warning)" 
	   << "You just tried to add a hit to a track twice. "
	   <<" Don't do that. "<< endl;
#endif
      return 0;
  }
  TrkHitOnTrk* defaultHot = 0;
  std::pair<TrkRepIter,TrkRepIter> reps = _theTrack->uniqueReps();
  for (TrkRepIter i=reps.first; i != reps.second; ++i) {
    TrkHitOnTrk* h = theHot->clone(i.get());
    i->addHot(h);
    if (i->particleType() == _theTrack->defaultType()) defaultHot = h;
  }
  return defaultHot;
}

TrkHitOnTrk*
TrkHitList::appendHit(const TrkHitUse& theHit)
{
  // Test whether hit already on this track
  if (theHit.hit().usedOnTrack(_theTrack)) {
#ifdef MDCPATREC_WARNING
    cout <<"ErrMsg(warning)" 
	 <<"You just tried to add a hit to a track twice."
	 <<" Don't do that. " << endl;
#endif
    return 0;
  }
  TrkHitOnTrk* defaultHot = 0;
    
  std::pair<TrkRepIter,TrkRepIter> reps = _theTrack->uniqueReps();
  for (TrkRepIter i= reps.first; i != reps.second; ++i) {
    TrkHitOnTrk* h = theHit.createHitOnTrk(*i);
    
    i->addHot(h);
    if (i->particleType() == _theTrack->defaultType()) defaultHot = h;
  }
  return defaultHot;
}

bool
TrkHitList::append(const TrkHitList& list)
{
   bool x(true);
   for(TrkHitList::hot_iterator i = list.begin(); i!=list.end();++i) {
        TrkHitOnTrk* h = appendHot(i.get());
        x = ( x && h!=0);
   }
   return x;
}
