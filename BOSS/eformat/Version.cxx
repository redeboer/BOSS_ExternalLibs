//Dear emacs, this is -*- c++ -*-

/**
 * @file Version.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the Version helper class.
 */

#include "eformat/Version.h"
#include <sstream>

eformat::helper::Version::Version (uint32_t v) 
  : m_minor(0xffff&v), m_major(v>>16)
{
}

uint32_t eformat::helper::Version::code (void) const 
{
  uint32_t retval = m_major;
  retval <<= 16;
  retval |= m_minor;
  return retval;
}

std::string eformat::helper::Version::human_major (void) const
{
  std::ostringstream oss;
  oss << (major2() >> 8) << "." << (major2() & 0xff);
  return oss.str();
}

std::string eformat::helper::Version::human_minor (void) const
{
  std::ostringstream oss;
  oss << (minor2() >> 8) << "." << (minor2() & 0xff);
  return oss.str();
}

std::string eformat::helper::Version::human (void) const
{
  std::string retval = human_major();
  retval += "-";
  retval += human_minor();
  return retval;
}

