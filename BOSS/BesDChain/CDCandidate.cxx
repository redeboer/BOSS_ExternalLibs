//
//  File:	CDCandidate.cc
//  Author: 	Simon Patton
//  Package:	Taxi - a prototype set of objects for Physics analysis
//  Contents:	Definitions of the members of `CDCandidate' class.
//
//  Class Description: See CDCandidate.h.
//
// $Id: CDCandidate.cxx,v 1.4 2011/10/27 06:15:12 zoujh Exp $
//

#include <stdlib.h>  // For 'exit'
#include <iostream>

#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "DecayChain/Element/ReferenceHolder.h"

#include "BesDChain/util/KinematicData.h"
#include "BesDChain/CDCandidate.h"
#include "BesDChain/CDDecay.h"

using namespace dchain;

using DecayChain::KinematicData;

#ifdef EXTEND
      //for recover 4p after kinematic fit
      void CDCandidate::recover() const { kinematicData()->recover(); }
#endif
//------ Constructor -----
// copy constructor
//
CDCandidate::CDCandidate( const CDCandidate& aOtherCDCandidate ) :
   ReferenceCount(),
   m_kinematicDataPtr(0),
   m_footPrint( aOtherCDCandidate.footPrint() )
{
   if ( 0 != aOtherCDCandidate.m_kinematicDataPtr ) {
      setKinematicData( * (aOtherCDCandidate.kinematicData()) ) ;
   }
}


//------ Constructor -----
// constructor with just a footprint
//
CDCandidate::CDCandidate( const CDFootPrint& aCDFootPrint ) :
   m_kinematicDataPtr( 0 ) ,
   m_footPrint( aCDFootPrint )
{
}

//------ Destructor -----
//
CDCandidate::~CDCandidate()
{
   delete m_kinematicDataPtr ;
}


//------ assignment -----
// fill an *empty* candidate with aother CDCandidate's information
//
const CDCandidate& CDCandidate::operator=( const CDCandidate& aOtherCDCandidate )
{
   if ( 0 == aOtherCDCandidate.m_kinematicDataPtr ) {
      delete m_kinematicDataPtr ;
      m_kinematicDataPtr = 0 ;
   }
   else {
      setKinematicData( * (aOtherCDCandidate.kinematicData()) ) ;
   }
   m_footPrint = aOtherCDCandidate.footPrint() ;
   return ( *this ) ;
}


void CDCandidate::setUserTag(int tag)
{
    this->modifiableKinematicData()->setUserTag( tag );
}

//------ setMomentum -----
//
CDCandidate& CDCandidate::setP4( const HepLorentzVector& aMomentum )
{
   this->modifiableKinematicData()->setP4( aMomentum ) ;
   return( *this ) ;
}


//------ setKinematicData -----
// set the KTKinematicData to a new values
// Does not fill initial value with defaultKinematicData.
//
void CDCandidate::setKinematicData( const KinematicData& aKinematicData )
{
   if ( 0 == m_kinematicDataPtr ) {
      m_kinematicDataPtr = new KinematicData( aKinematicData ) ;
      if ( 0 == m_kinematicDataPtr ) {
	 std::cerr << "No memory to allocate another kinematicData" << std::endl ;
	 exit( 1 ) ;
      }
      return ;
   }
   (*(this->modifiableKinematicData())) = aKinematicData ;
   return ;
}


//------ setCDFootPrint -----
// set the CDFootPrint to a new value
//
void CDCandidate::setCDFootPrint( const CDFootPrint& aCDFootPrint )
{
   m_footPrint = aCDFootPrint ;
}

//------ modifiableKinematicData -----
// get the non-const KTKinematicData for this candidate
//
KinematicData* CDCandidate::modifiableKinematicData()
{
   if ( 0 == m_kinematicDataPtr ) {
      m_kinematicDataPtr = defaultKinematicData() ;
      if ( 0 == m_kinematicDataPtr ) {
	 std::cerr << "No memory to allocate another kinematicData" << std::endl ;
	 exit( 1 ) ;
      }
   }
   return m_kinematicDataPtr ;
}

int CDCandidate::userTag() const
{
    return kinematicData()->userTag();
}

//------ mass -----
// get the mass for this CDCandidate
//
double CDCandidate::mass() const
{
   return kinematicData()->mass();
}


//------ charge -----
// get the charge for this CDCandidate
//
int CDCandidate::charge() const
{
   return kinematicData()->charge();
}


//------ energy -----
// get the energy for this CDCandidate
//
double CDCandidate::energy() const
{
   return kinematicData()->energy();
}


//------ momentum -----
// get the momentum for this CDCandidate
//
const Hep3Vector& CDCandidate::momentum() const
{
   return kinematicData()->p4();
}

