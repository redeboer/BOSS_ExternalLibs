// -*- C++ -*-
// CLASSDOC OFF
// $Id: ConstAList.h,v 1.2 2010/03/25 09:55:57 zhangy Exp $
// CLASSDOC ON
//
// This file is a part of what might become CLHEP -
// a Class Library for High Energy Physics.
// 
// This is the definition of the HepConstAList class. It is used in the same
// way as the HepAList class but the object are stored as const pointers.
// HepAListIterator cannot be used for this list, use HepConstAListIterator
// instead.
//
// .SS See Also
// AIterator.h, List.h, ConstAIterator.h, AListBase.h, AIteratorBase.h
//
// .SS History
// HepConstAList was developed from an original (non-template) list class
// written by Dag Bruck.
//
// Author: Leif Lonnblad
//

#ifndef _CONSTALIST_H_
#define _CONSTALIST_H_

#include "MdcRecoUtil/AList.h"

#ifdef HEP_NO_INLINE_IN_TEMPLATE_DECLARATION
#define inline
#endif

template <class T>
class  HepConstAList : public HepAListBase {

public:

  inline HepConstAList();
  // Constructs a list with no objects.

  inline HepConstAList(const HepAList<T> &);
  // Copy constructor from non-const HepAList

  inline HepConstAList(const HepConstAList<T> &);
  // Copy constructor

  inline ~HepConstAList();
  // Destroys the list. The objects in the list are not destroyed.

  inline void insert(const T *);
  inline void insert(const T &);
  // Inserts an object first in the list.

  inline void insert(const T * e, const T * r);
  inline void insert(const T & e, const T & r);
  // Inserts the object e just before the first occurence of
  // object r in the list. 

  inline void insert(const T * e, unsigned pos);
  inline void insert(const T & e, unsigned pos);
  // Inserts the object e at the position pos in the list. If pos is outside
  // the list, the object will be appended.

  inline void append(const T *);
  inline void append(const T & e);
  inline void operator += (const T *);
  inline void operator += (const T &);
  // Appends an object in the end of the list

  inline void append(const T * e, const T * r);
  inline void append(const T & e, const T & r);
  // Appends the object e just after the last occurrence of object r
  // in the list

  inline void append(const HepAList<T> & l);
  inline void append(const HepConstAList<T> & l);
  inline void operator += (const HepAList<T> & l);
  inline void operator += (const HepConstAList<T> & l);
  // Appends all objects of list l to the end of this list.

  inline void remove(const T *);
  inline void remove(const T &);
  // Remove all occurencies of the object from the list.

  inline void remove(unsigned);
  // Remove an object from the list.

  inline void remove(const HepAList<T> &);
  inline void remove(const HepConstAList<T> &);
  // Remove all occurencies of the objects in list l from this list.

  inline HepBoolean member(const T *) const;
  inline HepBoolean member(const T &) const;
  inline HepBoolean hasMember(const T *) const;
  inline HepBoolean hasMember(const T &) const;
  // Returns true if the object is a member of the list.

  inline void replace(const T * eo, const T * en);
  inline void replace(const T & eo, const T & en);
  // Replace all occurencies of object eo with object en.

  inline const T * operator[] (unsigned i) const;
  // Returns the i:th object in the list.
  // NOTE! the objects are numbered 0 to n-1.

  inline int index(const T *) const;
  inline int index(const T &) const;
  // Returns the index of the last occurrence of the object.
  // NOTE! the objects are numbered 0 to n-1.

  inline int fIndex(const T *) const;
  inline int fIndex(const T &) const;
  // Returns the index of the first occurence of the object.
  // NOTE! the objects are numbered 0 to n-1.

  inline const T * first() const;
  inline const T * last() const;
  // Returns a pointer to the first and last object in the list.

  inline void operator = (const HepAList<T> &);
  inline void operator = (const HepConstAList<T> &);
  // Assignment.
  
  inline void sort(int (*compfunc)(const T **, const T **) );
  // sort the list according to the function.
  // the function is the same as one used by the C standard routine qsort.

};

#ifdef HEP_NO_INLINE_IN_TEMPLATE_DECLARATION
#undef inline
#endif

#ifdef HEP_SHORT_NAMES
#define CAList HepConstAList
#endif

#include "MdcRecoUtil/ConstAList.icc"

#endif
