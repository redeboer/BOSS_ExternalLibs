//Dear emacs, this is -*- c++ -*-

/**
 * @file old/Header.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the old Header class
 */

#include "eformat/old/Header.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Version.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/BadVersionIssue.h"
#include "eformat/WrongMarkerIssue.h"
#include "eformat/NoSuchChildIssue.h"

eformat::old::Header::Header (const uint32_t* it, uint32_t match)
  : m_start(it)
{
  //get endiannesso
  uint32_t m = marker();
  if (m != match) throw EFORMAT_WRONG_MARKER(m, match);
}

bool eformat::old::Header::check () const
{
  if ( version() >> 16 != eformat::MAJOR_OLD_VERSION )
    throw EFORMAT_BAD_VERSION(version() >> 16, eformat::MAJOR_OLD_VERSION);
  if ( header_size_word() != (9 + nstatus() + noffset() + nspecific()) )
    throw EFORMAT_SIZE_CHECK(header_size_word(),
			     (9 + nstatus() + noffset() + nspecific()));
  return true;
}

const uint32_t* eformat::old::Header::child (size_t n) const
{
  if (n >= noffset()) throw EFORMAT_NO_SUCH_CHILD(n, noffset());
  return &m_start[0xffffff & offset()[n]];
}




