#include "RootEventData/THltEvent.h"
#include "TCollection.h"  // Declares TIter

ClassImp(THltEvent)

// Allocate the TObjArray just once
TObjArray *THltEvent::s_staticHltRawCol = 0;
//THltInf *THltEvent::s_staticHltInf = 0;
//TDstHltInf *THltEvent::s_staticDstHltInf = 0;
THltInf *THltEvent::s_staticHltInf = 0;
TDstHltInf *THltEvent::s_staticDstHltInf = 0;

//***************************************************************
THltEvent::THltEvent() 
{
  if (! s_staticHltRawCol ) {
    s_staticHltRawCol = new TObjArray();
  }
  if (! s_staticHltInf ) {
    s_staticHltInf = new THltInf();
  }
  if (! s_staticDstHltInf ) {
    s_staticDstHltInf = new TDstHltInf();
  }

  m_hltRawCol = s_staticHltRawCol;
  m_hltInf = s_staticHltInf;
  m_dstHltInf = s_staticDstHltInf;

  Clear();
}

//*****************************************************************
THltEvent::~THltEvent() {
  if(m_hltRawCol == s_staticHltRawCol ) s_staticHltRawCol = 0;
  m_hltRawCol->Delete();
  delete m_hltRawCol;
  m_hltRawCol = 0;

  if(m_hltInf == s_staticHltInf ) s_staticHltInf = 0;
  delete m_hltInf;
  m_hltInf = 0;

  if(m_dstHltInf == s_staticDstHltInf ) s_staticDstHltInf = 0;
  delete m_dstHltInf;
  m_dstHltInf = 0;
}

//*****************************************************************
//o void THltEvent::initialize(Bool_t fromMc){ 
//o     m_fromMc = fromMc;
//o }
  
//*****************************************************************
void THltEvent::Clear(Option_t *option) {

}

//*****************************************************************************
void THltEvent::Print(Option_t *option) const {
    TObject::Print(option);
}

///HltRaw
void  THltEvent::addHltRaw(THltRaw * hltRaw){
    m_hltRawCol->Add(hltRaw);
}
const THltRaw*  THltEvent::getHltRaw(Int_t i) const {
  if (Int_t(i) >= m_hltRawCol->GetEntries()) 
    return 0;
  return (THltRaw*)m_hltRawCol->At(i);
}
const TObjArray*  THltEvent::getHltRawCol() const {
  return (TObjArray*)m_hltRawCol ;
}
void  THltEvent::addHltInf(THltInf * hltInf){
    m_hltInf = hltInf;
}
const THltInf*  THltEvent::getHltInf() const {
        return (THltInf*)m_hltInf ;
}
void  THltEvent::addDstHltInf(TDstHltInf * hltInf){
    m_dstHltInf = hltInf;
}
const TDstHltInf*  THltEvent::getDstHltInf() const {
        return (TDstHltInf*)m_dstHltInf ;
}
