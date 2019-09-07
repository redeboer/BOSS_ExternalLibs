//
// Directly from fzisan, useful class created by Kakuno-san
//
#ifndef KalFitList_FLAG_
#define KalFitList_FLAG_

//#include <alloc.h>
#include <stdlib.h>

template <class T>
class KalFitList{
public:
  
  /// default constructor
  KalFitList(int length_to_alloc = 100);

  /// copy constructor
  KalFitList(const KalFitList<T> &);

  /// destructor
  ~KalFitList();

  /// append an object into the end of the list
  int append(T x);

  /// append objects into the end of the list
  int append(const KalFitList<T> &);

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
  T operator[] (unsigned i) const;

  /// returns the reference of a object by index
  T & operator() (unsigned i) const;

  /// returns the first object in the list
  T first(void) const;

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
#ifdef KalFitList_NO_INLINE
#define inline
#else
#undef inline
#define KalFitList_INLINE_DEFINE_HERE
#endif

#ifdef KalFitList_INLINE_DEFINE_HERE

template <class T>
inline
KalFitList<T>::KalFitList(int length_to_alloc) 
  : _length(0),
    _remain(length_to_alloc),
    _length_to_alloc(length_to_alloc),
    _obj((T *) malloc(length_to_alloc * sizeof(T))){
}

template <class T> 
KalFitList<T>::KalFitList(const KalFitList<T> & src)
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
KalFitList<T>::~KalFitList(){
  free(_obj);
}

template <class T>
inline
int
KalFitList<T>::append(T src){
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
KalFitList<T>::remove(int & iterator){
  *(_obj+(iterator--)) = *(_obj+(--_length));
  _remain++;
  return _length;
}

template <class T>
inline
void
KalFitList<T>::remove2(int iterator){
  *(_obj+iterator) = *(_obj+(--_length));
  _remain++;
}

template <class T>
inline
void
KalFitList<T>::replace(int i, T src){
  *(_obj+i) = src;
}

template <class T>
inline
int
KalFitList<T>::deleteObj(int & iterator){
  delete *(_obj+iterator);
  *(_obj+(iterator--)) = *(_obj+(--_length));
  _remain++;
  return _length;
}

template <class T>
inline
void
KalFitList<T>::removeLast(void){
  _length--;
  _remain++;
}

template <class T>
inline
void
KalFitList<T>::clear(void){
  _remain += _length;
  _length = 0;
}

template <class T>
inline
void
KalFitList<T>::removeAll(void){
  free(_obj);
  _remain = 0;
  _length = 0;
  _obj = NULL;
}


template <class T>
inline
void
KalFitList<T>::resize(void){
  _obj = (T *) realloc(_obj,_length * sizeof(T));
  _remain = 0;
  _length_to_alloc = _length;
}

template <class T>
inline
T
KalFitList<T>::operator[] (unsigned i) const{
  return *(_obj+i);
}

template <class T>
inline
T &
KalFitList<T>::operator() (unsigned i) const{
  return *(_obj+i);
}

template <class T>
inline
T
KalFitList<T>::first(void) const{
  return *_obj;
}

template <class T>
inline
T*
KalFitList<T>::firstPtr(void) const{
  return _obj;
}

template <class T>
inline
T*
KalFitList<T>::lastPtr(void) const{
  return _obj + (_length - 1);
}

template <class T>
inline
int 
KalFitList<T>::length(void) const{
  return _length;
}

template <class T> 
int KalFitList<T>::append(const KalFitList<T> & src)
{
  int srcLength = src._length;
  T * srcObj = src._obj;
  int i = 0;
  if (_remain < srcLength){
    _obj = (T *) realloc(_obj,(_length_to_alloc + srcLength) * sizeof(T));
    while(i^srcLength) *(_obj+(_length++)) = *(srcObj+(i++));
  }else{
    while(i^srcLength) *(_obj+(_length++)) = *(srcObj+(i++));
    _remain -= srcLength;
  }
  return _length;
}

template <class T> 
void KalFitList<T>::deleteAll(void)
{
  int i = _length;
  while (i) delete *(_obj + (--i));
  clear();
}

#endif

#undef inline

#endif /* KalFitList_FLAG_ */
