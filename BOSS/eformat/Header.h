//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/Header.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André Rabello dos
 * ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $ 
 *
 * @brief Defines the Header entity. The definition is based on the
 * update of ATL-DAQ-98-129, by D.Francis et al.
 */

#ifndef EFORMAT_HEADER_H
#define EFORMAT_HEADER_H

#include "eformat/Version.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/util.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/BadVersionIssue.h"
#include "eformat/WrongMarkerIssue.h"
#include "eformat/WrongSizeIssue.h"
#include "eformat/NoSuchChildIssue.h"
#include "ers/ers.h"

namespace eformat {

  /**
   * Contains the information on the Header of a fragment as described
   * by the original note. The header is a composite entity, build from
   * two parts:
   *
   * -# The generic part, containing the header type, size and version
   * information;
   * -# The specific part, containing data that is specific for a
   * particular fragment.
   */
  template <class TPointer> class Header {

  public:
   
   /**
     * To build a header given the containing buffer. I need to know
     * where the header starts in order to do that.
     *
     * @param it The exact position where this header should start.
     * @param match The word that this header should match.
     */
    Header (const TPointer& it, uint32_t match);

    /**
     * Builds an empty, otherwise useless Header
     */
    Header () : m_start() {}

    /**
     * Copy constructor
     *
     * @param other The other header to construct from
     */
    Header (const Header& other) : m_start(other.m_start) {}

    /**
     * Destructor virtualisation
     */
    virtual ~Header() {}

    /**
     * Assigment operator
     *
     * @param other The other header to assign from
     */
    Header& operator= (const Header& other)
    { m_start = other.m_start; return *this; }

    /**
     * Reassign this header
     *
     * @param it The exact position where this header should start.
     * @param match The word that this header should match.
     */
    Header& assign (const TPointer& it, uint32_t match);

    /**
     * Says if the generic part of the header is valid. This may throw
     * exceptions.
     */
    virtual bool check () const;

    /**
     * Returns the fragment type.
     */
    inline uint32_t marker() const { return m_start[0]; }

    /**
     * Returns the size, in words, of the current fragment.
     */
    inline uint32_t fragment_size_word() const { return m_start[1]; }

    /**
     * Returns the size, in words, of the current header. That does include
     * the specific part of the header.
     */
    inline uint32_t header_size_word() const { return m_start[2]; }

    /**
     * Returns the formatting version.
     */
    inline uint32_t version() const { return m_start[3]; }

    /**
     * Returns the full source identifier.
     */
    inline uint32_t source_id() const { return m_start[4]; }

    /**
     * Returns the number of status words available
     */
    inline uint32_t nstatus () const { return m_start[5]; }

    /**
     * Sets the pointer to my start
     *
     * @param it The pointer to set
     */
    inline void start (TPointer& it) const { it = m_start; }

    /**
     * Sets the pointer to where the payload starts
     *
     * @param it The pointer to set
     */
    inline void payload (TPointer& it) const
    { it = m_start; it+= header_size_word(); }

    /**
     * Sets the pointer to one-past my end position
     *
     * @param it The pointer to set
     */
    inline void end (TPointer& it) const 
    { it = m_start; it += fragment_size_word(); }

    /**
     * Returns the payload size
     */
    inline uint32_t payload_size_word (void) const
    { return fragment_size_word() - header_size_word(); }

    /**
     * Returns the status words, as an iterator to the status words available.
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    inline void status (TPointer& it) const { it = m_start; it += 6; }


    /**
     * Returns the number of specific words available in the specific header
     * part 
     */
    inline uint32_t nspecific () const { return m_start[6 + nstatus()]; }

    /**
     * Returns an iterator to the start of the specific header part (this
     * does not include the number of specific header fragments)
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    inline void specific_header (TPointer& it) const
    { it = m_start; it += 7 + nstatus(); }

    /**
     * Returns the number of children available.
     */
    virtual uint32_t nchildren () const;

    /**
     * Returns the nth child fragment. If the nth fragment doesn't exist, the
     * behaviour is undefined.
     *
     * @param p A preallocated pointer you should provide.
     * @param n The fragment position, starting at zero, of the child fragment
     * you would like to get.
     */
    virtual void child (TPointer& p, size_t n) const;

    /**
     * Returns the nth child fragment. If the nth fragment doesn't exist, an
     * exception is thrown. 
     *
     * @warning Use this method with care, it is slower than the equivalent
     * method that doesn't check.
     *
     * @param p A preallocated pointer you should provide.
     * @param n The fragment position, starting at zero, of the child fragment
     * you would like to get.
     */
    virtual void child_check (TPointer& p, size_t n) const;

    /**
     * Returns all the children of this fragment. The input to this method is a
     * valid set of iterators to existing, pre-allocated pointers
     *
     * @param p A pointer to a set of preallocated TPointer's to set to the
     * position of the children of this fragment.
     * @param max The maximum number of children, p can point to.
     *
     * @return The number of children found on this fragment
     */
    virtual uint32_t children (TPointer* p, size_t max) const;

  private: ///< representation

    TPointer m_start; ///< my start word

  };

}

