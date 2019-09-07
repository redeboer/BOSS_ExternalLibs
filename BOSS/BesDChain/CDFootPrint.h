#ifndef CLEODCHAIN_CDFOOTPRINT_H
#define CLEODCHAIN_CDFOOTPRINT_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      CDFootPrint
// 
// Description: keep record of which building-block a CDCandidate uses.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Oct 30 13:29:19 EST 1996
// $Id: CDFootPrint.h,v 1.3 2009/10/14 07:18:50 hujf Exp $
//
// Revision history
//  Modified by Sang-Joon Lee                             06/26/96
//
// $Log: CDFootPrint.h,v $
// Revision 1.3  2009/10/14 07:18:50  hujf
// see ChangeLog
//
// Revision 1.2  2009/09/22 08:24:41  hujf
// see ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:17  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/30 20:13:09  cdj
// CDFootPrint now resets its m_numberIssued when the last CDFootPrint is deleted
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.12  1998/04/17 18:54:22  sjp
// Modified to use latest CLEO types
//
// Revision 1.11  1997/08/19 23:01:42  sjp
// Restructured package to be independent of CleoDChain
//
// Revision 1.10  1997/08/19 20:42:02  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.9  1997/01/21 20:36:06  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.8  1996/12/20 21:26:59  sjp
// major overhaul, and fixed memoery bug
//
// Revision 1.7  1996/11/04 16:48:31  sjp
// Variable length storage is implemented
//
// Revision 1.4  1996/04/06 02:49:24  sjp
// Added equality check and `contains' function
//
// Revision 1.3  1996/02/27 15:03:57  sjp
// Extended Array size to 6 to handle MCParticles
//
// Revision 1.2  1996/02/20 00:36:15  sjp
// Changed 'fresh' to return CDFootPrint, made other return values 'const'
//
// Revision 1.1  1995/11/09 20:02:10  sjp
// New class to keep track of object that have already been `used'.
//

// system include files
#include <stdint.h>
#include <iostream>

// user include files

// forward declarations
class FingerPrint;

class CDFootPrint
{
    // friend classses and functions
    friend std::ostream& operator << ( std::ostream& os, const CDFootPrint& obj );
    friend class FingerPrint;

    public:
    // Constructors and destructor
    CDFootPrint() ;
    CDFootPrint( const CDFootPrint& ) ;
    virtual ~CDFootPrint() ;

    // assignment operator(s)
    const CDFootPrint& operator=( const CDFootPrint& );
    const CDFootPrint& operator+=( const CDFootPrint& aOtherPrint ) ;

    // member functions
    CDFootPrint& fresh() ;

    // const member functions
    bool operator==( const CDFootPrint& aOtherPrint ) const ;
    bool operator!=( const CDFootPrint& aOtherPrint ) const ;
    CDFootPrint operator+( const CDFootPrint& aOtherPrint ) const ;
    bool overlap( const CDFootPrint& aOtherPrint) const ;
    bool contains( const CDFootPrint& aOtherPrint) const ;

    // static member functions
    static void reset() ;
    
    uint32_t ToUInt32() const { return m_array[0]; }
    // protected const member functions

    private:
    // Constructors and destructor

    // private member functions
    void resize( const uint32_t aNewSize ) ;

    // private const member functions

    // data members
    uint32_t  m_size ;
    uint32_t* m_array ;

    // static data members
    static uint32_t m_numberIssued ;
    //when number of footprints reaches 0, reset m_numberIssued
    static uint32_t m_numberFootprints;

} ;

// inline function definitions

#endif /* CLEODCHAIN_CDFOOTPRINT_H */
