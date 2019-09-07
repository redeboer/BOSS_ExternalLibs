// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDKs
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      David Urner
// Created:     Thu Mar 22 16:23:36 EST 2001
// $Id: CDKs.cxx,v 1.2 2009/06/22 14:55:48 zoujh Exp $
//
// Revision history
//
// $Log: CDKs.cxx,v $
// Revision 1.2  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.6  2006/01/11 20:37:25  cdj
// work with renaming done in DChain package
//
// Revision 1.5  2004/03/05 22:01:44  chengp
// implemented Monte Carlo matching
//
// Revision 1.4  2003/05/15 19:58:09  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2001/11/06 15:20:46  ajm36
// override defaultKinematicData for pi0, eta, ks, lambda
//
// Revision 1.2  2001/04/25 12:52:12  ajm36
// plugged memory leak in ctor for pi0's, k shorts, etas
//
// Revision 1.1  2001/04/11 13:19:02  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/24 03:07:56  cdj
// fixed CDPi0 CDKs and CDEta classes so could properly assess info from CDCandidate class
//
// Revision 1.1  2001/03/23 23:05:28  urner
// added pi0 eta and CDKs decay lists
//

// system include files

// user include files
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "BesDChain/util/KinematicData.h"
#include "BesDChain/CDChargedPion.h"
#include "BesDChain/CDKs.h"
//zoujh: ...
//#include  "AnalEvent/BComposedParticle.h"

//
// constants, enums and typedefs
//
using namespace dchain;
using DecayChain::KinematicData;

//
// static data member definitions
//

//
// constructors and destructor
//
CDKs::CDKs(const EvtRecVeeVertex* aVisible) :
  m_ks(aVisible)
{
   EvtRecVeeVertex* ks = const_cast<EvtRecVeeVertex*>(aVisible);
   ReferenceHolder<CDChargedPion> tmp = new CDChargedPion(ks->daughter(0));
   addChild(*tmp);
   tmp = new CDChargedPion(ks->daughter(1));
   addChild(*tmp);
}


CDKs::~CDKs()
{
}


//------ buildFromCDKs -----
// return `true' as CDKs is made up from VisibleEvidence
//
bool CDKs::builtFromCDKs() const
{
   return true;
}


//------ CDKs -----
// return NavKs for this CDKs
//
const EvtRecVeeVertex* CDKs::navKshort() const
{
   return m_ks ;
}

KinematicData* CDKs::defaultKinematicData() const
{
   KinematicData* data = new KinematicData();

   double mass = m_ks->mass();
   const HepVector& w_ks = m_ks->w();

   double e = sqrt(mass*mass + w_ks[0]*w_ks[0] + w_ks[1]*w_ks[1] + w_ks[2]*w_ks[2]);

   data->setP4( HepLorentzVector(w_ks[0], w_ks[1], w_ks[2], e) );

   return data;
}

