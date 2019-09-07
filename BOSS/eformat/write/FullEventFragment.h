//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/write/FullEventFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Helps the user to define and build a FullEvent fragment.
 */

#ifndef EFORMAT_WRITE_FULLEVENTFRAGMENT_H
#define EFORMAT_WRITE_FULLEVENTFRAGMENT_H

#include "eformat/write/SubDetectorFragment.h"
#include <cstring>

namespace eformat {

  namespace write {

    /**
     * Defines a helper class to aid the creation of FullEvent fragments.
     */
    class FullEventFragment {

    public:

      /**
       * Builds a new FullEvent fragment from scratch, using the current time
       * as the time label.
       *
       * @param source_id The source identifier to be using for this
       * FullEvent 
       * @param global_id The global identifier for this fragment
       * @param run_no The run number for this fragment
       * @param lvl1_id The LVL1 trigger identifier for this fragment
       * @param lvl1_type The LVL1 trigger type for this fragment
       * @param lvl2_info The LVL2 trigger info for this fragment
       * @parma ef_info The Event Filter info for this fragment: a pointer to 4
       * pre-allocated words.
       */
      FullEventFragment (uint32_t source_id,
			 uint32_t global_id, uint32_t run_no,
			 uint32_t lvl1_id, uint32_t lvl1_type,
			 uint32_t lvl2_info, const uint32_t* ef_info);

      /**
       * Builds a new FullEvent fragment from scratch
       *
       * @param source_id The source identifier to be using for this
       * FullEvent 
       * @param use_time The creation time in seconds since 1/1/1970 00:00 UTC
       * for this fragment
       * @param global_id The global identifier for this fragment
       * @param run_no The run number for this fragment
       * @param lvl1_id The LVL1 trigger identifier for this fragment
       * @param lvl1_type The LVL1 trigger type for this fragment
       * @param lvl2_info The LVL2 trigger info for this fragment
       * @parma ef_info The Event Filter info for this fragment: a pointer to 4
       * pre-allocated words.
       */
      FullEventFragment (uint32_t source_id, uint32_t use_time,
			 uint32_t global_id, uint32_t run_no,
			 uint32_t lvl1_id, uint32_t lvl1_type,
			 uint32_t lvl2_info, const uint32_t* ef_info);

      /**
       * Builds a new FullEvent fragment from an existing 
       * FullEvent fragment in contiguous memory.
       *
       * @param fe The existing FullEvent fragment
       */
      FullEventFragment (uint32_t* fe);

      /**
       * Builds a new FullEvent fragment from an existing FullEvent
       * fragment in non-contiguous memory. The top-level fragment header is
       * expected to be on a contiguous area of memory, together with the first
       * word of the first child fragment (i.e. the ROS header marker). The
       * following data can be spread around.
       * 
       * @param fe The existing FullEvent fragment, as a list of nodes,
       * pre-concatenated by the caller.
       */
      FullEventFragment (eformat::write::node_t* fe);

      /**
       * Builds a new empty FullEventFragment, otherwise invalid. This is
       * useful for array builds and standard containers.
       */
      FullEventFragment ();

      /**
       * Copy constructor. This will only copy the meta data, not the fragment
       * relationships and block-data (children and status block) contained in
       * the to-be-copied fragment. If you wish this fragment has the same
       * children of the copied fragment, you have to do this operation
       * manually, after copying. If you wish to make a copy of the status as
       * well, do it manually and then assign it to this fragment using the
       * status() method.
       *
       * @param other The other fragment to take the meta data from.
       */
      FullEventFragment (const FullEventFragment& other);

      /**
       * Base destructor
       */
      virtual ~FullEventFragment () {}

      /**
       * Assigment operator. This will only copy the meta data, not the
       * fragment relationships and block-data (children and parent and status
       * block) contained in the to-be-copied fragment. If you wish this
       * fragment has the same parents, and children of the copied fragment,
       * you have to do this operation manually, after copying. If you wish to
       * make a copy of the status as well, do it manually and then assign it
       * to this fragment using the status() method.
       *
       * @param other The other fragment to take the meta data from.
       */
      FullEventFragment& operator= (const FullEventFragment& other);

      /**
       * Changes the number of status words and the status words themselves
       * from the fragment
       *
       * @param n How many status words the underlying FullEventFragment
       * fragment supposed to have.
       * @param status A pointer to <tt>n</tt> pre-allocated words
       */
      void status (uint32_t n, const uint32_t* status);

      /**
       * Returns the number of status wors in this fragment
       */
      inline uint32_t nstatus (void) const { return m_node[0].base[5]; }

      /**
       * Returns a pointer to the first status word to be used by this fragment
       */
      inline const uint32_t* status (void) const { return m_node[1].base; }

      /**
       * Changes the minor version number of the fragment
       *
       * @param v The new minor version for this header
       */
      inline void minor_version (uint16_t v) 
      { m_node[0].base[3] = eformat::DEFAULT_VERSION | v; }

      /**
       * Returns the minor version number of the fragment
       */
      inline uint16_t minor_version (void) const 
      { return 0xffff & m_node[0].base[3]; }

      /**
       * Changes the source identifier for this fragment
       *
       * @param s The new value to set
       */
      inline void source_id (uint32_t s) 
      { m_node[0].base[4] = s; }

