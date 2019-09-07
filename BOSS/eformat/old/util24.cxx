//Dear emacs, this is -*- c++ -*-

/**
 * @file util.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the utilities described in the equivalent header.
 */

#include "eformat/old/util.h"
#include "eformat/old/UnboundSourceIdentifierIssue.h"
#include "eformat/old/FullEventFragment.h"
#include "eformat/old/SubDetectorFragment.h"
#include "eformat/old/ROSFragment.h"
#include "eformat/old/ROBFragment.h"
#include "eformat/old/RODFragment.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/HeaderMarker.h"
#include "eformat/WrongMarkerIssue.h"
#include "eformat/Version.h"
#include "eformat/BadVersionIssue.h"
#include "ers/StreamFactory.h"
#include "eformat/write/FullEventFragment.h"

uint32_t eformat::old::convert_source (uint32_t old_id)
{
  using namespace eformat;

  uint16_t id = old_id & 0x00ff;
  uint8_t sd = ( old_id >> 8 ) & 0xff;
  uint8_t md = ( old_id >> 16 ) & 0xff;
  uint32_t retval = 0;

  switch ((SubDetector)sd) {
  case FULL_SD_EVENT:
  case PIXEL_BARREL:
  case PIXEL_FORWARD_A_SIDE:
  case PIXEL_FORWARD_C_SIDE:
  case PIXEL_B_LAYER:
  case SCT_BARREL_A_SIDE:
  case SCT_BARREL_C_SIDE:
  case SCT_ENDCAP_A_SIDE:
  case SCT_ENDCAP_C_SIDE:
  case TRT_ANCILLARY_CRATE:
  case TRT_BARREL_A_SIDE:
  case TRT_BARREL_C_SIDE:
  case TRT_ENDCAP_A_SIDE:
  case TRT_ENDCAP_C_SIDE:
  case LAR_EM_BARREL_A_SIDE:
  case LAR_EM_BARREL_C_SIDE:
  case LAR_EM_ENDCAP_A_SIDE:
  case LAR_EM_ENDCAP_C_SIDE:
  case LAR_HAD_ENDCAP_A_SIDE:
  case LAR_HAD_ENDCAP_C_SIDE:
  case LAR_FCAL_A_SIDE:
  case LAR_FCAL_C_SIDE:
  case TILECAL_LASER_CRATE:
  case TILECAL_BARREL_A_SIDE:
  case TILECAL_BARREL_C_SIDE:
  case TILECAL_EXT_A_SIDE:
  case TILECAL_EXT_C_SIDE:
  case MUON_ANCILLARY_CRATE:
  case MUON_MDT_BARREL_A_SIDE:
  case MUON_MDT_BARREL_C_SIDE:
  case MUON_MDT_ENDCAP_A_SIDE:
  case MUON_MDT_ENDCAP_C_SIDE:
  case MUON_RPC_BARREL_A_SIDE:
  case MUON_RPC_BARREL_C_SIDE:
  case MUON_TGC_ENDCAP_A_SIDE:
  case MUON_TGC_ENDCAP_C_SIDE:
  case MUON_CSC_ENDCAP_A_SIDE:
  case MUON_CSC_ENDCAP_C_SIDE:
  case TDAQ_BEAM_CRATE:
  case TDAQ_CALO_PREPROC:
  case TDAQ_CALO_CLUSTER_PROC_DAQ:
  case OTHER:
    retval = sd;
    break;
  case TDAQ_CALO_CLUSTER_PROC_ROI: //old TDAQ_CALO_JET
    retval = 0x74;
    break;
  case TDAQ_CALO_JET_PROC_DAQ: //old TDAQ_CTP
    retval = 0x77;
    break;
  case TDAQ_CALO_JET_PROC_ROI: //old TDAQ_MUON_INTERFACE
    retval = 0x76;
    break;
  case TDAQ_MUON_CTP_INTERFACE: //old TDAQ_DATAFLOW
    switch (md) {
    case 0x02: //LVL1 ROSes
    case 0x04: //old SUPERVISOR
      retval = 0x78; //new L2SV
      break;
    case 0x06: //old SFI
      retval = 0x79; //new SFI
      break;
    case 0x07: //old SFO
      retval = 0x7a; //new SFO
      break;
    case 0x0a: //old OTHER_MODULE
      retval = OTHER; //new OTHER
      break;
    default:
      ERS_WARN("%s%x%s%s", 
	       "Propose an equivalent of the source identifier 0x", old_id,
	       " [v2.4], for the 3.0 format, to eformat developers.",
	       "The subdetector identifier field will be zero'd for now.");
    } 
    break;
  case TDAQ_CTP: //old TDAQ_LVL2
    switch (md) {
    case 0x01: //old L2PU->PROS
    case 0x02: //old L2PU->PROS
    case 0x05: //old HLT_PROCESSOR
      retval = 0x7b; //new LVL2
      break;
    case 0x04: //old SUPERVISOR
      retval = 0x78; //new L2SV
      break;
    case 0x06: //old SFI marker for PROS data
      retval = 0x79; //new SFI
      break;
    default:
      ERS_WARN("%s%x%s%s", 
	       "Propose an equivalent of the source identifier 0x", old_id,
	       " [v2.4], for the 3.0 format, to eformat developers.",
	       "The subdetector identifier field will be zero'd for now.");
    }
    break;
  case TDAQ_L2SV: //old TDAQ_EVENT_FILTER
    switch (md) {
    case 0x05: //old HLT_PROCESSOR
      retval = 0x7b; //new LVL2
    default:
      ERS_WARN("%s%x%s%s", 
	       "Propose an equivalent of the source identifier 0x", old_id,
	       " [v2.4], for the 3.0 format, to eformat developers.",
	       "The subdetector identifier field will be zero'd for now.");
    }
    break;
  case TDAQ_SFI:
  case TDAQ_SFO:
  case TDAQ_LVL2:
  case TDAQ_EVENT_FILTER:
  default:
    ERS_WARN("%s%x%s%s",
	     "Propose an equivalent of the source identifier 0x", old_id,
	     " [v2.4], for the 3.0 format, to eformat developers.",
	     "The subdetector identifier field will be zero'd for now.");
    break;
  }
  retval <<= 16;
  retval |= id;
  ERS_DEBUG_3("Source identifier 0x%x [v2.4] was converted to 0x%x [v3.0]",
	      old_id, retval);
  return retval;
}

