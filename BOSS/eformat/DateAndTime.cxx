//Dear emacs, this is -*- c++ -*-

/**
 * @file DateAndTime.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements date and time interpretation
 */

#include "eformat/DateAndTime.h"
#include <ctime>

eformat::helper::DateAndTime::DateAndTime ()
  : m_val(time(0))
{
}

eformat::helper::DateAndTime::DateAndTime (uint32_t val)
  : m_val(val)
{
}

std::string eformat::helper::DateAndTime::iso8601 (void) const
{
  struct tm t;
  time_t val = code();
  gmtime_r(&val, &t);
  char buf[100];
  strftime(buf, 100, "%Y-%m-%dT%H:%M:%SZ", &t);
  return buf;
}

std::string eformat::helper::DateAndTime::human (void) const
{
  struct tm t;
  time_t val = code();
  gmtime_r(&val, &t);
  char buf[100];
  strftime(buf, 100, "%a, %d %b %Y %H:%M:%S UTC", &t);
  return buf;
}
