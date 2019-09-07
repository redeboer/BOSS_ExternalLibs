//  File:  GmsList.hh
//  Authors:  Alan Breakstone, Gary Word
//   sfs -- added insertAfter()

/* This class is derived from a similar class in "A C++ Toolkit",
   which is Copyright 1991 by Jonathan S. Shapiro, and is used
   with permission.  "A C++ Toolkit" is published by Prentice Hall, Inc. */

//  Contents ---------------------------------------------------------
//
//	GmsList
//
//  Description:
//   
//	C++ header file for Gismo GmsList class
//
//  End --------------------------------------------------------------


//  Interface Dependencies -------------------------------------------

#ifndef GMSLIST_H
#define GMSLIST_H

#include "MdcTrkRecon/GmsListLink.h"
 
//  End Interface Dependencies ---------------------------------------


//  Class  //

class GmsList
{
protected:
	GmsListLink *_first;
	GmsListLink *_last;
	unsigned int _count;
public:
	GmsList() { _first = _last = 0; _count = 0;}
	virtual ~GmsList();
	GmsListLink *last()const     { return _last; }
	GmsListLink *first()const    { return _first; }
	unsigned int count()const { return _count;}

	GmsList& append( GmsListLink * );
	GmsList& prepend( GmsListLink * );
	GmsList& remove( GmsListLink * );
        GmsList& insertAfter( GmsListLink *link, GmsListLink *insertHere);
        GmsList& moveAfter( GmsListLink *link, GmsListLink *insertHere);

        void reset() { _first = _last = 0; _count = 0;}
};

#endif
