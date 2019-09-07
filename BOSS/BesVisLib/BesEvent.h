//
// BesEvent.h
//

#ifndef BES_EVENT
#define BES_EVENT

#include <ctime>        //Long Peixun's update: time.h -> ctime
#include <TObject.h>
#include <TString.h>

#include "BesEventHeader.h"
#include "BesGeoTrack.h"

#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TEvtHeader.h"
#include "RootEventData/TDisTrack.h"
#include "RootEventData/TTrigEvent.h"
#include "RootEventData/TTrigData.h"
#include "RootEventData/TRecEvTime.h"

class BesEvent : public TObject {

private:
    Double_t 		f_Magnetic;
    BesEventHeader      fEventHeader;         // Event header
    TDigiEvent          *fDigiEvent;
    TEvtHeader          *fEvtHeader;
    TTrigEvent          *fTrigEvent;
    TRecEvTime          *fRecEvTime;

    TObjArray           *fMdcTrackCol;
    TObjArray           *fTofTrackCol;
    TObjArray           *fEmcTrackCol;
    TObjArray           *fMucTrackCol;
    TObjArray           *fExtTrackCol;

    bool		Is_tofBarrel(UInt_t status);
    bool		Is_tofCounter(UInt_t status);
public:
    BesEvent();
    virtual ~BesEvent();

    virtual void Clear(Option_t *option = "");
    virtual void Delete(Option_t *option = "");
    virtual void Print(Option_t *option = "");

    //virtual void SetEvent(TDigiEvent *digiEvent, TDisTrack *recEvent, TEvtHeader *evtHeader, TTrigEvent *trigEvent);
    //Long Peixun's update: add parameter "isRec"
    virtual void SetEvent(TDigiEvent *digiEvent, TDisTrack *recEvent, TEvtHeader *evtHeader, TRecEvTime *recEvTime, Bool_t isRec);//, TTrigEvent *trigEvent);
    virtual void SetHits();
    virtual void SetTracks(TDisTrack *recEvent);
    virtual void SetMdcTracks(TDisTrack *recEvent);
    virtual void SetTofTracks(TDisTrack *recEvent);
    virtual void SetEmcShowers(TDisTrack *recEvent);
    virtual void SetMucTracks(TDisTrack *recEvent);
    virtual void SetExtTracks(TDisTrack *recEvent);
    virtual void ConstructMdcTrackFromRec(BesGeoTrack *mdcTrack, const TRecMdcTrack *recTrack,TDisTrack *recEvent);
    virtual void ConstructMucTrackFromRec(BesGeoTrack *mucTrack, const TRecMucTrack *recTrack);
    virtual void ConstructEmcTrackFromRec(BesGeoTrack *emcTrack, const TRecEmcShower *recTrack);
    //virtual void ConstructTofTrackFromRec(BesGeoTrack *tofTrack, const TRecBTofTrack *recBTrack);
    //virtual void ConstructTofTrackFromRec(BesGeoTrack *tofTrack, const TRecETofTrack *recETrack);
    virtual void ConstructTofTrackFromRec(BesGeoTrack *tofTrack, const TRecTofTrack *recTrack,TDisTrack *recEvent);
    //virtual void ConstructExtTrackFromRec(BesGeoTrack *extTrack, const TExtTrack *recTrack);
    virtual void DrawHits(Option_t *option);
    virtual void DrawTracks(Option_t *option);
    void SetMagnetic(Double_t input) {f_Magnetic = input;}

    const TObjArray* GetMdcDigiCol() const      {
        if (fDigiEvent) return fDigiEvent->getMdcDigiCol();
        else return 0;
    }
    const TObjArray* GetTofDigiCol() const      {
        if (fDigiEvent) return fDigiEvent->getTofDigiCol();
        else return 0;
    }
    const TObjArray* GetEmcDigiCol() const      {
        if (fDigiEvent) return fDigiEvent->getEmcDigiCol();
        else return 0;
    }
    const TObjArray* GetMucDigiCol() const      {
        if (fDigiEvent) return fDigiEvent->getMucDigiCol();
        else return 0;
    }
    const Int_t      GetMdcDigiNum() const      {
        if (fDigiEvent) return fDigiEvent->getMdcDigiCol()->GetEntries();
        else return -1;
    }
    const Int_t      GetTofDigiNum() const      {
        if (fDigiEvent) return fDigiEvent->getTofDigiCol()->GetEntries();
        else return -1;
    }
    const Int_t      GetEmcDigiNum() const      {
        if (fDigiEvent) return fDigiEvent->getEmcDigiCol()->GetEntries();
        else return -1;
    }
    const Int_t      GetMucDigiNum() const      {
        if (fDigiEvent) return fDigiEvent->getMucDigiCol()->GetEntries();
        else return -1;
    }
    const TMdcDigi*  GetMdcDigi(Int_t i) const  {
        if (fDigiEvent) return fDigiEvent->getMdcDigi(i);
        else return 0;
    }   //Long Peixun's update: Remove semicolon
    const TTofDigi*  GetTofDigi(Int_t i) const  {
        if (fDigiEvent) return fDigiEvent->getTofDigi(i);
        else return 0;
    }
    const TEmcDigi*  GetEmcDigi(Int_t i) const  {
        if (fDigiEvent) return fDigiEvent->getEmcDigi(i);
        else return 0;
    }
    const TMucDigi*  GetMucDigi(Int_t i) const  {
        if (fDigiEvent) return fDigiEvent->getMucDigi(i);
        else return 0;
    }

