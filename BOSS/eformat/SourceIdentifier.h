//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/SourceIdentifier.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André dos ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Declares a type that can perform the conversion between source
 * identifier components and the its 32-bit version.
 */

#ifndef EFORMAT_HELPER_SOURCEIDENTIFIER_H
#define EFORMAT_HELPER_SOURCEIDENTIFIER_H

#include <stdint.h>
#include <string>

namespace eformat {

  /**
   * Sub-detector ID
   */
  enum SubDetector { FULL_SD_EVENT = 0x00,
		     PIXEL_BARREL = 0x11,
		     PIXEL_FORWARD_A_SIDE = 0x12,
		     PIXEL_FORWARD_C_SIDE = 0x13,
		     PIXEL_B_LAYER = 0x14,
		     SCT_BARREL_A_SIDE = 0x21,
		     SCT_BARREL_C_SIDE = 0x22,
		     SCT_ENDCAP_A_SIDE = 0x23,
		     SCT_ENDCAP_C_SIDE = 0x24,
		     TRT_ANCILLARY_CRATE = 0x30,
		     TRT_BARREL_A_SIDE = 0x31,
		     TRT_BARREL_C_SIDE = 0x32,
		     TRT_ENDCAP_A_SIDE = 0x33,
		     TRT_ENDCAP_C_SIDE = 0x34,
		     LAR_EM_BARREL_A_SIDE = 0x41,
		     LAR_EM_BARREL_C_SIDE = 0x42,
		     LAR_EM_ENDCAP_A_SIDE = 0x43,
		     LAR_EM_ENDCAP_C_SIDE = 0x44,
		     LAR_HAD_ENDCAP_A_SIDE = 0x45,
		     LAR_HAD_ENDCAP_C_SIDE = 0x46,
		     LAR_FCAL_A_SIDE = 0x47,
		     LAR_FCAL_C_SIDE = 0x48,
		     TILECAL_LASER_CRATE = 0x50,
		     TILECAL_BARREL_A_SIDE = 0x51,
		     TILECAL_BARREL_C_SIDE = 0x52,
		     TILECAL_EXT_A_SIDE = 0x53,
		     TILECAL_EXT_C_SIDE = 0x54,
		     MUON_ANCILLARY_CRATE = 0x60,
		     MUON_MDT_BARREL_A_SIDE = 0x61,
		     MUON_MDT_BARREL_C_SIDE = 0x62,
		     MUON_MDT_ENDCAP_A_SIDE = 0x63,
		     MUON_MDT_ENDCAP_C_SIDE = 0x64,
		     MUON_RPC_BARREL_A_SIDE = 0x65,
		     MUON_RPC_BARREL_C_SIDE = 0x66,
		     MUON_TGC_ENDCAP_A_SIDE = 0x67,
		     MUON_TGC_ENDCAP_C_SIDE = 0x68,
		     MUON_CSC_ENDCAP_A_SIDE = 0x69,
		     MUON_CSC_ENDCAP_C_SIDE = 0x6A,
		     TDAQ_BEAM_CRATE = 0x70,
		     TDAQ_CALO_PREPROC = 0x71,
		     TDAQ_CALO_CLUSTER_PROC_DAQ = 0x72,
		     TDAQ_CALO_CLUSTER_PROC_ROI = 0x73,
		     TDAQ_CALO_JET_PROC_DAQ = 0x74,
		     TDAQ_CALO_JET_PROC_ROI = 0x75,
		     TDAQ_MUON_CTP_INTERFACE = 0x76,
		     TDAQ_CTP = 0x77,
		     TDAQ_L2SV = 0x78,
		     TDAQ_SFI = 0x79,
		     TDAQ_SFO = 0x7a,
		     TDAQ_LVL2 = 0x7b,
		     TDAQ_EVENT_FILTER = 0x7c,
                     OTHER = 0x81,
		     BESIII_MDC = 0xa1,
		     BESIII_TOF = 0xa2,
		     BESIII_EMC = 0xa3,
		     BESIII_MUC = 0xa4,
		     BESIII_TRG = 0xa5
		};

  /**
   * An alias
   */
  typedef enum SubDetector SubDetector;

  namespace helper {

    /**
     * Defines converters between source identifiers and its components.
     */
    class SourceIdentifier {

    public:
      /**
       * Constructor. Takes the components to form a source identifier.
       *
       * @param subdet The subdetector for this source
       * @param id The module identifier
       */
      SourceIdentifier (eformat::SubDetector subdet, uint16_t id);

      /**
       * Constructor. Takes the source identifier to understand the components
       * from.
       *
       * @param sid The source identifier, fully built.
       * @warning This source identifier <b>has</b> to conform to the current
       * version of the library or unpredictable results might occur.
       */
      SourceIdentifier (uint32_t sid);

      /**
       * Extracts the SubDetector component of the identifier.
       */
      inline eformat::SubDetector subdetector_id (void) const { return m_sd; }

      /**
       * Extracts the Module identifier from the source identifier.
       */
      inline uint16_t module_id (void) const { return m_id; }

      /**
       * Gets the full 32-bit number made by assembling the 3 numbers
       * above.
       */
      uint32_t code (void) const;

      /**
       * Returns a string that represents the source identifier in a human
       * readable format
       */
      std::string human (void) const;

      /**
       * Returns a string that represents the subdetector identifier in a human
       * readable format
       */
      std::string human_detector (void) const;

    private: //representation

      eformat::SubDetector m_sd; ///< The subdetector component
      uint16_t m_id; ///< The module identifier

    };

  }

}

#endif //EVENTFORMAT_HELPER_SOURCEID_H
