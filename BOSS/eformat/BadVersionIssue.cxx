//Dear emacs, this is -*- c++ -*-

/**
 * @file BadVersionIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the bad-version Issue
 */

#include "eformat/BadVersionIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* CURRENT_VERSION_KEY = "Current version";
static const char* SUPPORTED_VERSION_KEY = "Supported version";

eformat::BadVersionIssue::BadVersionIssue(const ers::Context& context,
					  ers::severity_t severity,
					  uint16_t current, 
					  uint16_t supported)
  : eformat::Issue(context,severity)
{
  set_value(CURRENT_VERSION_KEY, current);
  set_value(SUPPORTED_VERSION_KEY, supported);
  finish_setup("Unsupported eformat version detected");
}

uint16_t eformat::BadVersionIssue::current () const
{
  return get_int_value(CURRENT_VERSION_KEY);
}

uint16_t eformat::BadVersionIssue::supported () const
{
  return get_int_value(SUPPORTED_VERSION_KEY);
}



