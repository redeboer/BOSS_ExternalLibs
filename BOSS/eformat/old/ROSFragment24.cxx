//Dear emacs, this is -*- c++ -*-

/**
 * @file old/ROSFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the old ROS fragment, as defined in eformat 2.4
 */

#include "eformat/old/ROSFragment.h"
#include "eformat/old/ROBFragment.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/HeaderMarker.h"

eformat::old::ROSFragment::ROSFragment (const uint32_t* it)
  : eformat::old::Header(it, eformat::ROS),
    m_start(specific_header())
{
}

bool eformat::old::ROSFragment::check () const
{
  eformat::old::Header::check(); //< first do a generic check
  if (nspecific() != 2) {
    throw EFORMAT_SIZE_CHECK(2, nspecific());
  }
  return true;
}

bool eformat::old::ROSFragment::check_tree () const
{
  check(); // check myself
  for (size_t i=0; i<noffset(); ++i) {
    eformat::old::ROBFragment rob(child(i));
    rob.check_tree();
  }
  return true;
}




