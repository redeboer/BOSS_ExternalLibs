//Dear emacs, this is -*- c++ -*-

/**
 * @file ROBFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements the ROB fragment writing helper class
 */

#include "eformat/write/ROBFragment.h"
#include "eformat/write/ROSFragment.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Status.h"
#include <cstring>
#include "ers/StreamFactory.h"

eformat::write::ROBFragment::ROBFragment 
(uint32_t source_id, 
 uint32_t run_no, 
 uint32_t lvl1_id,
 uint32_t bc_id, 
 uint32_t lvl1_type, 
 uint32_t detev_type,
 uint32_t ndata, const uint32_t* data, uint32_t status_position)
  : m_parent(0),
    m_next(0)
{
  m_header[0] = eformat::ROB; //marker
  m_header[1] = 21 + ndata; //total fragment size in words
  m_header[2] = 8; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = source_id;
  m_header[5] = 1; //number of status
  m_header[6] = 0; //number of fragment specific
  m_rod_header[0] = eformat::ROD; //ROD marker
  m_rod_header[1] = 9; //ROD header size
  m_rod_header[2] = eformat::DEFAULT_VERSION; //format version
  m_rod_header[3] = source_id;
  m_rod_header[4] = run_no;
  m_rod_header[5] = lvl1_id;
  m_rod_header[6] = bc_id;
  m_rod_header[7] = lvl1_type;
  m_rod_header[8] = detev_type;
  m_rod_trailer[0] = 1; //number of status in the ROD
  m_rod_trailer[1] = ndata; //number of data words in the ROD
  m_rod_trailer[2] = status_position;

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 1, &m_node[3]); //specific part
  if (m_rod_trailer[2] == eformat::STATUS_FRONT) {
    set(m_node[3], m_rod_header, 9, &m_node[4]);
    set(m_node[4], &eformat::DEFAULT_STATUS, 1, &m_node[5]); //status
    set(m_node[5], data, ndata, &m_node[6]); //data
  }
  else {
    set(m_node[3], m_rod_header, 9, &m_node[5]);
    set(m_node[5], data, ndata, &m_node[4]); //data
    set(m_node[4], &eformat::DEFAULT_STATUS, 1, &m_node[6]); //status
  }
  set(m_node[6], m_rod_trailer, 3, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) rob/rod fragment from scratch, with",
	      ROBFragment::source_id(), rod_lvl1_id(), rod_run_no());
}

eformat::write::ROBFragment::ROBFragment ()
  : m_parent(0),
    m_next(0)
{
  m_header[0] = eformat::ROB; //marker
  m_header[1] = 21;
  m_header[2] = 8; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = 0; //source identifier of the ROB fragment
  m_header[5] = 1; //number of status
  m_header[6] = 0; //number of fragment specific
  m_rod_header[0] = eformat::ROD; //ROD marker
  m_rod_header[1] = 9; //ROD header size
  m_rod_header[2] = eformat::DEFAULT_VERSION; //format version
  m_rod_header[3] = 0; //source identifier of the ROD fragment
  m_rod_header[4] = 0; //run number
  m_rod_header[5] = 0; //LVL1 identifier
  m_rod_header[6] = 0; //bunch crossing identifier
  m_rod_header[7] = 0; //LVL1 type
  m_rod_header[8] = 0; //detector event type
  m_rod_trailer[0] = 1; //number of status in the ROD
  m_rod_trailer[1] = 0; //number of data words in the ROD
  m_rod_trailer[2] = eformat::STATUS_FRONT; //status block position

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 1, &m_node[3]); //specific part
  if (m_rod_trailer[2] == eformat::STATUS_FRONT) {
    set(m_node[3], m_rod_header, 9, &m_node[4]);
    set(m_node[4], &eformat::DEFAULT_STATUS, 1, &m_node[5]); //status
    set(m_node[5], 0, 0, &m_node[6]); //data
  }
  else {
    set(m_node[3], m_rod_header, 9, &m_node[5]);
    set(m_node[5], 0, 0, &m_node[4]); //data
    set(m_node[4], &eformat::DEFAULT_STATUS, 1, &m_node[6]); //status
  }
  set(m_node[6], m_rod_trailer, 3, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d",
	      "Built (write) empty rob/rod fragment, with",
	      ROBFragment::source_id(), rod_lvl1_id(), rod_run_no());
}

eformat::write::ROBFragment::ROBFragment 
(const eformat::write::ROBFragment& other)
  : m_parent(0),
    m_next(0)
{
  *this = other;
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built new (write) rob/rod fragment from copy, with",
	      ROBFragment::source_id(), rod_lvl1_id(), rod_run_no());
}

eformat::write::ROBFragment::ROBFragment (uint32_t* rod, uint32_t size_word)
  : m_parent(0),
    m_next(0)
{
  m_header[0] = eformat::ROB; //marker
  m_header[1] = 8 + size_word; //total fragment size in words
  m_header[2] = 8; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = rod[3];
  m_header[5] = 1; //number of status
  m_header[6] = 0; //number of fragment specific
  
  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 1, &m_node[3]);
  set(m_node[6], &rod[size_word-3], 3, 0); //ROD trailer
  if (m_node[6].base[2] == eformat::STATUS_FRONT) {
    set(m_node[3], rod, 9, &m_node[4]); //ROD header
    set(m_node[4], &rod[9], m_node[6].base[0], &m_node[5]); //status
    set(m_node[5], &rod[9+m_node[6].base[0]], m_node[6].base[1], &m_node[6]);
  }
  else {
    set(m_node[3], rod, 9, &m_node[5]); //ROD header
    set(m_node[4], &rod[9+m_node[6].base[1]], m_node[6].base[0], &m_node[6]);
    set(m_node[5], &rod[9], m_node[6].base[1], &m_node[4]); //data
  }
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) rob/rod fragment from rod on memory, with",
	      source_id(), rod_lvl1_id(), rod_run_no());
}

