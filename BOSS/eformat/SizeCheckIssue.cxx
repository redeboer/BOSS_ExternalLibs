//Dear emacs, this is -*- c++ -*-

/**
 * @file SizeCheckIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the size-check issue
 */

#include "eformat/SizeCheckIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* ACTUAL_SIZE_KEY = "The true size value (in words)";
static const char* INFORMED_SIZE_KEY = "The value at the fragment (in words)";

eformat::SizeCheckIssue::SizeCheckIssue(const ers::Context& context,
					ers::severity_t severity,
					uint32_t actual, uint32_t informed)
  : eformat::Issue(context,severity)
{
  set_value(ACTUAL_SIZE_KEY, actual);
  set_value(INFORMED_SIZE_KEY, informed);
  finish_setup("Informed fragment (or header) size is wrong");
}

uint32_t eformat::SizeCheckIssue::actual () const
{
  return get_int_value(ACTUAL_SIZE_KEY);
}

uint32_t eformat::SizeCheckIssue::informed () const
{
  return get_int_value(INFORMED_SIZE_KEY);
}


