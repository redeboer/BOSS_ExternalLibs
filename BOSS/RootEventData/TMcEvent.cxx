#include "RootEventData/TMcEvent.h"
#include "TCollection.h"  // Declares TIter

ClassImp(TMcEvent)

// Allocate the TObjArray just once
TObjArray *TMcEvent::s_staticMdcMcHitCol = 0;
TObjArray *TMcEvent::s_staticEmcMcHitCol = 0;
TObjArray *TMcEvent::s_staticTofMcHitCol = 0;
TObjArray *TMcEvent::s_staticMucMcHitCol = 0;
TObjArray *TMcEvent::s_staticMcParticleCol = 0;
//TObject *TMcEvent::s_staticDecayMode = 0;


//***************************************************************
TMcEvent::TMcEvent() 
{

    if (!s_staticMdcMcHitCol) {
	    s_staticMdcMcHitCol = new TObjArray();
    }
    m_mdcMcHitCol = s_staticMdcMcHitCol;

    if (!s_staticEmcMcHitCol ){
	    s_staticEmcMcHitCol = new TObjArray();
    }
    m_emcMcHitCol = s_staticEmcMcHitCol;
    
    if (! s_staticTofMcHitCol ){
           s_staticTofMcHitCol = new TObjArray();
     }
     m_tofMcHitCol = s_staticTofMcHitCol;
     
    if (! s_staticMucMcHitCol ){
           s_staticMucMcHitCol = new TObjArray();
     }
     m_mucMcHitCol = s_staticMucMcHitCol;

     if (! s_staticMcParticleCol ){
           s_staticMcParticleCol = new TObjArray();
     }
     m_mcParticleCol = s_staticMcParticleCol;
  /*  
    if (! s_staticDecayMode ){
           s_staticDecayMode = new TObject();
     }
     m_decayMode = s_staticDecayMode;
*/
    Clear();
    
}

//*****************************************************************
TMcEvent::~TMcEvent() {
    if(m_mdcMcHitCol == s_staticMdcMcHitCol) s_staticMdcMcHitCol = 0;
    m_mdcMcHitCol->Delete();
    delete m_mdcMcHitCol;
    m_mdcMcHitCol = 0;

    if(m_emcMcHitCol == s_staticEmcMcHitCol) s_staticEmcMcHitCol = 0;
    m_emcMcHitCol->Delete();
    delete m_emcMcHitCol;
    m_emcMcHitCol = 0;

    if(m_tofMcHitCol == s_staticTofMcHitCol ) s_staticTofMcHitCol = 0;
    m_tofMcHitCol->Delete();
    delete m_tofMcHitCol;
    m_tofMcHitCol = 0;
    
    if(m_mucMcHitCol == s_staticMucMcHitCol ) s_staticMucMcHitCol = 0;
    m_mucMcHitCol->Delete();
    delete m_mucMcHitCol;
    m_mucMcHitCol = 0;

    if(m_mcParticleCol == s_staticMcParticleCol ) s_staticMcParticleCol = 0;
    m_mcParticleCol->Delete();
    delete m_mcParticleCol;
    m_mcParticleCol = 0;
/*
    if(m_decayMode == s_staticDecayMode ) s_staticDecayMode = 0;
    m_decayMode->Delete();
    delete m_decayMode;
    m_decayMode = 0;	
*/
}

//*****************************************************************
void TMcEvent::initialize(){ 

}
  
//*****************************************************************
void TMcEvent::Clear(Option_t *option) {

}

//*****************************************************************************
void TMcEvent::Print(Option_t *option) const {
    TObject::Print(option);
}

/// Mdc
void TMcEvent::addMdcMc(TMdcMc *mcHit) {
    m_mdcMcHitCol->Add(mcHit);
}

const TMdcMc* TMcEvent::getMdcMc(Int_t i) const {
    if (Int_t(i) >= m_mdcMcHitCol->GetEntries()) 
	    return 0;
    return (TMdcMc*)m_mdcMcHitCol->At(i);
}
///Emc
void TMcEvent::addEmcMc(TEmcMc * mcHit) {
    m_emcMcHitCol->Add(mcHit);
}

const TEmcMc* TMcEvent::getEmcMc( Int_t i) const {
    if(Int_t(i) >= m_emcMcHitCol->GetEntries())
	   return 0;
    return (TEmcMc*)m_emcMcHitCol->At(i);
}

///Tof 
void  TMcEvent::addTofMc(TTofMc * mcHit){
    m_tofMcHitCol->Add(mcHit);
}


const TTofMc*  TMcEvent::getTofMc(Int_t i) const {
	if(Int_t(i) >=m_tofMcHitCol->GetEntries())
		return 0;
	return (TTofMc*) m_tofMcHitCol->At(i);
}

///Muc 
void  TMcEvent::addMucMc(TMucMc * mcHit){
    m_mucMcHitCol->Add(mcHit);
}


const TMucMc*  TMcEvent::getMucMc(Int_t i) const {
	if(Int_t(i) >=m_mucMcHitCol->GetEntries())
		return 0;
	return (TMucMc*) m_mucMcHitCol->At(i);
}

///McParticle 
void  TMcEvent::addMcParticle(TMcParticle * mcHit){
    m_mcParticleCol->Add(mcHit);
}


const TMcParticle*  TMcEvent::getMcParticle(Int_t i) const {
	if(Int_t(i) >=m_mcParticleCol->GetEntries())
		return 0;
	return (TMcParticle*) m_mcParticleCol->At(i);
}
/*
///DecayMode
void  TMcEvent::addDecayMode(TDecayMode * decayMode){
    m_decayMode = decayMode;
}


const TDecayMode*  TMcEvent::getDecayMode() const {
	return (TDecayMode*)m_decayMode ;
}
*/
