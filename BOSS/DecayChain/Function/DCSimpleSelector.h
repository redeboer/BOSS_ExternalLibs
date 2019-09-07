#ifndef DCHAIN_DCSIMPLESELECTOR_H
#define DCHAIN_DCSIMPLESELECTOR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      DCSimpleSelector
// 
/**\class DCSimpleSelector DCSimpleSelector.h DChain/DCSimpleSelector.h

 Description: Create a DCSelectionFunction from simple parts

 Usage:
    To make use of this ability, you must place your selection functions in
    the DChain namespace (avoids having && and || override builtin version)
    
    namespace DChain {
    class Selector1 ...
    class Selector2 ...
    class Selector3 ...
    }
    using DChain::Selector1;
    using DChain::Selector2;

    Then in our code
    Selector1 s1; Selector2 s2; Selector3 s3;
    DCSimpleSelector<Decay> jointSelector = s1 && s2 || s3;
    DCDecayList<Decay> myStuff( jointSelector);
*/
//
// Author:      Chris D Jones
// Created:     Tue Sep 30 15:41:54 EDT 2003
// $Id: DCSimpleSelector.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCSimpleSelector.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.4  2006/01/11 20:28:13  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.3  2003/10/27 22:59:25  cdj
// added operator= to DCSimpleSelector
//
// Revision 1.2  2003/10/12 23:16:44  cdj
// simplified the use of the simple selector
//
// Revision 1.1  2003/10/02 23:55:00  cdj
// changed compound selection to be simple selector (including expression templates)
//
// Revision 1.1  2003/10/01 23:45:17  cdj
// added compound selection function
//

// system include files

// user include files
#include "DecayChain/Function/DCSelectionFunction.h"

// forward declarations
namespace DChain {
// base class for operation used to combine Selectors
   template<class Arg>
   class MethodBase {
      public:
	 typedef Arg arg_type;
	 virtual bool select(Arg&) = 0;
	 virtual MethodBase* clone() const = 0;
   };

   //combine Selectors using logical AND
   template<class LHS, class RHS, class Arg>
   class MethodAnd : public MethodBase<Arg> {
      public:
	 typedef Arg arg_type;

	 MethodAnd( const LHS& iLHS, const RHS& iRHS) :
	 m_lhs(iLHS),
	 m_rhs(iRHS) {
	 }

	 MethodAnd();

	 bool select(Arg& iArg) {
	    return m_lhs.select(iArg) && m_rhs.select(iArg);
	 }

	 MethodBase<Arg>* clone() const {
	    return new MethodAnd<LHS,RHS,Arg>( *this );
	 }
      private:
	 LHS m_lhs;
	 RHS m_rhs;
   };

   //combine Selectors using logical OR
   template<class LHS, class RHS, class Arg>
   class MethodOr : public MethodBase<Arg> {
      public:
	 typedef Arg arg_type;

	 MethodOr( const LHS& iLHS, const RHS& iRHS) :
	 m_lhs(iLHS),
	 m_rhs(iRHS) {
	 }
	 MethodOr();

	 bool select(Arg& iArg) {
	    return m_lhs.select(iArg) || m_rhs.select(iArg);
	 }

	 MethodBase<Arg>* clone() const {
	    return new MethodOr<LHS,RHS,Arg>( *this );
	 }
      private:
	 LHS m_lhs;
	 RHS m_rhs;
   };

   //Adapter for a Selector so it can be used as a Method
   template<class Arg>
   class MethodHolder : public MethodBase<Arg> {
      public:
	 typedef Arg arg_type;

	 MethodHolder(DCSelectionFunction<Arg>& iSelector) :
	    m_selector(iSelector) {}
	 MethodBase<Arg>* clone() const {
	    return new MethodHolder<Arg>(*this);
	 }
	 bool select(Arg& iArg ) {
	    return m_selector( iArg);
	 }
      private:
	 DCSelectionFunction<Arg>& m_selector;
   };

