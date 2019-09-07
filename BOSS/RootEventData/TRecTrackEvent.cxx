#include "RootEventData/TRecTrackEvent.h"
#include "TCollection.h"  // Declares TIter

ClassImp(TRecTrackEvent)

// Allocate the TObjArray just once

TObjArray *TRecTrackEvent::s_staticRecMdcTrackCol = 0;
TObjArray *TRecTrackEvent::s_staticRecMdcHitCol = 0;
TObjArray *TRecTrackEvent::s_staticTofTrackCol = 0;
TObjArray *TRecTrackEvent::s_staticEmcHitCol = 0;
TObjArray *TRecTrackEvent::s_staticEmcClusterCol = 0;
TObjArray *TRecTrackEvent::s_staticEmcShowerCol = 0;
TObjArray *TRecTrackEvent::s_staticMucTrackCol = 0;
TObjArray *TRecTrackEvent::s_staticRecMdcDedxCol = 0;
TObjArray *TRecTrackEvent::s_staticRecMdcDedxHitCol = 0;
TObjArray *TRecTrackEvent::s_staticExtTrackCol = 0;
TObjArray *TRecTrackEvent::s_staticRecMdcKalTrackCol = 0;
TObjArray *TRecTrackEvent::s_staticRecMdcKalHelixSegCol = 0;
TObjArray *TRecTrackEvent::s_staticEvTimeCol = 0;
TObjArray *TRecTrackEvent::s_staticRecZddChannelCol = 0;

//***************************************************************
TRecTrackEvent::TRecTrackEvent() 
{
   //Dst Track
     //Rec Track 
     if (!s_staticRecMdcTrackCol) {
            s_staticRecMdcTrackCol = new TObjArray();
     }
     m_recMdcTrackCol = s_staticRecMdcTrackCol;

     if (!s_staticRecMdcHitCol) {
       s_staticRecMdcHitCol = new TObjArray(); 
     }
     m_recMdcHitCol = s_staticRecMdcHitCol;


     if (! s_staticTofTrackCol ){
       s_staticTofTrackCol = new TObjArray();
     }
     m_recTofTrackCol = s_staticTofTrackCol;

     if (!s_staticEmcHitCol ){
 	    s_staticEmcHitCol = new TObjArray();
     }
     m_recEmcHitCol = s_staticEmcHitCol;

     if (!s_staticEmcClusterCol ){
 	    s_staticEmcClusterCol = new TObjArray();
     }
     m_recEmcClusterCol = s_staticEmcClusterCol;
   
     if (!s_staticEmcShowerCol ){
 	    s_staticEmcShowerCol = new TObjArray();
     }
     m_recEmcShowerCol = s_staticEmcShowerCol;
   
     
    if (! s_staticMucTrackCol ){
           s_staticMucTrackCol = new TObjArray();
     }
     m_recMucTrackCol = s_staticMucTrackCol;

     if (! s_staticRecMdcDedxCol ){
       s_staticRecMdcDedxCol = new TObjArray();
     }
     m_recMdcDedxCol = s_staticRecMdcDedxCol;

     if (! s_staticRecMdcDedxHitCol ){
       s_staticRecMdcDedxHitCol = new TObjArray();
     }
     m_recMdcDedxHitCol = s_staticRecMdcDedxHitCol;


     if (! s_staticExtTrackCol ){
            s_staticExtTrackCol = new TObjArray();
      }
      m_recExtTrackCol = s_staticExtTrackCol;
     
    
     if (! s_staticRecMdcKalTrackCol ){
	   s_staticRecMdcKalTrackCol = new TObjArray();
     }
     m_recMdcKalTrackCol = s_staticRecMdcKalTrackCol;

     if (! s_staticRecMdcKalHelixSegCol ){
	   s_staticRecMdcKalHelixSegCol = new TObjArray();
     }
     m_recMdcKalHelixSegCol = s_staticRecMdcKalHelixSegCol;

     if(! s_staticEvTimeCol ){
          s_staticEvTimeCol = new TObjArray();
     }
     m_recEvTimeCol = s_staticEvTimeCol;

     if ( ! s_staticRecZddChannelCol ) {
	 s_staticRecZddChannelCol = new TObjArray();
     }
     m_recZddChannelCol = s_staticRecZddChannelCol;

     Clear();
    
}

