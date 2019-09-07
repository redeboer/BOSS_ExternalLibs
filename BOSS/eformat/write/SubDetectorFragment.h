//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/write/SubDetectorFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Helps the user to define and build a SubDetector fragment.
 */

#ifndef EFORMAT_WRITE_SUBDETECTORFRAGMENT_H
#define EFORMAT_WRITE_SUBDETECTORFRAGMENT_H

#include "eformat/write/ROSFragment.h"

namespace eformat {

  namespace write {

    class FullEventFragment; ///< forward

    /**
     * Defines a helper class to aid the creation of SubDetector fragments.
     */
    class SubDetectorFragment {

    public:

      /**
       * Builds a new SubDetector fragment from scratch
       *
       * @param source_id The source identifier to be using for this
       * SubDetector 
       */
      SubDetectorFragment (uint32_t source_id);

      /**
       * Builds a new SubDetector fragment from an existing 
       * SubDetector fragment in contiguous memory.
       *
       * @param sd The existing SubDetector fragment
       */
      SubDetectorFragment (uint32_t* sd);

      /**
       * Builds a new SubDetector fragment from an existing SubDetector
       * fragment in non-contiguous memory. The top-level fragment header is
       * expected to be on a contiguous area of memory, together with the first
       * word of the first child fragment (i.e. the ROS header marker). The
       * following data can be spread around.
       * 
       * @param sd The existing SubDetector fragment, as a list of nodes,
       * pre-concatenated by the caller.
       */
      SubDetectorFragment (eformat::write::node_t* sd);

      /**
       * Builds a new empty SubDetectorFragment, otherwise invalid. This is
       * useful for array builds and standard containers.
       */
      SubDetectorFragment ();

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
      SubDetectorFragment (const SubDetectorFragment& other);

      /**
       * Base destructor
       */
      virtual ~SubDetectorFragment () {}

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
      SubDetectorFragment& operator= (const SubDetectorFragment& other);

      /**
       * Changes the number of status words and the status words themselves
       * from the fragment
       *
       * @param n How many status words the underlying SubDetectorFragment
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
       * @param s The new minor version for this header
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
       * Appends a new ROS fragment to this SubDetector fragment.
       *
       * @warning This will change the page structure of the last ROS fragment
       * inserted here, in order to concatenate the ROS fragments
       * together. Please note that this operation is not compatible with
       * multiple threads of operation, if you would like to share
       * eformat::write::ROSFragment's between threads. A better strategy would
       * be create, for every thread of operation, a proper ROSFragment
       * instead.
       *
       * @param ros The ROS fragment to be appended to myself
       */
      void append (eformat::write::ROSFragment* ros);

      /**
       * This returns the first child of this fragment. The system operates as
       * a concatenated list of fragments. From this child you can get to the
       * next.
       */
      inline const ROSFragment* first_child (void) const { return m_child; }

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
      inline const FullEventFragment* parent (void) const 
      { return m_parent; }

      /**
       * This sets the parent fragment
       *
       * @param fe The FullEventFragment parent fragment of this SubDetector
       */
      inline void parent (eformat::write::FullEventFragment* fe)
      { m_parent = fe; }

      /**
       * Returns the next sibling
       */
      inline const SubDetectorFragment* next (void) const { return m_next; }

      /**
       * Sets the next sibling
       *
       * @param n The sibling following this fragment
       */
      inline void next (const SubDetectorFragment* n) { m_next = n; }

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

    private: //representation

      uint32_t m_header[7]; ///< The SubDetector Header
      eformat::write::node_t m_node[3]; ///< Node representation
      eformat::write::FullEventFragment* m_parent; ///< my parent
      eformat::write::ROSFragment* m_child; ///< my ROS children
      eformat::write::ROSFragment* m_last; ///< my last ROS child
      const eformat::write::SubDetectorFragment* m_next; ///< Next sibling
      eformat::write::node_t m_extra; ///< Extra pages I may have
      uint32_t m_extra_count; ///< How many extra pages I have
      
    };

  }

}

#endif /* EFORMAT_WRITE_SUBDETECTORFRAGMENT_H */
