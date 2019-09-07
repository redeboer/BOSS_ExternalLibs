//Dear emacs, this is -*- c++ -*-

/**
 * @file node.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Implements IOV node functionality
 */

#include "eformat/write/node.h"
#include <cstring>

void eformat::write::set (node_t& i, const uint32_t* b, size_t l, node_t* n)
{ 
  i.base = const_cast<uint32_t*>(b); 
  i.size_word = l; 
  i.next = n;
}

void eformat::write::set (node_t& i, const struct iovec& v, node_t* n)
{ 
  i.base = reinterpret_cast<uint32_t*>(v.iov_base);
  i.size_word = v.iov_len/4; 
  i.next = n;
}

void eformat::write::cat (node_t* n, const struct iovec* v, uint32_t count)
{
  for (size_t i=0; i<(count-1); ++i) set(n[i], v[i], &n[i+1]);
  set(n[count-1], v[count-1], 0);
}

void eformat::write::cat (node_t* n, uint32_t count)
{
  for (size_t i=0; i<(count-1); ++i) n[i].next = &n[i+1];
  n[count-1].next = 0;
}

uint32_t eformat::write::count (const node_t& list)
{
  const eformat::write::node_t* current = &list;
  uint32_t retval = 0;
  while (current) {
    ++retval;
    current = current->next;
  }
  return retval;
}

uint32_t eformat::write::count_words (const node_t& list)
{
  const eformat::write::node_t* current = &list;
  uint32_t retval = 0;
  while (current) {
    retval += current->size_word;
    current = current->next;
  }
  return retval;
}

uint32_t eformat::write::copy (const node_t& list, uint32_t* dest, 
			       size_t max)
{
  const eformat::write::node_t* current = &list;
  uint32_t cpos = 0;
  while (current) {
    if (cpos + current->size_word > max) return 0;
    if (current->size_word > 0 && current->base) {
      memcpy(&dest[cpos], current->base, sizeof(uint32_t)*current->size_word);
      cpos += current->size_word;
    }
    current = current->next;
  }
  return cpos;
}

uint32_t eformat::write::shallow_copy (const node_t& list, struct iovec* dest,
				       uint32_t max)
{
  const eformat::write::node_t* current = &list;
  uint32_t cpos = 0;
  while (current) {
    if (cpos > max) return 0;
    if (current->size_word > 0 && current->base) {
      dest[cpos].iov_base = 
	reinterpret_cast<void*>(const_cast<uint32_t*>(current->base));
      dest[cpos].iov_len = sizeof(uint32_t)*current->size_word;
      ++cpos;
    }
    current = current->next;
  }
  return cpos;
}

