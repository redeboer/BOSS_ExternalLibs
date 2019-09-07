//Dear emacs, this is -*- c++ -*-

/**
 * @file NoSuchChildIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the no-such-child exception object.
 */

#include "eformat/NoSuchChildIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* REQUESTED_CHILD_KEY = "Number of the requested child";
static const char* NUMBER_OF_CHILDREN_KEY = "Total number of children";

eformat::NoSuchChildIssue::NoSuchChildIssue(const ers::Context& context,
					    ers::severity_t severity,
					    size_t req, size_t total)
  : eformat::Issue(context,severity)
{
  set_value(REQUESTED_CHILD_KEY, req);
  set_value(NUMBER_OF_CHILDREN_KEY, total);
  finish_setup("Requested child is out of bounds");
}

size_t eformat::NoSuchChildIssue::requested () const
{
  return get_int_value(REQUESTED_CHILD_KEY);
}

size_t eformat::NoSuchChildIssue::total () const
{
  return get_int_value(NUMBER_OF_CHILDREN_KEY);
}
