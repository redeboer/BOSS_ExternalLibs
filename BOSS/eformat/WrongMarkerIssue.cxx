//Dear emacs, this is -*- c++ -*-

/**
 * @file WrongMarkerIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the wrong-marker exception class
 */

#include "eformat/WrongMarkerIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* CURRENT_MARKER_KEY = "Current header marker";
static const char* EXPECTED_MARKER_KEY = "Expected header marker";

eformat::WrongMarkerIssue::WrongMarkerIssue(const ers::Context& context,
					    ers::severity_t severity,
					    uint32_t current, 
					    uint32_t expected)

  : eformat::Issue(context,severity)
{
  set_value(CURRENT_MARKER_KEY, current);
  set_value(EXPECTED_MARKER_KEY, expected);
  finish_setup("Unexpected header marker found on stream");
}

uint32_t eformat::WrongMarkerIssue::current () const
{
  return get_int_value(CURRENT_MARKER_KEY);
}

uint32_t eformat::WrongMarkerIssue::expected () const
{
  return get_int_value(EXPECTED_MARKER_KEY);
}
