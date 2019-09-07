#ifndef DCHAIN_FILLLISTWITHSELECTION_H
#define DCHAIN_FILLLISTWITHSELECTION_H
// -*- C++ -*-
//
// Package:     <DChain>
// Module:      FillListWithSelection
// 
/**\class FillListWithSelection FillListWithSelection.h DChain/FillListWithSelection.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      Chris D Jones
// Created:     Sat Dec 24 13:10:55 EST 2005
// $Id: FillListWithSelection.h,v 1.1.1.1.2.1 2014/04/18 00:56:43 maqm Exp $
//
// Revision history
//
// $Log: FillListWithSelection.h,v $
// Revision 1.1.1.1.2.1  2014/04/18 00:56:43  maqm
// remove mutable for TList ref.
//
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:27  cdj
// massive class renaming, addition of [] for selection and unit tests
//

// system include files

// user include files

// forward declarations

namespace dchain {
   template< class CandidateClass>
   class CombinatoricList;

   template<class TIList, class TSelector,class TOList>
   void fill_using_selector(const TIList& iList, 
			    const TSelector& iSelector, 
			    TOList& oList) {
      oList.fill(iList, iSelector);
   }
   
template<class TList, class TSelector>
class FillListWithSelection
{
   // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------

      // ---------- Constructors and destructor ----------------
      FillListWithSelection(TList& ioList, const TSelector& iSel):
	 m_list(ioList), m_selector(iSel) {}
      //virtual ~FillListWithSelection();

      // ---------- member functions ---------------------------
      const TList& operator=(const TList& iList) const {
	 m_list.erase();
	 fill_using_selector(iList, m_selector, m_list);
	 return m_list;
      }

      template <class TAList>
	 const TList& operator=(const TAList& iList) const {
	    m_list.erase();
	    fill_using_selector(iList, m_selector, m_list);
	    return m_list;
	 }

      template< class TCandidate>
	 const TList& operator=( const CombinatoricList< TCandidate >& aOtherList ) {
	    m_list.erase();
	    fill_using_selector(aOtherList, m_selector, m_list);
	    return m_list;
	 }

      // ---------- const member functions ---------------------

      // ---------- static member functions --------------------

   protected:
      // ---------- protected member functions -----------------

      // ---------- protected const member functions -----------

   private:
      // ---------- Constructors and destructor ----------------
      //FillListWithSelection( const FillListWithSelection& ); // stop default

      // ---------- assignment operator(s) ---------------------
      //const FillListWithSelection& operator=( const FillListWithSelection& ); // stop default

      // ---------- private member functions -------------------

      // ---------- private const member functions -------------

      // ---------- data members -------------------------------
      TList& m_list;
      const TSelector& m_selector;

      // ---------- static data members ------------------------

};

// inline function definitions

// Uncomment the following lines, if your class is templated 
// and has an implementation file (in the Template directory)
//#if defined(INCLUDE_TEMPLATE_DEFINITIONS)
//# include "DChain/Template/FillListWithSelection.cc"
//#endif
}
#endif /* DCHAIN_FILLLISTWITHSELECTION_H */