const HepLorentzVector& CDCandidate::p4() const
{
   return kinematicData()->p4();
}

//------ kinematicData -----
// get the kinematicData for this CDCandidate
//
const KinematicData* CDCandidate::kinematicData() const
{
   // Cast away `const' to pick up the function.
   // This is safe as the result is being returned as a `const'
   return ( (*(CDCandidate*)this).modifiableKinematicData() );
}

CDCandidate::TracksAndShowers
CDCandidate::finalChildren() const
{
   TracksAndShowers blocks;
   recurseNode(blocks, *this);
   return blocks;
}

void
CDCandidate::recurseNode(CDCandidate::TracksAndShowers& final,
                         const CDCandidate& cand) const
{
   if(cand.builtFromTrack())
   {
      final.first.push_back( cand.track() );
      return;
   }
   if(cand.builtFromCDPhoton())
   {
      final.second.push_back( cand.photon() );
      return;
   }
   const DecayEvidence& decay = cand.decay();
   const vector< ReferenceHolder<CDCandidate> >& children = decay.children();
   vector< ReferenceHolder<CDCandidate> >::const_iterator lastChild = children.end();
   for(vector<ReferenceHolder<CDCandidate> >::const_iterator child = children.begin();
       child != lastChild; ++child)
   {
      recurseNode(final, **child);
   }
}

//------ buildFromCDCharged -----
// false is there in no CDChargedEvidence associated with this CDCandidate
//
bool CDCandidate::builtFromTrack() const
{
   return ( false ) ;
}


//------ chargedEvidence -----
// return the CDChargedEvidence that is associated with this CDCandidate
//
const EvtRecTrack* CDCandidate::track() const
{
   std::cerr << "No navtrack for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( (EvtRecTrack*)0 ) ;
}


//------ builtFromCDPhoton -----
// false is there in no CDNeutralEvidence associated with this CDCandidate
//
bool CDCandidate::builtFromCDPhoton() const
{
   return ( false ) ;
}


//------ photon -----
// return the NavShower that is associated with this CDCandidate
//
const EvtRecTrack* CDCandidate::photon() const
{
   std::cerr << "No NavShower for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( (EvtRecTrack*)0 ) ;
}


//------ builtFromCDPi0 -----
// false is there in no CDNeutralEvidence associated with this CDCandidate
//
bool CDCandidate::builtFromCDPi0() const
{
   return ( false ) ;
}


//------ pi0 -----
// return the NavCDPi0 that is associated with this CDCandidate
//
const EvtRecPi0* CDCandidate::navPi0() const
{
   std::cerr << "No NavCDPi0 for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( (EvtRecPi0*)0 ) ;
}


//------ builtFromCDEta -----
// false is there in no CDNeutralEvidence associated with this CDCandidate
//
bool CDCandidate::builtFromCDEta() const
{
   return ( false ) ;
}


//------ eta -----
// return the NavCDEta that is associated with this CDCandidate
//
const EvtRecEtaToGG* CDCandidate::navEta() const
{
   std::cerr << "No NavCDEta for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( (EvtRecEtaToGG*)0 ) ;
}


//------ builtFromCDKs -----
// false is there in no CDNeutralEvidence associated with this CDCandidate
//
bool CDCandidate::builtFromCDKs() const
{
   return ( false ) ;
}


//------ CDKs -----
// return the NavKs that is associated with this CDCandidate
//
const EvtRecVeeVertex* CDCandidate::navKshort() const
{
   std::cerr << "No NavKs for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( (EvtRecVeeVertex*)0 ) ;
}


//------ builtFromCDLambda -----
// false is there in no CDNeutralEvidence associated with this CDCandidate
//
bool CDCandidate::builtFromCDLambda() const
{
   return ( false ) ;
}


//------ CDLambda -----
// return the NavLambda that is associated with this CDCandidate
//
const EvtRecVeeVertex* CDCandidate::navLambda() const
{
   std::cerr << "No NavLambda for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( (EvtRecVeeVertex*)0 ) ;
}


//------ builtFromCDDecay -----
// false is there in no CDDecay associated with this CDCandidate
//
bool CDCandidate::builtFromCDDecay() const
{
   return ( false ) ;
}


//------ decay -----
// return the decay that is associated with this CDCandidate
//
const CDDecay& CDCandidate::decay() const
{
   std::cerr << "No CDDecay for this CDCandidate" << std::endl ;
   exit( 1 ) ;
   return ( *(CDDecay*)0 ) ;
}

//------ overlap -----
// returns false if this Candodate and OtherCDCandidate have no elements
//   in common
//
bool CDCandidate::overlap( const CDCandidate& aOtherCDCandidate ) const
{
   return ( m_footPrint.overlap( aOtherCDCandidate.footPrint() ) ) ;
}

