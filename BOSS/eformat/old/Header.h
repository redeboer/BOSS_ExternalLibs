//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/Header.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André Rabello dos
 * ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $ 
 *
 * @brief Defines the Header entity. The definition is based on the
 * update of ATL-DAQ-98-129, version 2.4, by D.Francis et al.
 */

#ifndef EFORMAT_OLD_HEADER_H
#define EFORMAT_OLD_HEADER_H

#include <stdint.h>
#include <cstdlib>

namespace eformat {

  /**
   * Includes classes and non-member methods that allows provisional reading
   * from event format v2.4. The main purpose of this functionality is
   * conversion from the old to the new event format.
   */
  namespace old {

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
    class Header {

    public:
   
      /**
       * To build a header given the containing buffer. I need to know
       * where the header starts in order to do that.
       *
       * @param it The exact position where this header should start.
       * @param match The word that this header should match.
       */
      Header (const uint32_t* it, uint32_t match);

      /**
       * Destructor virtualisation
       */
      virtual ~Header() {}

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
       * Returns the current run number.
       */
      inline uint32_t run_no() const { return m_start[5]; }

      /**
       * Returns the number of status words available
       */
      inline uint32_t nstatus () const { return m_start[6]; }

      /**
       * Returns the status words, as an iterator to the status words
       * available.
       */
      inline const uint32_t* status () const { return &m_start[7]; }

      /**
       * Returns the number of offset words available. This will also determine
       * the number of child fragments available to this super-fragment.
       */
      inline uint32_t noffset () const { return m_start[7 + nstatus()]; }

      /**
       * Returns the offset words, as an iterator to the offset words
       * available. 
       */
      inline const uint32_t* offset (void) const 
      { return &m_start[8 + nstatus()]; }

      /**
       * Returns the number of specific words available in the specific header
       * part 
       */
      inline uint32_t nspecific () const
      { return m_start[8 + nstatus() + noffset()]; }

      /**
       * Returns an iterator to the start of the specific header part (this
       * includes the number of specific header fragments)
       */
      inline const uint32_t* specific_header (void) const
      { return &m_start[9 + nstatus() + noffset()]; }

      /**
       * Returns the nth child fragment. If the nth fragment doesn't exist, an
       * exception is thrown.
       *
       * @param n The fragment position, starting at zero, of the child
       * fragment you would like to get.
       */
      const uint32_t* child (size_t n) const;

    private: ///< representation

      const uint32_t* m_start; ///< my start word

    };

  }

}

#endif //EFORMAT_OLD_HEADER_H
