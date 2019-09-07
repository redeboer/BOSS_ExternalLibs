//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TrkVolume.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//	Class TrkVolume
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   May 6, 1997  - creation
//
// Copyright Information:
//	Copyright (C) 1997		Lawrence Berkeley Laboratory
//	Copyright (C) 1997	       CEA - Centre d'Etude de Saclay
//
//------------------------------------------------------------------------

//----------------
// BaBar Header --
//----------------
//#include "BaBar/BaBar.hh"

//-----------------------
// This Class's Header --
//-----------------------
#include "TrkBase/TrkVolume.h"


//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------



//----------------
// Constructors --
//----------------
TrkVolume::TrkVolume() : _tvname("Unknown")
{
}

TrkVolume::TrkVolume(const char* name) : _tvname(name)
{
}

//--------------
// Destructor --
//--------------
TrkVolume::~TrkVolume() 
{
}
