//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/write/ROSFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Helps the user to define and build a ROS fragment.
 */

#ifndef EFORMAT_WRITE_ROSFRAGMENT_H
#define EFORMAT_WRITE_ROSFRAGMENT_H

#include "eformat/write/ROBFragment.h"

namespace eformat {

  namespace write {

    class SubDetectorFragment; ///< forward

    /**
     * Defines a helper class to aid the creation of ROS fragments.
     */
    class ROSFragment {

    public:

      /**
       * Builds a new ROS fragment from scratch
       *
       * @param source_id The source identifier to be using for this ROS
       * @param run_no The run number for this ROS
       * @param lvl1_id The LVL1 identifier for this ROS
       * @param bc_id The bunch crossing identifier for this ROS
       */
      ROSFragment (uint32_t source_id, uint32_t run_no,
		   uint32_t lvl1_id, uint32_t bc_id);

      /**
       * Builds a new ROS fragment from an existing ROS fragment in contiguous
       * memory.
       *
       * @param ros The existing ROS fragment
       */
      ROSFragment (uint32_t* ros);

      /**
       * Builds a new ROS fragment from an existing ROS fragment in
       * non-contiguous memory. The top-level fragment header is expected to be
       * on a contiguous area of memory, together with the first word of the
       * first child fragment (i.e. the ROB header marker). The following data
       * can be spread around.
       * 
       * @param ros The existing ROS fragment, as a list of nodes,
       * pre-concatenated by the caller.
       */
      ROSFragment (eformat::write::node_t* ros);

      /**
       * Builds a new empty ROS fragment, otherwise invalid. This is useful for
       * array builds and standard containers.
       */
      ROSFragment ();

      /**
       * Copy constructor. This will only copy the meta data, not the fragment
       * relationships and block-data (children, parent and status block)
       * contained in the to-be-copied fragment. If you wish this fragment has
       * the same parents, and children of the copied fragment, you have to do
       * this operation manually, after copying. If you wish to make a copy of
       * the status as well, do it manually and then assign it to this
       * fragment using the status() method.
       *
       * @param other The other fragment to take the meta data from.
       */
      ROSFragment (const ROSFragment& other);

      /**
       * Base destructor
       */
      virtual ~ROSFragment () {}

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
      ROSFragment& operator= (const ROSFragment& other);

      /**
       * Changes the number of status words and the status words themselves
       * from the fragment
       *
       * @param n How many status words this fragment is supposed to have.
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
       * Returns the source identifier for this fragment
       */
      inline uint32_t source_id (void) const 
      { return m_node[0].base[4]; }

      /**
       * Changes the run number
       *
       * @param s The new value to set
       */
      inline void run_no (uint32_t s) 
      { m_node[2].base[1] = s; }

      /**
       * Returns the run number for this fragment
       */
      inline uint32_t run_no (void) const 
      { return m_node[2].base[1]; }

      /**
       * Changes the lvl1 identifier in this fragment
       *
       * @param s The new value to set
       */
      inline void lvl1_id (uint32_t s) 
      { m_node[2].base[2] = s; }

      /**
       * Returns the lvl1 identifier for this fragment
       */
      inline uint32_t lvl1_id (void) const 
      { return m_node[2].base[2]; }

      /**
       * Changes the bunch crossing identifier in this fragment
       *
       * @param s The new value to set
       */
      inline void bc_id (uint32_t s) 
      { m_node[2].base[3] = s; }

      /**
       * Returns the bunch crossing identifier for this fragment
       */
      inline uint32_t bc_id (void) const 
      { return m_node[2].base[3]; }

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
       * Appends a new ROB fragment to this ROS fragment.
       *
       * @warning This will change the page structure of the last ROB fragment
       * inserted here, in order to concatenate the ROB fragments
       * together. Please note that this operation is not compatible with
       * multiple threads of operation, if you would like to share
       * eformat::write::ROBFragment's between threads. A better strategy would
       * be create, for every thread of operation, a proper ROBFragment
       * instead.
       *
       * @param rob The ROB fragment to be appended to myself
       */
      void append (eformat::write::ROBFragment* rob);

      /**
       * This returns the first child of this fragment. The system operates as
       * a concatenated list of fragments. From this child you can get to the
       * next.
       */
      inline const ROBFragment* first_child (void) const { return m_child; }

      /**
       * This method is used by children of this fragment to notify fragment
       * size changes.
       *
       * @param o The old size, in 32-bit words
       * @param n The new size, in 32-bit words
       */
      void size_change (uint32_t o, uint32_t n);

      /**
       * This returns the parent fragment
       */
      inline const SubDetectorFragment* parent (void) const 
      { return m_parent; }

      /**
       * This sets the parent fragment
       *
       * @param sd The SubDetectorFragment parent fragment of this ROS
       */
      inline void parent (eformat::write::SubDetectorFragment* sd)
      { m_parent = sd; }

      /**
       * Returns the next sibling
       */
      inline const ROSFragment* next (void) const { return m_next; }

      /**
       * Sets the next sibling
       *
       * @param n The sibling following this fragment
       */
      inline void next (const ROSFragment* n) { m_next = n; }

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

      /**
       * Return the extra node of the fragment for OHFiller_write.
       * lifei 
       */ 
      const eformat::write::node_t* extra(void){return & m_extra;};

    private: //representation

      uint32_t m_header[10]; ///< The ROS Header
      eformat::write::node_t m_node[3]; ///< Node representation
      eformat::write::SubDetectorFragment* m_parent; ///< my parent
      eformat::write::ROBFragment* m_child; ///< my ROB children
      eformat::write::ROBFragment* m_last; ///< my last ROB child
      const eformat::write::ROSFragment* m_next; ///< Next sibling
      eformat::write::node_t m_extra; ///< Extra pages I may have
      uint32_t m_extra_count; ///< How many extra pages I have
    };

  }

}

#endif /* EFORMAT_WRITE_ROSFRAGMENT_H */
