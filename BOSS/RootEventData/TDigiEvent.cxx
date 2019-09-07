#include "RootEventData/TDigiEvent.h"
#include <iostream>
#include "TCollection.h"  // Declares TIter

ClassImp(TDigiEvent)

// Allocate the TObjArray just once
TObjArray *TDigiEvent::s_staticMdcDigiCol = 0;
TObjArray *TDigiEvent::s_staticEmcDigiCol = 0;
TObjArray *TDigiEvent::s_staticTofDigiCol = 0;
TObjArray *TDigiEvent::s_staticMucDigiCol = 0;
TObjArray *TDigiEvent::s_staticLumiDigiCol = 0;


//***************************************************************
TDigiEvent::TDigiEvent() 
{
    if (!s_staticMdcDigiCol) {
	    s_staticMdcDigiCol = new TObjArray();
    }
    m_mdcDigiCol = s_staticMdcDigiCol;

    if (!s_staticEmcDigiCol ){
	    s_staticEmcDigiCol = new TObjArray();
    }
    m_emcDigiCol = s_staticEmcDigiCol;
    
    if (! s_staticTofDigiCol ){
           s_staticTofDigiCol = new TObjArray();
     }
     m_tofDigiCol = s_staticTofDigiCol;
     
    if (! s_staticMucDigiCol ){
           s_staticMucDigiCol = new TObjArray();
     }
     m_mucDigiCol = s_staticMucDigiCol;

    if (! s_staticLumiDigiCol ) {
       s_staticLumiDigiCol = new TObjArray();
    }
    m_lumiDigiCol = s_staticLumiDigiCol;

    Clear();
    
}

//*****************************************************************
TDigiEvent::~TDigiEvent() {
	 
  if(m_mdcDigiCol == s_staticMdcDigiCol) s_staticMdcDigiCol = 0;
    m_mdcDigiCol->Delete();
    delete m_mdcDigiCol;
    m_mdcDigiCol = 0;

    if(m_emcDigiCol == s_staticEmcDigiCol) s_staticEmcDigiCol = 0;
    m_emcDigiCol->Delete();
    delete m_emcDigiCol;
    m_emcDigiCol = 0;

    if(m_tofDigiCol == s_staticTofDigiCol ) s_staticTofDigiCol = 0;
    m_tofDigiCol->Delete();
    delete m_tofDigiCol;
    m_tofDigiCol = 0;
    
    if(m_mucDigiCol == s_staticMucDigiCol ) s_staticMucDigiCol = 0;
    m_mucDigiCol->Delete();
    delete m_mucDigiCol;
    m_mucDigiCol = 0;  

    if ( m_lumiDigiCol == s_staticLumiDigiCol ) s_staticLumiDigiCol = 0;
    m_lumiDigiCol->Delete();
    delete m_lumiDigiCol;
    m_lumiDigiCol = 0;
	
}

//*****************************************************************
void TDigiEvent::initialize(Bool_t fromMc){ 
    m_fromMc = fromMc;
}
  
//*****************************************************************
void TDigiEvent::Clear(Option_t *option) {
  /*
    const Int_t nd = 20000;
    static Int_t limit = 100;
    static Int_t ind=0;
    static TMdcDigi* keep[nd];

    
    Int_t n = m_mdcDigiCol->GetEntries();
    if (n>limit) {
      limit=n+10;
      if (limit > nd)
        std::cout << "!!!Warning: limit for TMdcDigi is greater than " << nd << std::endl;
      for (Int_t j=0;j<ind;j++) delete keep[j];
      ind = 0;
    }
    for (Int_t i=0;i<n;i++) keep[ind+i] = (TMdcDigi*)m_mdcDigiCol->At(i);
    ind += n;
    if (ind > nd-limit) {
      for (Int_t j=0;j<ind;j++) delete keep[j];
      ind = 0;
    }
    m_mdcDigiCol->Clear();
*/
}

//*****************************************************************************
void TDigiEvent::Print(Option_t *option) const {
    TObject::Print(option);
    std::cout.precision(2);
    if (m_mdcDigiCol) 
      std::cout << "Number of TMdcDigis " << m_mdcDigiCol->GetEntries() << std::endl;
    else 
      std::cout << "Number of TMdcDigis 0" << std::endl;
}

/// Mdc
void TDigiEvent::addMdcDigi(TMdcDigi *digi) {
    m_mdcDigiCol->Add(digi);
}

const TMdcDigi* TDigiEvent::getMdcDigi(Int_t i) const {
    if (Int_t(i) >= m_mdcDigiCol->GetEntries()) 
	    return 0;
    return (TMdcDigi*)m_mdcDigiCol->At(i);
}
///Emc
void TDigiEvent::addEmcDigi(TEmcDigi * digi) {
    m_emcDigiCol->Add(digi);
}

const TEmcDigi* TDigiEvent::getEmcDigi( Int_t i) const {
    if(Int_t(i) >= m_emcDigiCol->GetEntries())
	   return 0;
    return (TEmcDigi*)m_emcDigiCol->At(i);
}

///Tof 
void  TDigiEvent::addTofDigi(TTofDigi * digi){
    m_tofDigiCol->Add(digi);
}


const TTofDigi*  TDigiEvent::getTofDigi(Int_t i) const {
	if(Int_t(i) >=m_tofDigiCol->GetEntries())
		return 0;
	return (TTofDigi*) m_tofDigiCol->At(i);
}

///Muc 
void  TDigiEvent::addMucDigi(TMucDigi * digi){
    m_mucDigiCol->Add(digi);
}


const TMucDigi*  TDigiEvent::getMucDigi(Int_t i) const {
	if(Int_t(i) >=m_mucDigiCol->GetEntries())
		return 0;
	return (TMucDigi*) m_mucDigiCol->At(i);
}

///Lumi
void TDigiEvent::addLumiDigi(TLumiDigi * digi) {
   m_lumiDigiCol->Add(digi);
}

const TLumiDigi* TDigiEvent::getLumiDigi(Int_t i) const {
   if ( i >= m_lumiDigiCol->GetEntries() )
      return 0;
   return (TLumiDigi*) m_lumiDigiCol->At(i);
}
