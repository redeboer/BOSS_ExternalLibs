//Dear emacs, this is -*- c++ -*-

/**
 * @file NotAlignedIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the not-aligned exception.
 */

#include <sstream>
#include <string>
#include "eformat/NotAlignedIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* BASE_ADDRESS_KEY = "Base address";
static const char* SIZE_KEY = "Total block size";

eformat::NotAlignedIssue::NotAlignedIssue(const ers::Context& context,
					  ers::severity_t severity,
					  const void* base, size_t size)
  : eformat::Issue(context,severity)
{
  std::ostringstream oss;
  oss << base;
  set_value(BASE_ADDRESS_KEY, oss.str());
  set_value(SIZE_KEY, size);
  finish_setup("Misaligned data block");
}

const void* eformat::NotAlignedIssue::base () const
{
  std::istringstream iss;
  iss.str(get_value(BASE_ADDRESS_KEY));
  size_t x;
  iss >> x;
  return (void*)x; //nasty cast
}

size_t eformat::NotAlignedIssue::size () const
{
  return get_int_value(SIZE_KEY);
}
