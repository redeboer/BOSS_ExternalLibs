//Dear emacs, this is -*- c++ -*-

/**
 * @file WrongSizeIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the size-check issue
 */

#include "eformat/WrongSizeIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* SIZE_KEY = "The informed (wrong) size value";

eformat::WrongSizeIssue::WrongSizeIssue(const ers::Context& context,
					ers::severity_t severity,
					uint32_t size)
  : eformat::Issue(context,severity)
{
  set_value(SIZE_KEY, size);
  finish_setup("Informed fragment size is wrong");
}

uint32_t eformat::WrongSizeIssue::size () const
{
  return get_int_value(SIZE_KEY);
}