template <class TPointer>
eformat::Header<TPointer>::Header (const TPointer& it, uint32_t match)
  : m_start(it)
{
  ERS_DEBUG_3("Building header 0x%x from iterator", match);
  if (marker() != match) {
    throw EFORMAT_WRONG_MARKER(marker(), match);
  }
  ERS_DEBUG_1("Initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(match).human().c_str());
}

template <class TPointer> eformat::Header<TPointer>& 
eformat::Header<TPointer>::assign (const TPointer& it, uint32_t match)
{
  ERS_DEBUG_3("Rebuilding header 0x%x from iterator", match);
  m_start = it;
  if (marker() != match) {
    throw EFORMAT_WRONG_MARKER(marker(), match);
  }
  ERS_DEBUG_1("Re-initialized header with source identifier = %s",
	      eformat::helper::SourceIdentifier(match).human().c_str());
  return *this;
}

template <class TPointer>
bool eformat::Header<TPointer>::check () const
{
  ERS_DEBUG_2("Checking for consistency of fragment of type %s [%s]",
	      eformat::marker2string(marker()).c_str(),
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  if ( version() >> 16 != eformat::MAJOR_DEFAULT_VERSION )
    throw EFORMAT_BAD_VERSION(version() >> 16, eformat::MAJOR_DEFAULT_VERSION);
  if ( header_size_word() != (7 + nstatus() + nspecific()) )
    throw EFORMAT_SIZE_CHECK(header_size_word(),
			     (7 + nstatus() + nspecific()));
  return true;
}

template <class TPointer>
uint32_t eformat::Header<TPointer>::nchildren () const
{
  ERS_DEBUG_2("User asked for number of children of fragment type %s [%s]",
	      eformat::marker2string(marker()).c_str(),
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  return children(0,0);
}

template <class TPointer> 
void eformat::Header<TPointer>::child (TPointer& p, size_t n) const
{
  ERS_DEBUG_2("User asked for child %ud of fragment type %s [%s]", n,
	      eformat::marker2string(marker()).c_str(),
	      eformat::helper::SourceIdentifier(source_id()).human().c_str());
  TPointer next;
  payload(next);
  for (size_t i=0; i<n; ++i) next += next[1];
  ERS_DEBUG_3("Set user object");
  p = next;
}

template <class TPointer>
void eformat::Header<TPointer>::child_check (TPointer& p, size_t n) const
{
  uint32_t total = nchildren();
  if (n >= total) throw EFORMAT_NO_SUCH_CHILD(n, total);
  child(p, n);
}

template <class TPointer> 
uint32_t eformat::Header<TPointer>::children (TPointer* p, size_t max) const
{
  ERS_DEBUG_2("Retrieving all children...");
  TPointer payload_start;
  payload(payload_start);
  try {
    return find_fragments(child_marker(static_cast<HeaderMarker>(marker())),
			  payload_start, payload_size_word(), p, max);
  }
  catch (WrongMarkerIssue& ex) {
    //This normally means the fragment size is wrong...
    throw EFORMAT_WRONG_SIZE(fragment_size_word());
  }
  return 0;
}

#endif //EFORMAT_HEADER_H
