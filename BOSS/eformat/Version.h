//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/Version.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Defines a helper class to encode and decode version numbers.
 */

#ifndef EFORMAT_VERSION_H
#define EFORMAT_VERSION_H

#include <stdint.h>
#include <string>

namespace eformat {

  /**
   * The default eformat version to use
   */
  const uint32_t DEFAULT_VERSION = 0x03000000;

  /**
   * The major default eformat version to use
   */
  const uint16_t MAJOR_DEFAULT_VERSION = 0x0300;

  /**
   * The major default eformat version to use
   */
  const uint16_t MAJOR_OLD_VERSION = 0x0204;

  namespace helper {

    /**
     * Defines converters between version numbers and its components
     */
    class Version {

    public:
      /**
       * Constructor. Takes the components to form a version
       *
       * @param minor The minor version
       * @param major The major version
       */
      Version (uint16_t minor, uint16_t major=MAJOR_DEFAULT_VERSION)
	: m_minor(minor), m_major(major) {}

      /**
       * Constructor. Takes the version to understand the components from.
       *
       * @param v The version number, fully built.
       * @warning This version number <b>has</b> to conform to the current
       * version of the library or unpredictable results might occur.
       */
      Version (uint32_t v=DEFAULT_VERSION);

      /**
       * Extracts the major version part of this version
       */
      inline uint16_t major2 (void) const { return m_major; }

      /**
       * Extracts the minor version part of this version
       */
      inline uint16_t minor2 (void) const { return m_minor; }

      /**
       * Gets the full 32-bit number made by assembling the 2 numbers
       * above.
       */
      uint32_t code (void) const;

      /**
       * Returns a string representation of the major version number
       */
      std::string human_major (void) const;

      /**
       * Returns a string representation of the minor version number
       */
      std::string human_minor (void) const;

      /**
       * Returns a string representation of the version number
       */
      std::string human (void) const;

    private: //representation

      uint16_t m_minor; ///< The minor part of the version number
      uint16_t m_major; ///< The major part of the version number

    };

  }

}

#endif /* EFORMAT_VERSION_H */
