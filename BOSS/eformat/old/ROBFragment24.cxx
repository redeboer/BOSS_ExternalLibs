//Dear emacs, this is -*- c++ -*-

/**
 * @file old/ROBFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the old eformat 2.4 ROB fragment
 */

#include "eformat/old/ROBFragment.h"
#include "eformat/old/RODFragment.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/HeaderMarker.h"

eformat::old::ROBFragment::ROBFragment (const uint32_t* it)
  : eformat::old::Header(it, eformat::ROB),
    m_start(specific_header())
{
}

bool eformat::old::ROBFragment::check () const
{
  eformat::old::Header::check(); //< first do a generic check
  if (nspecific() != 4) throw EFORMAT_SIZE_CHECK(4, nspecific());
  return true;
}

bool eformat::old::ROBFragment::check_tree () const
{
  check(); // check myself
  for (size_t i=0; i<noffset(); ++i) {
    size_t rod_size = 0;
    //typical
    if (noffset() == 1) rod_size = fragment_size_word() - header_size_word();
    else { //more atypical, have to calculate
      if (i != noffset() - 1) rod_size = (0xffffff & offset()[i+1]);
      else rod_size = fragment_size_word();
      rod_size -= (0xffffff & offset()[i]);
    }
    eformat::old::RODFragment f(child(i), rod_size);
    f.check();
  }
  return true;
}

eformat::old::RODFragment eformat::old::ROBFragment::rod (size_t n) const
{
  check();
  size_t rod_size = 0;
  //typical
  if (noffset() == 1) 
    rod_size = fragment_size_word() - header_size_word();
  else { //more atypical, have to calculate
    if (n != noffset() - 1) rod_size = (0xffffff & offset()[n+1]);
    else rod_size = fragment_size_word();
    rod_size -= (0xffffff & offset()[n]);
  }
  eformat::old::RODFragment f(child(n), rod_size);
  f.check();
  return f;
}



