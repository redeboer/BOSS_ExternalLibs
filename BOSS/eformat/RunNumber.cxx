//Dear emacs, this is -*- c++ -*-

/**
 * @file RunNumber.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the RunNumber helper class.
 */

#include "eformat/RunNumber.h"

eformat::helper::RunNumber::RunNumber (uint32_t rn)
  : m_type(static_cast<eformat::RunType>(rn>>24)),
    m_n(0xffffff&rn)
{
}

uint32_t eformat::helper::RunNumber::code (void) const 
{
  uint32_t retval = m_type;
  retval <<= 24;
  retval |= m_n;
  return retval;
}


