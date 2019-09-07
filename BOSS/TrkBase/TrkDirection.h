#ifndef TRKDIRECTION_H
#define TRKDIRECTION_H
// -----------------------------------------------------------------------
// File and Version Information:
// $Id: TrkDirection.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// These enums define standard values for directions along a track.
// Out is defined as the direction of increasing flight length, physically
// the direction of energy loss.  In is define by decreasing flight length
// and energy gain.
//
// Dave Brown, LBL
//
// Copyright Information:
//	Copyright (C) 1997		Lawrence Berkeley Laboratory
//
// Revision History:
//	19991228  M. Kelsey -- Add output operator for symbolic writing.
//------------------------------------------------------------------------

#include <iosfwd>

enum trkDirection { trkOut=0, trkIn };

// Output operator to print "names" instead of numbers

std::ostream& operator<<(std::ostream& os, const trkDirection& dir);

#endif
