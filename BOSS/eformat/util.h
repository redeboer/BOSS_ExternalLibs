//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/util.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André Rabello dos
 * ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines a set of utilities common to many event operations.
 */

#ifndef EFORMAT_UTIL_H
#define EFORMAT_UTIL_H

#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include "eformat/HeaderMarker.h"
#include "eformat/WrongMarkerIssue.h"

namespace eformat {

  /**
   * This function will read the next fragment in a normal, already opened,
   * std::fstream. The space for the fragment will be allocated dynamically and
   * the user should free it. Otherwise, if the user wants to re-use a
   * pre-allocated memory space, the second and third arguments can be given,
   * in which case the function will check if the space is enough. If the space
   * is not enough, NULL is returned.
   *
   * @param fs The input filestream
   * @param addr The optional user allocated space
   * @param size The optional size, in bytes, of the allocated space
   */
  uint32_t* next_fragment (std::fstream& fs, uint32_t* addr=0, size_t size=0);

  /**
   * Returns pointers (to words) to the start of each ROD fragment block in a
   * piece of memory. This is primarily intended for LVL2 supervisor usage.
   *
   * @warning This will invert the data order, meaning the last ROD will be the
   * first in the vector to appear, due to the way the ROD readout has to
   * happen (back to front of the stream). If that is not satisfactory, you
   * have to sort back the vector yourself.
   *
   * @param block_start The start address of the data block you want to dig the
   * ROD pointers from
   * @param block_size The size of the block, in 32-bit words 
   * @param rod A (optional) pointer to set of pre-allocated pointers
   * @param rod_size A (optional) pointer to set of pre-allocated size_t's,
   * where the size of each ROD, in words, will be stored.
   * @param max_count The maximum number of blocks to dig out from
   * <tt>block_start</tt>, if rod is not NULL. If rod is NULL, this is
   * meaningless.
   *
   * @return The number of ROD fragments the function actually found on the
   * block, irrespective of max_count.
   */
  size_t find_rods (const uint32_t* block_start, size_t block_size,
		    const uint32_t** rod=0, uint32_t* rod_size=0,
		    size_t max_count=0);

  /**
   * A generic method to find all fragments in a contiguous area of
   * memory. These fragments cannot be RODFragments. For that, use the
   * eformat::find_rods().
   *
   * @param marker The marker you are searching for.
   * @param block_start A pointer to the block start
   * @param block_size The overall size of this block, in 32-bit words
   * @param frag A (optional) preallocated set of pointers to hold the found
   * fragments. If frag is NULL, only counts the number of children.
   * @param max_count The maximum number of fragments I'll search for, if frag
   * is not NULL. If frag is NULL, this flag has no meaning.
   *
   * @return The number of fragments the function actually found, irrespective
   * of max_count.
   */
  template <class TPointer>
  size_t find_fragments (eformat::HeaderMarker marker, 
			 TPointer block_start, size_t block_size,
			 TPointer* frag=0, size_t max_count=0);

  /**
   * Gets pointers to all ROB fragments from a fragment of a type which is not
   * known in advance.
   *
   * @param fragment The top level fragment to extract the other ROBFragment's
   * from.
   * @param rod A pointer to set of pre-allocated pointers
   * @param max_count The maximum number of blocks to dig out from
   * <tt>block_start</tt>.
   *
   * @return The number of ROBFragments the function actually found
   */
  size_t get_robs (const uint32_t* fragment, const uint32_t** rob,
		   size_t max_count);

}

/**
 * Prints in hexadecimal format, in an ostream
 */
#ifndef HEX
#define HEX(m) "0x" << std::hex << (int)m  << std::dec << " (" << (int)m << ")"
#endif

template <class TPointer>
size_t eformat::find_fragments (eformat::HeaderMarker marker, 
				TPointer block_start, size_t block_size,
				TPointer* frag, size_t max_count)
{
  uint32_t counter = 0;
  TPointer next = block_start;
  TPointer endp = block_start;
  endp += block_size;
  while (next < endp) {
    if (next[0] != marker) throw EFORMAT_WRONG_MARKER(next[0], marker);
    if (frag && counter < max_count) frag[counter] = next;
    ++counter;
    next += next[1];
  }
  return counter;
}


#endif //EFORMAT_UTIL_H
