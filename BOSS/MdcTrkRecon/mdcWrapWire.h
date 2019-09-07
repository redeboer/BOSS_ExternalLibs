// mdcWrapWire.h -- utility routine to handle wrap-around wire numbers

#ifndef MDCWRAPWIRE_H
#define MDCWRAPWIRE_H

inline int mdcWrapWire(int wireIn, int nCell) {

  int mdcWrapWire = wireIn % nCell;
  if (mdcWrapWire < 0) mdcWrapWire += nCell;
  return mdcWrapWire;
}

#endif







