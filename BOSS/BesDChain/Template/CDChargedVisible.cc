#ifndef CLEODCHAIN_CDCHARGEDVISIBLE_CC
#define CLEODCHAIN_CDCHARGEDVISIBLE_CC
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDChargedVisible
// 
// Description: Implementation of candidate for visible charged particles
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Tue Mar 18 13:23:11 EST 1997
// $Id: CDChargedVisible.cc,v 1.4 2009/10/28 08:23:22 petez Exp $
//
// Revision history
//
// $Log: CDChargedVisible.cc,v $
// Revision 1.4  2009/10/28 08:23:22  petez
// see ChangeLog for changes to KinematicData and CDChargedVisible
//
// Revision 1.3  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.2  2009/03/09 20:00:43  chunlei
// get charge and momentum from KalTrack helix information
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.3  2004/03/05 22:01:54  chengp
// implemented Monte Carlo matching
//
// Revision 1.2  2001/04/25 12:52:17  ajm36
// plugged memory leak in ctor for pi0's, k shorts, etas
//
// Revision 1.1  2001/04/11 13:19:35  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.3  2001/04/03 17:26:46  cdj
// moved builtFromCDCharged to builtFromTrack
//
// Revision 1.2  2001/03/23 23:05:42  urner
// added pi0 eta and CDKs decay lists
//
// Revision 1.1.1.1  2000/12/18 22:17:27  cdj
// imported CleoDChain
//
// Revision 1.11  1998/05/01 20:30:21  sjp
// Modified include path for new placement of CO classes
//
// Revision 1.10  1998/05/01 20:21:35  sjp
// Added necessary string include
//
// Revision 1.9  1998/04/21 05:18:23  sjp
// Modified to use CDMud
//
// Revision 1.8  1998/04/17 18:50:24  sjp
// Modifed to use latest CLEO types
//
// Revision 1.7  1997/12/23 21:54:54  sjp
// Modified package to be completely templated
//
// Revision 1.6  1997/09/03 14:58:47  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.5  1997/08/29 17:01:24  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.4  1997/08/19 20:41:47  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.3  1997/01/31 20:10:24  sjp
// Modified to use the new `bug' include files
//
// Revision 1.2  1997/01/21 20:30:30  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.1  1996/11/04 17:15:04  sjp
// Replacement for CHargedHypothesis Class
//

// system include files
#include <string>

// user include files
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
//zoujh: ...
//#include "AnalEvent/BStableParticle.h"

#include "BesDChain/util/KinematicData.h"
#include "BesDChain/CDMud.h"

#include "BesDChain/CDChargedVisible.h"

using DecayChain::KinematicData;

//
// constructors and destructor
//

template < DBCandidate::Hypo H >
CDChargedVisible<H>::CDChargedVisible( const EvtRecTrack* aVisible ) :
   CDCandidate( CDMud< EvtRecTrack >::get( aVisible ) ) ,
   m_visibleEvidence( aVisible )
{
}

//zoujh: ...
/*
template < DBCandidate::Hypo H >
CDChargedVisible<H>::CDChargedVisible( BPion* aPion ) :
   CDCandidate( CDMud< EvtRecTrack >::get( aPion->pionTrk()->bTrack()->dstTrack() ) ),
   m_visibleEvidence( aPion->pionTrk()->bTrack()->dstTrack() )
{
}

template < DBCandidate::Hypo H >
CDChargedVisible<H>::CDChargedVisible( BKaon* aKaon ) :
   CDCandidate( CDMud< EvtRecTrack >::get( aKaon->kaonTrk()->bTrack()->dstTrack() ) ),
   m_visibleEvidence( aKaon->kaonTrk()->bTrack()->dstTrack() )
{
}
*/

//------ Constructor -----
// copy constructor
//
template < DBCandidate::Hypo H >
CDChargedVisible<H>::CDChargedVisible( const CDChargedVisible<H>& aOtherCDChargedHypo ) :
   CDCandidate( aOtherCDChargedHypo ) ,
   m_visibleEvidence( aOtherCDChargedHypo.m_visibleEvidence )
{
}

//------ Destructor -----
//

