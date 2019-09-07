//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/SizeCheckIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief When size checks do not match, this exception must be thrown.
 */

#ifndef EFORMAT_SIZECHECKEX_H
#define EFORMAT_SIZECHECKEX_H

#include "eformat/Issue.h"
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when the user is looking for a
   * region of memory not covered inside the buffer area.
   */
  class SizeCheckIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new size-check exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param actual The correct value for the fragment size added up
     * @param informed The informed value for the fragment size
     */
    SizeCheckIssue(const ers::Context& context, ers::severity_t severity,
		   uint32_t actual, uint32_t informed);


    /**
     * Destructor virtualisation
     */
    virtual ~SizeCheckIssue() throw() {}

    /**
     * Access the actual value of the current fragment
     */
    uint32_t actual () const;

    /**
     * Access the informed (by the header) value of the current fragment
     */
    uint32_t informed (void) const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param actual The actual value for the fragment size added up
 * @param informed The informed value for the fragment size
 */
#define EFORMAT_SIZE_CHECK(actual, informed) \
  eformat::SizeCheckIssue(ERS_HERE, ers::error, actual, informed)

#endif /* EFORMAT_SIZECHECKEX_H */
