//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/Issue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines the base eformat Issue
 */

#ifndef EFORMAT_ISSUE_H
#define EFORMAT_ISSUE_H

#include "ers/Issue.h"

namespace eformat {

  /**
   * This exception is supposed to be thrown when version problems are found
   */
  class Issue : public ers::Issue {

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
    Issue(const ers::Context& context, ers::severity_t severity);

    /**
     * Destructor virtualisation
     */
    virtual ~Issue() throw() {}

  };

}

/**
 * Simplifies the use of this Issue
 */
#define EFORMAT_ISSUE eformat::Issue(ERS_HERE, ers::error)

#endif /* EFORMAT_ISSUE_H */
