//Dear emacs, this is -*- c++ -*-

/**
 * @file src/old/UnboundSourceIdentifierIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the functionality described in the header.
 */

#include "eformat/old/UnboundSourceIdentifierIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* SOURCE_ID_KEY = "Unbound source identifier";
static const char* CONTEXT_KEY = "Context";

eformat::UnboundSourceIdentifierIssue::UnboundSourceIdentifierIssue
(const ers::Context& context,
 ers::severity_t severity,
 uint32_t source_id, 
 const std::string& my_context)
  : eformat::Issue(context,severity)
{
  set_value(SOURCE_ID_KEY, source_id);
  set_value(CONTEXT_KEY, my_context);
  finish_setup("Unbound (old) source identifier detected");
}

uint32_t eformat::UnboundSourceIdentifierIssue::source_id () const
{
  return get_int_value(SOURCE_ID_KEY);
}

const std::string& eformat::UnboundSourceIdentifierIssue::my_context () const
{
  return get_value(CONTEXT_KEY);
}