//*****************************************************************
TRecTrackEvent::~TRecTrackEvent() {
	
	/****************************    Dst Track *************************/
    if(m_recMdcTrackCol == s_staticRecMdcTrackCol) s_staticRecMdcTrackCol = 0;
     m_recMdcTrackCol->Delete();
     delete m_recMdcTrackCol;
     m_recMdcTrackCol = 0;

    if(m_recMdcHitCol == s_staticRecMdcHitCol) s_staticRecMdcHitCol = 0;
     m_recMdcHitCol->Delete();
     delete m_recMdcHitCol;
     m_recMdcHitCol = 0;

     if(m_recTofTrackCol == s_staticTofTrackCol ) s_staticTofTrackCol = 0;
     m_recTofTrackCol->Delete();
     delete m_recTofTrackCol;
     m_recTofTrackCol = 0;

     if(m_recEmcHitCol == s_staticEmcHitCol) s_staticEmcHitCol = 0;
     m_recEmcHitCol->Delete();
     delete m_recEmcHitCol;
     m_recEmcHitCol = 0;

     if(m_recEmcClusterCol == s_staticEmcClusterCol) s_staticEmcClusterCol = 0;
     m_recEmcClusterCol->Delete();
     delete m_recEmcClusterCol;
     m_recEmcClusterCol = 0;

     if(m_recEmcShowerCol == s_staticEmcShowerCol) s_staticEmcShowerCol = 0;
     m_recEmcShowerCol->Delete();
     delete m_recEmcShowerCol;
     m_recEmcShowerCol = 0;


     if(m_recMucTrackCol == s_staticMucTrackCol ) s_staticMucTrackCol = 0;
     m_recMucTrackCol->Delete();
     delete m_recMucTrackCol;
     m_recMucTrackCol = 0;

     if(m_recMdcDedxCol == s_staticRecMdcDedxCol ) s_staticRecMdcDedxCol = 0;
     m_recMdcDedxCol->Delete();
     delete m_recMdcDedxCol;
     m_recMdcDedxCol = 0;

     if(m_recMdcDedxHitCol == s_staticRecMdcDedxHitCol ) s_staticRecMdcDedxHitCol = 0;
     m_recMdcDedxHitCol->Delete();
     delete m_recMdcDedxHitCol;
     m_recMdcDedxHitCol = 0;


     if(m_recExtTrackCol == s_staticExtTrackCol ) s_staticExtTrackCol = 0;
     m_recExtTrackCol->Delete();
     delete m_recExtTrackCol;
     m_recExtTrackCol = 0;
    
    if(m_recMdcKalTrackCol == s_staticRecMdcKalTrackCol ) s_staticRecMdcKalTrackCol = 0;
    m_recMdcKalTrackCol->Delete();
    delete m_recMdcKalTrackCol;
    m_recMdcKalTrackCol = 0;

    if(m_recMdcKalHelixSegCol == s_staticRecMdcKalHelixSegCol ) s_staticRecMdcKalHelixSegCol = 0;
    m_recMdcKalHelixSegCol->Delete();
    delete m_recMdcKalHelixSegCol;
    m_recMdcKalHelixSegCol = 0;

    if(m_recEvTimeCol == s_staticEvTimeCol ) s_staticEvTimeCol = 0;
    m_recEvTimeCol->Delete();
    delete m_recEvTimeCol;
    m_recEvTimeCol = 0; 

    if ( m_recZddChannelCol == s_staticRecZddChannelCol ) s_staticRecZddChannelCol = 0;
    m_recZddChannelCol->Delete();
    delete m_recZddChannelCol;
    m_recZddChannelCol = 0;
}

//*****************************************************************
void TRecTrackEvent::initialize(){ 
}
  
//*****************************************************************
void TRecTrackEvent::Clear(Option_t *option) {

}

//*****************************************************************************
void TRecTrackEvent::Print(Option_t *option) const {
    TObject::Print(option);
}


/****************     Dst Track     ************************/
/// Mdc
 void TRecTrackEvent::addRecMdcTrack(TRecMdcTrack *Track) {
     m_recMdcTrackCol->Add(Track);
 }

 const TRecMdcTrack* TRecTrackEvent::getRecMdcTrack(Int_t i) const {
     if (Int_t(i) >= m_recMdcTrackCol->GetEntries())
            return 0;
     return (TRecMdcTrack*)m_recMdcTrackCol->At(i);
 }

 void TRecTrackEvent::addRecMdcHit(TRecMdcHit *Hit) {
     m_recMdcHitCol->Add(Hit);
 }

 const TRecMdcHit* TRecTrackEvent::getRecMdcHit(Int_t i) const {
     if (Int_t(i) >= m_recMdcHitCol->GetEntries())
            return 0;
     return (TRecMdcHit*)m_recMdcHitCol->At(i);
 }

///Tof
void  TRecTrackEvent::addTofTrack(TRecTofTrack * Track){
  m_recTofTrackCol->Add(Track);
}

