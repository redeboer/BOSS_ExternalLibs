//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/BadVersionIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Exception thrown when versions do not match
 */

#ifndef EFORMAT_BADVERSIONISSUE_H
#define EFORMAT_BADVERSIONISSUE_H

#include "eformat/Issue.h"
#include <cstdlib>
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when version problems are found
   */
  class BadVersionIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new bad-version exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param current The version number that this fragment has
     * @param supported The version which is supposed to be supported here
     */
    BadVersionIssue(const ers::Context& context, ers::severity_t severity,
		    uint16_t current, uint16_t supported);

    /**
     * Destructor virtualisation
     */
    virtual ~BadVersionIssue() throw() {}

    /**
     * Access the current object version
     */
    uint16_t current () const;

    /**
     * Access the supported version
     */
    uint16_t supported () const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param current The current version you are trying to read
 * @param supported The supported version by this library
 */
#define EFORMAT_BAD_VERSION(current, supported) \
  eformat::BadVersionIssue(ERS_HERE, ers::error, current, supported)

#endif /* EFORMAT_BADVERSIONISSUE_H */
