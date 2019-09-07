// -*- C++ -*-
// CLASSDOC OFF
// $Id: CList.h,v 1.2 2010/03/25 09:55:57 zhangy Exp $
// CLASSDOC ON
//
// This file is a part of what might become CLHEP -
// a Class Library for High Energy Physics.
// 
// This is the definition of the HepCList class.
//
// HepCList is a template based list class for storing copies of
// objects.  A HepCList can be made of any class where a copy
// constructor and a operator== is defined.
// 
//
// .SS Usage // A list of eg. integers is declared as
//
// .ft B
//      HepCList<int> intlist;
//
//
// .SS See Also
//
// AList.h AIterator.h, ConstAList.h, ConstAIterator.h, AListBase.h,
// AIteratorBase.h
//
// Author: Leif Lonnblad
//

#ifndef _CLIST_H_
#define _CLIST_H_

#include "MdcRecoUtil/AListBase.h"
#include "MdcRecoUtil/AList.h"
#include "MdcRecoUtil/ConstAList.h"

#ifdef HEP_NO_INLINE_IN_TEMPLATE_DECLARATION
#define inline
#endif

template <class T>
class  HepCList : public HepAListBase {

public:

  inline HepCList();
  // Constructs a list with no objects.

  inline HepCList(const HepCList<T> &);
  // copy constructor

  inline HepCList(const HepAList<T> &);
  inline HepCList(const HepConstAList<T> &);
  // copy from an AList

  inline ~HepCList();
  // Destroys the list. All objects are destroyed.

  inline void insert(const T &);
  // Inserts an object first in the list.

  inline void insert(const T & e, const T & r);
  // Inserts the object e just before the first occurence of
  // object r in the list. 

  inline void insert(const T & e, unsigned pos);
  // Inserts the object e at the position pos in the list. If pos is outside
  // the list, the object will be appended.

  inline void append(const T &);
  inline void operator += (const T &);
  // Appends an object in the end of the list

  inline void append(const T & e, const T & r);
  // Appends the object e just after the last occurrence of object r
  // in the list

  inline void append(const HepCList<T> &);
  inline void operator += (const HepCList<T> &);
  inline void append(const HepAList<T> &);
  inline void operator += (const HepAList<T> &);
  inline void append(const HepConstAList<T> &);
  inline void operator += (const HepConstAList<T> &);
  // Appends copies of all objects of list l to the end of this list.

  inline void remove(const T &);
  // Remove and delete all occurencies equal to  the object from the list.

  inline void remove(const HepCList<T> &);
  inline void remove(const HepConstAList<T> &);
  inline void remove(const HepAList<T> &);
  // Remove and delete all occurencies equal to the objects in list l
  // from this list.

  inline void remove(unsigned);
  // Remove and delete an object in the list.

  inline void removeAll();
  // Remove and delete all objects in the list.

  inline void purge();
  // Remove and delete all duplicate objects in the list.

  inline HepBoolean member(const T &) const;
  inline HepBoolean hasMember(const T &) const;
  // Returns true if the object is equal to a member of the list.

  inline void replace(const T & eo, const T & en);
  // Replace all occurencies of object eo with object en.

  inline T * operator[] (unsigned i) const;
  // Returns the i:th object in the list.
  // NOTE! the objects are numbered 0 to n-1.

  inline int index(const T &) const;
  // Returns the index of the last occurrence equal to the object.
  // NOTE! the objects are numbered 0 to n-1.

  inline int fIndex(const T &) const;
  // Returns the index of the first occurence of the object.
  // NOTE! the objects are numbered 0 to n-1.

  inline T * first() const;
  inline T * last() const;
  // Returns a pointer to the first and last object in the list.

  inline void operator = (const HepCList<T> &);
  inline void operator = (const HepAList<T> &);
  inline void operator = (const HepConstAList<T> &);
  // Assignment.

protected:

  inline T * Nth(unsigned i) const;
  // Returns the i:th object in the list.
  
};

#ifdef HEP_NO_INLINE_IN_TEMPLATE_DECLARATION
#undef inline
#endif

#ifdef HEP_SHORT_NAMES
#define CList HepCList
#endif

#include "MdcRecoUtil/CList.icc"

#endif

