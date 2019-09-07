//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/TooBigCountIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Describes an exception that happens when the total number of blocks
 * I'm managing is bigger than the one I was asked to manage initially.
 */

#ifndef EFORMAT_TOOBIGCOUNTISSUE_H
#define EFORMAT_TOOBIGCOUNTISSUE_H

#include "eformat/Issue.h"
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when the user is trying to add
   * more blocks than the number of blocks I can handle.
   */
  class TooBigCountIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new size-check exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param count The number of blocks I was asked to deal with
     * @param maxcount The maximum number of blocks I can deal with
     */
    TooBigCountIssue(const ers::Context& context, ers::severity_t severity,
		     size_t count, size_t maxcount);


    /**
     * Destructor virtualisation
     */
    virtual ~TooBigCountIssue() throw() {}

    /**
     * The number of blocks I have to deal with
     */
    size_t count () const;

    /**
     * The maximum number of blocks I can deal with
     */
    size_t max_count () const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param count The number of blocks I was asked to deal with
 * @param maxcount The maximum number of blocks I can deal with
 */
#define EFORMAT_TOO_BIG_COUNT(count, maxcount) \
  eformat::TooBigCountIssue(ERS_HERE, ers::error, count, maxcount)

#endif /* EFORMAT_TOOBIGCOUNTISSUE_H */
