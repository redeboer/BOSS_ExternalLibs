//Dear emacs, this is -*- c++ -*-

/**
 * @file old/RODFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the old (v2.4) ROD fragment class
 */

#include "eformat/old/RODFragment.h"
#include "eformat/util.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/BadVersionIssue.h"
#include "eformat/WrongMarkerIssue.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Version.h"
#include "eformat/Status.h"

eformat::old::RODFragment::RODFragment (const uint32_t* it, size_t size_word)
  : m_start(it),
    m_size(size_word)
{
  uint32_t m = marker();
  if (m != eformat::ROD) throw EFORMAT_WRONG_MARKER(m, eformat::ROD);
}

uint32_t eformat::old::RODFragment::fragment_size_word() const
{
  return header_size_word() + trailer_size_word() + nstatus() + ndata();
}

const uint32_t* eformat::old::RODFragment::status (void) const
{
  if (status_position() == eformat::STATUS_FRONT) return &m_start[9];
  return &m_start[9 + ndata()];
}

const uint32_t* eformat::old::RODFragment::data (void) const
{
  if (status_position() == eformat::STATUS_BACK) return &m_start[9];
  return &m_start[9 + nstatus()];
}

bool eformat::old::RODFragment::check () const
{
  if ( version() >> 16 != eformat::MAJOR_OLD_VERSION )
    throw EFORMAT_BAD_VERSION(version() >> 16, eformat::MAJOR_OLD_VERSION);
  if ( header_size_word() != 9 )
    throw EFORMAT_SIZE_CHECK(9, header_size_word());
  if ( m_size != 12 + nstatus() + ndata() )
    throw EFORMAT_SIZE_CHECK(m_size, (12 + nstatus() + ndata()));
  return true;
}




