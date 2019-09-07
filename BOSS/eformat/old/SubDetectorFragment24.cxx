//Dear emacs, this is -*- c++ -*-

/**
 * @file old/SubDetectorFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the old SubDetector fragment interface
 */

#include "eformat/old/SubDetectorFragment.h"
#include "eformat/old/ROSFragment.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/HeaderMarker.h"

eformat::old::SubDetectorFragment::SubDetectorFragment (const uint32_t* it)
  : eformat::old::Header(it, eformat::SUB_DETECTOR),
    m_start(specific_header())
{ 
}

bool eformat::old::SubDetectorFragment::check () const
{
  eformat::old::Header::check(); //< first do a generic check
  if (nspecific() != 1) throw EFORMAT_SIZE_CHECK(1, nspecific());
  return true;
}

bool eformat::old::SubDetectorFragment::check_tree () const
{
  check(); // check myself
  for (size_t i=0; i<noffset(); ++i) {
    eformat::old::ROSFragment ros(child(i));
    ros.check_tree();
  }
  return true;
}



