#include "RootEventData/TEvtRecObject.h"
#include "TCollection.h"  // Declares TIter

ClassImp(TEvtRecObject)

// Allocate the TObjArray just once

TEvtRecEvent* TEvtRecObject::s_staticEvtRecEvent = 0;
TObjArray* TEvtRecObject::s_staticEvtRecTrackCol = 0;
TEvtRecPrimaryVertex *TEvtRecObject::s_staticEvtRecPrimaryVertex = 0;
TObjArray *TEvtRecObject::s_staticEvtRecVeeVertexCol = 0;
TObjArray *TEvtRecObject::s_staticEvtRecPi0Col = 0;
TObjArray *TEvtRecObject::s_staticEvtRecEtaToGGCol = 0;
TObjArray *TEvtRecObject::s_staticEvtRecDTagCol = 0;

TEvtRecObject::TEvtRecObject()
{
   if ( ! s_staticEvtRecEvent ) {
      s_staticEvtRecEvent = new TEvtRecEvent;
   }
   m_evtRecEvent = s_staticEvtRecEvent;

   if ( !s_staticEvtRecTrackCol) {
      s_staticEvtRecTrackCol = new TObjArray();
   }
   m_evtRecTrackCol = s_staticEvtRecTrackCol;

   if ( ! s_staticEvtRecPrimaryVertex ) {
      s_staticEvtRecPrimaryVertex = new TEvtRecPrimaryVertex();
   }
   m_evtRecPrimaryVertex = s_staticEvtRecPrimaryVertex;

   if ( ! s_staticEvtRecVeeVertexCol ) {
      s_staticEvtRecVeeVertexCol = new TObjArray();
   }
   m_evtRecVeeVertexCol = s_staticEvtRecVeeVertexCol;
   
   if ( ! s_staticEvtRecPi0Col ) {
      s_staticEvtRecPi0Col = new TObjArray();
   }
   m_evtRecPi0Col = s_staticEvtRecPi0Col;

   if ( ! s_staticEvtRecEtaToGGCol ) {
      s_staticEvtRecEtaToGGCol = new TObjArray();
   }
   m_evtRecEtaToGGCol = s_staticEvtRecEtaToGGCol;

   if ( ! s_staticEvtRecDTagCol ) {
      s_staticEvtRecDTagCol = new TObjArray();
   }
   m_evtRecDTagCol = s_staticEvtRecDTagCol;



   Clear();
}

TEvtRecObject::~TEvtRecObject() {

   if ( m_evtRecEvent == s_staticEvtRecEvent ) s_staticEvtRecEvent = 0;
   delete m_evtRecEvent;
   m_evtRecEvent = 0;

   if ( m_evtRecTrackCol == s_staticEvtRecTrackCol ) s_staticEvtRecTrackCol = 0;
   m_evtRecTrackCol->Delete();
   delete m_evtRecTrackCol;
   m_evtRecTrackCol = 0;

   if ( m_evtRecPrimaryVertex == s_staticEvtRecPrimaryVertex ) s_staticEvtRecPrimaryVertex = 0;
   delete m_evtRecPrimaryVertex;
   m_evtRecPrimaryVertex = 0;

   if ( m_evtRecVeeVertexCol == s_staticEvtRecVeeVertexCol ) s_staticEvtRecVeeVertexCol = 0;
   m_evtRecVeeVertexCol->Delete();
   delete m_evtRecVeeVertexCol;
   m_evtRecVeeVertexCol = 0;
   
   if ( m_evtRecPi0Col == s_staticEvtRecPi0Col ) s_staticEvtRecPi0Col = 0;
   m_evtRecPi0Col->Delete();
   delete m_evtRecPi0Col;
   m_evtRecPi0Col = 0;

   if ( m_evtRecEtaToGGCol == s_staticEvtRecEtaToGGCol ) s_staticEvtRecEtaToGGCol = 0;
   m_evtRecEtaToGGCol->Delete();
   delete m_evtRecEtaToGGCol;
   m_evtRecEtaToGGCol = 0;
   
   if ( m_evtRecDTagCol == s_staticEvtRecDTagCol ) s_staticEvtRecDTagCol = 0;
   m_evtRecDTagCol->Delete();
   delete m_evtRecDTagCol;
   m_evtRecDTagCol = 0;

}

void TEvtRecObject::initialize() {
}

void TEvtRecObject::Clear(Option_t* option) {
}

