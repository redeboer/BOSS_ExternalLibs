#ifndef RootEventData_TEvtRecObject_H
#define RootEventData_TEvtRecObject_H

#include "TObject.h"
#include "TObjArray.h"

#include "TEvtRecEvent.h"
#include "TEvtRecTrack.h"
#include "TEvtRecPrimaryVertex.h"
#include "TEvtRecVeeVertex.h"
#include "TEvtRecPi0.h"
#include "TEvtRecEtaToGG.h"
#include "TEvtRecDTag.h"

class TEvtRecObject : public TObject {

public:

   TEvtRecObject();
   virtual ~TEvtRecObject();

   void initialize();

   void Clear(Option_t* option = "");

   void Print(Option_t* option = "") const;

   // Interfaces of EvtRecEvent
   const TEvtRecEvent* getEvtRecEvent() const { return m_evtRecEvent; }
   void setEvtRecEvent(TEvtRecEvent* evtRecEvent);

   // Interfaces of EvtRecTracks
   const TObjArray* getEvtRecTrackCol() const { return m_evtRecTrackCol; }
   void addEvtRecTrack(TEvtRecTrack* track);
   const TEvtRecTrack* getEvtRecTrack(Int_t i) const;
   void clearEvtRecTrackCol() { m_evtRecTrackCol->Clear(); }

   // Interfaces of EvtRecPrimaryVertex
   const TEvtRecPrimaryVertex* getEvtRecPrimaryVertex() const {return m_evtRecPrimaryVertex; }
   void setEvtRecPrimaryVertex(TEvtRecPrimaryVertex* evtRecPrimaryVertex);

   // Interfaces of EvtRecVeeVertex
   const TObjArray* getEvtRecVeeVertexCol() const {return m_evtRecVeeVertexCol; }
   void addEvtRecVeeVertex(TEvtRecVeeVertex* veeVertex);
   const TEvtRecVeeVertex* getEvtRecVeeVertex(Int_t i) const;
   void clearEvtRecVeeVertexCol() { m_evtRecVeeVertexCol->Clear(); }


    
   // Interfaces of EvtRecPi0
   const TObjArray* getEvtRecPi0Col() const {return m_evtRecPi0Col; }
   void addEvtRecPi0(TEvtRecPi0* pi0);
   const TEvtRecPi0* getEvtRecPi0(Int_t i) const;
   void clearEvtRecPi0Col() { m_evtRecPi0Col->Clear(); }

   // Interfaces of EvtRecEtaToGG
   const TObjArray* getEvtRecEtaToGGCol() const {return m_evtRecEtaToGGCol; }
   void addEvtRecEtaToGG(TEvtRecEtaToGG* eta);
   const TEvtRecEtaToGG* getEvtRecEtaToGG(Int_t i) const;
   void clearEvtRecEtaToGGCol() { m_evtRecEtaToGGCol->Clear(); }

   

   
   // Interfaces of EvtRecDTag
   const TObjArray* getEvtRecDTagCol() const {return m_evtRecDTagCol; }
   void addEvtRecDTag(TEvtRecDTag* dtag);
   const TEvtRecDTag* getEvtRecDTag(Int_t i) const;
   void clearEvtRecDTagCol() { m_evtRecDTagCol->Clear(); }
   



private:

   /// data members of EvtRecEvent and EvtRecTracks
   static TEvtRecEvent* s_staticEvtRecEvent;
   TEvtRecEvent*        m_evtRecEvent;  //->
   static TObjArray*    s_staticEvtRecTrackCol;
   TObjArray*           m_evtRecTrackCol;  //->

   /// data members to store EvtRecPrimaryVertex and EvtRecVeeVertex
   static TEvtRecPrimaryVertex* s_staticEvtRecPrimaryVertex;
   TEvtRecPrimaryVertex* m_evtRecPrimaryVertex;  //->
   static TObjArray* s_staticEvtRecVeeVertexCol;
   TObjArray* m_evtRecVeeVertexCol;  //->

   // data members to store EvtRecPi0
   static TObjArray* s_staticEvtRecPi0Col;
   TObjArray* m_evtRecPi0Col;  //->


   // data members to store EvtRecEtaToGG
   static TObjArray* s_staticEvtRecEtaToGGCol;
   TObjArray* m_evtRecEtaToGGCol;  //->
   
   

   // data members to store EvtRecDTag
   static TObjArray* s_staticEvtRecDTagCol;
   TObjArray* m_evtRecDTagCol;  //->

   ClassDef(TEvtRecObject, 2)
};

#endif
