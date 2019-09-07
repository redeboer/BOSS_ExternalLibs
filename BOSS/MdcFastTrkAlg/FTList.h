// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTList
// 
// Description:  list class template for MdcFastTrkAlg
//

#ifndef FTList_FLAG_
#define FTList_FLAG_

//#include <alloc.h>
#include <stdlib.h>

template <class T>
class FTList{
public:
  
  /// default constructor
  FTList(int length_to_alloc = 100);

  /// copy constructor
  FTList(const FTList<T> &);

  /// destructor
  ~FTList();

  /// append an object into the end of the list
  int append(const T& x);

  /// append objects into the end of the list
  int append(const FTList<T> &);

  /// remove objects by index and returns decremented index and length
  int remove(int &);

  /// remove objects by index
  void remove2(int);

  /// replace index-th object by the object src
  void replace(int i, T src);

  /// delete objects by index and returns decremented index and length
  int deleteObj(int &);

  /// remove just last objects of the list
  void removeLast(void);

  /// clear lists but the allocated memory remains same
  void clear(void);

  /// clear lists and free memory
  void removeAll(void);

  /// delete all object and clear(allocated memory remains same)
  void deleteAll(void);

  /// re-allocate memory to reduce size
  void resize(void);

  /// returns a object by index
  T & operator[] (unsigned i) const;

  /// returns the reference of a object by index
  T & operator() (unsigned i) const;

  /// returns the first object in the list
  T & first(void) const;

  /// returns the pointer of first object
  T* firstPtr(void) const;

  /// returns the pointer of last object
  T* lastPtr(void) const;

  /// returns the length of the list
  int length(void) const;

private: // private data members
  int _length;
  int _remain;
  int _length_to_alloc;
  T * _obj;
};

//----------------------------------------------
#ifdef FTList_NO_INLINE
#define inline
#else
#undef inline
#define FTList_INLINE_DEFINE_HERE
#endif

#ifdef FTList_INLINE_DEFINE_HERE

template <class T>
inline
FTList<T>::FTList(int length_to_alloc) 
  : _length(0),
    _remain(length_to_alloc),
    _length_to_alloc(length_to_alloc),
    _obj((T *) malloc(length_to_alloc * sizeof(T))){
}

template <class T> 
FTList<T>::FTList(const FTList<T> & src)
  : _length(src._length),
    _remain(src._remain),
    _length_to_alloc(src._length_to_alloc)
{
  _obj = (T *) malloc((_length + _remain) * sizeof(T));
  T * srcObj = src._obj;
  for (int i = 0; i < _length; i++){
    *(_obj+i) = *(srcObj+i);
  }
}

template <class T>
inline
FTList<T>::~FTList(){
  free(_obj);
}

template <class T>
inline
int
FTList<T>::append(const T& src){
  if (!_remain){
    _obj = (T *) realloc(_obj,(_length+_length_to_alloc) * sizeof(T));
    _remain = _length_to_alloc;
  }
  *(_obj+(_length++)) = src;
  _remain--;
  return _length;
}

template <class T>
inline
int
FTList<T>::remove(int & iterator){
  *(_obj+(iterator--)) = *(_obj+(--_length));
  _remain++;
  return _length;
}

template <class T>
inline
void
FTList<T>::remove2(int iterator){
  *(_obj+iterator) = *(_obj+(--_length));
  _remain++;
}

template <class T>
inline
void
FTList<T>::replace(int i, T src){
  *(_obj+i) = src;
}

template <class T>
inline
int
FTList<T>::deleteObj(int & iterator){
  delete *(_obj+iterator);
  *(_obj+(iterator--)) = *(_obj+(--_length));
  _remain++;
  return _length;
}

template <class T>
inline
void
FTList<T>::removeLast(void){
  _length--;
  _remain++;
}

template <class T>
inline
void
FTList<T>::clear(void){
  _remain += _length;
  _length = 0;
}

template <class T>
inline
void
FTList<T>::removeAll(void){
  free(_obj);
  _remain = 0;
  _length = 0;
  _obj = NULL;
}


template <class T>
inline
void
FTList<T>::resize(void){
  _obj = (T *) realloc(_obj,_length * sizeof(T));
  _remain = 0;
  _length_to_alloc = _length;
}

template <class T>
inline
T &
FTList<T>::operator[] (unsigned i) const{
  return *(_obj+i);
}

template <class T>
inline
T &
FTList<T>::operator() (unsigned i) const{
  return *(_obj+i);
}

template <class T>
inline
T &
FTList<T>::first(void) const{
  return *_obj;
}

template <class T>
inline
T*
FTList<T>::firstPtr(void) const{
  return _obj;
}

template <class T>
inline
T*
FTList<T>::lastPtr(void) const{
  return _obj + (_length - 1);
}

template <class T>
inline
int 
FTList<T>::length(void) const{
  return _length;
}

template <class T> 
int FTList<T>::append(const FTList<T> & src)
{
  int srcLength = src._length;
  T * srcObj = src._obj;
  int i = 0;
  if (_remain < srcLength){
    _obj = (T *) realloc(_obj,(_length + _remain + srcLength) * sizeof(T));
    while(i^srcLength) *(_obj+(_length++)) = *(srcObj+(i++));
  }else{
    while(i^srcLength) *(_obj+(_length++)) = *(srcObj+(i++));
    _remain -= srcLength;
  }
  return _length;
}

template <class T> 
void FTList<T>::deleteAll(void)
{
  int i = _length;
  while (i) delete *(_obj + (--i));
  clear();
}

#endif

#undef inline

#endif /* FTList_FLAG_ */

