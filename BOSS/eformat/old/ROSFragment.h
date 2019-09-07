//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/ROSFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Describes the ROS fragment as defined in the Event Format note,
 * version 2.4
 */

#ifndef EFORMAT_OLD_ROSFRAGMENT_H
#define EFORMAT_OLD_ROSFRAGMENT_H

#include "eformat/old/Header.h"

namespace eformat {

  namespace old {

    /**
     * Describes how to access the contents of a subdetector fragment, as
     * prescribed by the event format note.
     */
    class ROSFragment : public eformat::old::Header {

    public: //interface

      /**
       * To build a fragment given the containing buffer. I need to know
       * where the fragment starts in order to do that.
       *
       * @param it The exact position where this fragment should start.
       */
      ROSFragment (const uint32_t* it);

      /**
       * Destructor virtualisation
       */
      virtual ~ROSFragment() {}

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

    private: //representation

      const uint32_t* m_start; ///< my start word

    };

  }

}

#endif /* EFORMAT_OLD_ROSFRAGMENT_H */
