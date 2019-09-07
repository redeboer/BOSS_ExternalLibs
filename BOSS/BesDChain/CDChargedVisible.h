#ifndef CLEODCHAIN_CDCHARGEDVISIBLE_H
#define CLEODCHAIN_CDCHARGEDVISIBLE_H
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDChargedVisible
// 
// Description: CDCharged candidate built from charged or truth table
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDChargedVisible.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDChargedVisible.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2004/03/05 22:01:49  chengp
// implemented Monte Carlo matching
//
// Revision 1.1  2001/04/11 13:19:13  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.3  2001/04/03 17:26:43  cdj
// moved builtFromCDCharged to builtFromTrack
//
// Revision 1.2  2001/03/23 23:05:34  urner
// added pi0 eta and CDKs decay lists
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.18  1998/04/17 18:52:21  sjp
// Modifed to use latest CLEO types
//
// Revision 1.17  1997/12/23 21:54:47  sjp
// Modified package to be completely templated
//
// Revision 1.16  1997/09/03 14:58:42  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.15  1997/08/29 17:01:17  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.14  1997/08/19 20:41:32  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.13  1997/01/31 20:10:18  sjp
// Modified to use the new `bug' include files
//
// Revision 1.12  1997/01/21 20:30:23  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.11  1996/12/20 21:01:42  sjp
// Extended pathname for Includes.
//
// Revision 1.10  1996/11/04 17:01:27  sjp
// Changed name to CDChargedVisible
//

// system include files

// user include files
#include "BesDChain/DBCandidate.h" // for enumerator
#include "BesDChain/CDCandidate.h"

// forward declarations
namespace DecayChain {
   class KinematicData;
}

class EvtRecTrack;
//zoujh: should be replaced in the future
//class BPion;
//class BKaon;

template < DBCandidate::Hypo H >
class CDChargedVisible : public CDCandidate
{
      // friend classes and functions

   public:
      // constants, enums and typedefs
      typedef CDCandidate CandidateClass ;
      typedef EvtRecTrack VisibleEvidence ;

      // Constructors and destructor
      CDChargedVisible( const EvtRecTrack* aVisible ) ;
      //zoujh: ...
      //CDChargedVisible( BPion* aPion ) ;
      //CDChargedVisible( BKaon* aKaon ) ;

      CDChargedVisible( const CDChargedVisible< H >& aOtherCDChargedVisible ) ;
      virtual ~CDChargedVisible() ;

      // member functions

      // const member functions
      virtual bool builtFromTrack() const ;
      virtual const EvtRecTrack* track() const ;
      inline virtual DBCandidate::Hypo hypo() const;

      // static member functions

   protected:
      // Constructors and destructor

      // protected member functions

      // protected const member functions
      virtual DecayChain::KinematicData* defaultKinematicData() const;

   private:
      // Constructors and destructor
      CDChargedVisible() ; // stop default

      // assignment operator(s)
      const CDChargedVisible& operator=( const CDChargedVisible< H >& aOtherCDChargedVisible ) ; // stop default

      // private member functions

      // private const member functions

      // data members
      const VisibleEvidence* m_visibleEvidence ;

      // static data members

} ;

template < DBCandidate::Hypo H >
inline DBCandidate::Hypo CDChargedVisible<H>::hypo() const
{
  return H;
}

#include "BesDChain/Template/CDChargedVisible.cc"

#endif /* CLEODCHAIN_CDCHARGEDVISIBLE_H */
