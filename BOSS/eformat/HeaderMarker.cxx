//Dear emacs, this is -*- c++ -*-

/**
 * @file HeaderMarker.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements a few functionality around the header marker
 */

#include "eformat/HeaderMarker.h"

eformat::HeaderMarker eformat::child_marker(eformat::HeaderMarker e)
{
  using namespace eformat;
  switch (e) {
  case FULL_EVENT:
    return SUB_DETECTOR;
  case SUB_DETECTOR:
    return ROS;
  case ROS:
    return ROB;
  case ROB:
    return ROD;
  default:
    break;
  }
  return FULL_EVENT;
}

std::string eformat::marker2string (const eformat::HeaderMarker& e)
{
  using namespace eformat;
  switch (e) {
  case ROD:
    return "ROD";
  case ROB:
    return "ROB";
  case ROS:
    return "ROS";
  case SUB_DETECTOR:
    return "SUB_DETECTOR";
  case FULL_EVENT:
    return "FULL_EVENT";
  }
  return "UNKNOWN_MARKER";
}

std::string eformat::marker2string (uint32_t e)
{
  return marker2string((eformat::HeaderMarker)e);
}
