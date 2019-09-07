//Dear emacs, this is -*- c++ -*-

/**
 * @file ROSFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/02/20 23:38:21 $
 *
 * Implements the ROS fragment writing helper class
 */

#include "eformat/write/ROSFragment.h"
#include "eformat/write/SubDetectorFragment.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Status.h"
#include "ers/StreamFactory.h"
#include <cstring>
eformat::write::ROSFragment::ROSFragment 
(uint32_t source_id, uint32_t run_no, uint32_t lvl1_id, uint32_t bc_id)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  m_header[0] = eformat::ROS; //marker
  m_header[1] = 11; //this header size + status size
  m_header[2] = 11; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = source_id;
  m_header[5] = 1; //number of status
  m_header[6] = 3; //number of fragment specific
  m_header[7] = run_no;
  m_header[8] = lvl1_id;
  m_header[9] = bc_id;

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 4, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) ros fragment from scratch, with", 
	      ROSFragment::source_id(), ROSFragment::lvl1_id(), 
	      ROSFragment::run_no());
}

eformat::write::ROSFragment::ROSFragment (uint32_t* ros)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  //now initialize pages
  set(m_node[0], ros, 6, &m_node[1]);
  set(m_node[1], &ros[6], ros[5], &m_node[2]);
  set(m_node[2], &ros[6+ros[5]], 4, &m_extra);
  eformat::write::set(m_extra, &ros[10+ros[5]], ros[1]-ros[2]);
  ++m_extra_count;
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) ros fragment from contiguous memory, with", 
	      source_id(), lvl1_id(), run_no());
}

eformat::write::ROSFragment::ROSFragment (eformat::write::node_t* ros)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  //now initialize pages
  set(m_node[0], ros->base, 6, &m_node[1]);
  set(m_node[1], &ros->base[6], ros->base[5], &m_node[2]);
  set(m_node[2], &ros->base[6+ros->base[5]], 4, &m_extra);
  eformat::write::set(m_extra, &ros->base[10+ros->base[5]],
		      ros->size_word - ros->base[2], ros->next);
  m_extra_count += eformat::write::count(m_extra);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) ros fragment from paged memory, with", 
	      source_id(), lvl1_id(), run_no());
}

eformat::write::ROSFragment::ROSFragment ()
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  m_header[0] = eformat::ROS; //marker
  m_header[1] = 11; //this header size + status size
  m_header[2] = 11; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = 0; //source identifier
  m_header[5] = 1; //number of status
  m_header[6] = 3; //number of fragment specific
  m_header[7] = 0; //run number
  m_header[8] = 0; //LVL1 identifier
  m_header[9] = 0; //bunch crossing identifier

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 4, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built empty (write) ros fragment from scratch, with", 
	      ROSFragment::source_id(), ROSFragment::lvl1_id(), 
	      ROSFragment::run_no());
}

eformat::write::ROSFragment::ROSFragment 
(const eformat::write::ROSFragment& other)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  *this = other;
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Built (write) ros fragment from copy, with", 
	      ROSFragment::source_id(), ROSFragment::lvl1_id(),
	      ROSFragment::run_no());
}

eformat::write::ROSFragment& eformat::write::ROSFragment::operator= 
  (const eformat::write::ROSFragment& other)
{
  memcpy(reinterpret_cast<void*>(m_header),
	 reinterpret_cast<const void*>(other.m_node[0].base), 
	 6*sizeof(uint32_t));
  memcpy(reinterpret_cast<void*>(m_header+6),
	 reinterpret_cast<const void*>(other.m_node[2].base),
	 4*sizeof(uint32_t));

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], other.m_node[1].base, other.m_node[1].size_word, &m_node[2]);
  set(m_node[2], &m_header[6], 4, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x., LVL1 Id. = %d, Run Number = %d", 
	      "Copied (write) ros fragment, with", 
	      ROSFragment::source_id(), ROSFragment::lvl1_id(),
	      ROSFragment::run_no());
  return *this;
}

void eformat::write::ROSFragment::status (uint32_t n, const uint32_t* status)
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

void eformat::write::ROSFragment::size_change (uint32_t o, uint32_t n)
{ 
  uint32_t old_size = m_node[0].base[1];
  m_node[0].base[1] -= o;
  m_node[0].base[1] += n;
  if (m_parent) m_parent->size_change(old_size, m_node[0].base[1]);
}

void eformat::write::ROSFragment::append (eformat::write::ROBFragment* rob)
{
  ERS_DEBUG_3("%s Source Id. = 0x%x to ros fragment with Source Id. = 0x%x", 
	      "Appending rob fragment with",
	      rob->source_id(), source_id());
  rob->parent(this);
  uint32_t old_size = m_node[0].base[1];
  m_node[0].base[1] += rob->size_word();

  //adjust `m_last' and `m_child' to point to the new last ROB
  if (m_last) m_last->next(rob);
  else m_child = rob;
  m_last = rob;

  //propagate changes to my parent
  if (m_parent) m_parent->size_change(old_size, m_node[0].base[1]);
}

uint32_t eformat::write::ROSFragment::page_count (void) const
{
  uint32_t retval = 3 + m_extra_count;
  for (const ROBFragment* curr = m_child; curr; curr = curr->next())
    retval += curr->page_count();
  return retval;
}

const eformat::write::node_t* eformat::write::ROSFragment::bind (void)
{
  //the header is already concatenated by construction
  eformat::write::node_t* last = &m_node[2];
  if (m_extra_count) {
    last = &m_extra;
    while (last->next) last = last->next;
  }
  for (const ROBFragment* curr = m_child; curr; curr = curr->next()) {
    last->next = const_cast<eformat::write::node_t*>(curr->bind());
    while (last->next) last = last->next; //advance until end
  }
  return m_node;
}
