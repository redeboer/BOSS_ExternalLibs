//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/NotAlignedIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Describes the exception where some chunk of memory is not 32-bit
 * aligned.
 */

#ifndef EFORMAT_NOTALIGNEDISSUE_H
#define EFORMAT_NOTALIGNEDISSUE_H

#include "eformat/Issue.h"
#include <cstdlib>

namespace eformat {

  /**
   * Describes the out-of-bounds exception, that is thrown when the user
   * requests a memory location out of the bounds of a buffer.
   */
  class NotAlignedIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new not-aligned exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param base The address of the page
     * @param size The size of this page, in bytes
     */
    NotAlignedIssue(const ers::Context& context, ers::severity_t severity,
		    const void* base, size_t size);

    /**
     * Destructor virtualisation
     */
    virtual ~NotAlignedIssue() throw() {}

    /**
     * Access the address of the first page of the IO vector
     */
    const void* base () const;

    /**
     * The size of this block, in bytes
     */
    size_t size () const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param req The request child
 * @param total The total child number available
 */
#define EFORMAT_NOT_ALIGNED(base, size) \
  eformat::NotAlignedIssue(ERS_HERE, ers::error, base, size)

#endif /* EFORMAT_NOTALIGNEDISSUE_H */
