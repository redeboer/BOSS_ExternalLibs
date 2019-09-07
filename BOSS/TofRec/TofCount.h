#ifndef TOFCOUNT_H
#define TOFCOUNT_H

#include "TofRec/TofTrack.h"

const unsigned int nParBhabha = 15;
const unsigned int nParDimu   = 8;

class TofCount{
 public:
  TofCount();
  ~TofCount() {}

  void init();

  void setExtTrackNum( unsigned int ntrk );
  void setTrack1( TofTrack*& tof );
  void setTrack1Col( std::vector<TofTrack*>*& tofTrackVec );
  void setTrack2( TofTrack*& tof );
  void setTrack3( TofTrack*& tof );
  void setTrack4( TofTrack*& tof );

  void final();

  void initBhabha();
  void addNumber( unsigned int i );
  void finalBhabha( std::string calibData );

 private:
  int extTrkNum;
  int barrelTrk, endcapTrk;
  int inner1, outer1, double1, east1, west1, nohit1, unchanged1, changed1;
  int inner2, outer2, double2, east2, west2, nohit2, unchanged2, changed2;
  int inner3, outer3, double3, east3, west3, nohit3, unchanged3, changed3;
  int inner4, outer4, double4, east4, west4, nohit4, unchanged4, changed4;
  int bNCounter1, eNCounter1, bNCounter2, eNCounter2;

  int nbb[nParBhabha];
};

#endif
