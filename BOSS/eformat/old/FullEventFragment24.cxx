//Dear emacs, this is -*- c++ -*-

/**
 * @file old/FullEventFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the old FullEventFragment class
 */

#include "eformat/old/FullEventFragment.h"
#include "eformat/old/SubDetectorFragment.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/HeaderMarker.h"

eformat::old::FullEventFragment::FullEventFragment (const uint32_t* it)
  : eformat::old::Header(it, eformat::FULL_EVENT),
    m_start(specific_header())
{
}

bool eformat::old::FullEventFragment::check () const
{
  eformat::old::Header::check(); //< first do a generic check
  if (nspecific() != 10) { //@warning THIS IS WRONG!!! It should be 9
    throw EFORMAT_SIZE_CHECK(10, nspecific());
  }
  return true;
}

bool eformat::old::FullEventFragment::check_tree () const
{
  check(); // check myself
  for (size_t i=0; i<noffset(); ++i) {
    eformat::old::SubDetectorFragment sd(child(i));
    sd.check_tree();
  }
  return true;
}



