#include "RecTrackList/RecTrackList.h"

RecTrackList::RecTrackList()
{
	myTrkId = -1;
	myMdcTrk = 0;
	myKalTrk = 0;
	myMdcDedx = 0;
	myExtTrk = 0;
	myTofTrk = 0;
	myRecEmcShower = 0;
	myMucTrk = 0;
	myMdcTrkIdx = -1;
	myKalTrkIdx = -1;
	myMdcDedxIdx = -1;
	myExtTrkIdx = -1;
	myTofTrkIdx = -1;
	myRecEmcShowerIdx = -1;
	myMucTrkIdx = -1;
}

RecTrackList::RecTrackList(const RecTrackList & trk) 
{
	myTrkId = trk.myTrkId;
	myMdcTrk = trk.myMdcTrk;
	myKalTrk = trk.myKalTrk;
	myMdcDedx = trk.myMdcDedx;
	myExtTrk = trk.myExtTrk;
	myTofTrk = trk.myTofTrk;
	myRecEmcShower = trk.myRecEmcShower;
	myMucTrk = trk.myMucTrk;
	myMdcTrkIdx = trk.myMdcTrkIdx;
	myKalTrkIdx = trk.myKalTrkIdx;
	myMdcDedxIdx = trk.myMdcDedxIdx;
	myExtTrkIdx = trk.myExtTrkIdx;
	myTofTrkIdx = trk.myTofTrkIdx;
	myRecEmcShowerIdx = trk.myRecEmcShowerIdx;
	myMucTrkIdx = trk.myMucTrkIdx;
}

RecTrackList & RecTrackList::operator = (const RecTrackList & trk)
{
	myTrkId = trk.myTrkId;
	myMdcTrk = trk.myMdcTrk;
	myKalTrk = trk.myKalTrk;
	myMdcDedx = trk.myMdcDedx;
	myExtTrk = trk.myExtTrk;
	myTofTrk = trk.myTofTrk;
	myRecEmcShower = trk.myRecEmcShower;
	myMucTrk = trk.myMucTrk;
	myMdcTrkIdx = trk.myMdcTrkIdx;
	myKalTrkIdx = trk.myKalTrkIdx;
	myMdcDedxIdx = trk.myMdcDedxIdx;
	myExtTrkIdx = trk.myExtTrkIdx;
	myTofTrkIdx = trk.myTofTrkIdx;
	myRecEmcShowerIdx = trk.myRecEmcShowerIdx;
	myMucTrkIdx = trk.myMucTrkIdx;

	return *this;
}
