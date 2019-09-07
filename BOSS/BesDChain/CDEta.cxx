// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDEta
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// petez (2009.2.25): This is a modified copy of BesDChain CDPi0.h
//

// system include files

// user include files
#include "EvtRecEvent/EvtRecEtaToGG.h"
#include "BesDChain/CDPhoton.h"
#include "BesDChain/util/KinematicData.h"
#include "BesDChain/CDEta.h"

//
// constants, enums and typedefs
//
using namespace dchain;
using DecayChain::KinematicData;

//
// constructors and destructor
//
CDEta::CDEta(const EvtRecEtaToGG* aVisible) :
  m_eta(aVisible)
{
  ReferenceHolder<CDPhoton> tmp = new CDPhoton(aVisible->hiEnGamma());
  addChild(*tmp);
  tmp = new CDPhoton(aVisible->loEnGamma());
  addChild(*tmp);
}

// CDEta::CDEta( const CDEta& rhs )
// {
//    // do actual copying here; if you implemented
//    // operator= correctly, you may be able to use just say      
//    *this = rhs;
// }

CDEta::~CDEta()
{
}




//------ buildFromCDEta -----
// return `true' as CDEta is made up from VisibleEvidence
//
bool CDEta::builtFromCDEta() const
{
   return true;
}


//------ eta -----
// return NavCDEta for this CDEta
//
const EvtRecEtaToGG* CDEta::navEta() const
{
   return ( m_eta ) ;
}

KinematicData* CDEta::defaultKinematicData() const
{
   return new KinematicData( m_eta->hiPfit() + m_eta->loPfit() );
}
//
// assignment operators
//
// const CDEta& CDEta::operator=( const CDEta& rhs )
// {
//   if( this != &rhs ) {
//      // do actual copying here, plus:
//      // "SuperClass"::operator=( rhs );
//   }
//
//   return *this;
// }

//
// member functions
//

//
// const member functions
//
