#include "RootEventData/TTrigEvent.h"
#include "TCollection.h"  // Declares TIter

ClassImp(TTrigEvent)

// Allocate the TObjArray just once
TTrigData *TTrigEvent::s_staticTrigData = 0;

//***************************************************************
TTrigEvent::TTrigEvent() 
   : m_trigData(0)
{
  if (! s_staticTrigData ) {
    s_staticTrigData = new TTrigData();
  }

  m_trigData = s_staticTrigData;

  Clear();
}

//*****************************************************************
TTrigEvent::~TTrigEvent() {
  if(m_trigData == s_staticTrigData ) s_staticTrigData = 0;
   delete m_trigData;
   m_trigData = 0;
}

//*****************************************************************
//o void TTrigEvent::initialize(Bool_t fromMc){ 
//o    m_fromMc = fromMc;
//o }
  
//*****************************************************************
void TTrigEvent::Clear(Option_t *option) {

}

//*****************************************************************************
void TTrigEvent::Print(Option_t *option) const {
    TObject::Print(option);
}

///TrigData
void  TTrigEvent::addTrigData(TTrigData * trigData){
    m_trigData = trigData;
}


const TTrigData*  TTrigEvent::getTrigData() const {
        return (TTrigData*)m_trigData ;
}
