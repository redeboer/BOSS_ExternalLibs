//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdStrKey.cxx,v 1.1.1.1 2005/04/21 01:18:05 zhangy Exp $
//
// Description:
//    Implementation of IfdStrKey.  See .hh for details.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      18 Sep 97         Creation of first version
//
// Copyright Information:
//      Copyright (C) 1997
//
// Bugs:
//
//------------------------------------------------------------------------

//#include "BaBar/BaBar.hh"
#include "ProxyDict/IfdStrKey.h"
#if !(defined(__GNUC__) && (__GNUC__ < 3) && (__GNUC_MINOR__ < 95)) // BABAR_IOSTREAMS_MIGRATION
using std::ostream;
#endif                                                              // BABAR_IOSTREAMS_MIGRATION

//****************************************************************************
IfdStrKey::IfdStrKey( const char *s )
//****************************************************************************

  : IfdKey( strKey )

{

    // NB: strncpy would be nice below, but we've already died in strlen
    // if thats an issue.

  register size_t strSize =  strlen(s)+1;

  // If the space allocated in this instance of IfdStrKey is big enough
  // to hold the string, then put it there; otherwise do the slower operation
  // of allocating it in the heap.  Either way, make strVal point at it
  // to avoid "if allocated" sort of logic later.

  if ( strSize < IFDKEY_BUFSIZE  ) {
      strcpy(_stringBuf, s);
      strVal = &_stringBuf[0];
      _onHeap = false;
  } else {
      strVal = new char[ strSize ];
      strcpy(strVal, s);
      _onHeap = true;
  }

  enum {
    MaxChar = 64 		// Limit computation.  Don't need strlen()
  };

  _hashVal = 0;

  size_t n = 0;
  while ( (s[n] != '\0') && (n < MaxChar) ) {
    _hashVal = ( (_hashVal<<8) + s[n] ) % _nHashBuckets;
    n++;
  }
}

#ifndef VXWORKS
//****************************************************************************
IfdStrKey::IfdStrKey( const std::string& str )
//****************************************************************************

  : IfdKey( strKey )

{
  using std::string;
  const char* s = str.c_str();
  register size_t strSize =  str.size()+1;

  // register size_t strSize =  strlen(s)+1;

  // If the space allocated in this instance of IfdStrKey is big enough
  // to hold the string, then put it there; otherwise do the slower operation
  // of allocating it in the heap.  Either way, make strVal point at it
  // to avoid "if allocated" sort of logic later.

  if ( strSize < IFDKEY_BUFSIZE  ) {
      strcpy(_stringBuf, s);
      strVal = &_stringBuf[0];
      _onHeap = false;
  } else {
      strVal = new char[ strSize ];
      strcpy(strVal, s);
      _onHeap = true;
  }

  enum {
    MaxChar = 64 		// Limit computation.  Don't need strlen()
  };

  _hashVal = 0;

  size_t n = 0;
  while ( (s[n] != '\0') && (n < MaxChar) ) {
    _hashVal = ( (_hashVal<<8) + s[n] ) % _nHashBuckets;
    n++;
  }
}
#endif

//****************************************************************************
IfdStrKey::IfdStrKey( const char *s, unsigned int hashVal )
//****************************************************************************
// Used for the clone function.  We keep this private because we don't trust
// anyone else to set the hash value correctly/consistently.  The aim here
// is to save the cost of calling the hash function during a clone.

  : IfdKey( strKey )
{

  // This code is cut/paste from the other constructor.  This is harder
  // to maintain but in this case we very much need the speed gained by
  // saving a call.

  register size_t strSize =  strlen(s)+1;

  if ( strSize < IFDKEY_BUFSIZE  ) {
      strcpy(_stringBuf, s);
      strVal = &_stringBuf[0];
      _onHeap = false;
  } else {
      strVal = new char[strlen(s)+1];
      strcpy(strVal, s);
      _onHeap = true;
  }

  _hashVal =  hashVal;		// Override the IfdKey base class init of this.
}

//****************************************************************************
IfdStrKey::~IfdStrKey() {
//****************************************************************************
  if (_onHeap) delete[] strVal;
  strVal=0;
}

//****************************************************************************
IfdKey*
IfdStrKey::clone(void) const {
//****************************************************************************
  return new IfdStrKey( strVal, _hashVal );
}

//****************************************************************************
int
IfdStrKey::operator==( const IfdKey& k ) const {
//****************************************************************************


    if ( (strKey == k.getKeyKind()) && (_hashVal == k._hashVal) ) {

	// Then they might actually match!  Check the strings.
	// We used to use strcmp, but this is faster by x2.2.
        char* s1 = strVal;
        char* s2 = k.strVal;
	while (*s1 == *s2++) {
	  if ( *s1++ == '\0') return 1;   //NB: == in while => *s2 == \0 too.
	}
     }

     return 0;
  }


//****************************************************************************
int
IfdStrKey::operator<( const IfdKey& k ) const {
//****************************************************************************
  return ( strKey == k.getKeyKind() ) && (strcmp(strVal, k.strVal) < 0);
}

//****************************************************************************
void
IfdStrKey::print( ostream &o) const {
//****************************************************************************
  o << "IfdStrKey(" << strVal << ")"; 
}

