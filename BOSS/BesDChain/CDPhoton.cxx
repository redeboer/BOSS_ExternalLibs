// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDPhoton
// 
// Description: Implementation of candidate for CDPhotons
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Tue Mar 18 13:23:11 EST 1997
// $Id: CDPhoton.cxx,v 1.3 2010/01/11 08:05:20 zoujh Exp $
//
// Revision history
//
// $Log: CDPhoton.cxx,v $
// Revision 1.3  2010/01/11 08:05:20  zoujh
// Add setIP for vertex correction
//
// Revision 1.2  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.4  2004/03/05 22:01:44  chengp
// implemented Monte Carlo matching
//
// Revision 1.3  2002/02/28 02:17:51  cdj
// removed unnecessary include of CDTruth.h
//
// Revision 1.2  2001/04/25 12:52:13  ajm36
// plugged memory leak in ctor for pi0's, k shorts, etas
//
// Revision 1.1  2001/04/11 13:19:02  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1  2001/03/23 23:05:28  urner
// added pi0 eta and CDKs decay lists
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.15  1998/05/01 20:30:07  sjp
// Modified include path for new placement of CO classes
//
// Revision 1.14  1998/04/21 05:18:26  sjp
// Modified to use CDMud
//
// Revision 1.13  1998/04/17 18:55:50  sjp
// Modified to use latest types
//
// Revision 1.12  1997/12/23 21:54:26  sjp
// Modified package to be completely templated
//
// Revision 1.11  1997/09/03 14:58:36  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.10  1997/08/29 17:00:37  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.9  1997/08/19 20:40:18  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.8  1997/01/21 20:30:01  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.7  1996/11/04 16:58:09  sjp
// Updated to work with new CDCandidate
//
// Revision 1.6  1996/07/16 19:05:30  sjp
// Restructed Libraries
// Put relative pathnames into all includes
//
// Revision 1.5  1996/06/13 18:20:13  sjp
// Implmented KTKinematicData caching.
// Added capability to be built from a MCParticle
//
// Revision 1.4  1996/06/04 15:00:04  sjp
// Coverted to use kinematicData opertaion of Shower
//
// Revision 1.3  1996/04/06 19:44:07  sjp
// Changed `true' to `!false'
//
// Revision 1.2  1996/04/05 20:07:29  sjp
// Added matching information
//
// Revision 1.1  1995/11/26 23:03:24  sjp
// New Classes to handle Showers and CDPhotons.

// system include files
//#include <iostream>

// user include files
#include "BesDChain/util/KinematicData.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "BesDChain/CDMud.h"
#include "BesDChain/CDPhoton.h"

using DecayChain::KinematicData;

Hep3Vector CDPhoton::m_IPV(0.0, 0.0, 0.0);

//------ Constructor -----
// constructor using VisibleEvidence
//
CDPhoton::CDPhoton( const VisibleEvidence* aVisible ) :
   CDCandidate( CDMud< VisibleEvidence >::get( aVisible ) ) ,
   m_visibleEvidence( aVisible )
{
}

//------ Constructor -----
// copy constructor
//
CDPhoton::CDPhoton( const CDPhoton& aOtherCDPhoton ) :
   CDCandidate( aOtherCDPhoton ) ,
   m_visibleEvidence( aOtherCDPhoton.m_visibleEvidence )
{
}


//------ Destructor -----
//
CDPhoton::~CDPhoton()
{
}


//------ buildFromCDPhoton -----
// return `true' as CDPhoton is made up from VisibleEvidence
//
bool CDPhoton::builtFromCDPhoton() const
{
   return ( 0 != m_visibleEvidence ) ;
}


//------ photon -----
// return NavShower for this CDPhoton
//
const EvtRecTrack* CDPhoton::photon() const
{
   return m_visibleEvidence ;
}

//------ defaultKinematicData -----
// create the kinematicData for this CDCandidate
//
KinematicData* CDPhoton::defaultKinematicData() const
{
   RecEmcShower* emcShower = (const_cast<VisibleEvidence*>(m_visibleEvidence))->emcShower();

   double eraw = emcShower->energy();

   //double phi  = emcShower->phi();
   //double the  = emcShower->theta();

   Hep3Vector EmcV( emcShower->x(), emcShower->y(), emcShower->z() );
   Hep3Vector PhotonV = EmcV - m_IPV;

   double phi = PhotonV.phi();
   double the = PhotonV.theta();

   double px = eraw*sin(the)*cos(phi);
   double py = eraw*sin(the)*sin(phi);
   double pz = eraw*cos(the);

   KinematicData* tmp = new KinematicData();

   tmp->setP4( HepLorentzVector(px, py, pz, eraw) );

   return tmp;
}

