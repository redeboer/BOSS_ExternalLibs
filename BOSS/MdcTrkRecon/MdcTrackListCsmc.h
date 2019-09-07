//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrackListCsmc.h,v 1.5 2008/04/01 03:14:36 zhangy Exp $
//
// Description:
//     Defines list of MdcTracks, and holds MdcTrkRecon track-finding fcns for 
//      straight tracks.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef MDCTRACKLISTCSMC_H
#define MDCTRACKLISTCSMC_H
#include "MdcTrkRecon/MdcTrackListBase.h"

class MdcSegList;
class MdcTrack;
class TrkRecoTrk;
class MdcDetector;
class MdcTrackMC;
class MdcSegGrouper;
class MdcHitMap;
class TrkContext;
class MdcTrackParams;

// Class definition//
class MdcTrackListCsmc : public MdcTrackListBase {

public:
  MdcTrackListCsmc(const MdcTrackParams &tkPar);
  ~MdcTrackListCsmc();
  
  //void setCosmic(TrkRecoTrk* theTr);      
  int createFromSegs(MdcSegList *segs, const MdcHitMap*, 
		     const MdcDetector*, TrkContext&, double trackT0);
  int finish3d(TrkRecoTrk &trk);

//Remove and delete:
  void remove( MdcTrack *atrack );

private:
};


#endif







