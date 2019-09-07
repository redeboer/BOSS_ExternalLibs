//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/PagedMemory.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: maqm $
 * $Revision: 1.2 $
 * $Date: 2011/03/09 23:25:55 $
 *
 * @brief Represents and facilitates access to a linked list of eformat::node's
 */

#ifndef EFORMAT_PAGEDMEMORY_H
#define EFORMAT_PAGEDMEMORY_H

#include <stdint.h>
#include <sys/uio.h>
#include "eformat/util.h"
#include "eformat/TooBigCountIssue.h"
#include "eformat/NotAlignedIssue.h"
#include "eformat/OutOfBoundsIssue.h"

/**
 * Defines how to cast to 32-bit unsigned integers
 */
#define CAST32(x) reinterpret_cast<const uint32_t*>(x)

namespace eformat {

  /**
   * A list of nodes. To gain speed, the PagedMemory uses an internal page
   * index. The default maximum size of this index is 128. The user can
   * instanciate bigger PagedMemory objects by increasing the default template
   * parameter size to a more suitable value.
   *
   * @warn <b>Attention:</b> PagedMemory's with different index sizes cannot
   * interact directly.
   */
  template <unsigned int TMAXPAGES=128> class PagedMemory {

  public: //export template symbol

    static const unsigned int MAXPAGES;

  public: //interface

    /**
     * Simpler constructor: starts from an existing set of base addresses and
     * sizes. Please note that for reasons of simplicity, the data in all
     * memory blocks have to be 32-bit aligned. Otherwise, the implementation
     * should be DC::Buffer, where this has all been thought about.
     *
     * Also note that this class is intended for reading data and optimized for
     * such. If you just want to manipulate fragments you are better doing it
     * your self or using the writing part of the library.
     *
     * @param pages The memory area I would like to read data from
     * @param count The number of pages pointed by the variable pages. This
     * has to be smaller than TMAXPAGES.
     */
    PagedMemory (const struct iovec* pages, size_t count);

    /**
     * Returns the base address of the input IO vector
     */
    inline const struct iovec* pages (void) const { return m_iovec; }

    /**
     * Returns the number of blocks in this IO vector
     */
    inline size_t count (void) const { return m_count; }

    /**
     * Returns the size in bytes words for this PagedMemory.
     */
    inline size_t size (void) const { return m_size; }

  public: //indexing

    /**
     * Define a private type that is used to index the access to this type of
     * eformat::Memory. The performance of this iterator is dependent on the
     * distance from the current offset and on the page sizes. Try to use the
     * iterator for small scale reach and readjust if necessary.
     */
    typedef struct node_t {
      const struct iovec* page; //a pointer to the page
      size_t start; //the cumulative, logical start of this page in words
      size_t end; //the cumulative, logical end of this page in words
      node_t* previous; //the previos page
      node_t* next; //the next page
    } node_t;

  public: //iteration

    /**
     * Defines an iterator-like type for this type of memory. There are no
     * guarantees on the iteration. The user should test and only iterate from
     * begin() till end()
     */
    class const_iterator {

    private: //only parent class can use this
      
      friend class PagedMemory;

      /**
       * Builds a new iterator giving an index and an offset inside that
       * index.
       *
       * @param base The base address of the memory block being pointed now
       * @param size The total size of this block, in 32-bit words 
       * @param offset The offset to where I should point to, in 32-bit words
       */
      const_iterator (const node_t* node, size_t offset)
	: m_node(node), m_off(offset) {}
      

    public: //access methods

      /**
       * Returns an iterator that points nowhere, to fake similar behaviour as
       * the normal pointers.
       */
      const_iterator () : m_node(), m_off() {}

      /**
       * Copies the value of an iterator
       *
       * @param other The iterator to copy the data from
       */
      const_iterator (const const_iterator& other)
	: m_node(other.m_node), m_off(other.m_off) {}

      /**
       * Destroys an iterator
       */
      ~const_iterator () {}

      /**
       * Copies the value of an iterator
       *
       * @param other The iterator to copy the data from
       */
      const_iterator& operator= (const const_iterator& other);

      /**
       * Compares two pointers
       *
       * @param other The other iterator to compare this one with.
       */
      bool operator!= (const const_iterator& other) const;

      /**
       * Compares two pointers
       *
       * @param other The other iterator to compare this one with.
       */
      inline bool operator== (const const_iterator& other) const
      { return !(*this != other); }

      /**
       * Gets a value away N positions from this iterator, unchecked
       *
       * @param pos The number "N" of positions away from this iterator
       */
      uint32_t operator[] (size_t pos) const;

      /**
       * Gets a value away N positions from this iterator, checked and may
       * throw an exception
       *
       * @param pos The number "N" of positions away from this iterator
       */
      uint32_t at (size_t pos) const;

      /**
       * Gets the current value for the pointed position, as a 32-bit
       * integer. This is unchecked so you have to make sure you are before
       * end().
       */
      inline uint32_t operator* (void) const 
      { return CAST32(m_node->page->iov_base)[m_off-m_node->start]; }

      /**
       * Advances forward on the pointing place, 4 bytes or 1 word. This is the
       * @b prefix operator.
       */
      const_iterator& operator++ (void);

      /**
       * Step back on the pointing place, 4 bytes or 1 word. This is the
       * @b prefix operator.
       */
      const_iterator& operator-- ();

      /**
       * Advances forward on the pointing place, by a number of (32-bit) words
       *
       * @param offset The amount of words to advance
       */
      const_iterator& operator+= (size_t offset);

      /**
       * Retrocesses on the pointing place, by a number of (32-bit) words
       *
       * @param offset The amount of words to advance
       */
      const_iterator& operator-= (size_t offset);

      /**
       * Compares two operators (offset comparison)
       *
       * @param other The other iterator to compare to
       */
      inline bool operator< (const const_iterator& other) const
      { return m_off < other.m_off; }

      /**
       * Compares two operators (offset comparison)
       *
       * @param other The other iterator to compare to
       */
      inline bool operator> (const const_iterator& other) const
      { return m_off > other.m_off; }

      /**
       * Returns the difference in position between two iterators
       *
       * @param other The other iterator to compare to
       */
      inline int32_t operator- (const const_iterator& other) const
      { return (int32_t)m_off - (int32_t)other.m_off; }

    private: //representation

      const node_t* m_node; ///< base address of the index table
      size_t m_off; ///< the current offset, in 32-bit words

    };

  public: //more interface declarations

    /**
     * Returns a const iterator to the begin of this list
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    inline const_iterator begin (void) const 
    { return const_iterator(m_node, 0); }

    /**
     * Returns a const iterator to the end of this list
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    inline const_iterator end (void) const
    { return const_iterator(&m_node[m_count], m_size); }

  private: //representation

    const struct iovec* m_iovec; ///< my iovec set of pages
    size_t m_count; ///< how many pages do I have in the list above
    node_t m_node[TMAXPAGES]; ///< index
    size_t m_size; ///< the number of words I have in this Memory

  };

}

template <unsigned int TMAXPAGES> const unsigned int 
eformat::PagedMemory<TMAXPAGES>::MAXPAGES = TMAXPAGES;

template <unsigned int TMAXPAGES> eformat::PagedMemory<TMAXPAGES>::PagedMemory 
(const struct iovec* pages, size_t count)
  : m_iovec(pages),
    m_count(count),
    m_size(0)
{
  if (m_count > MAXPAGES) {
    //Problem, I cannot index this amount of data
    throw EFORMAT_TOO_BIG_COUNT(count, MAXPAGES);
  }
  for (size_t i=0; i<count; ++i) {
    m_node[i].page = &pages[i];
    if (pages[i].iov_len % 4 != 0) {
      //Problem, one of the data blocks is *not* 32-bit aligned
      throw EFORMAT_NOT_ALIGNED(pages[i].iov_base, pages[i].iov_len);
    }
    m_node[i].start = m_size;
    m_size += pages[i].iov_len >> 2;
    m_node[i].end = m_size;
    if (i > 0) m_node[i].previous = &m_node[i-1];
    else m_node[i].previous = 0;
    if (i < (count-1) ) m_node[i].next = &m_node[i+1];
    else m_node[i].next = 0;
  }
}

template <unsigned int TMAXPAGES> typename eformat::PagedMemory<TMAXPAGES>::const_iterator&
eformat::PagedMemory<TMAXPAGES>::const_iterator::operator=
  (const typename eformat::PagedMemory<TMAXPAGES>::const_iterator& other)
{
  m_node = other.m_node;
  m_off = other.m_off;
  return *this;
}

template <unsigned int TMAXPAGES>
bool eformat::PagedMemory<TMAXPAGES>::const_iterator::operator!= 
(const typename eformat::PagedMemory<TMAXPAGES>::const_iterator& other) const
{
  return (m_node == other.m_node && m_off == other.m_off)?false:true;
}

template <unsigned int TMAXPAGES> 
uint32_t eformat::PagedMemory<TMAXPAGES>::const_iterator::operator[] 
(size_t pos) const
{
  if (m_off + pos < m_node->end) { //trying to reach data on the current page
    return CAST32(m_node->page->iov_base)[(m_off+pos)-m_node->start];
  }
  //otherwise it is not on this page, I have to find the page
  const_iterator it = *this;
  it += pos;
  return *it;
}

template <unsigned int TMAXPAGES>
uint32_t eformat::PagedMemory<TMAXPAGES>::const_iterator::at (size_t pos) const
{
  //immediate return (most of the cases)
  if (m_off + pos < m_node->end) { //trying to reach data on the current page
    return CAST32(m_node->page->iov_base)[(m_off+pos)-m_node->start];
  }

  //immediate points of failure
  if (!m_node || (m_node->next == 0 && (m_off+pos) >= m_node->end))
    throw EFORMAT_OUT_OF_BOUNDS(0, pos << 2);

  //if I get here, the data is on another page and the request might still be
  //out of bounds, so watch.
  const_iterator it = *this;
  it += pos;
  return it.at(0);
}

template <unsigned int TMAXPAGES> typename eformat::PagedMemory<TMAXPAGES>::const_iterator&
eformat::PagedMemory<TMAXPAGES>::const_iterator::operator++ (void)
{ 
  ++m_off;
  if ( m_off >= (m_node->end) ) {
    if (!m_node->next) { //it was the last
      m_off = m_node->end;
      return *this;
    }
    m_node = m_node->next;
    m_off = m_node->start;
  }
  return *this; 
}

template <unsigned int TMAXPAGES> typename eformat::PagedMemory<TMAXPAGES>::const_iterator&
eformat::PagedMemory<TMAXPAGES>::const_iterator::operator-- (void)
{ 
  --m_off;
  if ( m_off < (m_node->start) ) {
    if (!m_node->previous) { //it was the last
      m_off = m_node->start;
      return *this;
    }
    m_node = m_node->previous;
    m_off = m_node->end - 1;
  }
  return *this; 
}

template <unsigned int TMAXPAGES> typename eformat::PagedMemory<TMAXPAGES>::const_iterator&
eformat::PagedMemory<TMAXPAGES>::const_iterator::operator+= (size_t offset) 
{
  size_t aim = offset + m_off;
  while (aim >= (m_node->end)) {
    if (m_node->next) m_node = m_node->next;
    else { //it was the last
      m_off = m_node->end;
      return *this;
    }
  }
  m_off = aim;
  return *this;
}

template <unsigned int TMAXPAGES> typename eformat::PagedMemory<TMAXPAGES>::const_iterator&
eformat::PagedMemory<TMAXPAGES>::const_iterator::operator-= (size_t offset) 
{
  size_t aim = m_off - offset;
  while (aim < (m_node->start)) {
    if (m_node->previous) m_node = m_node->previous;
    else { //it was the first
      m_off = m_node->start;
      return *this;
    }
  }
  m_off = aim;
  return *this;
}

#endif /* EFORMAT_PAGEDMEMORY_H */
