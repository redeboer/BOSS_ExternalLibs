//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/SubDetectorFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines the subdetector fragment entity. The definition is based on
 * the update of ATL-DAQ-98-129, version 2.4, by D.Francis et al.
 */

#ifndef EFORMAT_OLD_SUBDETECTORFRAGMENT_H
#define EFORMAT_OLD_SUBDETECTORFRAGMENT_H

#include "eformat/old/Header.h"

namespace eformat {

  namespace old {

    /**
     * Describes how to access the contents of a subdetector fragment, as
     * prescribed by the event format note.
     */
    class SubDetectorFragment : public eformat::old::Header {

    public: //interface

      /**
       * To build a fragment given the containing buffer. I need to know
       * where the fragment starts in order to do that.
       *
       * @param it The exact position where this fragment should start.
       */
      SubDetectorFragment (const uint32_t* it);

      /**
       * Destructor virtualisation
       */
      virtual ~SubDetectorFragment() {}

      /**
       * Says if the the fragment is valid. This may throw exceptions.
       */
      virtual bool check () const;

      /**
       * Says if the the fragment is valid. This may throw exceptions.
       */
      bool check_tree () const;

      /**
       * Returns the lvl1 trigger type
       */
      inline uint32_t lvl1_trigger_type() const { return m_start[0]; }

    private: //representation

      const uint32_t* m_start; ///< my start word

    };

  }

}

#endif /* EFORMAT_OLD_SUBDETECTORFRAGMENT_H */
