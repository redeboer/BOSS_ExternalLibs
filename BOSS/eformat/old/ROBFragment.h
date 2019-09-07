//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/ROBFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines the ROB fragment entity as described in the Event Format
 * note.
 */

#ifndef EFORMAT_OLD_ROBFRAGMENT_H
#define EFORMAT_OLD_ROBFRAGMENT_H

#include "eformat/old/Header.h"
#include "eformat/old/RODFragment.h"

namespace eformat {

  namespace old {

    /**
     * Describes how to access the contents of a subdetector fragment, as
     * prescribed by the event format note.
     */
    class ROBFragment : public eformat::old::Header {

    public: //interface

      /**
       * To build a fragment given the containing buffer. I need to know
       * where the fragment starts in order to do that.
       *
       * @param it The exact position where this fragment should start.
       */
      ROBFragment (const uint32_t* it);

      /**
       * Destructor virtualisation
       */
      virtual ~ROBFragment() {}

      /**
       * Says if the the fragment is valid. This may throw exceptions.
       */
      virtual bool check () const;

      /**
       * Says if the the fragment is valid. This may throw exceptions.
       */
      bool check_tree () const;

      /**
       * Returns the lvl1 identifier
       */
      inline uint32_t lvl1_id() const { return m_start[0]; }

      /**
       * Returns the bunch crossing identifier
       */
      inline uint32_t bc_id() const { return m_start[1]; }

      /**
       * Returns the lvl1 trigger type
       */
      inline uint32_t lvl1_trigger_type() const { return m_start[2]; }

      /**
       * Returns the detector event type
       */
      inline uint32_t detev_type() const { return m_start[3]; }

      /**
       * Returns the nth child fragment. If the nth fragment doesn't exist, an
       * exception is thrown.
       *
       * @param n The fragment position, starting at zero, of the child
       * fragment you would like to get.
       */
      eformat::old::RODFragment rod (size_t n) const;

    private: //representation

      const uint32_t* m_start; ///< my last word

    };

  }

}

#endif /* EFORMAT_OLD_ROBFRAGMENT_H */
