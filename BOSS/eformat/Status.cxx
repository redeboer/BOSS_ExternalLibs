//Dear emacs, this is -*- c++ -*-

/**
 * @file Status.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the Status helper class.
 */

#include "eformat/Status.h"

eformat::helper::Status::Status (uint32_t v)
  : m_gen(static_cast<eformat::GenericStatus>(v&0xffff)),
    m_spec((v>>16)&0xffff)
{
}

uint32_t eformat::helper::Status::code (void) const 
{
  uint32_t retval = m_spec;
  retval <<= 16;
  retval |= m_gen;
  return retval;
}
