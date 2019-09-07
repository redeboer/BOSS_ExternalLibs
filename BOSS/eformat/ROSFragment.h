//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/ROSFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/02/20 23:38:21 $
 *
 * @brief Describes the ROS fragment as defined in the Event Format note.
 */

#ifndef EFORMAT_ROSFRAGMENT_H
#define EFORMAT_ROSFRAGMENT_H

#include "eformat/Header.h"
#include "eformat/HeaderMarker.h"
#include "eformat/ROBFragment.h"
#include "eformat/SizeCheckIssue.h"

namespace eformat {

  /**
   * Describes how to access the contents of a subdetector fragment, as
   * prescribed by the event format note.
   */
  template <class TPointer>
  class ROSFragment : public eformat::Header<TPointer> {

  public: //interface
    
    /**
     * To build a fragment given the containing buffer. I need to know
     * where the fragment starts in order to do that.
     *
     * @param it The exact position where this fragment should start.
     */
    ROSFragment (const TPointer& it);

    /**
     * Builds an empty, otherwise useless ROSFragment
     */
    ROSFragment () : Header<TPointer>(), m_start() {}

    /**
     * Copy constructor
     *
     * @param other The fragment to be copied
     */
    ROSFragment (const ROSFragment& other) 
      : Header<TPointer>(other), m_start(other.m_start) {}

    /**
     * Destructor virtualisation
     */
    virtual ~ROSFragment() {}

    /**
     * Copy operator
     *
     * @param other The fragment to be copied
     */
    ROSFragment& operator= (const ROSFragment& other) 
    { Header<TPointer>::operator=(other); m_start=other.m_start; return *this; }

    /**
     * Manual re-assignment
     *
     * @param it The position pointing the first word of this fragment
     */
    ROSFragment& assign (const TPointer& it);

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    virtual bool check () const;

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    bool check_tree () const;

    /**
     * Returns the run number
     */
    inline uint32_t run_no() const { return m_start[0]; }

    /**
     * Returns the lvl1 identifier
     */
    inline uint32_t lvl1_id() const { return m_start[1]; }

    /**
     * Returns the bunch crossing identifier
     */
    inline uint32_t bc_id() const { return m_start[2]; }

  private: //static stuff

    static const uint32_t NSPECIFIC;

  private: //representation

    TPointer m_start; ///< my start word

  };

}

template <class TPointer> 
const uint32_t eformat::ROSFragment<TPointer>::NSPECIFIC = 3;

template <class TPointer>
eformat::ROSFragment<TPointer>::ROSFragment (const TPointer& it)
  : eformat::Header<TPointer>(it, eformat::ROS),
    m_start()
{
  specific_header(m_start);
}

template <class TPointer> eformat::ROSFragment<TPointer>& 
eformat::ROSFragment<TPointer>::assign (const TPointer& it)
{
  ERS_DEBUG_3("Re-building ROSFragment from pointer");
  eformat::Header<TPointer>::assign(it, eformat::ROS);
  specific_header(m_start);
  ERS_DEBUG_1("Initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  return *this;
}

template <class TPointer>
bool eformat::ROSFragment<TPointer>::check () const
{
  ERS_DEBUG_2("Checking for consistency of ROSFragment");
  eformat::Header<TPointer>::check(); //< first do a generic check
  if (eformat::Header<TPointer>::nspecific() != NSPECIFIC)
    throw EFORMAT_SIZE_CHECK(NSPECIFIC, this->nspecific());
  return true;
}

template <class TPointer>
bool eformat::ROSFragment<TPointer>::check_tree () const
{
  ERS_DEBUG_2("Checking recursively for consistency of ROSFragment");
  check(); // check myself
  uint32_t total = this->nchildren();
  for (size_t i=0; i<total; ++i) {
    TPointer fp;
    child(fp, i);
    eformat::ROBFragment<TPointer> f(fp);
    f.check_tree();
  }
  return true;
}

#endif /* EFORMAT_ROSFRAGMENT_H */
