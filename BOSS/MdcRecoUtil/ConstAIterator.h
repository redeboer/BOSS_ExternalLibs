// -*- C++ -*-
// CLASSDOC OFF
// $Id: ConstAIterator.h,v 1.2 2010/03/25 09:55:57 zhangy Exp $
// CLASSDOC ON
//
// This file is a part of what might beome the CLHEP -
// a Class Library for High Energy Physics.
// 
// This is the definition of the HepConstAListIterator class. It is used
// in the same way as the HepAListIterator class but can also be used with
// HepConstAList and it only returns const pointers to objects.
//
// .SS
// AList.h, ConstAList.h, AIterator.h, AListBase.h, AIteratorBase.h
//
// .SS History
// HepConstAListIterator was developed from an original (non-template) list
// iterator class written by Dag Bruck.
//
// Author: Leif Lonnblad
//

#ifndef _CONSTAITERATOR_H_
#define _CONSTAITERATOR_H_

#include "MdcRecoUtil/AIteratorBase.h"
#include "MdcRecoUtil/ConstAList.h"

#ifdef HEP_NO_INLINE_IN_TEMPLATE_DECLARATION
#define inline
#endif

template <class T>
class HepConstAListIterator : public HepAListIteratorBase {

public:

  inline HepConstAListIterator(const HepAList<T> &);
  inline HepConstAListIterator(const HepConstAList<T> &);
  // Constructor taking a corresponding list as argument. Starting at the 
  // first element.

  inline HepConstAListIterator(const HepConstAListIterator<T> &);
  // Copy constructor.

  inline ~HepConstAListIterator();

  inline const T * operator () ();
  inline const T * next();
  // Returns a pointer to the current object in the associated list, moving
  // forward to the next. Returns 0 if all objects are done.

  inline const T * prev();
  // Moves backward one step in the list and returns the object found there.
  // If current object is the first in the list, no stepping is done and 0 is
  // returned.

  inline const T * current() const;
  // Returns a pointer to the current object in the associated list,
  // without incrementing the index.
  
};

#ifdef HEP_NO_INLINE_IN_TEMPLATE_DECLARATION
#undef inline
#endif

#ifdef HEP_SHORT_NAMES
#define CAIterator HepConstAListIterator
#endif

#include "MdcRecoUtil/ConstAIterator.icc"

#endif // _ALISTITERATOR_H_
