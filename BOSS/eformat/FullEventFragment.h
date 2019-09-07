//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/FullEventFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/02/20 23:38:21 $
 *
 * @brief Defines the Event Fragment entity. The definition is based on the
 * update of ATL-DAQ-98-129, by D.Francis et al.
 */

#ifndef EFORMAT_FULLEVENTFRAGMENT_H
#define EFORMAT_FULLEVENTFRAGMENT_H

#include "eformat/Header.h"
#include "eformat/HeaderMarker.h"
#include "eformat/SubDetectorFragment.h"
#include "eformat/SizeCheckIssue.h"

namespace eformat {

  /**
   * Describes how to access the contents of an event fragment, as prescribed
   * by the event format note.
   */
  template <class TPointer> 
  class FullEventFragment : public eformat::Header<TPointer> {

  public: //interface

   /**
     * To build a fragment given the containing buffer. I need to know
     * where the fragment starts in order to do that.
     *
     * @param it The exact position where this fragment should start.
     */
    FullEventFragment (const TPointer& it);

    /**
     * Builds an empty, otherwise useless FullEventFragment
     */
    FullEventFragment () : Header<TPointer>(), m_start() {}

    /**
     * Copy constructor
     *
     * @param other The fragment to be copied
     */
    FullEventFragment (const FullEventFragment& other) 
      : Header<TPointer>(other), m_start(other.m_start) {}

    /**
     * Destructor virtualisation
     */
    virtual ~FullEventFragment() {}

    /**
     * Copy operator
     *
     * @param other The fragment to be copied
     */
    FullEventFragment& operator= (const FullEventFragment& other) 
    { Header<TPointer>::operator=(other); m_start=other.m_start; return *this; }

    /**
     * Manual re-assignment
     *
     * @param it The position pointing the first word of this fragment
     */
    FullEventFragment& assign (const TPointer& it);

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    virtual bool check () const;

    /**
     * Says if the Fragment and all children are valid.
     */
    bool check_tree () const;

    /**
     * Returns the date, in seconds elapsed since the 1st. January,
     * 1970. The timezone is fixed to UTC.
     */
    inline uint32_t time() const
    { return m_start[0]; }

    /**
     * Returns the global identifier
     */
    inline uint32_t global_id() const
    { return m_start[1]; }

    /**
     * Returns the run number
     */
    inline uint32_t run_no() const
    { return m_start[2]; }

    /**
     * Returns the lvl1 identifier
     */
    inline uint32_t lvl1_id() const
    { return m_start[3]; }

    /**
     * Retursn the lvl1 trigger type
     */
    inline uint32_t lvl1_trigger_type() const
    { return m_start[4]; }

    /**
     * Returns the lvl2 trigger info
     */
    inline uint32_t lvl2_trigger_info() const
    { return m_start[5]; }

    /**
     * Returns the number of Event Filter words (constant = 4)
     */
    inline uint32_t nevent_filter_info () const { return 4; }

    /**
     * Returns an iterator to the first of the (4) EF words
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    void event_filter_info(TPointer& it) const;

  private: //static stuff

    static const uint32_t NSPECIFIC;

  private: //representation

    TPointer m_start; ///< my start word

  };

}

template <class TPointer> 
const uint32_t eformat::FullEventFragment<TPointer>::NSPECIFIC = 10;

template <class TPointer>
eformat::FullEventFragment<TPointer>::FullEventFragment (const TPointer& it)
  : eformat::Header<TPointer>(it, eformat::FULL_EVENT),
    m_start()
{
  specific_header(m_start);
}

template <class TPointer> eformat::FullEventFragment<TPointer>& 
eformat::FullEventFragment<TPointer>::assign (const TPointer& it)
{
  ERS_DEBUG_3("Re-building FullEventFragment from pointer");
  eformat::Header<TPointer>::assign(it, eformat::FULL_EVENT);
  specific_header(m_start);
  ERS_DEBUG_1("Initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  return *this;
}

template <class TPointer>
void eformat::FullEventFragment<TPointer>::event_filter_info(TPointer& it) 
  const 
{
  it = m_start;
  it += 6;
}

template <class TPointer>
bool eformat::FullEventFragment<TPointer>::check () const
{
  ERS_DEBUG_2("Checking for consistency of FullEventFragment");
  eformat::Header<TPointer>::check(); //< first do a generic check
  if (this->nspecific() != NSPECIFIC) { 
    throw EFORMAT_SIZE_CHECK(NSPECIFIC, this->nspecific());
  }
  return true;
}

template <class TPointer>
bool eformat::FullEventFragment<TPointer>::check_tree () const
{
  ERS_DEBUG_2("Checking recursively for consistency of FullEventFragment");
  check(); // check myself
  uint32_t total = this->nchildren();
  for (size_t i=0; i<total; ++i) {
    TPointer fp;
    child(fp, i);
    eformat::SubDetectorFragment<TPointer> f(fp);
    f.check_tree();
  }
  return true;
}

#endif /* EFORMAT_FULLEVENTFRAGMENT_H */
