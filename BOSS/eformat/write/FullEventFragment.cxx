//Dear emacs, this is -*- c++ -*-

/**
 * @file SubDetectorFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the SubDetector fragment writing helper class
 */

#include "eformat/write/SubDetectorFragment.h"
#include "eformat/write/FullEventFragment.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Status.h"
#include "ers/StreamFactory.h"
#include <ctime>

eformat::write::FullEventFragment::FullEventFragment
(uint32_t source_id, uint32_t use_time,
 uint32_t global_id, uint32_t run_no,
 uint32_t lvl1_id, uint32_t lvl1_type,
 uint32_t lvl2_info, const uint32_t* ef_info)
  : m_child(0),
    m_last(0),
    m_extra_count(0)
{
  m_header[0] = eformat::FULL_EVENT; //marker
  m_header[1] = 18; //this header size + status size
  m_header[2] = 18; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = source_id;
  m_header[5] = 1; //number of status
  m_header[6] = 10; //number of fragment specific
  m_header[7] = use_time;
  m_header[8] = global_id;
  m_header[9] = run_no;
  m_header[10] = lvl1_id;
  m_header[11] = lvl1_type;
  m_header[12] = lvl2_info;
  memcpy(&m_header[13], ef_info, 4*sizeof(uint32_t));

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 11, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d", 
	      "Built (write) full event from scratch, with",
	      FullEventFragment::source_id(), FullEventFragment::lvl1_id(), 
	      FullEventFragment::run_no());
}

eformat::write::FullEventFragment::FullEventFragment
(uint32_t source_id, uint32_t global_id, uint32_t run_no,
 uint32_t lvl1_id, uint32_t lvl1_type, 
 uint32_t lvl2_info, const uint32_t* ef_info)
  : m_child(0),
    m_last(0),
    m_extra_count(0)
{
  m_header[0] = eformat::FULL_EVENT; //marker
  m_header[1] = 18; //this header size + status size
  m_header[2] = 18; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = source_id;
  m_header[5] = 1; //number of status
  m_header[6] = 10; //number of fragment specific
  m_header[7] = std::time(0);
  m_header[8] = global_id;
  m_header[9] = run_no;
  m_header[10] = lvl1_id;
  m_header[11] = lvl1_type;
  m_header[12] = lvl2_info;
  memcpy(&m_header[13], ef_info, 4*sizeof(uint32_t));

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 11, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d", 
	      "Built (write) full event from scratch, with",
	      FullEventFragment::source_id(), FullEventFragment::lvl1_id(), 
	      FullEventFragment::run_no());
}

eformat::write::FullEventFragment::FullEventFragment (uint32_t* fe)
  : m_child(0),
    m_last(0),
    m_extra_count(0)
{
  //now initialize pages
  set(m_node[0], fe, 6, &m_node[1]);
  set(m_node[1], &fe[6], fe[5], &m_node[2]);
  set(m_node[2], &fe[6+fe[5]], 11, 0);
  eformat::write::set(m_extra, &fe[17+fe[5]], fe[1]-fe[2], 0);
  ++m_extra_count;
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d", 
	      "Built (write) full event from contiguos memory, with",
	      source_id(), lvl1_id(), run_no());
}

eformat::write::FullEventFragment::FullEventFragment
(eformat::write::node_t* fe)
  : m_child(0),
    m_last(0),
    m_extra_count(0)
{
  //now initialize pages
  set(m_node[0], fe->base, 6, &m_node[1]);
  set(m_node[1], &fe->base[6], fe->base[5], &m_node[2]);
  set(m_node[2], &fe->base[6+fe->base[5]], 11, 0);
  eformat::write::set(m_extra, &fe->base[17+fe->base[5]],
		      fe->size_word - fe->base[2], fe->next);
  m_extra_count += eformat::write::count(m_extra);
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d",
	      "Built (write) full event from paged memory, with",
	      source_id(), lvl1_id(), run_no());
}

