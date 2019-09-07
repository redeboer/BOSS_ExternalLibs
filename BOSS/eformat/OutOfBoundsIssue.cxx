//Dear emacs, this is -*- c++ -*-

/**
 * @file OutOfBoundsIssue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the out-of-bounds exception.
 */

#include "eformat/OutOfBoundsIssue.h"

/**
 * Strings to identify keys in ERS
 */
static const char* BUFFER_SIZE_KEY = "Size in bytes of the buffer";
static const char* REQUESTED_BYTE_KEY = "Requested byte to access";

eformat::OutOfBoundsIssue::OutOfBoundsIssue(const ers::Context& context,
					    ers::severity_t severity,
					    size_t size, size_t pos)
  : eformat::Issue(context,severity)
{
  set_value(BUFFER_SIZE_KEY, size);
  set_value(REQUESTED_BYTE_KEY, pos);
  finish_setup("Requested buffer position is out of bounds");
}

size_t eformat::OutOfBoundsIssue::size () const
{
  return get_int_value(BUFFER_SIZE_KEY);
}

size_t eformat::OutOfBoundsIssue::pos () const
{
  return get_int_value(REQUESTED_BYTE_KEY);
}




