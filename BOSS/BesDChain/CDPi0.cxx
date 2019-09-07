// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDPi0
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      David Urner
// Created:     Thu Mar 22 16:01:08 EST 2001
// $Id: CDPi0.cxx,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDPi0.cxx,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.7  2006/01/11 20:37:25  cdj
// work with renaming done in DChain package
//
// Revision 1.6  2004/03/05 22:01:44  chengp
// implemented Monte Carlo matching
//
// Revision 1.5  2003/05/15 19:58:09  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.4  2001/11/15 14:30:31  ajm36
// include PhdPi0
//
// Revision 1.3  2001/11/06 15:20:46  ajm36
// override defaultKinematicData for pi0, eta, ks, lambda
//
// Revision 1.2  2001/04/25 12:52:13  ajm36
// plugged memory leak in ctor for pi0's, k shorts, etas
//
// Revision 1.1  2001/04/11 13:19:03  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/24 03:07:56  cdj
// fixed CDPi0 CDKs and CDEta classes so could properly assess info from CDCandidate class
//
// Revision 1.1  2001/03/23 23:05:29  urner
// added pi0 eta and CDKs decay lists
//

// system include files

// user include files
#include "EvtRecEvent/EvtRecPi0.h"
#include "BesDChain/CDPhoton.h"
#include "BesDChain/util/KinematicData.h"
#include "BesDChain/CDPi0.h"
//zoujh: ...
//#include  "AnalEvent/BComposedParticle.h"

//
// constants, enums and typedefs
//
using namespace dchain;
using DecayChain::KinematicData;

//
// constructors and destructor
//
CDPi0::CDPi0(const EvtRecPi0* aVisible) :
  m_pi0(aVisible)
{
  ReferenceHolder<CDPhoton> tmp = new CDPhoton(aVisible->hiEnGamma());
  addChild(*tmp);
  tmp = new CDPhoton(aVisible->loEnGamma());
  addChild(*tmp);
}

// CDPi0::CDPi0( const CDPi0& rhs )
// {
//    // do actual copying here; if you implemented
//    // operator= correctly, you may be able to use just say      
//    *this = rhs;
// }

CDPi0::~CDPi0()
{
}




//------ buildFromCDPi0 -----
// return `true' as CDPi0 is made up from VisibleEvidence
//
bool CDPi0::builtFromCDPi0() const
{
   return true;
}


//------ pi0 -----
// return NavCDPi0 for this CDPi0
//
const EvtRecPi0* CDPi0::navPi0() const
{
   return ( m_pi0 ) ;
}

KinematicData* CDPi0::defaultKinematicData() const
{
   return new KinematicData( m_pi0->hiPfit() + m_pi0->loPfit() );
}
//
// assignment operators
//
// const CDPi0& CDPi0::operator=( const CDPi0& rhs )
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
