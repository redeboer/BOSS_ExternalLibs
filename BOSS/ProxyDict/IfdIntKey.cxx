//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdIntKey.cxx,v 1.1.1.1 2005/04/21 01:18:05 zhangy Exp $
//
// Description:
//    Implementation of IfdIntKey.  See .hh for details.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      19 Jan 00         Creation of first version by moving
//                                    old code from .hh to here.
//
// Copyright Information:
//      Copyright (C) 1997
//
// Bugs:
//
//------------------------------------------------------------------------

//#include "BaBar/BaBar.hh"
#include "ProxyDict/IfdIntKey.h"


IfdIntKey::IfdIntKey( const int i ) 
  : IfdKey( intKey )
{
   intVal   =  i; 
   _hashVal = i % _nHashBuckets;
}

