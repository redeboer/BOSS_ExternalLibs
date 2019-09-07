#ifndef DCHAIN_DCUNARYADAPTER_H
#define DCHAIN_DCUNARYADAPTER_H
// -*- C++ -*-
//
// Package:     <DChain>
// Module:      DCUnaryAdapter
// 
/**\class DCUnaryAdapter DCUnaryAdapter.h DChain/DCUnaryAdapter.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      Chris D Jones
// Created:     Thu Jan  5 13:30:20 EST 2006
// $Id: DCUnaryAdapter.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCUnaryAdapter.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:13  cdj
// massive class renaming, addition of [] for selection and unit tests
//

// system include files
#include <functional>

// user include files

// forward declarations
namespace dchain {
template<class TFunc>
class DCUnaryAdapter : public std::unary_function<typename TFunc::argument_type,
						  typename TFunc::result_type >
{
      // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------
      typedef typename TFunc::argument_type argument_type;
      typedef typename TFunc::result_type result_type;

      // ---------- Constructors and destructor ----------------
      DCUnaryAdapter() : m_func(0) {}
      DCUnaryAdapter(TFunc& iFunc) : m_func(&iFunc) {}
      DCUnaryAdapter(TFunc* iFunc) : m_func(iFunc) {}

      //virtual ~DCUnaryAdapter();

      // ---------- member functions ---------------------------

      // ---------- const member functions ---------------------
      result_type operator()(argument_type iArg) const {
	 return (*m_func)(iArg);
      }

      operator bool() {
	 return 0 != m_func;
      }

   private:
      // ---------- Constructors and destructor ----------------
      //DCUnaryAdapter( const DCUnaryAdapter& ); // stop default

      // ---------- assignment operator(s) ---------------------
      //const DCUnaryAdapter& operator=( const DCUnaryAdapter& ); // stop default

      // ---------- private member functions -------------------

      // ---------- private const member functions -------------

      // ---------- data members -------------------------------
      TFunc* m_func;

      // ---------- static data members ------------------------

};
}
// inline function definitions

#endif /* DCHAIN_DCUNARYADAPTER_H */
