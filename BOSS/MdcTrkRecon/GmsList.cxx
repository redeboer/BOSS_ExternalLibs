//  File:  GmsList.cc
//  Authors:  Alan Breakstone, Gary Word

/* This class is derived from a similar class in "A C++ Toolkit",
   which is Copyright 1991 by Jonathan S. Shapiro, and is used
   with permission.  "A C++ Toolkit" is published by Prentice Hall, Inc. */

//  Contents ----------------------------------------------------------
//
//	GmsList::append(  GmsListLink *l )
//	GmsList::prepend( GmsListLink *l )
//	GmsList::remove(  GmsListLink *l )
//
//  End ---------------------------------------------------------------

#include "MdcTrkRecon/GmsList.h"

GmsList::~GmsList() {}

GmsList& GmsList::append( GmsListLink *l )
{  // add an item to the end of a list
	if ( _last ) {
		_last->_next = l;
		l->_prev = _last;
	}
	else
		_first = l;
	
	_last = l;

	_count++;

	return *this;
}

GmsList& GmsList::prepend( GmsListLink *l )
{  // add an item to the beginning of a list
	if ( _first ) {
		_first->_prev = l;
		l->_next = _first;
	}
	else
		_last = l;

	_first = l;

	_count++;

	return *this;
}
GmsList& GmsList::insertAfter( GmsListLink *l, GmsListLink *here )
{  // add an item to the middle of a list
        GmsListLink *after = 0;
	if ( here != 0 ) {
	  after = here->_next;
	  here->_next = l;
	  l->_prev = here;
	}
	else {
	  after = _first;
	  l->_prev = 0;
	  _first = l;
	}
	l->_next = after;

	if (after == 0) {
	  _last = l;
	}
	else {
	  after->_prev = l;
	}

	_count++;

	return *this;
}
GmsList& GmsList::moveAfter( GmsListLink *l, GmsListLink *here )
{  // add an item from one place in list to another

        // First remove it from its current position
	if ( l == _first )
		_first = _first->_next;
	if ( l == _last )
		_last = _last->_prev;
	
	if ( l->_next ) {
		l->_next->_prev = l->_prev;
	}
	if ( l->_prev ) {
		l->_prev->_next = l->_next;
	}


        GmsListLink *after = 0;
	if ( here != 0 ) {
	  after = here->_next;
	  here->_next = l;
	  l->_prev = here;
	}
	else {
	  after = _first;
	  l->_prev = 0;
	  _first = l;
	}
	l->_next = after;

	if (after == 0) {
	  _last = l;
	}
	else {
	  after->_prev = l;
	}


	return *this;
}

GmsList& GmsList::remove( GmsListLink *l )
{  // remove an item from the list
	if ( l == _first )
		_first = _first->_next;
	if ( l == _last )
		_last = _last->_prev;
	
	if ( l->_next ) {
		l->_next->_prev = l->_prev;
	}
	if ( l->_prev ) {
		l->_prev->_next = l->_next;
	}
	l->_next = 0;
	l->_prev = 0;

	_count--;

	return *this;
}
