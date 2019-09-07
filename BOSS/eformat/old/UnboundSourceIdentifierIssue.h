//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/UnboundSourceIdentifierIssue.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Exception thrown when I cannot map an old SourceIdentifier (v2.4)
 * into a new one
 */

#ifndef EFORMAT_UNBOUNDSOURCEIDENTIFIERISSUE_H
#define EFORMAT_UNBOUNDSOURCEIDENTIFIERISSUE_H

#include "eformat/Issue.h"
#include <cstdlib>
#include <stdint.h>

namespace eformat {

  /**
   * This exception is supposed to be thrown when I find a old source
   * identifier I cannot map to a new one
   */
  class UnboundSourceIdentifierIssue : public eformat::Issue {

  public: //interface

    /**
     * Builds a new bad-version exception
     *
     * @param context The Error Reporting System context to be used to identify
     * the spot where this issue was created
     * @param severity The severity of this issue
     * @param source_id The source identifier I'm having problems to map
     * @param my_context An explanation of the context.
     */
    UnboundSourceIdentifierIssue(const ers::Context& context, 
				 ers::severity_t severity,
				 uint32_t source_id, 
				 const std::string& my_context);

    /**
     * Destructor virtualisation
     */
    virtual ~UnboundSourceIdentifierIssue() throw() {}

    /**
     * Access the current source identifier the user is having problems with
     */
    uint32_t source_id () const;

    /**
     * Access the context explanation
     */
    const std::string& my_context () const;

  };

}

/**
 * Simplifies the use of this Issue
 *
 * @param source_id The source identifier I cannot map
 * @param cont The context in which I found it
 */
#define EFORMAT_UNBOUND_SOURCE_IDENTIFIER(sid, cont) \
  eformat::UnboundSourceIdentifierIssue(ERS_HERE, ers::error, sid, cont)

#endif /* EFORMAT_UNBOUNDSOURCEIDENTIFIERISSUE_H */
