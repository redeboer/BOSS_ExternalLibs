//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/SubDetectorFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/02/20 23:38:21 $
 *
 * @brief Defines the subdetector fragment entity. The definition is based on
 * the update of ATL-DAQ-98-129, by D.Francis et al.
 */

#ifndef EFORMAT_SUBDETECTORFRAGMENT_H
#define EFORMAT_SUBDETECTORFRAGMENT_H

#include "eformat/Header.h"
#include "eformat/HeaderMarker.h"
#include "eformat/ROSFragment.h"
#include "eformat/SizeCheckIssue.h"

namespace eformat {

  /**
   * Describes how to access the contents of a subdetector fragment, as
   * prescribed by the event format note.
   */
  template <class TPointer>
  class SubDetectorFragment : public eformat::Header<TPointer> {

  public: //interface

   /**
     * To build a fragment given the containing buffer. I need to know
     * where the fragment starts in order to do that.
     *
     * @param it The exact position where this fragment should start.
     */
    SubDetectorFragment (const TPointer& it);

    /**
     * Builds an empty, otherwise useless SubDetectorFragment
     */
    SubDetectorFragment () : Header<TPointer>(), m_start() {}

    /**
     * Copy constructor
     *
     * @param other The fragment to be copied
     */
    SubDetectorFragment (const SubDetectorFragment& other) 
      : Header<TPointer>(other), m_start(other.m_start) {}

    /**
     * Destructor virtualisation
     */
    virtual ~SubDetectorFragment() {}

    /**
     * Copy operator
     *
     * @param other The fragment to be copied
     */
    SubDetectorFragment& operator= (const SubDetectorFragment& other) 
    { Header<TPointer>::operator=(other); m_start=other.m_start; return *this; }

    /**
     * Manual re-assignment
     *
     * @param it The position pointing the first word of this fragment
     */
    SubDetectorFragment& assign (const TPointer& it);

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    virtual bool check () const;

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    bool check_tree () const;

  private: //static stuff

    static const uint32_t NSPECIFIC;

  private: //representation

    TPointer m_start; ///< my start word

  };

}

template <class TPointer> 
const uint32_t eformat::SubDetectorFragment<TPointer>::NSPECIFIC = 0;

template <class TPointer>
eformat::SubDetectorFragment<TPointer>::SubDetectorFragment 
(const TPointer& it)
  : eformat::Header<TPointer>(it, eformat::SUB_DETECTOR),
    m_start()
{
  specific_header(m_start);
}

template <class TPointer> eformat::SubDetectorFragment<TPointer>& 
eformat::SubDetectorFragment<TPointer>::assign (const TPointer& it)
{
  ERS_DEBUG_3("Re-building SubDetectorFragment from pointer");
  eformat::Header<TPointer>::assign(it, eformat::SUB_DETECTOR);
  specific_header(m_start);
  ERS_DEBUG_1("Initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  return *this;
}

template <class TPointer>
bool eformat::SubDetectorFragment<TPointer>::check () const
{
  ERS_DEBUG_2("Checking for consistency of SubDetectorFragment");
  eformat::Header<TPointer>::check(); //< first do a generic check
  if (this->nspecific() != NSPECIFIC) {
    throw EFORMAT_SIZE_CHECK(NSPECIFIC, this->nspecific());
  }
  return true;
}

template <class TPointer>
bool eformat::SubDetectorFragment<TPointer>::check_tree () const
{
  ERS_DEBUG_2("Checking recursively for consistency of SubDetectorFragment");
  check(); // check myself
  uint32_t total = this->nchildren();
  for (size_t i=0; i<total; ++i) {
    TPointer fp;
    child(fp, i);
    eformat::ROSFragment<TPointer> f(fp);
    f.check_tree();
  }
  return true;
}

#endif /* EFORMAT_SUBDETECTORFRAGMENT_H */
