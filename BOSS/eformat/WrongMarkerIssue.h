//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/WrongMarkerIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines the wrong-marker exception, to be used when the wrong marker
 * is found on the event stream.
 */

#ifndef EFORMAT_WRONGMARKERISSUE_H
#define EFORMAT_WRONGMARKERISSUE_H

#include "eformat/Issue.h"
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when version problems are found
   */
  class WrongMarkerIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new size-check exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param current The current marker on the stream
     * @param expected The expected marker on the stream
     */
    WrongMarkerIssue(const ers::Context& context, ers::severity_t severity,
		     uint32_t current, uint32_t expected);
    
    /**
     * Destructor virtualisation
     */
    virtual ~WrongMarkerIssue() throw() {}

    /**
     * Access the current object marker
     */
    uint32_t current () const;

    /**
     * Access the expected object marker
     */
    uint32_t expected () const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param current The current marker on the stream
 * @param expected The expected marker on the stream
 */
#define EFORMAT_WRONG_MARKER(current, expected)	\
  eformat::WrongMarkerIssue(ERS_HERE, ers::error, current, expected)

#endif /* EFORMAT_WRONGMARKERISSUE_H */