/**
 * Converts a ROS fragment, from the old to new format, using the space of
 * contiguous memory storage area given. If the event given is already on v3.0
 * format, no conversion takes place.
 *
 * @param src A pointer to the first word of the fragment, lying in a @b
 * contiguous area of memory.
 * @param dest The destination area of memory, preallocated
 * @param max The maximum number of words that fit in the preallocated
 * memory area "dest".
 *
 * @return A counter, for the number of words copied from the source to the
 * destination. If that number is zero, something wrong happened.
 */
uint32_t convert_ros(const uint32_t* src, uint32_t* dest, uint32_t max)
{
  using namespace eformat;

  if (src[0] != ROS) {
    throw EFORMAT_WRONG_MARKER(src[0], ROS);
  }

  //check version
  helper::Version version(src[3]);
  if (version.major2() == MAJOR_DEFAULT_VERSION) {
    memcpy(dest, src, sizeof(uint32_t)*src[1]);
    return src[1];
  }
  if (version.major2() != MAJOR_OLD_VERSION)
    throw EFORMAT_BAD_VERSION(version.major2(), MAJOR_DEFAULT_VERSION);
  
  //this is from the old major version of eformat, proceed with conversion
  old::ROSFragment ros(src);
  ros.check_tree(); //this may throw
/**********renzy edit***/
//  write::ROSFragment nros(old::convert_source(ros.source_id()),
  eformat::write::ROSFragment nros(old::convert_source(ros.source_id()),
/**********renzy edit***/
			  ros.run_no(), ros.lvl1_id(), ros.bc_id());
  nros.status(ros.nstatus(), ros.status());
  helper::Version ros_version(ros.version());
  nros.minor_version(ros_version.minor2());

/**********renzy edit***/
  //std::vector<write::ROBFragment*> acc_rob;
  std::vector<eformat::write::ROBFragment*> acc_rob;
/**********renzy edit***/
  for (size_t k=0; k<ros.noffset(); ++k) {
    old::ROBFragment rob(ros.child(k));
    uint32_t source_id = rob.source_id();
    
    for (size_t l=0; l<rob.noffset(); ++l) {
      old::RODFragment rod(rob.rod(l));
      if (rob.noffset() != 1) source_id = rod.source_id();
/**********renzy edit***/
      //write::ROBFragment* nrob = new write::ROBFragment
      eformat::write::ROBFragment* nrob = new eformat::write::ROBFragment
/**********renzy edit***/
	(old::convert_source(source_id), rod.run_no(), rod.lvl1_id(),
	 rod.bc_id(), rod.lvl1_trigger_type(), rod.detev_type(), 
	 rod.ndata(), rod.data(), rod.status_position());
      nrob->status(rob.nstatus(), rob.status());
      nrob->rod_status(rod.nstatus(), rod.status());
      helper::Version rob_version(rob.version());
      nrob->minor_version(rob_version.minor2());
      helper::Version rod_version(rod.version());
      nrob->rod_minor_version(rod_version.minor2());
      
      //make this new ROB part of the new ROS
      nros.append(nrob);
      //make sure we don't forget to delete this guy
      acc_rob.push_back(nrob);
    }
  }

  //now the ROS is in `nros', bind
  const eformat::write::node_t* top = nros.bind();
  //memcpy the list of pages into contiguous memory
  uint32_t retval = eformat::write::copy(*top, dest, max);

  //delete the dynamically allocated stuff
  for (size_t i=0; i<acc_rob.size(); ++i) delete acc_rob[i];

  return retval;
}

