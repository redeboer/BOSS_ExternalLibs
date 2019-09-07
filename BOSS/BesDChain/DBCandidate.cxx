// -*- C++ -*-
//
// Package:     CleoDB
// Module:      DBCandidate
// 
// Description: 
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Mon Apr  7 16:52:09 EDT 1997
// $Id: DBCandidate.cxx,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: DBCandidate.cxx,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.7  1999/10/08 23:06:12  wsun
// Added static functions outwardHypo(), inwardHypo(), and hypoIsInward().
//
// Revision 1.6  1999/06/04 07:39:15  wsun
// Added two more significant figures to particle masses.
//
// Revision 1.5  1999/05/07 18:19:36  marsh
// Added remaining outgoing hypotheses.
//
// Revision 1.4  1998/11/06 19:56:50  marsh
// Removed unneeded 'const's.
//
// Revision 1.3  1997/08/22 18:04:07  sjp
// Made mass into a function call
//
// Revision 1.2  1997/07/09 18:01:22  sjp
// New <Package>/<File>.h structure
// Modified so can also compile under cxx v5.5
//
// Revision 1.1  1997/05/28 16:09:49  sjp
// New class used with Track Fit data
//

// system include files

// user include files
#include "BesDChain/DBCandidate.h"


// STL classes

//
// constants, enums and typedefs
//

const double DBCandidate::book_mass[ DBCandidate::kMaxHypo ] = { 0.000000,
							      0.000511,
							      0.105658,
							      0.139570,
							      0.493677,
							      0.938272,
							      0.000511,
							      0.105658,
							      0.139570,
							      0.493677,
							      0.938272 } ;

//
// const member functions
//

double
DBCandidate::mass( DBCandidate::Hypo aHypo )
{
   return ( book_mass [ aHypo ] ) ;
}

DBCandidate::Hypo
DBCandidate::outwardHypo( DBCandidate::Hypo aHypo )
{
   switch( aHypo )
   {
      case DBCandidate::kElectron:
	 return DBCandidate::kExitElectron ;
      case DBCandidate::kMuon:
	 return DBCandidate::kExitMuon ;
      case DBCandidate::kChargedPion:
	 return DBCandidate::kExitPion ;
      case DBCandidate::kChargedKaon:
	 return DBCandidate::kExitKaon ;
      case DBCandidate::kProton:
	 return DBCandidate::kExitProton ;
      default:
	 return aHypo ;
   }
}

DBCandidate::Hypo
DBCandidate::inwardHypo( DBCandidate::Hypo aHypo )
{
   switch( aHypo )
   {
      case DBCandidate::kExitElectron:
	 return DBCandidate::kElectron ;
      case DBCandidate::kExitMuon:
	 return DBCandidate::kMuon ;
      case DBCandidate::kExitPion:
	 return DBCandidate::kChargedPion ;
      case DBCandidate::kExitKaon:
	 return DBCandidate::kChargedKaon ;
      case DBCandidate::kExitProton:
	 return DBCandidate::kProton ;
      default:
	 return aHypo ;
   }
}

