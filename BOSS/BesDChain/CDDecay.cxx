// -*- C++ -*-
//
// Package:     DChain
// Module:      CDDecay
// 
// Description: CDDecay candidate built from other candidates
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Sat Oct 26 11:09:11 EDT 1996
// $Id: CDDecay.cxx,v 1.5 2011/12/08 15:28:10 zoujh Exp $
//
// Revision history
//
// $Log: CDDecay.cxx,v $
// Revision 1.5  2011/12/08 15:28:10  zoujh
// change the rule of tag calculation
//
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
// Revision 1.9  2006/03/17 19:36:31  ponyisi
// Fix mcParticle() to use gammaFSR when available while doing matching
//
// Revision 1.8  2006/01/11 20:37:25  cdj
// work with renaming done in DChain package
//
// Revision 1.7  2004/06/30 16:43:08  chengp
// deleted redundant lines
//
// Revision 1.6  2004/06/29 17:51:22  chengp
// modified to match Monte Carlo decay with FSR
//
// Revision 1.5  2004/03/12 15:22:41  cdj
// protect against case where get negative mass squared (sets it to 0)
//
// Revision 1.4  2004/03/05 22:01:43  chengp
// implemented Monte Carlo matching
//
// Revision 1.3  2003/05/15 19:58:08  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.2  2002/04/09 20:54:16  lyon
// Added CDDecay::setKinematicData
//
// Revision 1.1  2001/04/11 13:19:01  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.4  2001/04/04 15:16:55  cdj
// added a protected default constructor (used for storage)
//
// Revision 1.3  2001/04/03 17:25:55  cdj
// changed isCDDecay to builtFromCDDecay
//
// Revision 1.2  2001/03/30 19:41:33  cdj
// addChild now adds to footprint
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.13  1998/05/04 19:11:26  sjp
// Corrected order of arguments in KinematicData ctor
//
// Revision 1.12  1998/04/17 18:55:51  sjp
// Modified to use latest types
//
// Revision 1.11  1997/09/03 14:58:32  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.10  1997/08/29 17:00:34  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.9  1997/08/28 07:00:31  sjp
// Modified files to handle complete templating
//
// Revision 1.8  1997/08/20 12:49:29  sjp
// Fudged up work around for <math.h>
//
// Revision 1.7  1997/08/19 23:02:46  sjp
// Restructured package to be independent of CleoDChain
//
// Revision 1.6  1997/08/19 20:40:15  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.5  1997/01/31 20:10:12  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/21 20:29:59  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1997/01/19 20:32:11  sjp
// Changed LargeCount to Count
//
// Revision 1.2  1996/12/20 21:00:50  sjp
// Request of non-existant reference is now and error
//
// Revision 1.1  1996/11/04 16:55:10  sjp
// New class for CDDecay CDCandidate
//

// system include files
#include <stdlib.h>  // required for 'exit'
#include <cmath>  // required for trigonometry and fabs

// user include files
#include "BesDChain/util/KinematicData.h"
#include "DecayChain/Function/DCAnalysisFunction.h"
#include "BesDChain/CDDecay.h"

using namespace dchain;
using std::vector;
using DecayChain::KinematicData;

CDDecay::CDDecay( const CDDecay& aOtherCDDecay ) :
#ifdef EXTEND
    m_kmfit_chisq(aOtherCDDecay.chisq()),
    m_vxfit_chisq(aOtherCDDecay.vchisq()),
#endif
    CDCandidate( aOtherCDDecay ) ,
    m_children( *new vector< ReferenceHolder< CDCandidate > >( aOtherCDDecay.children() ) )
{
}

CDDecay::CDDecay( const CDCandidate& aProduct ) :
#ifdef EXTEND
    m_kmfit_chisq(500000.),
    m_vxfit_chisq(500000.),
#endif
    CDCandidate( aProduct.footPrint() ) ,
    m_children( *new vector< ReferenceHolder<CDCandidate> >
            ( 1, ReferenceHolder<CDCandidate>(&aProduct ) ) )
{
}

CDDecay::CDDecay( const CDDecay& aProducts, const CDCandidate& aAdditionalProduct ) :
#ifdef EXTEND
    m_kmfit_chisq(500000.),
    m_vxfit_chisq(500000.),
