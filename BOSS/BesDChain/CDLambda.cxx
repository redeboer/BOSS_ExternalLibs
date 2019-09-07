// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDLambda
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      David Urner
// Created:     Thu Mar 22 16:23:36 EST 2001
// $Id: CDLambda.cxx,v 1.2 2009/07/15 10:56:41 zoujh Exp $
//
// Revision history
//
// $Log: CDLambda.cxx,v $
// Revision 1.2  2009/07/15 10:56:41  zoujh
// *** empty log message ***
//
// Revision 1.1  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.4  2006/01/11 20:37:25  cdj
// work with renaming done in DChain package
//
// Revision 1.3  2003/05/15 19:58:09  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.2  2001/11/06 15:20:46  ajm36
// override defaultKinematicData for pi0, eta, ks, lambda
//
// Revision 1.1  2001/09/11 15:08:27  ajm36
// add files for lambdas
//
// Revision 1.2  2001/04/25 12:52:12  ajm36
// plugged memory leak in ctor for pi0's, k shorts, etas
//
// Revision 1.1  2001/04/11 13:19:02  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/24 03:07:56  cdj
// fixed CDPi0 CDLambda and CDEta classes so could properly assess info from CDCandidate class
//
// Revision 1.1  2001/03/23 23:05:28  urner
// added pi0 eta and CDLambda decay lists
//


// user include files
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "BesDChain/util/KinematicData.h"
#include "BesDChain/CDChargedPion.h"
#include "BesDChain/CDProton.h"
#include "BesDChain/CDLambda.h"


//
// constants, enums and typedefs
//
using namespace dchain;

//
// static data member definitions
//

//
// constructors and destructor
//
CDLambda::CDLambda(const EvtRecVeeVertex* aVisible):
//  CDDecay(*(new CDChargedPion(aVisible.piPlus()))),
  m_lambda(aVisible)
{
   EvtRecVeeVertex* lambda = const_cast<EvtRecVeeVertex*>(aVisible);

   int index[2]  = {0, 1};
   if (aVisible->vertexId() < 0) {
      index[0] = 1;
      index[1] = 0;
   }

   ReferenceHolder<CDProton> tmp = new CDProton(lambda->daughter(index[0]));
   addChild(*tmp);
   ReferenceHolder<CDChargedPion> tmp2 = new CDChargedPion(lambda->daughter(index[1]));
   addChild(*tmp2);
}


// CDLambda::CDLambda( const CDLambda& rhs )
// {
//    // do actual copying here; if you implemented
//    // operator= correctly, you may be able to use just say      
//    *this = rhs;
// }

CDLambda::~CDLambda()
{
}


//------ buildFromCDLambda -----
// return `true' as CDLambda is made up from VisibleEvidence
//
bool CDLambda::builtFromCDLambda() const
{
   return true;
}


//------ CDLambda -----
// return NavLambda for this CDLambda
//
const EvtRecVeeVertex* CDLambda::navLambda() const
{
   return m_lambda ;
}

KinematicData* CDLambda::defaultKinematicData() const
{
   KinematicData* data = new KinematicData();

   const HepVector& w_lambda = m_lambda->w();

   data->setP4( HepLorentzVector(w_lambda[0], w_lambda[1], w_lambda[2], w_lambda[3]) );

   return data;
}
