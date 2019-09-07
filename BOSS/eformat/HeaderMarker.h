//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/HeaderMarker.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André DOS ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $ 
 *
 * @brief Defines the constants used by Event Fragments.
 */

#ifndef EFORMAT_HEADERMARKER_H
#define EFORMAT_HEADERMARKER_H

#include <stdint.h>
#include <string>

namespace eformat {

  /**
   * The types of header markers available. They are all 32 bit
   * numbers, with 1234 in the middle for the identification of byte
   * ordering (endianness).
   */
  enum HeaderMarker { ROD = 0xee1234ee, ///< The ROD marker
		      ROB = 0xdd1234dd, ///< The ROB marker
		      ROS = 0xcc1234cc, ///< The ROS marker
		      SUB_DETECTOR = 0xbb1234bb, ///< The SubDet. marker
		      FULL_EVENT = 0xaa1234aa}; ///< The event marker
 
  /**
   * An alias
   */
  typedef enum HeaderMarker HeaderMarker;

  /**
   * Returns the child marker of a given parent marker
   *
   * @param e The marker from which you would like to the get child fragment
   * type from
   */
  HeaderMarker child_marker(HeaderMarker e);

  /**
   * Returns a string that represents the string of the equivalent marker
   *
   * @param e The enumeration value
   */
  std::string marker2string (const eformat::HeaderMarker& e);

  /**
   * Returns a string that represents the string of the equivalent marker
   *
   * @param e The enumeration value
   */
  std::string marker2string (uint32_t e);

}

#endif //EFORMAT_HEADERMARKER_H