template < DBCandidate::Hypo H >
CDChargedVisible<H>::~CDChargedVisible()
{
}

//
// assignment operators
//

//
// member functions
//

//
// const member functions
//

template < DBCandidate::Hypo H >
bool CDChargedVisible<H>::builtFromTrack() const
{
   return ( 0 != m_visibleEvidence ) ;
}

template < DBCandidate::Hypo H >
const EvtRecTrack* CDChargedVisible<H>::track() const
{
   return m_visibleEvidence;
}


//------ defaultKinematicData -----
// create the kinematicData for this CDCandidate
//

template < DBCandidate::Hypo H >
KinematicData* CDChargedVisible<H>::defaultKinematicData() const
{
   KinematicData* tmp = new KinematicData();

   double mass = DBCandidate::mass( static_cast<DBCandidate::Hypo>(H) );
   
   //  original set, can be used again once kalTrack has charge and momentum available in memory
   /* 
   if ( (const_cast<EvtRecTrack*>(m_visibleEvidence))->isMdcKalTrackValid() ) {
      RecMdcKalTrack* mdcKalTrack = (const_cast<EvtRecTrack*>(m_visibleEvidence))->mdcKalTrack();
      
      mdcKalTrack->setPidType( static_cast<RecMdcKalTrack::PidType>
    			      (static_cast<DBCandidate::Hypo>(H) - 1) );
      
      tmp->setCharge( mdcKalTrack->charge() );
      //tmp->setMass( DBCandidate::mass( static_cast<DBCandidate::Hypo>(H) ) );
       tmp->setMomentum( Hep3Vector(mdcKalTrack->px(), mdcKalTrack->py(), mdcKalTrack->pz()) );
    }
   */
   //get momentume and chage from helix information directly
   
    if ( (const_cast<EvtRecTrack*>(m_visibleEvidence))->isMdcKalTrackValid() ) {
     RecMdcKalTrack* mdcKalTrack = (const_cast<EvtRecTrack*>(m_visibleEvidence))->mdcKalTrack();
     
     mdcKalTrack->setPidType( static_cast<RecMdcKalTrack::PidType>
			      (static_cast<DBCandidate::Hypo>(H) - 1) );
     
     int pid=static_cast<DBCandidate::Hypo>(H) - 1;
     
     HepVector zhelix;
     if(pid==0)
       zhelix=mdcKalTrack->getZHelixE();
     else if(pid==1)
       zhelix=mdcKalTrack->getZHelixMu();
     else if(pid==2)
       zhelix=mdcKalTrack->getZHelix();
     else if(pid==3)
       zhelix=mdcKalTrack->getZHelixK();
     else 
       zhelix=mdcKalTrack->getZHelixP();
     
          
     double dr(0),phi0(0),kappa(0),dz(0),tanl(0);
     dr=zhelix[0];
     phi0=zhelix[1];
     kappa=zhelix[2];
     dz=zhelix[3];
     tanl=zhelix[4];
     
     int charge=0;
     
     if (kappa > 0.0000000001)
       charge = 1;
     else if (kappa < -0.0000000001)
        charge = -1;
     
     double pxy=0;
     if(kappa!=0) pxy = 1.0/fabs(kappa);
     
     double px = pxy * (-sin(phi0));
     double py = pxy * cos(phi0);
     double pz = pxy * tanl;

     double e  = sqrt( pxy*pxy + pz*pz + mass*mass );
     
     tmp->setCharge( charge );
     //tmp->setMass( DBCandidate::mass( static_cast<DBCandidate::Hypo>(H) ) );
     tmp->setP4( HepLorentzVector(px, py, pz, e));
   }
   
    else {
     RecMdcTrack* mdcTrack = (const_cast<EvtRecTrack*>(m_visibleEvidence))->mdcTrack();
      tmp->setCharge( mdcTrack->charge() );
      double e = sqrt( mass*mass + mdcTrack->p3().mag2() );
      tmp->setP4( HepLorentzVector(mdcTrack->p3(), e) );
    }

   return tmp;
}

#endif /* CLEODCHAIN_CDCHARGEDVISIBLE_CC */
