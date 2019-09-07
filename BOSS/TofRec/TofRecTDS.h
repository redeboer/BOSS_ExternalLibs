#ifndef TOFRECTDS_H
#define TOFRECTDS_H
#include "TofRec/TofTrack.h"

class StatusCode;

class TofRecTDS{
 public:

  StatusCode InitializeTDS();
  StatusCode RegisterNullRecTofTrackCol();

  StatusCode RegisterTDS( int runNumber, int eventNumber, std::vector<TofTrack*>*& tofTrackVec, bool m_forCalibration, std::string m_calibData );

  StatusCode RegisterReconEvent();
  StatusCode RegisterRecTofTrackCol( std::vector<TofTrack*>*& tofTrackVec );
  StatusCode RegisterRecBTofCalHitCol( int runNumber, int eventNumber, std::vector<TofTrack*>*& tofTrackVec, std::string calibData  );
  StatusCode RegisterRecETofCalHitCol( int runNumber, int eventNumber, std::vector<TofTrack*>*& tofTrackVec, std::string calibData  );

};
#endif

