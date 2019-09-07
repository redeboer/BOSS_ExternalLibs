//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/FullEventFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines the Event Fragment entity. The definition is based on the
 * update of ATL-DAQ-98-129, version 2.4, by D.Francis et al.
 */

#ifndef EFORMAT_OLD_FULLEVENTFRAGMENT_H
#define EFORMAT_OLD_FULLEVENTFRAGMENT_H

#include "eformat/old/Header.h"

namespace eformat {

  namespace old {

    /**
     * Describes how to access the contents of an event fragment, as prescribed
     * by the event format note.
     */
    class FullEventFragment : public eformat::old::Header {

    public: //interface

      /**
       * To build a fragment given the containing buffer. I need to know
       * where the fragment starts in order to do that.
       *
       * @param it The exact position where this fragment should start.
       */
      FullEventFragment (const uint32_t* it);

      /**
       * Destructor virtualisation
       */
      virtual ~FullEventFragment() {}

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
      inline uint32_t date() const { return m_start[0]; }

      /**
       * Returns the global identifier
       */
      inline uint32_t global_id() const { return m_start[1]; }

      /**
       * Returns the lvl1 identifier
       */
      inline uint32_t lvl1_id() const { return m_start[2]; }

      /**
       * Retursn the lvl1 trigger type
       */
      inline uint32_t lvl1_trigger_type() const { return m_start[3]; }

      /**
       * Returns the lvl2 trigger info
       */
      inline uint32_t lvl2_trigger_info() const { return m_start[4]; }

      /**
       * Returns the number of Event Filter words (constant = 5)
       */
      inline uint32_t nevent_filter_info () const { return 5; }

      /**
       * Returns an iterator to the first of the (5) EF words
       */
      inline const uint32_t* event_filter_info(void) const 
      { return &m_start[5]; }

    private: //representation

      const uint32_t* m_start; ///< my start word

    };

  }

}

#endif /* EFORMAT_OLD_FULLEVENTFRAGMENT_H */
