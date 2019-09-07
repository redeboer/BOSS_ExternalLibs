//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/old/util.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief A set of utilities to convert old eformat (v2.4) tags into new (v3)
 * ones.
 */

#ifndef EFORMAT_OLD_UTIL_H
#define EFORMAT_OLD_UTIL_H

#include <stdint.h>

namespace eformat {

  namespace old {

    /**
     * Gets an old source identitifier (v2.4) and transforms it into a new one,
     * for version 3.0 of the event format
     *
     * @param old_id The old source identifier
     */
    uint32_t convert_source (uint32_t old_id);

    /**
     * Converts a full event fragment, from the old to new format, using the
     * space of contiguous memory storage area given. If the event given is
     * already on v3.0 format, no conversion takes place.
     *
     * @param src A pointer to the first word of the event, lying in a @b
     * contiguous area of memory.
     * @param dest The destination area of memory, preallocated
     * @param max The maximum number of words that fit in the preallocated
     * memory area "dest".
     *
     * @return A counter, for the number of words copied from the source to the
     * destination. If that number is zero, something wrong happened.
     */
    uint32_t convert(const uint32_t* src, uint32_t* dest, uint32_t max);

  }

}

#endif /* EFORMAT_OLD_UTIL_H */
