//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/OutOfBoundsIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Out of bounds exception when trying to read beyond existing buffers.
 */

#ifndef EFORMAT_OUTOFBOUNDSISSUE_H
#define EFORMAT_OUTOFBOUNDSISSUE_H

#include "eformat/Issue.h"
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when the user is looking for a
   * region of memory not covered inside the buffer area.
   */
  class OutOfBoundsIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new out-of-bounds issue
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param size The current buffer size
     * @param pos The requested position
     */
    OutOfBoundsIssue(const ers::Context& context, ers::severity_t severity,
		     size_t size, size_t pos); 

    /**
     * Destructor virtualisation
     */
    virtual ~OutOfBoundsIssue() throw() {}

    /**
     * Access the size of the initial buffer, in bytes
     */
    size_t size (void) const;

    /**
     * Access the requested position to access
     */
    size_t pos (void) const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param size The current buffer size
 * @param pos The requested position
 */
#define EFORMAT_OUT_OF_BOUNDS(size, pos) \
  eformat::OutOfBoundsIssue(ERS_HERE, ers::error, size, pos)

#endif /* EFORMAT_OUTOFBOUNDSISSUE_H */
