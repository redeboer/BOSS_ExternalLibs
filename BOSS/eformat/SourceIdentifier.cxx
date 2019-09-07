//Dear emacs, this is -*- c++ -*-

/**
 * @file SourceIdentifier.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André Rabello dos
 * ANJOS</a>
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/02/20 23:38:21 $
 *
 * @brief Defines a type that can perform the conversion between source
 * identifier components and the its 32-bit version.
 */

#include "eformat/SourceIdentifier.h"
#include <sstream>

eformat::helper::SourceIdentifier::SourceIdentifier 
(eformat::SubDetector subdet, uint16_t id)
  : m_sd(subdet),
    m_id(id)
{
}

eformat::helper::SourceIdentifier::SourceIdentifier (uint32_t sid)
  : m_sd(static_cast<eformat::SubDetector>((sid >> 16) & 0xff)),
    m_id(static_cast<uint16_t>(0xffff & sid))
{
}

uint32_t eformat::helper::SourceIdentifier::code (void) const
{
  uint32_t retval = (0xff) & m_sd;
  retval <<= 16;
  retval |= m_id;
  return retval;
}

std::string eformat::helper::SourceIdentifier::human (void) const
{
  std::ostringstream oss;
  oss << human_detector() << ", Identifier = " << (unsigned int)module_id();
  return oss.str();
}

std::string eformat::helper::SourceIdentifier::human_detector (void) const
{
  using namespace eformat;
  std::string s;
  switch (subdetector_id()) {
  case FULL_SD_EVENT:
    s += "FULL_SD_EVENT";
    break;
  case PIXEL_BARREL:
    s += "PIXEL_BARREL";
    break;
  case PIXEL_FORWARD_A_SIDE:
    s += "PIXEL_FORWARD_A_SIDE";
    break;
  case PIXEL_FORWARD_C_SIDE:
    s += "PIXEL_FORWARD_C_SIDE";
    break;
  case PIXEL_B_LAYER:
    s += "PIXEL_B_LAYER";
    break;
  case SCT_BARREL_A_SIDE:
    s += "SCT_BARREL_A_SIDE";
    break;
  case SCT_BARREL_C_SIDE:
    s += "SCT_BARREL_C_SIDE";
    break;
  case SCT_ENDCAP_A_SIDE:
    s += "SCT_ENDCAP_A_SIDE";
    break;
  case SCT_ENDCAP_C_SIDE:
    s += "SCT_ENDCAP_C_SIDE";
    break;
  case TRT_ANCILLARY_CRATE:
    s += "TRT_ANCILLARY_CRATE";
    break;
  case TRT_BARREL_A_SIDE:
    s += "TRT_BARREL_A_SIDE";
    break;
  case TRT_BARREL_C_SIDE:
    s += "TRT_BARREL_C_SIDE";
    break;
  case TRT_ENDCAP_A_SIDE:
    s += "TRT_ENDCAP_A_SIDE";
    break;
  case TRT_ENDCAP_C_SIDE:
    s += "TRT_ENDCAP_C_SIDE";
    break;
  case LAR_EM_BARREL_A_SIDE:
    s += "LAR_EM_BARREL_A_SIDE";
    break;
  case LAR_EM_BARREL_C_SIDE:
    s += "LAR_EM_BARREL_C_SIDE";
    break;
  case LAR_EM_ENDCAP_A_SIDE:
    s += "LAR_EM_ENDCAP_A_SIDE";
    break;
  case LAR_EM_ENDCAP_C_SIDE:
    s += "LAR_EM_ENDCAP_C_SIDE";
    break;
  case LAR_HAD_ENDCAP_A_SIDE:
    s += "LAR_HAD_ENDCAP_A_SIDE";
    break;
  case LAR_HAD_ENDCAP_C_SIDE:
    s += "LAR_HAD_ENDCAP_C_SIDE";
    break;
  case LAR_FCAL_A_SIDE:
    s += "LAR_FCAL_A_SIDE";
    break;
  case LAR_FCAL_C_SIDE:
    s += "LAR_FCAL_C_SIDE";
    break;
  case TILECAL_LASER_CRATE:
    s += "TILECAL_LASER_CRATE";
    break;
  case TILECAL_BARREL_A_SIDE:
    s += "TILECAL_BARREL_A_SIDE";
    break;
  case TILECAL_BARREL_C_SIDE:
    s += "TILECAL_BARREL_C_SIDE";
    break;
  case TILECAL_EXT_A_SIDE:
    s += "TILECAL_EXT_A_SIDE";
    break;
  case TILECAL_EXT_C_SIDE:
    s += "TILECAL_EXT_C_SIDE";
    break;
  case MUON_ANCILLARY_CRATE:
    s += "MUON_ANCILLARY_CRATE";
    break;
  case MUON_MDT_BARREL_A_SIDE:
    s += "MUON_MDT_BARREL_A_SIDE";
    break;
  case MUON_MDT_BARREL_C_SIDE:
    s += "MUON_MDT_BARREL_C_SIDE";
    break;
  case MUON_MDT_ENDCAP_A_SIDE:
    s += "MUON_MDT_ENDCAP_A_SIDE";
    break;
  case MUON_MDT_ENDCAP_C_SIDE:
    s += "MUON_MDT_ENDCAP_C_SIDE";
    break;
  case MUON_RPC_BARREL_A_SIDE:
    s += "MUON_RPC_BARREL_A_SIDE";
    break;
  case MUON_RPC_BARREL_C_SIDE:
    s += "MUON_RPC_BARREL_C_SIDE";
    break;
  case MUON_TGC_ENDCAP_A_SIDE:
    s += "MUON_TGC_ENDCAP_A_SIDE";
    break;
  case MUON_TGC_ENDCAP_C_SIDE:
    s += "MUON_TGC_ENDCAP_C_SIDE";
    break;
  case MUON_CSC_ENDCAP_A_SIDE:
    s += "MUON_CSC_ENDCAP_A_SIDE";
    break;
  case MUON_CSC_ENDCAP_C_SIDE:
    s += "MUON_CSC_ENDCAP_C_SIDE";
    break;
  case TDAQ_BEAM_CRATE:
    s += "TDAQ_BEAM_CRATE";
    break;
  case TDAQ_CALO_PREPROC:
    s += "TDAQ_CALO_PREPROC";
    break;
  case TDAQ_CALO_CLUSTER_PROC_DAQ:
    s += "TDAQ_CALO_CLUSTER_PROC_DAQ";
    break;
  case TDAQ_CALO_CLUSTER_PROC_ROI:
    s += "TDAQ_CALO_CLUSTER_PROC_ROI";
    break;
  case TDAQ_CALO_JET_PROC_DAQ:
    s += "TDAQ_CALO_JET_PROC_DAQ";
    break;
  case TDAQ_CALO_JET_PROC_ROI:
    s += "TDAQ_CALO_JET_PROC_ROI";
    break;
  case TDAQ_MUON_CTP_INTERFACE:
    s += "TDAQ_MUON_CTP_INTERFACE";
    break;
  case TDAQ_CTP:
    s += "TDAQ_CTP";
    break;
  case TDAQ_L2SV:
    s += "TDAQ_L2SV";
    break;
  case TDAQ_SFI:
    s += "TDAQ_SFI";
    break;
  case TDAQ_SFO:
    s += "TDAQ_SFO";
    break;
  case TDAQ_LVL2:
    s += "TDAQ_LVL2";
    break;
  case TDAQ_EVENT_FILTER:
    s += "TDAQ_EVENT_FILTER";
    break;
  case OTHER:
    s += "OTHER";
    break;
  default:
    break;
  }
  return s;
}