eformat::write::FullEventFragment::FullEventFragment ()
  : m_child(0),
    m_last(0),
    m_extra_count(0)
{
  m_header[0] = eformat::FULL_EVENT; //marker
  m_header[1] = 18; //this header size + status size
  m_header[2] = 18; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = 0; //source identifier
  m_header[5] = 1; //number of status
  m_header[6] = 10; //number of fragment specific
  m_header[7] = std::time(0);
  m_header[8] = 0; //global identifier
  m_header[9] = 0; //run number
  m_header[10] = 0; //LVL1 identifier
  m_header[11] = 0; //LVL1 type
  m_header[12] = 0; //LVL2 info
  m_header[13] = 0; //EF info [0]
  m_header[14] = 0; //EF info [1]
  m_header[15] = 0; //EF info [2]
  m_header[16] = 0; //EF info [3]

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 11, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d", 
	      "Built empty (write) full event from scratch, with",
	      FullEventFragment::source_id(), FullEventFragment::lvl1_id(), 
	      FullEventFragment::run_no());
}

eformat::write::FullEventFragment::FullEventFragment 
(const eformat::write::FullEventFragment& other)
  : m_child(0),
    m_last(0),
    m_extra_count(0)
{
  *this = other;
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d", 
	      "Built (write) full event from copy, with",
	      FullEventFragment::source_id(), FullEventFragment::lvl1_id(), 
	      FullEventFragment::run_no());
}

eformat::write::FullEventFragment& 
eformat::write::FullEventFragment::operator= 
  (const eformat::write::FullEventFragment& other)
{
  memcpy(reinterpret_cast<void*>(m_header),
	 reinterpret_cast<const void*>(other.m_node[0].base), 
	 6*sizeof(uint32_t));
  memcpy(reinterpret_cast<void*>(m_header+6),
	 reinterpret_cast<const void*>(other.m_node[2].base),
	 11*sizeof(uint32_t));
  
  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], other.m_node[1].base, other.m_node[1].size_word, &m_node[2]);
  set(m_node[2], &m_header[6], 11, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x, LVL1 Id. = %d and Run Number = %d", 
	      "Copied (write) full event, with",
	      FullEventFragment::source_id(), FullEventFragment::lvl1_id(), 
	      FullEventFragment::run_no());
  return *this;
}

void eformat::write::FullEventFragment::status 
(uint32_t n, const uint32_t* status)
{ 
  m_node[0].base[1] -= m_node[0].base[5]; //remove count from previous status
  m_node[0].base[2] -= m_node[0].base[5]; //remove count from previous status
  m_node[1].size_word = m_node[0].base[5] = n; //set new values
  m_node[0].base[1] += n;
  m_node[0].base[2] += n;
  m_node[1].base = const_cast<uint32_t*>(status);
}

void eformat::write::FullEventFragment::append 
(eformat::write::SubDetectorFragment* sd)
{
  ERS_DEBUG_3("%s Source Id. = 0x%x to full event with Source Id. = 0x%x", 
	      "Appending subdetector with",
	      sd->source_id(), source_id());
  sd->parent(this);
  m_node[0].base[1] += sd->size_word();

  //adjust `m_last' and `m_child' to point to the new last ROB
  if (m_last) m_last->next(sd);
  else m_child = sd;
  m_last = sd;
}

uint32_t eformat::write::FullEventFragment::page_count (void) const
{
  uint32_t retval = 3 + m_extra_count;
  for (const SubDetectorFragment* curr = m_child; curr; curr = curr->next())
    retval += curr->page_count();
  return retval;
}

const eformat::write::node_t* 
eformat::write::FullEventFragment::bind (void)
{
  //the header is already concatenated by construction
  eformat::write::node_t* last = &m_node[2];
  if (m_extra_count) {
    last = &m_extra;
    while (last->next) last = last->next;
  }
  for (SubDetectorFragment* curr = m_child; curr; 
       curr = const_cast<SubDetectorFragment*>(curr->next())) {
    last->next = const_cast<eformat::write::node_t*>(curr->bind());
    while (last->next) last = last->next; //advance until end
  }
  return m_node;
}
