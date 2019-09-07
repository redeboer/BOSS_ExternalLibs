//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: BaBar.cc,v 1.1 2007/10/12 07:30:20 caogf Exp $
//
// Description:
//      See corresponding .hh file.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Rainer Bartoldus
//
// Copyright Information:
//      Copyright (C) 2004      Stanford Linear Accelerator Center
//
//--------------------------------------------------------------------------
#ifndef    BABAR_COMP_INST
#include "BaBar.hh"
#endif  // BABAR_COMP_INST

#if defined(__SUNPRO_CC)

#include <stddef.h>

namespace std
{
  template <class InputIterator, class T>
  ptrdiff_t
  count (InputIterator first, InputIterator last, const T& value)
  {
    ptrdiff_t n = 0;
    while (first != last)
      if (*first++ == value) ++n;
    return n;
  }

  template <class InputIterator, class Predicate>
  ptrdiff_t
  count_if (InputIterator first, InputIterator last, Predicate pred)
  {
    ptrdiff_t n = 0;
    while (first != last)
      if (pred(*first++)) ++n;
    return n;
  }

  template <class InputIterator>
  ptrdiff_t
  distance (InputIterator first, InputIterator last)
  {
    ptrdiff_t n = 0;
    while (first != last)
      { ++first; ++n; }
    return n;
  }
}
#endif // __SUNPRO_CC
