// File: Ext_err_valid.cc
//
// Check the validity of the error matrix and set the invalid element to 0.
//
// Creation: 13-Nov-1998
// Version: 04-Mar-1999
//
// $Id: Ext_err_valid.cxx,v 1.2 2010/03/25 03:20:12 wangll Exp $
//
// Revision history
//
// $Log: Ext_err_valid.cxx,v $
// Revision 1.2  2010/03/25 03:20:12  wangll
// see the ChangeLog
//
// Revision 1.1.1.1  2005/08/10 06:59:26  wangll
// First import TrkExtAlg.
//
// Revision 1.7  2000/04/13 22:13:14  katayama
// added std:: to cout,cerr,endl,stream etc
//
// Revision 1.6  1999/03/05 07:03:27  teramoto
// More for the treatment for the invalid error matrix.
//
// Revision 1.5  1999/03/05 02:13:43  teramoto
// Reduction of the frequency of the invalid error matrix message.
//
// Revision 1.4  1999/02/26 09:30:02  teramoto
// Suppress the invalid matrix error message and temporary fix the negative
// diagonal element problem of the error matrix by forcing them to zero.
//
// Revision 1.3  1999/02/20 10:18:02  teramoto
// Added error calculation skip function. Reduced error messages.
//
// Revision 1.2  1998/11/18 06:53:58  teramoto
// Reduce the error messages for the invalid track error matrix.
//
// Revision 1.1  1998/11/13 11:20:19  teramoto
// Modification for four purposes.
// (1) Put protections for invalid error matrix values with error messages.
// (2) Change the default media_list parameter from 0 to 1.
// (3) Fill both the version 0 and 1 format panther banks as default.
// (4) Put mandatory comment items in the heading comment lines.
//

#include	<iostream>
#include	"CLHEP/Matrix/SymMatrix.h"

using namespace CLHEP;

static const double	Large( 1.0e13 );		// large number.

/*
  valid(). Check the validity of the diagonal elements and if the
  element is not valid, force the element to 0.0.
*/

bool Ext_err_valid( bool msg, HepSymMatrix &error, const int dimension )
{
  bool valid( 1 );
  double trace( 0 );

  for( int i = 1; i<=dimension; i++ ){
    double elem( error( i, i ) );
    trace += elem;
    if( elem < 0.0 ){
      valid = 0;
      if( msg ){
	std::cout << "%ERROR detected at Ext_err_valid: error(" 
	<< i << "," << i << ") = " << elem << " < 0.0. "
	<< "Force to 0.0." << std::endl;
      }
      error( i, i ) = 0.0;
    } else if( elem > Large ){
      valid = 0;
      if( msg ){
	std::cout << "%ERROR detected at Ext_err_valid: error(" 
	<< i << "," << i << ") = " << elem << " > " << Large
	<< ". Force to " << Large << std::endl;
      }
      error( i, i ) = Large;
    }
  }
  if( !trace ) valid = 0;
  return( valid );
}

/*
  This only checks but it does not force to set 0.
*/

bool Ext_err_valid( bool msg, const HepSymMatrix &error, const int dimension )
{
  bool valid( 1 );
  double trace( 0 );

  for( int i = 1; i<=dimension; i++ ){
    double elem( error( i, i ) );
    trace += elem;
    if( elem < 0.0 ){
      valid = 0;
      if( msg ){
	std::cout << "%ERROR detected at Ext_err_valid: error matrix: error(" 
	<< i << "," << i << ")= " << elem << " < 0.0." << std::endl;
      }
    } else if( elem > Large ){
      valid = 0;
      if( msg ){
	std::cout << "%ERROR detected at Ext_err_valid: error matrix: error(" 
	<< i << "," << i << ")= " << elem << " > " << Large << std::endl;
      }
    }
  }
  if( !trace ) valid = 0;
  return( valid );
}
