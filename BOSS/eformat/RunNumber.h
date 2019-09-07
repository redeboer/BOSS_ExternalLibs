//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/RunNumber.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief A helper class to aid in composing valid run numbers
 */

#ifndef EFORMAT_RUNNUMBER_H
#define EFORMAT_RUNNUMBER_H

#include <stdint.h>

namespace eformat {

  /**
   * Physics Types
   */
  enum RunType { PHYSICS = 0x00,
		 CALIBRATION = 0x01,
		 COSMICS = 0x02,
		 TEST = 0x0f};

  /**
   * An alias
   */
  typedef enum RunType RunType;

  namespace helper {

    /**
     * Defines converters between version numbers and its components
     */
    class RunNumber {

    public:
      /**
       * Constructor. Takes the components to form a run number
       *
       * @param type The run type (@see types.h)
       * @param n The number 24-bits only are valid
       */
      RunNumber (eformat::RunType type, uint32_t n)
	: m_type(type), m_n(n) {}

      /**
       * Constructor. Takes the run number to understand the components from.
       *
       * @param rn The run number, fully built.
       * @warning This run number <b>has</b> to conform to the current
       * version of the library or unpredictable results might occur.
       */
      RunNumber (uint32_t rn);

      /**
       * Extracts the major version part of this version
       */
      inline eformat::RunType type (void) const { return m_type; }

      /**
       * Extracts the minor version part of this version
       */
      inline uint32_t number (void) const { return m_n; }

      /**
       * Gets the full 32-bit number made by assembling the 2 numbers
       * above.
       */
      uint32_t code (void) const;

    private: //representation

      eformat::RunType m_type; ///< This run type
      uint32_t m_n; ///< This run number

    };

  }

}

#endif /* EFORMAT_RUNNUMBER_H */
