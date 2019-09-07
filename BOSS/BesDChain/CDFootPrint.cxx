// -*- C++ -*-
//
// Package:     DChain
// Module:      CDFootPrint
// 
// Description: keep record of which building-block a CDCandidate uses.
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Wed Oct 30 13:29:34 EST 1996
// $Id: CDFootPrint.cxx,v 1.3 2009/10/14 07:18:50 hujf Exp $
//
// Revision history
//
// $Log: CDFootPrint.cxx,v $
// Revision 1.3  2009/10/14 07:18:50  hujf
// see ChangeLog
//
// Revision 1.2  2009/09/22 08:24:41  hujf
// see ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:01  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/30 20:13:08  cdj
// CDFootPrint now resets its m_numberIssued when the last CDFootPrint is deleted
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.13  1998/04/17 18:55:50  sjp
// Modified to use latest types
//
// Revision 1.12  1997/09/03 14:58:34  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.11  1997/08/29 17:00:36  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.10  1997/08/19 23:02:48  sjp
// Restructured package to be independent of CleoDChain
//
// Revision 1.9  1997/08/19 20:40:23  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.8  1997/01/21 20:36:02  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.7  1996/12/20 21:26:55  sjp
// major overhaul, and fixed memoery bug
//
// Revision 1.6  1996/11/04 16:48:35  sjp
// Variable length storage is implemented
//
// Revision 1.3  1996/04/06 02:49:28  sjp
// Added equality check and `contains' function
//
// Revision 1.2  1996/02/20 00:36:00  sjp
// Changed 'fresh' to return CDFootPrint, made other return values 'const'
//
// Revision 1.1  1995/11/09 20:01:57  sjp
// New class to keep track of object that have already been `used'.
//

// system include files
#include <stdlib.h>  // For 'exit'
#include <iostream>

// user include files
#include "BesDChain/CDFootPrint.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//
uint32_t CDFootPrint::m_numberIssued = 0 ;
uint32_t CDFootPrint::m_numberFootprints = 0 ;


std::ostream& operator << ( std::ostream& os, const CDFootPrint& obj ) {
   os << "0x" << std::hex;
   for ( int i = obj.m_size-1; i >= 0; i-- ) {
      os << obj.m_array[i];
   }
   os << std::dec;

   return os;
}
// friend classses and functions

//
// constructors and destructor
//

//------ default constructor -----
//
CDFootPrint::CDFootPrint() :
    m_size(0),
    m_array(0)
{
    ++m_numberFootprints;
}

//------ copy constructor -----
//
CDFootPrint::CDFootPrint( const CDFootPrint& aOtherPrint ) :
    m_size(0) ,
    m_array(0)
{
    //
    // book the memory and copy contents of aOtherPrint
    //
    resize( aOtherPrint.m_size );
    for ( uint32_t index = 0 ;
            index != m_size ;
            ++index ) {
        m_array[ index ] = aOtherPrint.m_array[ index ] ;
    }
    ++m_numberFootprints;
}

//------ Destructor -----
//
CDFootPrint::~CDFootPrint()
{
    //
    // delete memory
    //
    delete []  m_array ;
    if( 0 == --m_numberFootprints ) {
        reset();
    }
}

//
// assignment operators
//

const CDFootPrint& CDFootPrint::operator=( const CDFootPrint& aOtherPrint )
{
    if ( this == &aOtherPrint ) {
        //
        // if Footprint is assigned to itself do nothing.
        //
        return( *this ) ;
    }
    //
    // book the memory and copy contents of aOtherPrint
    //
    resize( aOtherPrint.m_size ) ;
    for ( uint32_t index = 0 ;
            index != m_size ;
            ++index ) {
        m_array[ index ] = aOtherPrint.m_array[ index ] ;
    }
    return ( *this ) ;
}

