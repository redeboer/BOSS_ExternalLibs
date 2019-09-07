#include "RootEventData/TDstEvent.h"
#include "TCollection.h"  // Declares TIter

ClassImp(TDstEvent)

// Allocate the TObjArray just once

TObjArray *TDstEvent::s_staticMdcTrackCol = 0;
TObjArray *TDstEvent::s_staticEmcTrackCol = 0;
TObjArray *TDstEvent::s_staticTofTrackCol = 0;
TObjArray *TDstEvent::s_staticMucTrackCol = 0;
TObjArray *TDstEvent::s_staticMdcDedxCol = 0;
TObjArray *TDstEvent::s_staticExtTrackCol = 0;
TObjArray *TDstEvent::s_staticMdcKalTrackCol = 0;

//***************************************************************
TDstEvent::TDstEvent() 
{
	 	//Dst Track

    if (!s_staticMdcTrackCol) {
	    s_staticMdcTrackCol = new TObjArray();
    }
    m_mdcTrackCol = s_staticMdcTrackCol;

    if (!s_staticEmcTrackCol ){
	    s_staticEmcTrackCol = new TObjArray();
    }
    m_emcTrackCol = s_staticEmcTrackCol;
   
    if (! s_staticTofTrackCol ){
           s_staticTofTrackCol = new TObjArray();
     }
     m_tofTrackCol = s_staticTofTrackCol;
     
    if (! s_staticMucTrackCol ){
           s_staticMucTrackCol = new TObjArray();
     }
     m_mucTrackCol = s_staticMucTrackCol;

    if (! s_staticMdcDedxCol ){
           s_staticMdcDedxCol = new TObjArray();
     }
     m_mdcDedxCol = s_staticMdcDedxCol;

    if (! s_staticExtTrackCol ){
           s_staticExtTrackCol = new TObjArray();
     }
     m_extTrackCol = s_staticExtTrackCol;
     
    
     if (! s_staticMdcKalTrackCol ){
	   s_staticMdcKalTrackCol = new TObjArray();
     }
     m_mdcKalTrackCol = s_staticMdcKalTrackCol;
    
     Clear();
    
}

//*****************************************************************
TDstEvent::~TDstEvent() {
	
	/****************************    Dst Track *************************/
   if(m_mdcTrackCol == s_staticMdcTrackCol) s_staticMdcTrackCol = 0;
    m_mdcTrackCol->Delete();
    delete m_mdcTrackCol;
    m_mdcTrackCol = 0;

    if(m_emcTrackCol == s_staticEmcTrackCol) s_staticEmcTrackCol = 0;
    m_emcTrackCol->Delete();
    delete m_emcTrackCol;
    m_emcTrackCol = 0;

    if(m_tofTrackCol == s_staticTofTrackCol ) s_staticTofTrackCol = 0;
    m_tofTrackCol->Delete();
    delete m_tofTrackCol;
    m_tofTrackCol = 0;

    if(m_mucTrackCol == s_staticMucTrackCol ) s_staticMucTrackCol = 0;
    m_mucTrackCol->Delete();
    delete m_mucTrackCol;
    m_mucTrackCol = 0;

    if(m_mdcDedxCol == s_staticMdcDedxCol ) s_staticMdcDedxCol = 0;
    m_mdcDedxCol->Delete();
    delete m_mdcDedxCol;
    m_mdcDedxCol = 0;

    if(m_extTrackCol == s_staticExtTrackCol ) s_staticExtTrackCol = 0;
    m_extTrackCol->Delete();
    delete m_extTrackCol;
    m_extTrackCol = 0;
    
    if(m_mdcKalTrackCol == s_staticMdcKalTrackCol ) s_staticMdcKalTrackCol = 0;
    m_mdcKalTrackCol->Delete();
    delete m_mdcKalTrackCol;
    m_mdcKalTrackCol = 0;
		 
    
}

//*****************************************************************
void TDstEvent::initialize(){ 
}
  
//*****************************************************************
void TDstEvent::Clear(Option_t *option) {

}

//*****************************************************************************
void TDstEvent::Print(Option_t *option) const {
    TObject::Print(option);
}


/****************     Dst Track     ************************/
/// Mdc
void TDstEvent::addMdcTrack(TMdcTrack *Track) {
    m_mdcTrackCol->Add(Track);
}

const TMdcTrack* TDstEvent::getMdcTrack(Int_t i) const {
    if (Int_t(i) >= m_mdcTrackCol->GetEntries()) 
	    return 0;
    return (TMdcTrack*)m_mdcTrackCol->At(i);
} 
///Emc
void TDstEvent::addEmcTrack(TEmcTrack * Track) {
    m_emcTrackCol->Add(Track);
}

const TEmcTrack* TDstEvent::getEmcTrack( Int_t i) const {
    if(Int_t(i) >= m_emcTrackCol->GetEntries())
	   return 0;
    return (TEmcTrack*)m_emcTrackCol->At(i);
}

///Tof 
void  TDstEvent::addTofTrack(TTofTrack * Track){
    m_tofTrackCol->Add(Track);
}


const TTofTrack*  TDstEvent::getTofTrack(Int_t i) const {
	if(Int_t(i) >=m_tofTrackCol->GetEntries())
		return 0;
	return (TTofTrack*) m_tofTrackCol->At(i);
}
	///Muc 
void  TDstEvent::addMucTrack(TMucTrack * Track){
    m_mucTrackCol->Add(Track);
}


const TMucTrack*  TDstEvent::getMucTrack(Int_t i) const {
	if(Int_t(i) >=m_mucTrackCol->GetEntries())
		return 0;
	return (TMucTrack*) m_mucTrackCol->At(i);
}
	///Dedx
void  TDstEvent::addMdcDedx(TMdcDedx * Track){
    m_mdcDedxCol->Add(Track);
}


const TMdcDedx*  TDstEvent::getMdcDedx(Int_t i) const {
	if(Int_t(i) >=m_mdcDedxCol->GetEntries())
		return 0;
	return (TMdcDedx*) m_mdcDedxCol->At(i);
}

// Ext
void  TDstEvent::addExtTrack(TExtTrack * Track){
    m_extTrackCol->Add(Track);
}

const TExtTrack*  TDstEvent::getExtTrack(Int_t i) const {
        if(Int_t(i) >=m_extTrackCol->GetEntries())
              return 0;
      return (TExtTrack*) m_extTrackCol->At(i);
}

//MdcKal
void  TDstEvent::addMdcKalTrack(TMdcKalTrack * Track){
    m_mdcKalTrackCol->Add(Track);
}

const TMdcKalTrack*  TDstEvent::getMdcKalTrack(Int_t i) const {
     if(Int_t(i) >=m_mdcKalTrackCol->GetEntries())
	return 0;
     return (TMdcKalTrack*) m_mdcKalTrackCol->At(i);
}

