#ifndef CLEODB_DBCANDIDATE_H
#define CLEODB_DBCANDIDATE_H
// -*- C++ -*-
//
// Package:     CleoDB
// Module:      DBCandidatefit
// 
// Description: Namespace for Identifier
//
// Usage:
//    // The DBCandidate class is effectively a namespace that contains
//    // the enumerator to differentiate between fit hypothesis.
//
// Author:      Simon Patton
// Created:     Fri May 28 08:38:02 EDT 1997
// $Id: DBCandidate.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: DBCandidate.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.6  1999/10/08 23:06:14  wsun
// Added static functions outwardHypo(), inwardHypo(), and hypoIsInward().
//
// Revision 1.5  1999/05/07 18:19:39  marsh
// Added remaining outgoing hypotheses.
//
// Revision 1.4  1998/11/06 19:56:54  marsh
// Removed unneeded 'const's.
//
// Revision 1.3  1997/08/22 18:04:09  sjp
// Made mass into a function call
//
// Revision 1.2  1997/07/09 18:01:37  sjp
// New <Package>/<File>.h structure
// Modified so can also compile under cxx v5.5
//
// Revision 1.1  1997/05/28 16:09:16  sjp
// New class used with Track Fit data
//

// system include files
#include <stdint.h>

// user include files


// STL include

// forward declarations

class DBCandidate
{
   public:
      // constants, enums and typedefs
      typedef uint32_t Identifier ;

      enum Hypo {
	 kPhoton ,
	 kElectron ,
	 kMuon ,
	 kChargedPion ,
	 kChargedKaon ,
	 kProton ,
         kExitElectron ,
         kExitMuon ,
	 kExitPion ,
	 kExitKaon ,
	 kExitProton ,
	 kMaxHypo
      } ;

      static double mass( DBCandidate::Hypo aHypo ) ;

      // Return the outward Hypo for a given inward Hypo.
      static Hypo outwardHypo( DBCandidate::Hypo aHypo ) ;

      // Return the inward Hypo for a given outward Hypo.
      static Hypo inwardHypo( DBCandidate::Hypo aHypo ) ;

      // Functions to check if a given hypothesis is inward.  If not, it
      // is outward.
      static bool hypoIsInward( DBCandidate::Hypo aHypo )
      { return aHypo < kExitElectron ; }

   private:
      static const double book_mass[ kMaxHypo ] ;
};

// inline function definitions

#endif /* CLEODB_DBCANDIDATE_H */