const CDFootPrint& CDFootPrint::operator+=( const CDFootPrint& aOtherPrint )
{
    if ( this == &aOtherPrint ) {
        //
        // if Footprint is added and assigned to itself do nothing.
        //
        return(*this);
    }   
    if ( m_size >= aOtherPrint.m_size ) {
        //
        // if this Footprint is larger or equal than to aOtherPrint,
        //   only `or' with the contents of aOtherPoint
        //
        for ( uint32_t index = 0 ;
                index != aOtherPrint.m_size ;
                ++index) {
            m_array[ index ] |= aOtherPrint.m_array[ index ] ;
        }
    }
    else {
        //
        // if this Footprint is smaller than to aOtherPrint book new memory
        // Note: can not use resize, as this thorws away old memory
        //
        uint32_t* tmp_ptr = new uint32_t[ aOtherPrint.m_size ];
        if ( 0 == tmp_ptr ) {
            std::cerr << "No memory to allocate another kinematicData" << std::endl ;
            exit( 1 ) ;
        }
        //
        // for the length of the old memory, fill the new memory with
        //   `or' of old and aOtherPrint
        //
        for ( uint32_t index_1 = 0 ;
                index_1 != m_size ;
                ++index_1 ) {
            tmp_ptr[ index_1 ] = m_array[ index_1 ] | aOtherPrint.m_array[ index_1 ] ;
        }
        //
        // for the rest of length of the new memory, fill with aOtherPrint
        //
        for ( uint32_t index_2 = m_size ;
                index_2 != aOtherPrint.m_size ;
                ++index_2) {
            tmp_ptr[ index_2 ] = aOtherPrint.m_array[ index_2 ] ;
        }
        //
        // delete old memory
        //    
        delete [] m_array ;
        //
        // update member data elements
        //
        m_size = aOtherPrint.m_size;
        m_array = tmp_ptr ;
    }
    return ( *this ) ;   
}

//
// member functions
//

//------ fresh -----
// assign a fresh footprint to this object
//
CDFootPrint& CDFootPrint::fresh( void )
{
    if ( m_size != 0 ) {
        //
        // if already assigned a value do nothing
        //
        return ( *this ) ;
    }

    const uint32_t kBitsInByte = 8 ;
    //
    // Take a number and increase the number of CDFootPrints issued
    //
    uint32_t freshNumber = m_numberIssued++ ;
    //
    // calculate which bit, and which element to set
    //
    uint32_t element = freshNumber / ( kBitsInByte * sizeof( uint32_t ) ) ;
    uint32_t offsetInElement = freshNumber % ( kBitsInByte * sizeof( uint32_t ) ) ;
    //
    // book enough memory
    //
    resize( element + 1 ) ;
    //
    // fill all but the last part of the memory with zeros
    //
    for ( uint32_t index = 0 ;
            index < element ;
            ++index ) {
        m_array[ index ] = uint32_t( 0 );
    }
    //
    // fill in last part of memory with correct bit set
    //
    m_array[ element ] = uint32_t(1) << offsetInElement ;
    return ( *this ) ;
}

//------ resize -----
// resize the arry for the footprint
// Note: This routine does NOT copy the current contents into the
//	   new memory
//
void CDFootPrint::resize( const uint32_t aNewSize )
{
    if ( aNewSize == m_size ) {
        //
        // if Footprint is already the right size do nothing.
        //
        return ;
    }
    //
    // allocate a new section of memory, and delete old section
    //
    uint32_t* tmp_ptr = new uint32_t[ aNewSize ] ;
    if ( 0 == tmp_ptr ) {
        std::cerr << "No memory to allocate another kinematicData" << std::endl ;
        exit( 1 ) ;
    }
    delete [] m_array ;
    //
    // update member data elements
    //
    m_size = aNewSize ;
    m_array = tmp_ptr ;
    //
    return ;
}

//
// const member functions
//

