//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/RODFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Definition of the ROD header entity, following the description of
 * the Event Format note, version 2.4
 */

#ifndef EFORMAT_OLD_RODFRAGMENT_H
#define EFORMAT_OLD_RODFRAGMENT_H

#include <stdint.h>
#include <cstdlib>

namespace eformat {

  namespace old {

    /**
     * Implements the access methods and checking for the ROD header object,
     * present in the event format stream. Manipulating ROD's is a bit
     * different than the other fragments, as one can't know its ending
     * position. There are two ways to solve that: the first being to give a
     * pointer and a size, but the second, a bit more elegant, to indicate the
     * end of the ROD instead of its begin.
     */
    class RODFragment {

    public: //interface

      /**
       * To build a header given the containing buffer. I need to know where
       * the header starts in order to do that.
       *
       * @param it The position pointing the first word of this fragment
       * @param size_word The size of this fragment, in words
       */
      RODFragment (const uint32_t* it, size_t size_word);

      /**
       * Destructor virtualisation
       */
      virtual ~RODFragment() {}

      /**
       * Says if the the header is valid. This may throw exceptions.
       */
      virtual bool check () const;

      /**
       * Returns the fragment type.
       */
      inline uint32_t marker() const { return m_start[0]; }

      /**
       * Returns the total fragment size
       */
      uint32_t fragment_size_word() const;

      /**
       * Returns the size, in words, of the current header. That does @b
       * not include the trailer.
       */
      inline uint32_t header_size_word() const { return m_start[1]; }

      /**
       * Returns the size, in words, of the trailer
       */
      inline uint32_t trailer_size_word() const { return 3; }

      /**
       * Returns the formatting version.
       */
      inline uint32_t version() const { return m_start[2]; }

      /**
       * Returns the full source identifier.
       */
      inline uint32_t source_id() const { return m_start[3]; }

      /**
       * Returns the current run number.
       */
      inline uint32_t run_no() const { return m_start[4]; }

      /**
       * Returns the lvl1 identifier
       */
      inline uint32_t lvl1_id() const { return m_start[5]; }

      /**
       * Returns the bunch crossing identifier
       */
      inline uint32_t bc_id() const { return m_start[6]; }

      /**
       * Returns the lvl1 trigger type
       */
      inline uint32_t lvl1_trigger_type() const { return m_start[7]; }

      /**
       * Returns the detector event type
       */
      inline uint32_t detev_type() const { return m_start[8]; }

      /**
       * Returns the number of status words available
       */
      inline uint32_t nstatus () const { return m_start[m_size-3]; }
    
      /**
       * Returns the status words, as an iterator to the status words
       * available.
       */
      const uint32_t* status (void) const;

      /**
       * Returns the number of data words available
       */
      inline uint32_t ndata () const { return m_start[m_size-2]; }

      /**
       * Returns a pointer to the first data word
       */
      const uint32_t* data (void) const;

      /**
       * Returns the status block position. A value of <tt>zero</tt> indicates
       * that the status block preceeds the data block. A value of <tt>one</tt>
       * means the contrary.
       */
      inline uint32_t status_position () const { return m_start[m_size-1]; }

    private: //representation

      const uint32_t* m_start; ///< my end word
      size_t m_size; ///< my total size, in number of words

    };

  }

}

#endif /* EFORMAT_OLD_RODFRAGMENT_H */