//------ footPrint -----
// get the CDFootPrint for this CDCandidate
//
const CDFootPrint& CDCandidate::footPrint() const
{
   return ( m_footPrint ) ;
}


//----------------------------------------------------------------------
//
// $Log: CDCandidate.cxx,v $
// Revision 1.4  2011/10/27 06:15:12  zoujh
// add user tag to particle data
//
// Revision 1.3  2009/09/22 08:24:41  hujf
// see ChangeLog
//
// Revision 1.2  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.7  2006/06/05 16:14:58  gregor
// Reordered constructor initializers to match .h file
//
// Revision 1.6  2006/01/11 20:37:25  cdj
// work with renaming done in DChain package
//
// Revision 1.5  2004/03/05 22:01:43  chengp
// implemented Monte Carlo matching
//
// Revision 1.4  2003/05/15 19:58:08  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2001/09/12 19:10:32  ajm36
// add lambda functions to CDCandidate
//
// Revision 1.2  2001/04/20 14:03:34  ajm36
// add finalChildren function to return tracks and showers
//
// Revision 1.1  2001/04/11 13:19:00  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.3  2001/04/03 17:24:57  cdj
// now always initialize m_kinematicDataPtr to 0
//
// Revision 1.2  2001/03/23 23:05:27  urner
// added pi0 eta and CDKs decay lists
//
// Revision 1.1.1.1  2000/12/18 22:17:24  cdj
// imported CleoDChain
//
// Revision 1.29  1998/05/04 19:10:41  sjp
// Fixed access confusion for kinematicData
//
// Revision 1.28  1998/04/17 18:55:49  sjp
// Modified to use latest types
//
// Revision 1.27  1997/09/03 14:58:30  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.26  1997/08/29 17:00:32  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.25  1997/08/25 02:50:34  sjp
// empty returns now use typedef names
//
// Revision 1.24  1997/08/22 16:18:51  sjp
// New name for access functions
//
// Revision 1.23  1997/08/20 12:45:36  sjp
// Removed unnecessary <math.h>
//
// Revision 1.22  1997/08/19 23:02:44  sjp
// Restructured package to be independent of CleoDChain
//
// Revision 1.21  1997/08/19 20:40:14  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.20  1997/01/21 20:29:57  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.19  1996/12/20 21:00:32  sjp
// Request of non-existant reference is now and error
//
// Revision 1.18  1996/11/04 16:58:33  sjp
// Separaqted of CDDecay part of CDCandidate into new class
//
// Revision 1.17  1996/07/16 19:05:28  sjp
// Restructed Libraries
// Put relative pathnames into all includes
//
// Revision 1.16  1996/06/21 21:21:19  sjp
// Fixed bug, which was missing a return statment
//
// Revision 1.15  1996/06/19 19:30:13  sjp
// Changed to use vector<> rather than deque<>
//
// Revision 1.14  1996/06/13 18:18:37  sjp
// KTKinematicData is now a cached value
//
// Revision 1.13  1996/06/04 14:54:17  sjp
// Coverted to use DB Classes
//
// Revision 1.12  1996/04/11 14:25:16  sjp
// Set Z of missedDistance to Z0CD (a kludge)
//
// Revision 1.11  1996/04/05 20:05:12  sjp
// Added function to get MCParticle, Track and Shower matches.
// Reorganized the file.
// Added a cobbled version of missedDistance.
//
// Revision 1.10  1996/02/27 15:50:15  sjp
// Added functions to link CDCandidate with Track and Showers.
// Allowed derived classes ability to addChild to children.
//
// Revision 1.9  1996/02/06 20:37:24  sjp
// Added new functionality to map a CDCandidate onto its `truth'.
//
// Revision 1.8  1995/11/28 17:34:46  sjp
// Changed `list' class to `deque' and added SJP_NOSTL switch.
// Added child() and iterateChild() operations, and thus added Children
//   include file for enum.
// Moved `Log' information.
//
// Revision 1.6  1995/11/25 23:26:43  sjp
// Ammended to use KTKinematicData a basis for kinematic data.
//
// Revision 1.5  1995/11/22 02:49:59  sjp
// list constructors no longer accept Size, now use set_max_size.
//
// Revision 1.4  1995/11/14 22:04:56  sjp
// Updated to use `list' rather than SimpleList, and to use STL style
//   iterators.
//
// Revision 1.3  1995/11/14 21:24:01  sjp
// Corrected bug, energy was not begin accumulated in constructor.
//
// Revision 1.2  1995/11/09 16:53:48  sjp
// Move responsibility for mass, charge, momentum and energy into this class.
//
// Revision 1.1  1995/11/07 21:26:57  sjp
// New base class for all CDCandidates.
//
//
