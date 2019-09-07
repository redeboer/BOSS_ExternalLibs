//Dear emacs, this is -*- c++ -*-

/**
 * @file TooBigCountIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the too-big-count issue
 */

#include "eformat/TooBigCountIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* CURRENT_COUNT_KEY = "The number of blocks I have to manage";
static const char* MAX_COUNT_KEY = "The maximum number of blocks I can handle";

eformat::TooBigCountIssue::TooBigCountIssue(const ers::Context& context,
					    ers::severity_t severity,
					    size_t count, size_t maxcount)
  : eformat::Issue(context,severity)
{
  set_value(CURRENT_COUNT_KEY, count);
  set_value(MAX_COUNT_KEY, maxcount);
  finish_setup("Overloaded paged-memory container");
}

size_t eformat::TooBigCountIssue::count () const
{
  return get_int_value(CURRENT_COUNT_KEY);
}

size_t eformat::TooBigCountIssue::max_count () const
{
  return get_int_value(MAX_COUNT_KEY);
}
