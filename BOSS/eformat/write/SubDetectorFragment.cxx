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

eformat::write::SubDetectorFragment::SubDetectorFragment (uint32_t source_id)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  m_header[0] = eformat::SUB_DETECTOR; //marker
  m_header[1] = 8; //this header size + status size
  m_header[2] = 8; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = source_id;
  m_header[5] = 1; //number of status
  m_header[6] = 0; //number of fragment specific

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 1, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x.", 
	      "Built (write) subdetector from scratch, with", 
	      SubDetectorFragment::source_id());
}

eformat::write::SubDetectorFragment::SubDetectorFragment ()
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  m_header[0] = eformat::SUB_DETECTOR; //marker
  m_header[1] = 8; //this header size + status size
  m_header[2] = 8; //this header size + status size
  m_header[3] = eformat::DEFAULT_VERSION; //format version
  m_header[4] = 0; //source identifier
  m_header[5] = 1; //number of status
  m_header[6] = 0; //number of fragment specific

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 1, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x.", 
	      "Built empty (write) subdetector from scratch, with", 
	      SubDetectorFragment::source_id());
}

eformat::write::SubDetectorFragment::SubDetectorFragment (uint32_t* sd)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  //now initialize pages
  set(m_node[0], sd, 6, &m_node[1]);
  set(m_node[1], &sd[6], sd[5], &m_node[2]);
  set(m_node[2], &sd[6+sd[5]], 1, &m_extra);
  eformat::write::set(m_extra, &sd[7+sd[5]], sd[1]-sd[2], 0);
  ++m_extra_count;
  ERS_DEBUG_3("%s Source Id. = 0x%x.", 
	      "Built (write) subdetector from contiguous memory, with",
	      source_id());
}

eformat::write::SubDetectorFragment::SubDetectorFragment
(eformat::write::node_t* sd)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  //now initialize pages
  set(m_node[0], sd->base, 6, &m_node[1]);
  set(m_node[1], &sd->base[6], sd->base[5], &m_node[2]);
  set(m_node[2], &sd->base[6+sd->base[5]], 1, &m_extra);
  eformat::write::set(m_extra, &sd->base[7+sd->base[5]],
		      sd->size_word - sd->base[2], sd->next);
  m_extra_count += eformat::write::count(m_extra);
  ERS_DEBUG_3("%s Source Id. = 0x%x.", 
	      "Built (write) subdetector from paged memory, with", 
	      source_id());
}

eformat::write::SubDetectorFragment::SubDetectorFragment
(const eformat::write::SubDetectorFragment& other)
  : m_parent(0),
    m_child(0),
    m_last(0),
    m_next(0),
    m_extra_count(0)
{
  *this = other;
  ERS_DEBUG_3("%s Source Id. = 0x%x.", 
	      "Built (write) subdetector from copy, with", 
	      source_id());
}

eformat::write::SubDetectorFragment& 
eformat::write::SubDetectorFragment::operator= 
  (const eformat::write::SubDetectorFragment& other)
{
  memcpy(reinterpret_cast<void*>(m_header),
	 reinterpret_cast<const void*>(other.m_node[0].base), 
	 6*sizeof(uint32_t));
  m_header[6] = other.m_node[2].base[0];

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], other.m_node[1].base, other.m_node[1].size_word, &m_node[2]);
  set(m_node[2], &m_header[6], 1, 0);
  ERS_DEBUG_3("%s Source Id. = 0x%x.",
	      "Copied (write) subdetector with",
	      SubDetectorFragment::source_id());
  return *this;
}

void eformat::write::SubDetectorFragment::status 
(uint32_t n, const uint32_t* status)
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

void eformat::write::SubDetectorFragment::size_change (uint32_t o, uint32_t n)
{ 
  uint32_t old_size = m_node[0].base[1];
  m_node[0].base[1] -= o;
  m_node[0].base[1] += n;
  if (m_parent) m_parent->size_change(old_size, m_node[0].base[1]);
}

void eformat::write::SubDetectorFragment::append 
(eformat::write::ROSFragment* ros)
{
  ERS_DEBUG_3("%s Source Id. = 0x%x to subdetector with Source Id. = 0x%x", 
	      "Appending ros fragment with",
	      ros->source_id(), source_id());
  ros->parent(this);
  uint32_t old_size = m_node[0].base[1];
  m_node[0].base[1] += ros->size_word();

  //adjust `m_last' and `m_child' to point to the new last ROB
  if (m_last) m_last->next(ros);
  else m_child = ros;
  m_last = ros;

  //propagate changes to my parent
  if (m_parent) m_parent->size_change(old_size, m_node[0].base[1]);
}

uint32_t eformat::write::SubDetectorFragment::page_count (void) const
{
  uint32_t retval = 3 + m_extra_count;
  for (const ROSFragment* curr = m_child; curr; curr = curr->next())
    retval += curr->page_count();
  return retval;
}

const eformat::write::node_t* 
eformat::write::SubDetectorFragment::bind (void)
{
  //the header is already concatenated by construction
  eformat::write::node_t* last = &m_node[2];
  if (m_extra_count) {
    last = &m_extra;
    while (last->next) last = last->next;
  }
  for (ROSFragment* curr = m_child; curr; 
       curr = const_cast<ROSFragment*>(curr->next())) {
    last->next = const_cast<eformat::write::node_t*>(curr->bind());
    while (last->next) last = last->next; //advance until end
  }
  return m_node;
}
