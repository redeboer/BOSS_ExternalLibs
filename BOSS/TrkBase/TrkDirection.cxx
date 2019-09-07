// -----------------------------------------------------------------------
// File and Version Information:
// $Id: TrkDirection.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// This defines an output operator to write the strings "trkIn" and "trkOut"
// for the trkDirection enumeration values.
//
// Michael Kelsey <kelsey@slac.stanford.edu>
//
// Copyright Information:
//	Copyright (C) 1999  Princeton University
//
//------------------------------------------------------------------------

#include "TrkBase/TrkDirection.h"
#include <iostream>
using std::ostream;

ostream& operator<<(ostream& os, const trkDirection& dir) {
  if (dir==trkIn) return os << "trkIn";
  if (dir==trkOut) return os << "trkOut";
  return os << (int)dir;
}