   //Create a test to see if a class inherits from DCSelectionFunction
   // this is needed to determine if we need to create a
   // MethodHolder

   //These classes are used only for the return value of our test
   struct PassTest {
	 char m_size[12];
   };
   struct FailTest {
	 char m_size[1];
   };

   //This function will be used if our class does inherits
   template <class Arg> PassTest testForHolder(const DCSelectionFunction<Arg>* );
   //This function will be used if our class doesn't inherit
   FailTest testForHolder(...);

   //This class actually determines if we should use a MethodHolder
   // for the class T
   template <class T, bool> class UseHolder {
      public:
	 typedef T Method;
   };
   template <class T> class UseHolder<T,false> {
      public: 
	 typedef T Method;
   }; 
   template <class T> class UseHolder<T,true> {
      public:
	 typedef MethodHolder<typename T::arg_type> Method;
	 typedef typename Method::arg_type arg_type;
   };
   //End code needed for test


   //Determine exactly what MethodType to use for class T
   //  mostly used to make using UseHolder easier
   template <class T> class MethodType {
      public:
	 //static const T& make();
	 typedef typename UseHolder<T, (sizeof(testForHolder((T*)0))==sizeof(PassTest) ) >::Method Method;
	 typedef typename Method::arg_type arg_type;
   };


   //If the two arguments are not identical, pick the one that is more specialized (I.e. if B inherits from A, pick B)

   //Chooses which of the first two template based on the third argument
   template<class T1, class T2, bool> struct ChooseArg {
	 typedef T1 arg_type;
   };
   template<class T1, class T2> struct ChooseArg<T1,T2,false> {
	 typedef T2 arg_type;
   };

   //This class needed to get around bug in g++
   template<class T1, class T2> struct PickArgTester {
	 static PassTest inheritsFrom(const T2*);
	 static FailTest inheritsFrom(...);
   };

   template<class T1, class T2> class PickArg {
      public:
	 static PassTest inheritsFrom(const T2*);
	 static FailTest inheritsFrom(...);

	 typedef typename ChooseArg<T1, T2, (
	    sizeof(PickArgTester<T1,T2>::inheritsFrom((T1*)0))
	    ==sizeof(PassTest) )  >::arg_type arg_type;
   };
   
   //overload && to allow nice syntax for constructing a DCSimpleSelector
   //This type is only needed to work around a Solaris 2.8 CC bug
   //  If the arg_type of LHS and RHS do not agree, pick the most 
   //  specialized one
   template< class LHS, class RHS> struct AndOpReturn {
	 typedef MethodAnd<typename MethodType<LHS>::Method,
	     typename MethodType<RHS>::Method,
             typename PickArg< typename MethodType<LHS>::arg_type,
	                       typename MethodType<RHS>::arg_type>::arg_type > Return;
   };

   template<class LHS, class RHS>
      inline
      typename AndOpReturn<LHS,RHS>::Return
      operator&&(LHS& iLHS, RHS& iRHS) {
      return
	 AndOpReturn<LHS,RHS>::Return(
	 typename MethodType<LHS>::Method(iLHS),
	 typename MethodType<RHS>::Method(iRHS) );
   }

   template<class LHS, class RHS>
      inline
      typename AndOpReturn<LHS,RHS>::Return
      operator&&(const LHS& iLHS, RHS& iRHS) {
      return 
	 AndOpReturn<LHS,RHS>::Return( 
	    typename MethodType<LHS>::Method(iLHS),
	    typename MethodType<RHS>::Method(iRHS) );
   }

   template<class LHS, class RHS>
      inline
      typename AndOpReturn<LHS,RHS>::Return
      operator&&(const LHS& iLHS, const RHS& iRHS) {
      return 
	 AndOpReturn<LHS,RHS>::Return( 
	    typename MethodType<LHS>::Method(iLHS),
	    typename MethodType<RHS>::Method(iRHS) );
   }