void TEvtRecObject::Print(Option_t* option) const {
   TObject::Print(option);
}

void TEvtRecObject::setEvtRecEvent(TEvtRecEvent* evtRecEvent) {
   m_evtRecEvent->setTotalTracks ( evtRecEvent->totalTracks() );
   m_evtRecEvent->setTotalCharged( evtRecEvent->totalCharged() );
   m_evtRecEvent->setTotalNeutral( evtRecEvent->totalNeutral() );
   m_evtRecEvent->setNumberOfVee ( evtRecEvent->numberOfVee() );
   m_evtRecEvent->setNumberOfPi0 ( evtRecEvent->numberOfPi0() );
   m_evtRecEvent->setNumberOfEtaToGG ( evtRecEvent->numberOfEtaToGG() );
   m_evtRecEvent->setNumberOfDTag( evtRecEvent->numberOfDTag() );
}

void TEvtRecObject::addEvtRecTrack(TEvtRecTrack* track) {
   m_evtRecTrackCol->Add(track);
}

const TEvtRecTrack* TEvtRecObject::getEvtRecTrack(Int_t i) const {
   if ( i >= m_evtRecTrackCol->GetEntries() || i < 0 )
      return 0;
   return (TEvtRecTrack*) m_evtRecTrackCol->At(i);
}

void TEvtRecObject::setEvtRecPrimaryVertex(TEvtRecPrimaryVertex* evtRecPrimaryVertex) {
   m_evtRecPrimaryVertex->setIsValid (evtRecPrimaryVertex->isValid() );
   m_evtRecPrimaryVertex->setNTracks (evtRecPrimaryVertex->nTracks() );
   m_evtRecPrimaryVertex->setTrackIdList (evtRecPrimaryVertex->trackIdList() );
   m_evtRecPrimaryVertex->setChi2    (evtRecPrimaryVertex->chi2() );
   m_evtRecPrimaryVertex->setNdof    (evtRecPrimaryVertex->ndof() );
   m_evtRecPrimaryVertex->setFitMethod (evtRecPrimaryVertex->fitMethod() );
   Double_t vtx[3];
   for (Int_t i = 0; i < 3; i++) {
     vtx[i] = evtRecPrimaryVertex->vertex(i);
   }
   Double_t Evtx[6];
   for (Int_t i = 0; i < 6; i++) {
     Evtx[i] = evtRecPrimaryVertex->errorVertex(i);
   }
   m_evtRecPrimaryVertex->setVertex  (vtx);
   m_evtRecPrimaryVertex->setErrorVertex (Evtx);
}

void TEvtRecObject::addEvtRecVeeVertex(TEvtRecVeeVertex* veeVertex) {
  m_evtRecVeeVertexCol->Add(veeVertex);
}



void TEvtRecObject::addEvtRecPi0(TEvtRecPi0* pi0) {
  m_evtRecPi0Col->Add(pi0);
}


void TEvtRecObject::addEvtRecEtaToGG(TEvtRecEtaToGG* eta) {
  m_evtRecEtaToGGCol->Add(eta);
}

void TEvtRecObject::addEvtRecDTag(TEvtRecDTag* dtag) {
  m_evtRecDTagCol->Add(dtag);
}

const TEvtRecVeeVertex* TEvtRecObject::getEvtRecVeeVertex(Int_t i) const {
  if ( i >= m_evtRecVeeVertexCol->GetEntries() || i < 0 )
     return 0;
  return (TEvtRecVeeVertex*) m_evtRecVeeVertexCol->At(i);
}


const TEvtRecPi0* TEvtRecObject::getEvtRecPi0(Int_t i) const {
  if ( i >= m_evtRecPi0Col->GetEntries() || i < 0 )
     return 0;
  return (TEvtRecPi0*) m_evtRecPi0Col->At(i);
}


const TEvtRecEtaToGG* TEvtRecObject::getEvtRecEtaToGG(Int_t i) const {
  if ( i >= m_evtRecEtaToGGCol->GetEntries() || i < 0 )
     return 0;
  return (TEvtRecEtaToGG*) m_evtRecEtaToGGCol->At(i);
}


const TEvtRecDTag* TEvtRecObject::getEvtRecDTag(Int_t i) const {
  if ( i >= m_evtRecDTagCol->GetEntries() || i < 0 )
     return 0;
  return (TEvtRecDTag*) m_evtRecDTagCol->At(i);
}