uint32_t eformat::old::convert(const uint32_t* src, uint32_t* dest,
			       uint32_t max)
{
  using namespace eformat;

  if (src[0] != FULL_EVENT) {
    if (src[0] != ROS) {
      throw EFORMAT_WRONG_MARKER(src[0], FULL_EVENT);
    }
    return convert_ros(src, dest, max);
  }

  //check version
  helper::Version version(src[3]);
  if (version.major2() == MAJOR_DEFAULT_VERSION) {
    memcpy(dest, src, sizeof(uint32_t)*src[1]);
    return src[1];
  }
  if (version.major2() != MAJOR_OLD_VERSION)
    throw EFORMAT_BAD_VERSION(version.major2(), MAJOR_DEFAULT_VERSION);
  
  //this is from the old major version of eformat, proceed with conversion
  old::FullEventFragment fe(src);
  fe.check_tree(); //this may throw

  //create the base FullEvent
/**********renzy edit***/
  //write::FullEventFragment nfe(convert_source(fe.source_id()), 
  eformat::write::FullEventFragment nfe(convert_source(fe.source_id()), 
/**********renzy edit***/
			       fe.date(), fe.global_id(), fe.run_no(),
			       fe.lvl1_id(), fe.lvl1_trigger_type(), 
			       fe.lvl2_trigger_info(), fe.event_filter_info());
  nfe.status(fe.nstatus(), fe.status());
  nfe.minor_version(version.minor2());

/**********renzy edit***/
  /*std::vector<write::SubDetectorFragment*> acc_sd;
  std::vector<write::ROSFragment*> acc_ros;
  std::vector<write::ROBFragment*> acc_rob;*/
  std::vector<eformat::write::SubDetectorFragment*> acc_sd;
  std::vector<eformat::write::ROSFragment*> acc_ros;
  std::vector<eformat::write::ROBFragment*> acc_rob;
/**********renzy edit***/
  for (size_t i=0; i<fe.noffset(); ++i) {
    old::SubDetectorFragment sd(fe.child(i));
    //create the new subdetector and set _all_ relevant stuff
/**********renzy edit***/
    /*write::SubDetectorFragment* nsd = 
      new write::SubDetectorFragment(convert_source(sd.source_id()));*/
    eformat::write::SubDetectorFragment* nsd = 
      new eformat::write::SubDetectorFragment(convert_source(sd.source_id()));
/**********renzy edit***/
    nsd->status(sd.nstatus(), sd.status());
    helper::Version sd_version(sd.version());
    nsd->minor_version(sd_version.minor2());

    for (size_t j=0; j<sd.noffset(); ++j) {
      old::ROSFragment ros(sd.child(j));
/**********renzy edit***/
      /*write::ROSFragment* nros = 
	new write::ROSFragment(convert_source(ros.source_id()),
			       ros.run_no(), ros.lvl1_id(), ros.bc_id());*/
     eformat::write::ROSFragment* nros = 
	new eformat::write::ROSFragment(convert_source(ros.source_id()),
			       ros.run_no(), ros.lvl1_id(), ros.bc_id());
/**********renzy edit***/
      nros->status(ros.nstatus(), ros.status());
      helper::Version ros_version(ros.version());
      nros->minor_version(ros_version.minor2());

      for (size_t k=0; k<ros.noffset(); ++k) {
	old::ROBFragment rob(ros.child(k));
	uint32_t source_id = rob.source_id();

	for (size_t l=0; l<rob.noffset(); ++l) {
	  old::RODFragment rod(rob.rod(l));
	  if (rob.noffset() != 1) source_id = rod.source_id();
/**********renzy edit***/
	  //write::ROBFragment* nrob = new write::ROBFragment
	  eformat::write::ROBFragment* nrob = new eformat::write::ROBFragment
/**********renzy edit***/
	    (convert_source(source_id), rod.run_no(), rod.lvl1_id(),
	     rod.bc_id(), rod.lvl1_trigger_type(), rod.detev_type(), 
	     rod.ndata(), rod.data(), rod.status_position());
	  nrob->status(rob.nstatus(), rob.status());
	  nrob->rod_status(rod.nstatus(), rod.status());
	  helper::Version rob_version(rob.version());
	  nrob->minor_version(rob_version.minor2());
	  helper::Version rod_version(rod.version());
	  nrob->rod_minor_version(rod_version.minor2());

	  //make this new ROB part of the new ROS
	  nros->append(nrob);
	  //make sure we don't forget to delete this guy
	  acc_rob.push_back(nrob);
	}
      }
      //maks this new ROS part of the new SD
      nsd->append(nros);
      //make sure we don't forget to delete this guy
      acc_ros.push_back(nros);
    }
    //make this new SD part of the new FE
    nfe.append(nsd);
    //make sure we don't forget to delete this guy
    acc_sd.push_back(nsd);
  }

  //now the FullEvent is in `nfe', bind
  const eformat::write::node_t* top = nfe.bind();
  //memcpy the list of pages into contiguous memory
  uint32_t retval = eformat::write::copy(*top, dest, max);

  //delete the allocated stuff
  for (size_t i=0; i<acc_rob.size(); ++i) delete acc_rob[i];
  for (size_t i=0; i<acc_ros.size(); ++i) delete acc_ros[i];
  for (size_t i=0; i<acc_sd.size(); ++i) delete acc_sd[i];

  return retval;
}



