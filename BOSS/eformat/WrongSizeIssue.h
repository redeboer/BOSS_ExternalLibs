//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/WrongSizeIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief When size checks do not match, this exception must be thrown.
 */

#ifndef EFORMAT_WRONGSIZEISSUE_H
#define EFORMAT_WRONGSIZEISSUE_H

#include "eformat/Issue.h"
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when the user is looking for a
   * region of memory not covered inside the buffer area.
   */
  class WrongSizeIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new wrong-size exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param size The wrong size informed
     */
    WrongSizeIssue(const ers::Context& context, ers::severity_t severity,
		   uint32_t size);


    /**
     * Destructor virtualisation
     */
    virtual ~WrongSizeIssue() throw() {}

    /**
     * Access the actual value of the current fragment
     */
    uint32_t size () const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param size The wrong size informed
 */
#define EFORMAT_WRONG_SIZE(size) \
  eformat::WrongSizeIssue(ERS_HERE, ers::error, size)

#endif /* EFORMAT_WRONGSIZEISSUE_H */
