#ifndef CLEODCHAIN_CDMUD_CC
#define CLEODCHAIN_CDMUD_CC
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDMud
// 
// Description: Manage the footprints for a given Evidence Class
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Mon Apr 20 20:47:30 PDT 1998
// $Id: CDMud.cc,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDMud.cc,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:36  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2000/12/19 15:11:34  cdj
// removed report statement
//
// Revision 1.1.1.1  2000/12/18 22:17:27  cdj
// imported CleoDChain
//
// Revision 1.4  1998/05/01 20:25:22  sjp
// Put in correct include file path
//
// Revision 1.3  1998/05/01 20:21:59  sjp
// Added line to handle g++ bug
//
// Revision 1.2  1998/04/22 01:24:02  sjp
// Corrected logic that expands list of CDFootPrints.
//
// Revision 1.1  1998/04/21 05:17:22  sjp
// New File
//

// system include files
#include <vector>

using std::vector;

// user include files
#include "BesDChain/CDFootPrint.h"
#include "BesDChain/CDMud.h"

//
// constants, enums and typedefs
//

template < class Evidence >
const CDFootPrint& CDMud< Evidence >::get( const Evidence* aEvidence )
{
   unsigned int id( aEvidence->trackId() ) ;
   vector< const CDFootPrint* >& prints( footprints() ) ;
   while ( id >= prints.size() ) {
      CDFootPrint* tmp( new CDFootPrint ) ;
      (*tmp).fresh() ;
      prints.push_back( tmp ) ;
   }
   return ( *(prints[ id ]) ) ;
}

template < class Evidence >
void CDMud< Evidence >::clear()
{
   vector< const CDFootPrint* >& prints( footprints() ) ;
   vector< const CDFootPrint* >::const_iterator finished( prints.end() ) ;
   for ( vector< const CDFootPrint* >::const_iterator print( prints.begin() ) ;
	 finished != print ;
	 ++print ) {

      // cast way const for delete to work
      delete ( (CDFootPrint*)(*print) ) ;
   }
   prints.erase( prints.begin() ,
		 prints.end() ) ;
}

template< class Evidence >
vector< const CDFootPrint* >&
CDMud< Evidence >::footprints()
{
   static vector< const CDFootPrint* > prints ;
   return ( prints ) ;
}

//
// constructors and destructor
//

//
// assignment operators
//

//
// member functions
//

//
// const member functions
//


#endif /* CLEODCHAIN_CDMUD_CC */