      /**
       * Returns the source identifier of this fragment
       */
      inline uint32_t source_id (void) const 
      { return m_node[0].base[4]; }

      /**
       * Changes the time this fragment was produced at
       *
       * @param s The new value to set
       */
      inline void time (uint32_t s) 
      { m_node[2].base[1] = s; }

      /**
       * Returns the time for this fragment
       */
      inline uint32_t time (void) const 
      { return m_node[2].base[1]; }

      /**
       * Changes the global identifier for this event
       *
       * @param s The new value to set
       */
      inline void global_id (uint32_t s) 
      { m_node[2].base[2] = s; }

      /**
       * Returns the global identifier for this event
       */
      inline uint32_t global_id (void) const
      { return m_node[2].base[2]; }

      /**
       * Changes the run number
       *
       * @param s The new value to set
       */
      inline void run_no (uint32_t s) 
      { m_node[2].base[3] = s; }

      /**
       * Returns the run number for this fragment
       */
      inline uint32_t run_no (void) const 
      { return m_node[2].base[3]; }

      /**
       * Changes the lvl1 identifier in this fragment
       *
       * @param s The new value to set
       */
      inline void lvl1_id (uint32_t s) 
      { m_node[2].base[4] = s; }

      /**
       * Returns the lvl1 identifier for this fragment
       */
      inline uint32_t lvl1_id (void) const 
      { return m_node[2].base[4]; }

      /**
       * Changes the lvl1 trigger type in this fragment
       *
       * @param s The new value to set
       */
      inline void lvl1_trigger_type (uint32_t s) 
      { m_node[2].base[5] = s; }

      /**
       * Returns the lvl1 trigger type for this fragment
       */
      inline uint32_t lvl1_trigger_type (void) const 
      { return m_node[2].base[5]; }

      /**
       * Changes the lvl2 trigger info in this fragment
       *
       * @param s The new value to set
       */
      inline void lvl2_trigger_info (uint32_t s) 
      { m_node[2].base[6] = s; }

      /**
       * Returns the lvl2 trigger info for this fragment
       */
      inline uint32_t lvl2_trigger_info (void) const
      { return m_node[2].base[6]; }

      /**
       * Changes the event filter trigger info in this fragment
       *
       * @param s The new value to set: a pointer with 4 word pre-allocated
       */
      inline void event_filter_info (const uint32_t* s)
      { memcpy(&m_node[2].base[7], s, 4*sizeof(uint32_t)); }

      /**
       * Returns the event filter trigger info for this fragment
       */
      inline const uint32_t* event_filter_info (void) const
      { return &m_node[2].base[7]; }

      /**
       * Returns the total size for the meta data (everything that does @b not
       * encompass the contents of the m_data pointer in the private
       * representation of this class) in the fragment, in words
       */
      inline uint32_t meta_size_word (void) const
      { return m_node[0].base[2]; }

      /**
       * Returns the total size for this fragment, in words
       */
      inline uint32_t size_word (void) const
      { return m_node[0].base[1]; }
      
      /**
       * Appends a new SubDetector fragment to this FullEvent fragment.
       *
       * @warning This will change the page structure of the last ROS fragment
       * inserted here, in order to concatenate the ROS fragments
       * together. Please note that this operation is not compatible with
       * multiple threads of operation, if you would like to share
       * eformat::write::Fragment's between threads. A better strategy would
       * be create, for every thread of operation, a proper ROSFragment
       * instead.
       *
       * @param sd The SubDetector fragment to be appended to myself
       */
      void append (eformat::write::SubDetectorFragment* sd);

      /**
       * This returns the first child of this fragment. The system operates as
       * a concatenated list of fragments. From this child you can get to the
       * next.
       */
      inline const SubDetectorFragment* first_child (void) const 
      { return m_child; }

      /**
       * This method is used by children of this fragment to notify fragment
       * size changes.
       *
       * @param o The old size, in 32-bit words
       * @param n The new size, in 32-bit words
       */
      inline void size_change (uint32_t o, uint32_t n)
      { m_node[0].base[1] -= o; m_node[0].base[1] += n; }

      /**
       * Returns the total number of (raw memory) pages this fragment is
       * composed of.
       *
       * @warning This operation navigates at a potentially large list of child
       * page nodes (for a full ATLAS event this should be bigger than 2,000
       * pages when built from scratch). If you don't do your bookkeeping,
       * avoid calling this too often.
       */
      uint32_t page_count (void) const;

      /**
       * Returns the first node of a list of nodes that represent the fragment
       * you have constructed. To make use of it, just browse the list as
       * defined in node.h
       */
      const eformat::write::node_t* bind (void);

      const eformat::write::node_t* rebind (void){return m_node;};
    private: //representation

      uint32_t m_header[17]; ///< The FullEvent Header
      eformat::write::node_t m_node[3]; ///< Node representation
      eformat::write::SubDetectorFragment* m_child; ///< my children
      eformat::write::SubDetectorFragment* m_last; ///< my last child
      eformat::write::node_t m_extra; ///< Extra pages I may have
      uint32_t m_extra_count; ///< How many extra pages I have

    };

  }
  
}

#endif /* EFORMAT_WRITE_FULLEVENTFRAGMENT_H */