const TRecTofTrack*  TRecTrackEvent::getTofTrack(Int_t i) const {
  if(Int_t(i) >=m_recTofTrackCol->GetEntries())
    return 0;
  return (TRecTofTrack*) m_recTofTrackCol->At(i);
}

 ///Emc
 void TRecTrackEvent::addEmcHit(TRecEmcHit * Track) {
     m_recEmcHitCol->Add(Track);
 }

 const TRecEmcHit* TRecTrackEvent::getEmcHit( Int_t i) const {
     if(Int_t(i) >= m_recEmcHitCol->GetEntries())
 	   return 0;
     return (TRecEmcHit*)m_recEmcHitCol->At(i);
 }

 void TRecTrackEvent::addEmcCluster(TRecEmcCluster * Track) {
     m_recEmcClusterCol->Add(Track);
 }

 const TRecEmcCluster* TRecTrackEvent::getEmcCluster( Int_t i) const {
     if(Int_t(i) >= m_recEmcClusterCol->GetEntries())
 	   return 0;
     return (TRecEmcCluster*)m_recEmcClusterCol->At(i);
 }

 void TRecTrackEvent::addEmcShower(TRecEmcShower * Track) {
     m_recEmcShowerCol->Add(Track);
 }

 const TRecEmcShower* TRecTrackEvent::getEmcShower( Int_t i) const {
     if(Int_t(i) >= m_recEmcShowerCol->GetEntries())
 	   return 0;
     return (TRecEmcShower*)m_recEmcShowerCol->At(i);
 }

	///Muc 
void  TRecTrackEvent::addMucTrack(TRecMucTrack * Track){
    m_recMucTrackCol->Add(Track);
}


const TRecMucTrack*  TRecTrackEvent::getMucTrack(Int_t i) const {
	if(Int_t(i) >=m_recMucTrackCol->GetEntries())
		return 0;
	return (TRecMucTrack*) m_recMucTrackCol->At(i);
}

///Dedx
 void  TRecTrackEvent::addRecMdcDedx(TRecMdcDedx * Track){
     m_recMdcDedxCol->Add(Track);
 }


 const TRecMdcDedx*  TRecTrackEvent::getRecMdcDedx(Int_t i) const {
 	if(Int_t(i) >=m_recMdcDedxCol->GetEntries())
 		return 0;
 	return (TRecMdcDedx*) m_recMdcDedxCol->At(i);
 }


void  TRecTrackEvent::addRecMdcDedxHit(TRecMdcDedxHit * Track){
  m_recMdcDedxHitCol->Add(Track);
}


 const TRecMdcDedxHit*  TRecTrackEvent::getRecMdcDedxHit(Int_t i) const {
   if(Int_t(i) >=m_recMdcDedxHitCol->GetEntries())
     return 0;
   return (TRecMdcDedxHit*) m_recMdcDedxHitCol->At(i);
 }

// // Ext
 void  TRecTrackEvent::addExtTrack(TRecExtTrack * Track){
     m_recExtTrackCol->Add(Track);
 }

 const TRecExtTrack*  TRecTrackEvent::getExtTrack(Int_t i) const {
         if(Int_t(i) >=m_recExtTrackCol->GetEntries())
               return 0;
       return (TRecExtTrack*) m_recExtTrackCol->At(i);
 }

//RecMdcKal
void  TRecTrackEvent::addRecMdcKalTrack(TRecMdcKalTrack * Track){
    m_recMdcKalTrackCol->Add(Track);
}

const TRecMdcKalTrack*  TRecTrackEvent::getRecMdcKalTrack(Int_t i) const {
     if(Int_t(i) >=m_recMdcKalTrackCol->GetEntries())
	return 0;
     return (TRecMdcKalTrack*) m_recMdcKalTrackCol->At(i);
}

void TRecTrackEvent::addRecMdcKalHelixSeg(TRecMdcKalHelixSeg * Track){
    m_recMdcKalHelixSegCol->Add(Track);
}

const TRecMdcKalHelixSeg* TRecTrackEvent::getRecMdcKalHelixSeg(Int_t i) const {
	if(Int_t(i) >= m_recMdcKalHelixSegCol->GetEntries())
		return 0;
	return ( TRecMdcKalHelixSeg*) m_recMdcKalHelixSegCol->At(i);
}

//EsTime
void  TRecTrackEvent::addEvTime(TRecEvTime * Track){
    m_recEvTimeCol->Add(Track);
}

const TRecEvTime*  TRecTrackEvent::getEvTime(Int_t i) const {
     if(Int_t(i) >=m_recEvTimeCol->GetEntries())
	return 0;
     return (TRecEvTime*) m_recEvTimeCol->At(i);
}

// ZDD
void TRecTrackEvent::addRecZddChannel(TRecZddChannel* zdd) {
  m_recZddChannelCol->Add(zdd);
}

const TRecZddChannel* TRecTrackEvent::getRecZddChannel(Int_t i) const {
  if ( i >= m_recZddChannelCol->GetEntries() || i < 0 )
     return 0;
  return (TRecZddChannel*) m_recZddChannelCol->At(i);
}
