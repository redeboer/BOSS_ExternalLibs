//Dear emacs, this is -*- c++ -*-

/**
 * @file util.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André Rabello dos
 * ANJOS</a>
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Implements a set of utilities common to many event operations.
 */

#include "eformat/util.h"
#include "eformat/ROBFragment.h"
#include "eformat/ROSFragment.h"
#include "eformat/SubDetectorFragment.h"
#include "eformat/FullEventFragment.h"
#include "ers/StreamFactory.h"
#include "eformat/WrongMarkerIssue.h"

uint32_t* eformat::next_fragment (std::fstream& fs, uint32_t* addr,
				  size_t size)
{
  using namespace eformat;

  off_t offset = fs.tellg();
  ERS_DEBUG_3("Current stream position is 0x%lx", offset);

  uint32_t data[2];
  if (fs && fs.good() && ! fs.eof()) {
    //soft check, so we don't make mistakes
    fs.read((char*)data, 8);
    if (!fs.good() || fs.eof()) return 0; // end-of-file 
    switch((HeaderMarker)data[0]) {
    case FULL_EVENT:
    case SUB_DETECTOR:
    case ROS:
    case ROB:
    case ROD:
      break;
    default:
      //stop!
      std::cout << "Word at offset " << HEX(offset) << " is not one of "
		<< HEX(FULL_EVENT) << ", "
		<< HEX(SUB_DETECTOR) << ", "
		<< HEX(ROS) << ", "
		<< HEX(ROB) << "or "
		<< HEX(ROS) << ". Stopping execution..." << std::endl;
      return 0;
    }
  }
  else return 0; //file is over

  //data[1] is the fragment size, in words. Take it and read the fragment
  ERS_DEBUG_3("Resetting stream position to 0x%lx...", offset);
  fs.seekg(offset);
  if (addr && (size >= (data[1]*4))) {
    //space is preallocated and checked
    ERS_DEBUG_3("Reading fragment data...");
    fs.read((char*)addr, data[1]*4);
    ERS_DEBUG_2("Size of fragment readout is %u bytes", 4*data[1]);
    ERS_DEBUG_3("Stream position is 0x%lx", offset = fs.tellg());
    return addr;
  }
  else if (addr) {
    std::cout << "The fragment at offset " << HEX(offset) << " has "
	      << data[1]*4 << " bytes and you provided only "
	      << size << " bytes in your buffer. Stopping execution..."
	      << std::endl;
    return 0;
  }

  //if I get here, is because I have to allocate space myself
  ERS_DEBUG_3("Allocating fragment data storage of size %ud bytes", 4*data[1]);
  uint32_t* retval = new uint32_t[data[1]];
  ERS_DEBUG_3("Reading fragment data...");
  fs.read((char*)retval, data[1]*4);
  ERS_DEBUG_2("Size of fragment readout is %u bytes", 4*data[1]);
  ERS_DEBUG_3("Stream position is 0x%lx", offset = fs.tellg());
  return retval;
}

size_t eformat::find_rods (const uint32_t* block_start, size_t block_size,
			   const uint32_t** rod, uint32_t* rod_size,
			   size_t max_count)
{
  const uint32_t* block_end = block_start + block_size;
  size_t counter = 0;
  while (block_end > block_start) {
    uint32_t curr_rod_size = 12; //< base size for a ROD as eformat 2.4
    curr_rod_size += *(block_end-3) + *(block_end-2); //status and data sizes
    block_end -= curr_rod_size;
    if (rod && counter < max_count) {
      if (block_end[0] != eformat::ROD) 
	throw EFORMAT_WRONG_MARKER(block_end[0], eformat::ROD);
      rod_size[counter] = curr_rod_size;
      rod[counter] = block_end;
    }
    ++counter;
  }
  return counter;
}

size_t eformat::get_robs (const uint32_t* fragment, const uint32_t** rob,
			  size_t max_count)
{
  using namespace eformat;
  ERS_DEBUG_1("Getting all ROB's from 0x%x...", fragment[0]);

  size_t counter = 0;
  switch ((eformat::HeaderMarker)(fragment[0])) {
  case ROD:
    return 0;
  case ROB:
    {
      if ( max_count > 0 ) {
	rob[0] = fragment;
	++counter;
      }
      else return 0; //not enough space
    }
    break;
  case ROS:
    {
      eformat::ROSFragment<const uint32_t*> ros(fragment);
      counter += ros.children(rob, max_count);
      ERS_DEBUG_1("ROS 0x%x contains %d ROB's", ros.source_id(), counter);
    }
    break;
  case SUB_DETECTOR:
    {
      eformat::SubDetectorFragment<const uint32_t*> sd(fragment);
      const uint32_t* ros[256];
      size_t ros_counter = sd.children(ros, 256);
      for (size_t i=0; i<ros_counter; ++i)
	counter += get_robs(ros[i], &rob[counter], max_count - counter);
      ERS_DEBUG_1("Subdetector 0x%x contains %d ROB's", sd.source_id(), 
		  counter);
    }
    break;
  case FULL_EVENT:
    {
      eformat::FullEventFragment<const uint32_t*> fe(fragment);
      const uint32_t* sd[64];
      size_t sd_counter = fe.children(sd, 64);
      for (size_t i=0; i<sd_counter; ++i)
	counter += get_robs(sd[i], &rob[counter], max_count - counter);
      ERS_DEBUG_1("Fullevent 0x%x contains %d ROB's", fe.source_id(), counter);
    }
    break;
  }

  return counter;
}