#endif
    CDCandidate( aProducts.footPrint() + aAdditionalProduct.footPrint() ) ,
    m_children(  *new vector< ReferenceHolder<CDCandidate> >( aProducts.children() ) )
{
    m_children.push_back( ReferenceHolder<CDCandidate>(&aAdditionalProduct) ) ;
}

CDDecay::~CDDecay()
{
    delete &m_children ;
}

CDDecay::CDDecay() :
#ifdef EXTEND
    m_kmfit_chisq(500000.),
    m_vxfit_chisq(500000.),
#endif
    CDCandidate( CDFootPrint() ),
    m_children( *new vector< ReferenceHolder<CDCandidate> > )
{
}
//
// assignment operators
//
/*
   const CDDecay& CDDecay::operator=( const CDDecay& aOtherCDDecay )
   {
 *this = *(new CDDecay(aOtherCDDecay));
 return *this;
 }
 */

//
// member functions
//

void CDDecay::addChild( const CDCandidate& aAdditionalProduct )
{
    setCDFootPrint( footPrint() + aAdditionalProduct.footPrint() );
    m_children.push_back( ReferenceHolder<CDCandidate>(&aAdditionalProduct) ) ;
}

void CDDecay::setKinematicData( const KinematicData& aKinematicData )
{
    // Call setKinematicData of my super-class
    CDCandidate::setKinematicData( aKinematicData );
}

//
// const member functions
//

bool CDDecay::builtFromCDDecay() const
{
    return ( !false ) ;
}

const CDDecay& CDDecay::decay() const
{
    return ( *this ) ;
}

//------ numberChildren -----
// return the number of DCChildren a candidate has
//
uint32_t CDDecay::numberChildren() const
{
    return ( m_children.size() ) ;
}

//------ iterateChildern -----
// execute given function over all DCChildren
//
void CDDecay::iterateChildren( void ( * analyze )( const CDCandidate& ) ) const
{
    vector< ReferenceHolder<CDCandidate> >::const_iterator finished( m_children.end() ) ;
    for ( vector< ReferenceHolder<CDCandidate> >::const_iterator child( m_children.begin() ) ;
            child != finished ;
            ++child ) {
        analyze( (*(*child)) ) ;
    }
}

//------ iterateChildern -----
// execute given function over all DCChildren
//
void CDDecay::iterateChildren( DCAnalysisFunction< CDCandidate >& analyze ) const
{
    vector< ReferenceHolder<CDCandidate> >::const_iterator finished ( m_children.end() ) ;
    for ( vector< ReferenceHolder<CDCandidate> >::const_iterator child ( m_children.begin() ) ;
            child != finished ;
            ++child ) {
        analyze( (*(*child)) ) ;
    }
}

const CDCandidate& CDDecay::child( unsigned int aPosition ) const
{
    if ( aPosition >= m_children.size() ) {
        std::cerr << "Trying to access an invalid Child" << std::endl ;
        return ( *(m_children[ 0 ]) ) ;
    }
    return ( *(m_children[ aPosition ]) ) ;
}

const vector< ReferenceHolder<CDCandidate> >& CDDecay::children() const
{
    return ( m_children ) ;
}

KinematicData* 
CDDecay::defaultKinematicData() const
{
    int    userTag   = 1 ;
    double energySum = 0 ;
    int    chargeSum = 0 ;
    HepLorentzVector p4( 0.0 , 0.0 , 0.0, 0.0 ) ;
    vector< ReferenceHolder<CDCandidate> >::const_iterator finished( m_children.end() ) ;
    for ( vector< ReferenceHolder<CDCandidate> >::const_iterator child( m_children.begin() ) ;
            child != finished ;
            ++child ) {
	userTag &= (*(*child)).userTag();
        chargeSum += (*(*child)).charge() ;
        p4 += (*(*child)).p4() ;
    }

    KinematicData* tmp = new KinematicData();

    tmp->setUserTag( userTag );
    tmp->setCharge( chargeSum );
    tmp->setP4( p4 );

    return tmp;
}
//
// static member functions
//
