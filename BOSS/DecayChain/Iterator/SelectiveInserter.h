#ifndef DCHAIN_SELECTIVEINSERTER_H
#define DCHAIN_SELECTIVEINSERTER_H
// -*- C++ -*-
//
// Package:     <DChain>
// Module:      SelectiveInserter
// 
/**\class SelectiveInserter SelectiveInserter.h DChain/SelectiveInserter.h

 Description: Inserts objects into a list only if those object pass the selection

 Usage:
    <usage>

*/
//
// Author:      Chris D Jones
// Created:     Thu Jan  5 15:40:59 EST 2006
// $Id: SelectiveInserter.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: SelectiveInserter.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:18  cdj
// massive class renaming, addition of [] for selection and unit tests
//

// system include files
#include <iterator>

// user include files

// forward declarations
namespace dchain {
   template<class TList, class TSelect>
      struct SelectiveInserter {
	 typedef std::output_iterator_tag iterator_category;
	 typedef void value_type;
	 typedef void difference_type;

	 SelectiveInserter(TList& iList,
	       const TSelect& iSelect):
	    m_list(&iList),
	    m_select(iSelect) {}
	 SelectiveInserter<TList,TSelect>& operator*() {
	    return *this; }

	 SelectiveInserter<TList,TSelect>& operator++() {
	    return *this;
	 }

	 SelectiveInserter<TList,TSelect>& operator+(int) {
	    return *this;
	 }

	 ///returns false if object not added to list
	 template<class THolder>
	    bool operator=(THolder iHolder){
	       return m_list->attempt_insert(iHolder, m_select);
	    }
	 private:
	 TList* m_list;
	 TSelect m_select;
      };
}
#endif /* DCHAIN_SELECTIVEINSERTER_H */