eformat::write::ROBFragment::ROBFragment (uint32_t* rob)
  : m_parent(0),
    m_next(0)
{
  //now initialize pages
  set(m_node[0], rob, 6, &m_node[1]);
  set(m_node[1], &rob[6], rob[5], &m_node[2]);
  set(m_node[2], &rob[6+rob[5]], 1, &m_node[3]);
  set(m_node[6], &rob[rob[1]-3], 3, 0); //ROD trailer
  if (m_node[6].base[2] == eformat::STATUS_FRONT) {
    set(m_node[3], &rob[rob[2]], 9, &m_node[4]); //ROD header
    set(m_node[4], &rob[rob[2]+9], m_node[6].base[0], &m_node[5]); //status
    set(m_node[5], &rob[rob[2]+9+m_node[6].base[0]], m_node[6].base[1], 
	&m_node[6]); //data
  }
  else {
    set(m_node[3], &rob[rob[2]], 9, &m_node[5]); //ROD header
    set(m_node[4], &rob[rob[2]+9+m_node[6].base[1]], m_node[6].base[0], 
	&m_node[6]);
    set(m_node[5], &rob[rob[2]+9], m_node[6].base[1], &m_node[4]); //data
  }
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) rob/rod fragment from cont. memory, with",
	      source_id(), rod_lvl1_id(), rod_run_no());
}

eformat::write::ROBFragment& eformat::write::ROBFragment::operator= 
  (const eformat::write::ROBFragment& other)
{
  //get the ROB header
  memcpy(reinterpret_cast<void*>(m_header),
	 reinterpret_cast<const void*>(other.m_node[0].base), 
	 6*sizeof(uint32_t));
  m_header[6] = other.m_node[2].base[0];
  memcpy(reinterpret_cast<void*>(m_rod_header),
	 reinterpret_cast<const void*>(other.m_node[3].base),
	 9*sizeof(uint32_t));
  memcpy(reinterpret_cast<void*>(m_rod_trailer), 
	 reinterpret_cast<const void*>(other.m_node[6].base),
	 3*sizeof(uint32_t));

  //now re-initialize the pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], other.m_node[1].base, other.m_node[1].size_word, &m_node[2]);
  set(m_node[2], &m_header[6], 1, &m_node[3]); //specific part
  if (m_rod_trailer[2] == eformat::STATUS_FRONT) {
    set(m_node[3], m_rod_header, 9, &m_node[4]);
    set(m_node[4], other.m_node[4].base, 
	other.m_node[4].size_word, &m_node[5]); //status
    set(m_node[5], other.m_node[5].base, 
	other.m_node[5].size_word, &m_node[6]); //data
  }
  else {
    set(m_node[3], m_rod_header, 9, &m_node[5]);
    set(m_node[5], other.m_node[5].base, 
	other.m_node[5].size_word, &m_node[4]); //data
    set(m_node[4], other.m_node[4].base, 
	other.m_node[4].size_word, &m_node[6]); //status
  }
  set(m_node[6], m_rod_trailer, 3, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Copied (write) rob/rod fragment with",
	      ROBFragment::source_id(), rod_lvl1_id(), rod_run_no());

  return *this;
}

void eformat::write::ROBFragment::status (uint32_t n, const uint32_t* status)
{ 
  if (m_parent) m_parent->size_change(m_node[0].base[1],
				      m_node[0].base[1]-m_node[0].base[5]+n);
  m_node[0].base[1] -= m_node[0].base[5]; //remove count from previous status
  m_node[0].base[2] -= m_node[0].base[5]; //remove count from previous status
  m_node[1].size_word = m_node[0].base[5] = n; //set new values
  m_node[0].base[1] += n;
  m_node[0].base[2] += n;
  m_node[1].base = const_cast<uint32_t*>(status);
}

void eformat::write::ROBFragment::rod_status (uint32_t n, 
					      const uint32_t* status)
{ 
  if (m_parent) m_parent->size_change(m_node[0].base[1], 
				      m_node[0].base[1]-m_node[6].base[0]+n);
  m_node[0].base[1] -= m_node[6].base[0]; //remove count from previous status
  m_node[4].size_word = m_node[6].base[0] = n; //set new values
  m_node[0].base[1] += n; //set ROB header's total fragment size
  m_node[4].base = const_cast<uint32_t*>(status); 
}

void eformat::write::ROBFragment::status_position (uint32_t v)
{ 
  if (v == m_node[6].base[2]) return; //do nothing in this case:)
  m_node[6].base[2] = v;
  if (m_node[6].base[2] == eformat::STATUS_FRONT) {
    m_node[3].next = &m_node[4];
    m_node[4].next = &m_node[5];
    m_node[5].next = &m_node[6];
  }
  else {
    m_node[3].next = &m_node[5];
    m_node[5].next = &m_node[4];
    m_node[4].next = &m_node[6];
  }
}

void eformat::write::ROBFragment::rod_data (uint32_t n, const uint32_t* data)
{
  if (m_parent) m_parent->size_change(m_node[0].base[1], 
				      m_node[0].base[1]-m_node[6].base[1]+n);

  //remove count from previous data size
  m_node[0].base[1] -= m_node[6].base[1]; 
  m_node[5].size_word = m_node[6].base[1] = n; //set new values
  m_node[0].base[1] += n; //set ROB header's total fragment size back
  m_node[5].base = const_cast<uint32_t*>(data); 
}
