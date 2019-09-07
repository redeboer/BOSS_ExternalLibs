//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/write/ROBFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Helps the user to define and build a ROB fragment.
 */

#ifndef EFORMAT_WRITE_ROBFRAGMENT_H
#define EFORMAT_WRITE_ROBFRAGMENT_H

#include "eformat/write/node.h"
#include "eformat/Version.h"

namespace eformat {

  namespace write {

    class ROSFragment; ///< forward

    /**
     * Defines a helper class to aid the creation of ROB fragments.
     */
    class ROBFragment {

    public:

      /**
       * Builds a new ROB fragment from scratch
       *
       * @param source_id The source identifier to be using for this ROB
       * @param run_no The run number for this ROB/ROD
       * @param lvl1_id The LVL1 identifier for this ROB/ROD
       * @param bc_id The bunch crossing identifier for this ROB/ROD
       * @param lvl1_type The LVL1 trigger type identifier for this ROB/ROD
       * @param detev_type The detector event type for this ROB/ROD
       * @param ndata The number of 32-bits pointed by "data" bellow
       * @param data A set of 32-bit words that consist the data block of
       * this ROB fragment
       * @param status_position The status block position you want to have on
       * the ROD format. Use the definitions available in Status.h to
       * get generic identifiers for the possibilities.
       */
      ROBFragment (uint32_t source_id,
		   uint32_t run_no,
		   uint32_t lvl1_id,
		   uint32_t bc_id,
		   uint32_t lvl1_type,
		   uint32_t detev_type,
		   uint32_t ndata,
		   const uint32_t* data,
		   uint32_t status_position);

      /**
       * Builds a new "empty" ROB fragment, otherwise invalid. Use the methods
       * bellow to set its fields. This is useful to pre-allocate objects of
       * this class in the stack and later use them by assigment. Another
       * option are standard containers, that demand the contained classes to
       * have this method.
       */
      ROBFragment ();

      /**
       * Copy constructor. This will perform a shallow copy of the fragment. In
       * this case, the data and status blocks are not copied, only the meta
       * information, i.e., @b all but the payload of the underlying ROD
       * fragment and non-variable data blocks are copied. If you wish to have
       * the data/status copied, you have to copy it manually yourself and set
       * the data pointer with the rod_data() and status() and rod_status()
       * methods. The parent fragment remains unset, and you have to append
       * this fragment manually after creation. Binding also doesn't take
       * place.
       *
       * @param other The other fragment to copy the meta data from.
       */
      ROBFragment (const ROBFragment& other);

      /**
       * Builds a new ROB fragment from scratch, starting from an existing ROD
       * fragment as basis.
       *
       * @param rod The rod fragment you want to start with, in a contiguous
       * block of memory
       * @param size_word The size of the ROD fragment, in words
       */
      ROBFragment (uint32_t* rod, uint32_t size_word);

      /**
       * Builds a new ROB fragment from an existing ROB fragment in contiguous
       * memory
       *
       * @warning You cannot build from non-contiguous memory. If you have that
       * requirement, please post it to the eformat developers.
       *
       * @param rob The ROB fragment you want to start with, in a contiguous
       * block of memory
       */
      ROBFragment (uint32_t* rob);

      /**
       * Base destructor
       */
      virtual ~ROBFragment () {}

      /**
       * Assignment operator. This will perform a shallow copy of the
       * fragment. In this case, the data and status blocks are not copied,
       * only the meta information, i.e., @b all but the payload of the
       * underlying ROD fragment and non-variable data blocks are copied. If
       * you wish to have the data/status copied, you have to copy it manually
       * yourself and set the data pointer with the rod_data() and status() and
       * rod_status() methods. The parent fragment is @b not changed in this
       * operation. Previous binding operations remain valid after assignment.
       *
       * @param other The other fragment to copy the meta data from.
       */
      ROBFragment& operator= (const ROBFragment& other);

      /**
       * Changes the number of status words and the status words themselves
       * from the fragment
       *
       * @param n How many status words the underlying ROD fragment supposed
       * to have.
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
       * Changes the number of status words and the status words themselves
       * from the ROD fragment
       *
       * @param n How many status words the underlying ROD fragment supposed to
       * have 
       * @param status A pointer to <tt>n</tt> pre-allocated words
       */
      void rod_status (uint32_t n, const uint32_t* status);

      /**
       * Returns the number of status wors in this fragment
       */
      inline uint32_t rod_nstatus (void) const { return m_node[6].base[0]; }

      /**
       * Returns a pointer to the first ROD status word to be used by this
       * fragment 
       */
      inline const uint32_t* rod_status (void) const { return m_node[4].base; }

      /**
       * Changes the order of the status and data blocks in the ROD fragment
       *
       * @param s The new value. If v is zero, the status will preceed the
       * data, while the contrary will happen if v equals 1.
       */
      void status_position (uint32_t v);

      /**
       * Returns the current status position
       */
      inline uint32_t status_position (void) const 
      { return m_node[6].base[2]; }

