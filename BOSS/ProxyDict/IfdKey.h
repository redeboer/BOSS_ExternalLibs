#ifndef IfdKey_HH
#define IfdKey_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdKey.h,v 1.1.1.1 2005/04/21 01:18:05 zhangy Exp $
//
// Description:
//    IfdKey is a Key, i.e., something that knows operator== and,
//    maybe someday, operator<.  Subclasses, like IfdIntKey and IfdStrKey
//    allow keys based on integer and string values.  Another derivative,
//    IfdTypeKey<T> serves as mapping between C++ static classes and
//    Keys.  Finally, IfdKey wroks with its derivative, IfdCompositeKey,
//    to form a composite pattern that allows lists, trees, etc. of
//    keys to be formed and yet have an op== defined on it.
//
//    Usage:  Keys do not know op=, i.e., assignemnt, and I have supressed
//    the copy ctor.  You should pass them by reference or poiner.  There
//    is a clone() method, so a consumer can effective copy a key.  Note
//    that this means that the caller retains ownership of the key, and
//    the "consumer" that takes a copy *owns* the copy.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//
// Bugs:
//
//------------------------------------------------------------------------

//
//  Reading IntIfdKey.hh after this is helpful.  Then tackle CompositeIfdKey.hh
//


#include <assert.h>
#include <stdlib.h>
class HepString;
#if !(defined(__GNUC__) && (__GNUC__ < 3) && (__GNUC_MINOR__ < 95)) // BABAR_IOSTREAMS_MIGRATION
#include <iosfwd>
#else                                                               // BABAR_IOSTREAMS_MIGRATION
class ostream;
#endif                                                              // BABAR_IOSTREAMS_MIGRATION
class IfdDictKey;
//template<class T> class TypeKey;

class IfdKey {
public:
   virtual ~IfdKey();

   virtual int operator==( const IfdKey & ) const = 0;
   inline int operator!=( const IfdKey &k ) const;


   virtual void add( const IfdKey& );

   inline int cardinality(void) const;
      // For Composite keys, == # of added elements.  For Non-Composites, == 0;

#if !(defined(__GNUC__) && (__GNUC__ < 3) && (__GNUC_MINOR__ < 95)) // BABAR_IOSTREAMS_MIGRATION
   virtual void print( std::ostream &o ) const = 0;
#else                                                               // BABAR_IOSTREAMS_MIGRATION
   virtual void print( ostream &o ) const = 0;
#endif                                                              // BABAR_IOSTREAMS_MIGRATION
#if !(defined(__GNUC__) && (__GNUC__ < 3) && (__GNUC_MINOR__ < 95)) // BABAR_IOSTREAMS_MIGRATION
   friend std::ostream& operator<<( std::ostream& o, const IfdKey & k );
#else                                                               // BABAR_IOSTREAMS_MIGRATION
   friend ostream& operator<<( ostream& o, const IfdKey & k );
#endif                                                              // BABAR_IOSTREAMS_MIGRATION

   virtual IfdKey*    clone( void ) const = 0;
      // Clone makes an exact copy of an object.  It is crucial that
      // derivatives implement this in a way that places the created
      // object on the heap.

    virtual unsigned int hash( void ) const
        { return _hashVal;}

    unsigned int  _hashVal;     //$$  public (ick) for now.  clean later.
                                
    static unsigned int nHashBuckets( void ) 
       { return _nHashBuckets; }

protected:
   enum         { _nHashBuckets = 1031 }; // .33 msec/ev, not opt

   enum keyKind { intKey, strKey, compositeKey,
		   typeKey, odfTypeKey };

   IfdKey( keyKind kind );
      // IfdKey is a pure interface.  Prevent creation of them.  Only
      // derivatives can be created and this ctor requires them to
      // define their keykind at ctor time.   This could have been
      // a virtual getKeyKind, but that affected performance.  See below.


   inline IfdKey::keyKind getKeyKind( void ) const { return _myKeyKind; }
      // IfdKeys are things that understand operator==.  We have made
      // this machinery rather than a simple overloaded global op==
      // because we wanted CompositeKey's, i.e., IfdKeys with subfields.
      // The enum above allows the subfields to be of various kinds,
      // each of which understands operator==.  Thus we can have
      // a composite key in which the first field is a TypeKey and the
      // second is a string. Below is an anonymous
      // union that we use to store the actual token being compared.
      // The enum allows us to access the union correctly.  If we
      // were to put these data into the derivative classes, we
      // would end up having to cast the argument of operator==(IfdKey &k)
      // to a specific type before we could do the ==.  This seems
      // cleaner.  This is slightly gross, but (1) we really do gain
      // something (2) Once we've handled int,string, and TypeKeys,
      // we are basically done, i.e., we don't expect much extension
      // here.

   keyKind _myKeyKind;
      // Originally, getKeyKind was virtual.  But that was too slow.  It
      // is reasonable to consider this a difference in value rather than
      // difference in behavior, so I've made getKeyKind a non-virtual 
      // accessor to _myKeyKind and we set _myKeyKind in the ctors.

   int _myCardinality;
      // See accessor.

   union {
      int          intVal;
      unsigned int uintVal;
      char*        strVal;
   };

 
   friend class IfdIntKey;
   friend class IfdStrKey;
   friend class IfdTypeKeyIFace;
   friend class IfdCompositeKey;
   friend class BdbOdfIfdTypeKeyIFace;
      // We need to declare these classes as friends because, for
      // example, IntKey::op==( IfdKey& k) must get at k.intVal, which
      // is protected.  Even though all of the IfdKey derivatives inherit
      // from IfdKey, the rule is that you can only get at protected
      // data in your *own* class, so IntKey can not get at IfdKey::intVal,
      // only IntKey::intVal.

  friend unsigned ifdKeyHash(const IfdDictKey& k);

private:
   virtual void make_vtab( void ) const;

   // Copy ctor and assignment op.
   //   Not sure what to do about these.  I think they may not
   //   make any sense, e.g., what if someone has IfdIntKey ik, IfdStrKey sk,
   //   and tries to do ik=sk?  That is nonsense.  It may be OK to do
   //   a copy ctor and implement it via clone; however, I will just turn
   //   off the copy ctor for now too.  If someone needs it, ask me
   //   and I'll think more about it.
   // 

   IfdKey( const IfdKey &) { ::abort(); }
   IfdKey& operator=( IfdKey &) { if ( this != 0 ) ::abort(); return *this; }
};


//*****************************************************************************
inline
int
IfdKey::cardinality(void) const { 
//*****************************************************************************
    return _myCardinality;
}

//*****************************************************************************
//inline
//IfdKey::keyKind
//IfdKey::getKeyKind( void ) const {
//*****************************************************************************
//    return _myKeyKind;
//}

//*****************************************************************************
inline
int
IfdKey::operator != ( const IfdKey &k ) const {
//*****************************************************************************

    return ! ( *this == k );
}


#endif /* IFDKEY_HH */
