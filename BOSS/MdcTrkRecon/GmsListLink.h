//  File:  GmsListLink.h
//  Author:  Alan Breakstone

/* This class is derived from a similar class in "A C++ Toolkit",
   which is Copyright 1991 by Jonathan S. Shapiro, and is used
   with permission.  "A C++ Toolkit" is published by Prentice Hall, Inc. */

//  Contents ---------------------------------------------------------
//
//	GmsListLink
//
//  Description:
//   
//	C++ header file for Gismo GmsListLink class
//
//  End --------------------------------------------------------------


//  Interface Dependencies -------------------------------------------

#ifndef GMSLISTLINK_H
#define GMSLISTLINK_H
 
//  End Interface Dependencies ---------------------------------------


//  Class  //

class GmsListLink
{  // base class for any object that needs to be part of a doubly-linked list
	friend class GmsList;
protected:              	// make available to derived classes
	GmsListLink *_next;	// pointer to next item in list
	GmsListLink *_prev;	// pointer to previous item in list
public:
	GmsListLink() { _next = _prev = 0; }
	virtual ~GmsListLink();
	GmsListLink *next()const { return _next; }
	GmsListLink *prev()const { return _prev; }
};

#endif
