// MdcTrack.h
//   class to hold a drift chamber track; contains a TrkRecoTrk.

#ifndef MDCTRACK_H
#define MDCTRACK_H

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
class MdcSeg;
class MdcSegListO;
class MdcLayer;
class BesAngle;
class TrkRep;
class TrkRecoTrk;
class TrkExchangePar;
class TrkContext;

// Class definition
class MdcTrack  {
public:
  MdcTrack(TrkRecoTrk* aTrack);
  MdcTrack(int nsupers, const TrkExchangePar& par, double chisq, 
	   TrkContext&, double trackT0);
  ~MdcTrack();
  bool operator==(const MdcTrack& tk) const;

  TrkRecoTrk& track()                                     {return *_theTrack;}
  const TrkRecoTrk& track() const                         {return *_theTrack;}
  void setTrack(TrkRecoTrk* trk)                          {_theTrack = trk;}
  int hasCurled() const {return _haveCurled;}
  const MdcLayer *firstLayer() const {return _firstLayer;}
  const MdcLayer *lastLayer() const {return _lastLayer;}

  void setHasCurled(bool c = true)                           {_haveCurled = c;}
  void setFirstLayer(const MdcLayer *l)                      {_firstLayer = l;}
  void setLastLayer(const MdcLayer *l)                       {_lastLayer = l;}

  //projectToR ret -1 if no intersect
  int projectToR(double radius, BesAngle &phiIntersect, int lCurl = 0) const ; 
  int projectToR(double radius, BesAngle &phiIntersect, double &arcLength, 
		 int lCurl = 0) const; 
  //yzhang for store to TDS
  void storeTrack(int trackId, RecMdcTrackCol* trackList, RecMdcHitCol* hitList,int tkStat);
  //zhangy
private:
  TrkRecoTrk* _theTrack;         
  const MdcLayer *_firstLayer;  // first layer known to have good hit on track
  const MdcLayer *_lastLayer;   // you figure it out
  int _haveCurled;  // includes hits past curl point

  MdcTrack& operator= (const MdcTrack&);
  MdcTrack(const MdcTrack&);
};

#endif







