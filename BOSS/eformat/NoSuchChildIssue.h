//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/NoSuchChildIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Exception thrown when the position of a child doesn't exist in the
 * current fragment.
 */

#ifndef EFORMAT_NOSUCHCHILDISSUE_H
#define EFORMAT_NOSUCHCHILDISSUE_H

#include "eformat/Issue.h"

namespace eformat {

  /**
   * This exception is supposed to be thrown when the user is looking for a
   * child fragment that doesn't exist.
   */
  class NoSuchChildIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new no-such-child exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param req The request child
     * @param total The total child number available
     */
    NoSuchChildIssue(const ers::Context& context, ers::severity_t severity,
		     size_t req, size_t total);

    /**
     * Destructor virtualisation
     */
    virtual ~NoSuchChildIssue() throw() {}

    /**
     * Access the number of the requested child
     */
    size_t requested (void) const;

    /**
     * Access the number of children said to exist in the current fragment
     */
    size_t total (void) const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param req The request child
 * @param total The total child number available
 */
#define EFORMAT_NO_SUCH_CHILD(req, total) \
  eformat::NoSuchChildIssue(ERS_HERE, ers::error, req, total)

#endif /* EFORMAT_NOSUCHCHILDISSUE_H */
