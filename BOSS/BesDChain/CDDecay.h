#ifndef CLEODCHAIN_CDDECAY_H
#define CLEODCHAIN_CDDECAY_H
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDDecay
// 
// Description: CDDecay candidate built from other candidates
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDDecay.h,v 1.2 2009/09/22 08:24:41 hujf Exp $
//
// Revision history
//
// $Log: CDDecay.h,v $
// Revision 1.2  2009/09/22 08:24:41  hujf
// see ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.8  2006/03/17 19:36:47  ponyisi
// Fix mcParticle() to use gammaFSR when available while doing matching
//
// Revision 1.7  2006/01/11 20:37:28  cdj
// work with renaming done in DChain package
//
// Revision 1.6  2004/10/19 13:39:38  cdj
// removed unimplemented CDDecay::parent() method
//
// Revision 1.5  2004/06/02 23:24:55  ryd
// Made constructor of CDDecay from CDCandidate explicit
//
// Revision 1.4  2004/03/05 22:01:49  chengp
// implemented Monte Carlo matching
//
// Revision 1.3  2003/05/15 19:58:15  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.2  2002/04/09 20:54:19  lyon
// Added CDDecay::setKinematicData
//
// Revision 1.1  2001/04/11 13:19:14  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.3  2001/04/04 15:16:50  cdj
// added a protected default constructor (used for storage)
//
// Revision 1.2  2001/04/03 17:25:57  cdj
// changed isCDDecay to builtFromCDDecay
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.11  1998/04/17 18:49:12  sjp
// Modifed to use latest CLEO types
//
// Revision 1.10  1997/09/03 14:58:43  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.9  1997/08/29 17:01:19  sjp
// Modified to handle new CDDecay Templated classes
//
// Revision 1.8  1997/08/26 04:02:08  sjp
// Added typedef for CandidateClass
//
// Revision 1.7  1997/08/19 23:01:40  sjp
// Restructured package to be independent of CleoDChain
//
// Revision 1.6  1997/08/19 20:41:37  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.5  1997/01/31 20:10:20  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/21 20:30:25  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1997/01/19 20:32:06  sjp
// Changed LargeCount to Count
//
// Revision 1.2  1996/12/20 20:59:50  sjp
// Extended pathname for Includes.
//
// Revision 1.1  1996/11/04 17:01:57  sjp
// New class for CDDecay CDCandidate
//

// system include files
#include <stdint.h>
#include <vector>

// user include files
#include "DecayChain/Element/DCChildren.h" // For enumerator
#include "BesDChain/CDCandidate.h" // superclass
#include "DecayChain/Element/ReferenceHolder.h"

// forward declarations
template < class Arg > class DCAnalysisFunction ;

namespace DecayChain {
   class KinematicData;
}

class CDDecay : public CDCandidate
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef CDCandidate CandidateClass ;
            
      // Constructors and destructor
      CDDecay( const CDDecay& aOtherCDDecay ) ;
      explicit CDDecay( const CDCandidate& aProduct ) ;
      CDDecay( const CDDecay& aProducts  ,
	     const CDCandidate& aAdditionalProduct ) ;
      virtual ~CDDecay() ;

      // member functions
      void addChild( const CDCandidate& aAdditionalProduct ) ;

      // Replace the KinematicData for this decay. Useful for
      // saving results of a fit.
      void setKinematicData( const DecayChain::KinematicData& aKinematicData );
      
      // const member functions
      virtual bool builtFromCDDecay( void ) const ;
      virtual const CDDecay& decay( void ) const ;
      uint32_t numberChildren() const ;
      void iterateChildren( void ( * analyze )( const CDCandidate& ) ) const ;
      void iterateChildren( DCAnalysisFunction< CDCandidate >& analyze ) const ;
      const CDCandidate& child( unsigned int aPosition ) const ;
      const std::vector<dchain::ReferenceHolder<CDCandidate> >& children() const ;
      // static member functions
      // add for kinematic fit
#ifdef EXTEND
      double chisq() const { return m_kmfit_chisq;}
      void chisq(double c) { m_kmfit_chisq = c;}
      double vchisq() const { return m_vxfit_chisq;}
      void vchisq(double c) { m_vxfit_chisq = c;}
   protected:
      double m_kmfit_chisq;
      double m_vxfit_chisq;
#endif
   protected:
      // Constructors and destructor
      CDDecay() ; // used for storage

      // protected member functions
      virtual DecayChain::KinematicData* defaultKinematicData() const ;

   private:
      // Constructors and destructor

      // assignment operator(s)
      const CDDecay& operator=( const CDDecay& aOtherCDDecay ) ; // stop default

      // private member functions

      // data members
      std::vector< dchain::ReferenceHolder<CDCandidate> >& m_children ;

      // static data members

} ;

typedef const CDDecay& FixedCDDecay ;
typedef CDDecay& PossibleCDDecay ;

// inline function definitions

#endif /* CLEODCHAIN_CDDECAY_H */