      /**
       * Changes the minor version number of the underlying ROD fragment
       *
       * @param v The new minor version for this header
       */
      inline void rod_minor_version (uint16_t v)
      { m_node[3].base[2] = eformat::DEFAULT_VERSION | v; }

      /**
       * Changes the number of data words and the data words themselves
       * from the ROD fragment
       *
       * @param n How many data words the underlying ROD fragment supposed to
       * have 
       * @param status A pointer to <tt>n</tt> pre-allocated words
       */
      void rod_data (uint32_t n, const uint32_t* data);

      /**
       * Returns the number of data words at the ROD fragment
       */
      inline uint32_t rod_ndata (void) const { return m_node[6].base[1]; }

      /**
       * Returns a pointer to the first of the data words at the ROD fragment
       */
      inline const uint32_t* rod_data (void) const { return m_node[5].base; }

      /**
       * Changes the source identifier for both the ROB and the ROD fragments
       *
       * @param s The new value to set
       */
      inline void source_id (uint32_t s) 
      { m_node[0].base[4] = m_node[3].base[3] = s; }

      /**
       * Returns the source identifier for both the ROB and the ROD fragments
       */
      inline uint32_t source_id (void) const 
      { return m_node[0].base[4]; }

      /**
       * Changes the run number for the ROD fragment
       *
       * @param s The new value to set
       */
      inline void rod_run_no (uint32_t s) 
      { m_node[3].base[4] = s; }

      /**
       * Returns the run number for the ROD fragment
       */
      inline uint32_t rod_run_no (void) const 
      { return m_node[3].base[4]; }

      /**
       * Changes the lvl1 identifier in the ROD fragment
       *
       * @param s The new value to set
       */
      inline void rod_lvl1_id (uint32_t s) 
      { m_node[3].base[5] = s; }

      /**
       * Returns the lvl1 identifier for the ROD fragment
       */
      inline uint32_t rod_lvl1_id (void) const 
      { return m_node[3].base[5]; }

      /**
       * Changes the bunch crossing identifier in the ROD fragment
       *
       * @param s The new value to set
       */
      inline void rod_bc_id (uint32_t s) 
      { m_node[3].base[6] = s; }

      /**
       * Returns the bunch crossing identifier for the ROD fragment
       */
      inline uint32_t rod_bc_id (void) const 
      { return m_node[3].base[6]; }

      /**
       * Changes the lvl1 trigger type in the ROD fragment
       *
       * @param s The new value to set
       */
      inline void rod_lvl1_type (uint32_t s) 
      { m_node[3].base[7] = s; }

      /**
       * Returns the lvl1 event type identifier for the ROD fragment
       */
      inline uint32_t rod_lvl1_type (void) const 
      { return m_node[3].base[7]; }

      /**
       * Changes the detector event type in the ROD fragment
       *
       * @param s The new value to set
       */
      inline void rod_detev_type (uint32_t s) 
      { m_node[3].base[8] = s; }

      /**
       * Returns the detector event type identifier for the ROD fragment
       */
      inline uint32_t rod_detev_type (void) const 
      { return m_node[3].base[8]; }

      /**
       * Returns the total size for the meta data (everything that does @b not
       * encompass the contents of the m_data pointer in the private
       * representation of this class) in the fragment, in words
       */
      inline uint32_t meta_size_word (void) const
      { return 7 + m_node[0].base[5] + 12 + m_node[6].base[0]; }

      /**
       * Returns the total size for this fragment, in words
       */
      inline uint32_t size_word (void) const
      { return meta_size_word() + m_node[6].base[1]; }

      /**
       * Returns the number of pages of memory I have 
       */
      inline uint32_t page_count (void) const { return 7; }

      /**
       * Outputs a concatenation of eformat::write::node making up a list with
       * the contents of this ROB fragment.
       */
      inline const eformat::write::node_t* bind (void) const
      { return m_node; }

      /**
       * This sets the parent fragment
       *
       * @param ros The ROS parent fragment of this ROB/ROD
       */
      inline void parent (eformat::write::ROSFragment* ros) 
      { m_parent = ros; }

      /**
       * This returns the parent fragment
       */
      inline const ROSFragment* parent (void) const { return m_parent; }

      /**
       * Sets the next sibling
       *
       * @param n The sibling following this fragment
       */
      inline void next (const ROBFragment* n) { m_next = n; }

      /**
       * Returns the next sibling
       */
      inline const ROBFragment* next (void) const { return m_next; }

    private: //representation

      uint32_t m_header[7]; ///< The ROB Header
      uint32_t m_rod_header[9]; ///< The ROD Header
      uint32_t m_rod_trailer[3]; ///< The ROD trailer
      eformat::write::node_t m_node[7]; ///< Node representation
      eformat::write::ROSFragment* m_parent; ///< My parent
      const eformat::write::ROBFragment* m_next; ///< Next sibling

    };

  }

}

#endif /* EFORMAT_WRITE_ROBFRAGMENT_H */
