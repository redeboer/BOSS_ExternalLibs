//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/Status.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief A helper class to help the user to interpret the status information
 * in the first status word and to code it properly.
 */

#ifndef EFORMAT_STATUS_H
#define EFORMAT_STATUS_H

#include <stdint.h>

namespace eformat {

  /**
   * Defines possible generic errors for the status words
   */
  enum GenericStatus {
    NO_ERROR = 0x0,
    BCID_CHECK_FAIL = 0x1,
    LVL1ID_CHECK_FAIL = 0x2,
    TIMEOUT = 0x4,
    DATA_CORRUPTION = 0x8,
    INTERNAL_OVERFLOW = 0x16
  };

  /**
   * An alias
   */
  typedef enum GenericStatus GenericStatus;

  const uint32_t STATUS_FRONT = 0; ///< status goes in front of data block
  const uint32_t STATUS_BACK = 1; ///< status goes after data block 

  /**
   * The default status to use
   */
  const uint32_t DEFAULT_STATUS = NO_ERROR;

  namespace helper {

    /**
     * Defines converters between version numbers and its components
     */
    class Status {

    public:
      /**
       * Constructor. Takes the components to form a status
       *
       * @param gen The generic component
       * @param spec The specific component
       */
      Status (eformat::GenericStatus gen, uint16_t spec)
	: m_gen(gen), m_spec(spec) {}

      /**
       * Constructor. Takes the status to understand the components from.
       *
       * @param s The status number, fully built.
       * @warning This status value <b>has</b> to conform to the current
       * version of the library or unpredictable results might occur.
       */
      Status (uint32_t v);

      /**
       * Extracts the generic part of this status word
       */
      inline eformat::GenericStatus generic (void) const { return m_gen; }

      /**
       * Extracts the specific part of this status word
       */
      inline uint16_t specific (void) const { return m_spec; }

      /**
       * Gets the full 32-bit number made by assembling the 2 numbers
       * above.
       */
      uint32_t code (void) const;

    private: //representation

      eformat::GenericStatus m_gen; ///< The generic part
      uint16_t m_spec; ///< The specific part

    };

  }

}

#endif /* EFORMAT_STATUS_H */