//------ equality -----
// test to see if two footprints are identical
//
bool CDFootPrint::operator==(const  CDFootPrint& aOtherPrint ) const
{
    if ( this == &aOtherPrint ) {
        //
        // if being compare with itself return `true'
        //
        return( !false ) ;
    }
    //
    // find the shortest length to compare the two memory sections
    //
    uint32_t shorterSize ;
    if ( m_size > aOtherPrint.m_size ) {
        shorterSize = aOtherPrint.m_size ;
    }
    else {
        shorterSize = m_size ;
    }
    //
    // check shorter CDFootPrint matches the section longer CDFootPrint
    //
    uint32_t index = 0;      
    while ( ( index != shorterSize ) &&
            ( m_array[ index ] == aOtherPrint.m_array[ index ] ) ) {
        ++index ;
    }
    //
    // if check finished before the shorter CDFootPrint was covered,
    //   then two CDFootPrints are unequal
    //
    if ( index != shorterSize ) {
        return ( false ) ;
    }
    //
    // check that the rest of the longer footprint is zero
    //
    if ( m_size >= aOtherPrint.m_size ) {
        while ( ( index != m_size ) &&
                ( m_array[ index ] == uint32_t( 0 ) ) )  {
            ++index ;
        }
        return ( m_size == index );
    }
    else {
        while( ( index != aOtherPrint.m_size ) &&
                (aOtherPrint.m_array[ index ] == uint32_t( 0 ) ) ) {
            ++index ;
        }
        return ( aOtherPrint.m_size == index );
    }
}

//------ non-equality -----
// test to see if two footprints are not idential
//
bool CDFootPrint::operator!=( const CDFootPrint& aOtherPrint ) const
{
    return ( ! operator == ( aOtherPrint ) ) ;
}

//------ addition -----
// combine two CDFootPrints to create a third
//
CDFootPrint CDFootPrint::operator+( const CDFootPrint& aOtherPrint ) const
{
    CDFootPrint result( *this ) ;
    result += aOtherPrint ;
    return ( result ) ;
}


//------ overlap -----
// true if this CDFootPrint overlaps with the other one
//
bool CDFootPrint::overlap( const CDFootPrint& aOtherPrint) const
{
    if (this == & aOtherPrint) {
        //
        // if being tested with itself return `true'
        //
        return( !false );
    } 
    //
    // find the shortest length to compare the two memoery sections
    //
    uint32_t shorterSize ;
    if ( m_size > aOtherPrint.m_size ) {
        shorterSize = aOtherPrint.m_size ;
    }
    else {
        shorterSize = m_size ;
    }
    //
    // check shorter CDFootPrint matches the section longer CDFootPrint
    //
    uint32_t index = 0;      
    while ( ( index != shorterSize ) &&
            ( 0 == ( m_array[ index ] & aOtherPrint.m_array[ index ] ) ) ) {
        ++index ;
    }
    //
    // if check finished before the shorter CDFootPrint was covered,
    //   then two CDFootPrints have at least one bit in common
    //
    return ( index != shorterSize ) ;
}

//------ contains -----
// true if this CDFootPrint contains the other one
//
bool CDFootPrint::contains( const CDFootPrint& aOtherPrint) const
{
    if (this == & aOtherPrint) {
        //
        // if being tested with itself return `true'
        //
        return(! false);
    } 
    //
    // find the shortest length to compare the two memoery sections
    //
    uint32_t shorterSize ;
    if ( m_size > aOtherPrint.m_size ) {
        shorterSize = aOtherPrint.m_size ;
    }
    else {
        shorterSize = m_size ;
    }

    //
    // for shorter CDFootPrint check aOtherPrint is within this one
    //
    uint32_t index = 0 ;      
    while ( ( index != shorterSize ) &&
            ( m_array[ index ] ==( m_array[index] | aOtherPrint.m_array[index] ) ) ) {
        ++index ;
    }
    //
    // if check finished before the shorter CDFootPrint was covered,
    //   then aOtherPrint has at least one bit outside this CDFootPrint
    //
    if ( index != shorterSize ) {
        return ( false ) ;
    }
    //
    // if this CDFootPrint is the longer then aOtherPrint is totally contained
    //
    if ( m_size > aOtherPrint.m_size ) {
        return ( !false ) ;
    }
    //
    // as aOtherPrint is longer need to check rest of it is zero
    //
    while( ( index != aOtherPrint.m_size ) &&
            (aOtherPrint.m_array[ index ] == uint32_t( 0 ) ) ) {
        ++index ;
    }
    //
    // if check finished before aOtherPrint was finished
    //   then aOtherPrint has at least one bit in the rest of it
    //
    return ( aOtherPrint.m_size == index ) ;
}

//
// static member functions
//

//------ reset (a Static Function) -----
// set number issued to zero
//
void CDFootPrint::reset()
{
    m_numberIssued = 0 ;
}

