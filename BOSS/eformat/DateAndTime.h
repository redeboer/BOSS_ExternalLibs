//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/DateAndTime.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Describes how to create and interpret a date and time field.
 */

#ifndef EFORMAT_HELPER_DATEANDTIME_H
#define EFORMAT_HELPER_DATEANDTIME_H

#include <stdint.h>
#include <string>

namespace eformat {

  namespace helper {

    /**
     * Interprets and composes a date and time field (32-bit unsigned integer)
     */
    class DateAndTime {

    public: //interface

      /**
       * Default constructor, it means, now
       */
      DateAndTime ();

      /**
       * Build from an existing date in time
       *
       * @param val The value of the compiled date and time field
       */
      DateAndTime (uint32_t val);

      /**
       * Get the current representation 
       */
      inline uint32_t code (void) const { return m_val; } 

      /**
       * Returns a string that represents the time in ISO8601
       */
      std::string iso8601 (void) const;

      /**
       * Returns a string that represents the time in a human readable format
       */
      std::string human (void) const;

    private: //implementation 

      uint32_t m_val; ///< my current value

    };

  }

}

#endif /* EFORMAT_HELPER_DATEANDTIME_H */
