//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/ROBFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/02/20 23:38:21 $
 *
 * @brief Defines the ROB fragment entity as described in the Event Format
 * note.
 */

#ifndef EFORMAT_ROBFRAGMENT_H
#define EFORMAT_ROBFRAGMENT_H

#include "eformat/Header.h"
#include "eformat/HeaderMarker.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/WrongMarkerIssue.h"
#include "eformat/BadVersionIssue.h"

namespace eformat {

  /**
   * Describes how to access the contents of a subdetector fragment, as
   * prescribed by the event format note.
   */
  template <class TPointer>
  class ROBFragment : public eformat::Header<TPointer> {

  public: //interface

   /**
     * To build a fragment given the containing buffer. I need to know
     * where the fragment starts in order to do that.
     *
     * @param it The exact position where this fragment should start.
     */
    ROBFragment (const TPointer& it);

    /**
     * Copy constructor
     *
     * @param other The fragment to be copied
     */
    ROBFragment (const ROBFragment& other) 
      : Header<TPointer>(other), m_start(other.m_start) {}

    /**
     * Builds an empty, otherwise useless ROBFragment
     */
    ROBFragment () : Header<TPointer>(), m_start() {}

    /**
     * Destructor virtualisation
     */
    virtual ~ROBFragment() {}

    /**
     * Assignment
     *
     * @param other The fragment to be copied
     */
    ROBFragment& operator= (const ROBFragment& other)
    { Header<TPointer>::operator=(other); m_start=other.m_start; return *this; }

    /**
     * Manual re-assignment
     *
     * @param it The position pointing the first word of this fragment
     */
    ROBFragment& assign (const TPointer& it);

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    virtual bool check () const;

    /**
     * Says if the the fragment is valid. This may throw exceptions.
     */
    inline bool check_tree () const { check(); return true; }

    /**
     * Returns the fragment type.
     */
    inline uint32_t rod_marker() const { return m_start[0]; }

    /**
     * Returns the total fragment size
     */
    inline uint32_t rod_fragment_size_word() const 
    { return this->payload_size_word(); }

    /**
     * Returns the size, in words, of the current header. That does @b
     * not include the trailer.
     */
    inline uint32_t rod_header_size_word() const { return m_start[1]; }

    /**
     * Returns the size, in words, of the trailer
     */
    inline uint32_t rod_trailer_size_word() const { return 3; }

    /**
     * Returns the formatting version.
     */
    inline uint32_t rod_version() const { return m_start[2]; }

    /**
     * Returns the full source identifier.
     */
    inline uint32_t rod_source_id() const { return m_start[3]; }

    /**
     * Returns the current run number.
     */
    inline uint32_t rod_run_no() const { return m_start[4]; }

    /**
     * Returns the lvl1 identifier
     */
    inline uint32_t rod_lvl1_id() const { return m_start[5]; }

    /**
     * Returns the bunch crossing identifier
     */
    inline uint32_t rod_bc_id() const { return m_start[6]; }

    /**
     * Returns the lvl1 trigger type
     */
    inline uint32_t rod_lvl1_trigger_type() const { return m_start[7]; }

    /**
     * Returns the detector event type
     */
    inline uint32_t rod_detev_type() const { return m_start[8]; }

    /**
     * Returns the number of status words available
     */
    inline uint32_t rod_nstatus () const
    { return m_start[this->payload_size_word()-3]; }
    
    /**
     * Returns the status words, as an iterator to the status words
     * available.
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    void rod_status (TPointer& it) const;

    /**
     * Returns the number of data words available
     */
    inline uint32_t rod_ndata () const 
    { return m_start[this->payload_size_word()-2]; }

    /**
     * Returns a pointer to the first data word
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    void rod_data (TPointer& it) const;

    /**
     * Returns the status block position. A value of <tt>zero</tt> indicates
     * that the status block preceeds the data block. A value of <tt>one</tt>
     * means the contrary.
     */
    inline uint32_t rod_status_position () const 
    { return m_start[this->payload_size_word()-1]; }

  private: //static stuff

    static const uint32_t NSPECIFIC;

  private: //representation

    TPointer m_start; ///< my last word

  };

}

template <class TPointer>
const uint32_t eformat::ROBFragment<TPointer>::NSPECIFIC = 0;

template <class TPointer>
eformat::ROBFragment<TPointer>::ROBFragment (const TPointer& it)
  : eformat::Header<TPointer>(it, eformat::ROB),
    m_start()
{
  ERS_DEBUG_3("Building ROBFragment from pointer");
  specific_header(m_start);
  ERS_DEBUG_1("Initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  ERS_DEBUG_3("Building underlying RODFragment");
  if (rod_marker() != eformat::ROD) 
    throw EFORMAT_WRONG_MARKER(rod_marker(), eformat::ROD);
  ERS_DEBUG_1("Initialized ROD header with source identifier = %s",
	      eformat::helper::SourceIdentifier(rod_source_id()).human().c_str());
}

template <class TPointer> eformat::ROBFragment<TPointer>&
eformat::ROBFragment<TPointer>::assign (const TPointer& it)
{
  ERS_DEBUG_3("Re-building ROBFragment from pointer");
  eformat::Header<TPointer>::assign(it, eformat::ROB);
  specific_header(m_start);
  ERS_DEBUG_1("Initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  ERS_DEBUG_3("Re-building underlying RODFragment");
  if (rod_marker() != eformat::ROD) 
    throw EFORMAT_WRONG_MARKER(rod_marker(), eformat::ROD);
  ERS_DEBUG_1("Reinitialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(rod_source_id()).human().c_str());
  return *this;
}

template <class TPointer>
bool eformat::ROBFragment<TPointer>::check () const
{
  //ROB checking
  ERS_DEBUG_2("Checking for consistency of ROBFragment [%s]",
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  eformat::Header<TPointer>::check(); //< first do a generic check
  if (eformat::Header<TPointer>::nspecific() != NSPECIFIC)
    throw EFORMAT_SIZE_CHECK(NSPECIFIC, this->nspecific());

  //ROD checking
  ERS_DEBUG_2("Checking for consistency of RODFragment [%s]",
	      eformat::helper::SourceIdentifier(rod_source_id()).human().c_str());
  if ( rod_version() >> 16 != eformat::MAJOR_DEFAULT_VERSION )
    throw EFORMAT_BAD_VERSION(rod_version() >> 16,
			      eformat::MAJOR_DEFAULT_VERSION);
  if ( rod_header_size_word() != 9 )
    throw EFORMAT_SIZE_CHECK(9, rod_header_size_word());
  if ( rod_fragment_size_word() != 12 + rod_nstatus() + rod_ndata() )
    throw EFORMAT_SIZE_CHECK(rod_fragment_size_word(), 
			     (12 + rod_nstatus() + rod_ndata()));
  return true;
}

template <class TPointer>
void eformat::ROBFragment<TPointer>::rod_status (TPointer& it) const
{
  it = m_start;
  it += 9;
  if (rod_status_position()) it += rod_ndata();
}

template <class TPointer> 
void eformat::ROBFragment<TPointer>::rod_data (TPointer& it) const
{
  it = m_start;
  it += 9;
  if (!rod_status_position()) it += rod_nstatus();
}

#endif /* EFORMAT_ROBFRAGMENT_H */
