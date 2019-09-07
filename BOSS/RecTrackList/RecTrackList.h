#ifndef RECTRACKLIST_H
#define RECTRACKLIST_H

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "EventModel/EventModel.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "ExtEvent/RecExtTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "MucRecEvent/RecMucTrack.h"

using namespace EventModel;

extern const CLID &CLID_RecTrackList;

class RecTrackList:virtual public ContainedObject
{
	public:
		virtual const CLID& clID() const {return  RecTrackList::classID();}
		static  const CLID& classID() { return CLID_RecTrackList;}
		
		RecTrackList();
		~RecTrackList() {;}
		
		RecTrackList(const RecTrackList &);

		bool IsMdcTrkValid() {return (myMdcTrk != 0);}
		bool IsMdcKalTrkValid() {return (myKalTrk != 0);}
		bool IsDedxValid() {return (myMdcDedx != 0);}
		bool IsExtTrkValid() {return (myExtTrk != 0);}
		bool IsTofTrkValid() {return (myTofTrk != 0);}
		bool IsEmcTrkValid() {return (myRecEmcShower != 0);}
		bool IsMucTrkValid() {return (myMucTrk != 0);}

		int GetTrkId() {return myTrkId;}
		RecMdcTrack* MdcTrk()  {return myMdcTrk;}
		RecMdcKalTrack* MdcKalTrk()  {return myKalTrk;}
		RecMdcDedx* DedxTrk()  {return myMdcDedx;}
		RecExtTrack* ExtTrk()  {return myExtTrk;}
		RecTofTrack* TofTrk()  {return myTofTrk;}
		RecEmcShower* EmcTrk()  {return myRecEmcShower;}
		RecMucTrack* MucTrk()  {return myMucTrk;}

		void setTrackID(const int trkID) { myTrkId = trkID;}
		void setMdcTrk( RecMdcTrack* trk) { myMdcTrk = trk;}
		void setMdcKalTrk( RecMdcKalTrack* trk) { myKalTrk = trk;}
		void setDedxTrk( RecMdcDedx* trk) { myMdcDedx = trk;}
		void setExtTrk( RecExtTrack* trk) { myExtTrk = trk;}   
		void setTofTrk( RecTofTrack* trk) { myTofTrk = trk;}
		void setEmcTrk( RecEmcShower* trk) { myRecEmcShower = trk;}
		void setMucTrk( RecMucTrack* trk) { myMucTrk = trk;}
		
		int MdcTrkIdx() { return myMdcTrkIdx;}
		int MdcKalTrkIdx() { return myKalTrkIdx;}
		int DedxTrkIdx() { return myMdcDedxIdx;}
		int ExtTrkIdx() { return myExtTrkIdx;}
		int TofTrkIdx() { return myTofTrkIdx;}
		int EmcTrkIdx() { return myRecEmcShowerIdx;}
		int MucTrkIdx() { return myMucTrkIdx;}

		void setMdcTrkIdx( int idx ) { myMdcTrkIdx = idx ;}
		void setMdcKalTrkIdx( int idx ) { myKalTrkIdx = idx ;}
		void setDedxTrkIdx( int idx ) { myMdcDedxIdx = idx ;}
		void setExtTrkIdx( int idx ) { myExtTrkIdx = idx ;}   
		void setTofTrkIdx( int idx ) { myTofTrkIdx = idx ;}
		void setEmcTrkIdx( int idx ) { myRecEmcShowerIdx = idx ;}
		void setMucTrkIdx( int idx ) { myMucTrkIdx = idx;}


		RecTrackList & operator = (const RecTrackList &);
		
		
	private:
		int myTrkId;
		SmartRef<RecMdcTrack> myMdcTrk;
		SmartRef<RecMdcKalTrack> myKalTrk;
		SmartRef<RecMdcDedx> myMdcDedx;
		SmartRef<RecExtTrack> myExtTrk;
		SmartRef<RecTofTrack> myTofTrk;
		SmartRef<RecEmcShower> myRecEmcShower;
		SmartRef<RecMucTrack> myMucTrk;

		int myMdcTrkIdx;
		int myKalTrkIdx;
		int myMdcDedxIdx;
		int myExtTrkIdx;
		int myTofTrkIdx;
		int myRecEmcShowerIdx;
		int myMucTrkIdx;
};

typedef ObjectVector<RecTrackList> RecTrackListCol;

typedef RecTrackListCol::iterator RecTrackIterator;
typedef RecMdcTrackCol::iterator RecMdcTrackIterator;
typedef RecMdcKalTrackCol::iterator RecMdcKalTrackIterator;


#endif
