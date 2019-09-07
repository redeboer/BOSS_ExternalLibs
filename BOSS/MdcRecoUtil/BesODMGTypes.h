/*
//--------------------------------------------------------------------------
// BesODMGTypes.h
//
// File and Version Information:
// 	$Id: BesODMGTypes.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Typedefs of ODMG d_* platform-invariant types for use in code
//      compiled without Objectivity.  (Objectivity will otherwise define
//      these types itself.)
//
// Environment:
//	Software developed for the Bes Detector at the SLAC B-Factory.
//
// Author List:
//	Dirk Duellman, RD45	Originator
//	David Quarrie   	Brought into Bes, bug fixes
//	Gregory Dubois-Felsmann	Moved to Bes/BesODMGTypes.h
//
// Copyright Information:
//	Copyright (C) 1997	California Institute of Technology et al.
// 
// History:
//	Migration for BESIII MDC
//
//--------------------------------------------------------------------------
*/

#ifndef BESODMGTYPES_H
#define BESODMGTYPES_H

/*
// Original RD45 header:
//                         -*- Mode: C++ -*- 
//  ODMGTypes.h -- define ODMG types based on C++ types
//  created on Thu Apr  4 13:17:51 1996
//  by         Dirk Duellmann 
//
// Brought into HepODBMS/odbms/HepODMGTypes.h by David Quarrie
// (Id: HepODMGTypes.h,v 1.5 1997/12/22 15:56:57 quarrie Exp )
//
// Copied from HEAD of HepODBMS (V00-00-07 plus RCSv1.5 of this file)
// by Gregory Dubois-Felsmann to package Bes, to implement common
// platform-invariant-size types for DataFlow.
//
// Comments changed to C style as a step toward making it useful in that
// language, too, but *this has not been otherwise tested*.
//
// d_Char and d_Octet changed from "signed char" to "char" to conform
// more precisely with Objectivity definitions.  This does mean that
// their signedness can vary from platform to platform, but this is 
// better than having the type of d_Char depend on whether Objectivity
// is included in a build or not.
*/

/* It is only legitimate to define these types if Objectivity has */
/* not itself been set up by including "oo.h".  oo.h defines all  */
/* the types below as macros (not typedefs).  The use of typedefs */
/* here comes from RD45.                                          */

#ifndef OO_H

/* Switch over architectures */

#if defined(__DECCXX) || defined(__alpha__)	

/* DEC Unix (C++ or g++) */

typedef signed short int      d_Short;
typedef signed int            d_Long;
typedef unsigned short int    d_UShort;
typedef unsigned int          d_ULong;
typedef float                 d_Float;
typedef double                d_Double;
typedef char                  d_Char; 
typedef char                  d_Octet;
typedef unsigned char         d_Boolean;
typedef unsigned char         uint8;

#else

/* All other architectures */

/* This is no longer needed with the aCC compiler on HPs. */
/*   No other Bes platforms had this property.          */
/* #ifdef Hep_NO_SIGNED_KEYWORD */
/*   / * hp compiler does not know about the signed keyword * / */
/*   #define signed */
/* #endif */


typedef signed short          d_Short;
typedef signed int            d_Long;
typedef unsigned short        d_UShort;
typedef unsigned int          d_ULong;
typedef float                 d_Float;
typedef double                d_Double;
typedef char                  d_Char; 
typedef char                  d_Octet;
typedef unsigned char         d_Boolean;
typedef unsigned char         uint8; 


/* #ifdef Hep_NO_SIGNED_KEYWORD */
/*   / * hp compiler does not know about the signed keyword * / */
/*   #undef signed */
/* #endif */


/* End of architecture selection */
#endif

/* End of check on OO_H */
#endif

/* End of BESODMGTYPES_H */
#endif 