   template< class LHS, class RHS> struct OrOpReturn {
	 typedef MethodOr<typename MethodType<LHS>::Method,
	                  typename MethodType<RHS>::Method,
             typename PickArg< typename MethodType<LHS>::arg_type,
	    typename MethodType<RHS>::arg_type>::arg_type > Return;
   };

   template<class LHS, class RHS>
      inline
      typename OrOpReturn<LHS,RHS>::Return
   operator||(LHS& iLHS, RHS& iRHS) {
      return OrOpReturn<LHS,RHS>::Return ( 
	    typename MethodType<LHS>::Method(iLHS),
	    typename MethodType<RHS>::Method(iRHS) );
   }

   template<class LHS, class RHS>
      inline
      typename OrOpReturn<LHS,RHS>::Return
   operator||(const LHS& iLHS, RHS& iRHS) {
      return OrOpReturn<LHS,RHS>::Return ( 
	    typename MethodType<LHS>::Method(iLHS),
	    typename MethodType<RHS>::Method(iRHS) );
   }

   template<class LHS, class RHS>
      inline
      typename OrOpReturn<LHS,RHS>::Return
   operator||(const LHS& iLHS, const RHS& iRHS) {
      return OrOpReturn<LHS,RHS>::Return ( 
	    typename MethodType<LHS>::Method(iLHS),
	    typename MethodType<RHS>::Method(iRHS) );
   }

   //Creates a Method that will allow you to call another MethodT that
   // has a different but compatable Arg
   template<class MethodT, class Arg> class MethodAdapter : public MethodBase<Arg> {
      public:
	 MethodAdapter(const MethodT& iMethod ) :
	    m_method(iMethod) {}
	 bool select(Arg& iArg) {
	    return m_method.select(iArg);
	 }

	 MethodBase<Arg>* clone() const {
	    return new MethodAdapter<MethodT,Arg>( *this );
	 }
      private:
	 MethodT m_method;

   };


template<class Arg>
class DCSimpleSelector : public DCSelectionFunction<Arg>
{
      // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------

      // ---------- Constructors and destructor ----------------
      DCSimpleSelector() : m_method(0) {}
      template<class T>
      DCSimpleSelector(const T& iMethod ) :
	 m_method( new DChain::MethodAdapter<T,Arg>(iMethod) ) {}
	   
      DCSimpleSelector(const DChain::MethodBase<Arg>& iMethod ) :
	 m_method( iMethod.clone() ) {}
      virtual ~DCSimpleSelector() {
	 delete m_method;
      }

      // ---------- member functions ---------------------------
      virtual bool operator()( Arg& iArg) {
	 if( 0 != m_method ) {
	    return m_method->select(iArg);
	 }
	 return true;
      }

      DCSimpleSelector( const DCSimpleSelector<Arg>& iCopy ) : m_method(0) {
	 if( 0 != iCopy.m_method) {
	    m_method = iCopy.m_method->clone();
	 }
      }
	    
      const DCSimpleSelector<Arg>& operator=( const DCSimpleSelector<Arg>& iRHS) {
         DCSimpleSelector<Arg> temp(iRHS);
         swap(temp);
         return *this;
      }
private:
      // ---------- Constructors and destructor ----------------

      void swap( DCSimpleSelector<Arg>& iOther ) {
         DChain::MethodBase<Arg>* temp = m_method;
         m_method = iOther.m_method;
         iOther.m_method = temp;
      }
      // ---------- assignment operator(s) ---------------------

      // ---------- data members -------------------------------
      DChain::MethodBase<Arg>* m_method;
};

}
using DChain::DCSimpleSelector;

// inline function definitions

// Uncomment the following lines, if your class is templated 
// and has an implementation file (in the Template directory)
//#if defined(INCLUDE_TEMPLATE_DEFINITIONS)
//# include "DChain/Template/DCSimpleSelector.cc"
//#endif

#endif /* DCHAIN_DCSIMPLESELECTOR_H */