    const TObjArray* GetMdcTrackCol(TDisTrack *recEvent) const       {
        if (recEvent)
            return recEvent->getRecMdcTrackCol();
        else return 0;
    }
    //const TObjArray* GetBTofTrackCol() const       {
    //    if (fRecEvent)
    //        return fRecEvent->getTofTrackCol();
    //    else return 0;
    //}
    const TObjArray* GetTofTrackCol(TDisTrack *recEvent) const       {
        if (recEvent)
            return recEvent->getTofTrackCol();
        else 
	  return 0;
    }
    const TObjArray* GetEmcShowerCol(TDisTrack *recEvent) const       {
      //  if (recEvent)
      //      return recEvent->getEmcShowerCol();
      //  else 
	  return 0;
    }
    const TObjArray* GetMucTrackCol(TDisTrack *recEvent) const       {
        if (recEvent)
            return recEvent->getMucTrackCol();
        else 
        return 0;
    }
    const TObjArray* GetExtTrackCol() const       {
         return 0;	// for test 
        //if (fRecEvent)
        //    return fRecEvent->getExtTrackCol();
        //else return 0;
    }
    const TObjArray* GetDedxCol(TDisTrack *recEvent)     const       {
     //   if (recEvent)
      //      return recEvent->getRecMdcDedxCol();
      //  else 
	  return 0;
    }
    const Int_t      GetMdcTrackNum(TDisTrack *recEvent) const       {
       if (recEvent)
//            return recEvent->getRecMdcTrackCol()->GetEntries();
       return recEvent->getMdcTrackNum();
     else 
	  return -1;
    }
    //const Int_t      GetBTofTrackNum() const       {
    //    if (fRecEvent)
    //        return fRecEvent->getTofTrackCol()->GetEntries();
    //    else return -1;
    //};
    const Int_t      GetTofTrackNum(TDisTrack *recEvent) const       {
        if (recEvent)
   //         return recEvent->getTofTrackCol()->GetEntries();
       return recEvent->getTofTrackNum();
        else 
	  return -1;
    }
    const Int_t      GetEmcShowerNum(TDisTrack *recEvent) const       {
      if (recEvent)
   //     return recEvent->getEmcShowerCol()->GetEntries();
     return recEvent->getEmcShowerNum(); 
       else 
	  return -1;
    }
    const Int_t      GetMucTrackNum(TDisTrack *recEvent) const       {
        if (recEvent)
         return recEvent->getMucTrackNum();
     //       return recEvent->getMucTrackCol()->GetEntries();
        else 
	  return -1;
    }
    const Int_t      GetExtTrackNum() const       {
        return -1;// for test 
        //if (fRecEvent)
        //    return fRecEvent->getExtTrackCol()->GetEntries();
        //else return -1;
    }
    const Int_t      GetDedxNum(TDisTrack *recEvent)     const       {
      //  if (fRecEvent)
      //      return fRecEvent->getRecMdcDedxCol()->GetEntries();
       // else 
	  return -1;
    }
    const TRecMdcTrack*  GetMdcTrack(Int_t i,TDisTrack *recEvent) const  {
        if (recEvent)
            return recEvent->getRecMdcTrack(i);
        else return 0;
    }
    //const TRecBTofTrack*  GetBTofTrack(Int_t i) const  {
    //    if (fRecEvent)
    //        return fRecEvent->getBTofTrack(i);
    //    else return 0;
    //};
    const TRecTofTrack*  GetTofTrack(Int_t i,TDisTrack *recEvent) const  {
        if (recEvent)
            return recEvent->getTofTrack(i);
        else
	  return 0;
    }
    const TRecEmcShower*  GetEmcShower(Int_t i,TDisTrack *recEvent) const  {
        if (recEvent)
            return recEvent->getEmcShower(i);
        else 
	  return 0;
    }
    const TRecMucTrack*  GetMucTrack(Int_t i,TDisTrack *recEvent) const  {
        if (recEvent)
           return recEvent->getMucTrack(i);
        else 
	  return 0;
    }
    //const TExtTrack*  GetExtTrack(Int_t i) const  {
    //    if (fRecEvent)
    //        return fRecEvent->getExtTrack(i);
    //    else return 0;
    //};
    const TRecMdcDedx*      GetDedx(Int_t i)     const  {
       // if (fRecEvent)
       //     return fRecEvent->getRecMdcDedx(i);
      //  else
	  return 0;
    }

    // header access functions
    inline Long64_t GetRun()   {
        return fEventHeader.GetRunNumber();
    }
    inline Long64_t GetEvent() {
        return fEventHeader.GetEventNumber();
    }
    inline Int_t GetYear()  {
        return fEventHeader.GetEventYear();
    }
    inline Int_t GetMonth() {
        return fEventHeader.GetEventMonth();
    }
    inline Int_t GetDay()   {
        return fEventHeader.GetEventDay();
    }
    inline Int_t GetHour()  {
        return fEventHeader.GetEventHour();
    }
    inline Int_t GetMin()   {
        return fEventHeader.GetEventMin();
    }
    inline Int_t GetSec()   {
        return fEventHeader.GetEventSec();
    }
    inline Int_t GetMC()    {
        return fEventHeader.GetEventMC();
    }
    inline BesEventHeader GetHeader(){
        return fEventHeader;
    }

    ClassDef(BesEvent,0) // BesVis Event
};

R__EXTERN BesEvent *gEvent;

#endif

