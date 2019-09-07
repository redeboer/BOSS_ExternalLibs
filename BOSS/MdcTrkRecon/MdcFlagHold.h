// Kludgy class to hold misc flags and parameters that are run-time settable
//    just a convenient way of moving them about

// Interface Dependencies ----------------------------------------------

#ifndef MDCFLAGHOLD_H
#define MDCFLAGHOLD_H

#include "MdcTrkRecon/MdcSegParams.h"
#include "MdcTrkRecon/MdcTrackParams.h"
#include <vector>

//  End Interface Dependencies -----------------------------------------

// Class //
class MdcFlagHold {
 public:
  void setDebug(int debugFlag);
  void setHist(int histFlag);
  int debugFlag() const {return m_debug;}

  int plot;
  int plotSegs;
  int plotSegList;
  int plotTrackList;
  int plotTracks;
  int lHist;
  int findTracks;   // for the moment, 2 => cosmic/prototype tracks
  int findSegs;
  MdcSegParams segPar;
  MdcTrackParams tkParTight;

  void readPar(std::string inname);
  void printPar();
  MdcFlagHold();
  ~MdcFlagHold() {};

private:
  int m_debug;
};

#endif







