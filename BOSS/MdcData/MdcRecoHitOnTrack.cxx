//  MdcRecoHitOnTrack.cxx
//
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcData/MdcHit.h"
// MdcGeom needed to verify if hit is inside of chamber...
// and to find the trajectory describing the hit, i.e. wire
#include "TrkBase/TrkRep.h"
//#include "TrkBase/TrkEnums.h"
//#include "TrkBase/TrkSimpTraj.h"


//-------------
// Constructors
//-------------

MdcRecoHitOnTrack::MdcRecoHitOnTrack(const MdcHit &baseHit, int ambig, double bunchTime)
  : MdcHitOnTrack(baseHit,baseHit, ambig,  bunchTime)
{ }


MdcRecoHitOnTrack::MdcRecoHitOnTrack(const MdcRecoHitOnTrack &hot,
                                     TrkRep *newRep,
                                     const TrkDifTraj* trkTraj)
  : MdcHitOnTrack(hot,newRep,trkTraj)
{ }

MdcRecoHitOnTrack::~MdcRecoHitOnTrack()
{ ; }

TrkHitOnTrk*
MdcRecoHitOnTrack::clone(TrkRep *rep, const TrkDifTraj *trkTraj) const
{
  return new MdcRecoHitOnTrack(*this,rep,trkTraj);
}

const MdcHit*
MdcRecoHitOnTrack::mdcHit() const 
{
  return static_cast<const MdcHit*>(hit());
}

unsigned
MdcRecoHitOnTrack::status() const
{
//  return mdcHit()->digi()->status();
  return mdcHit()->status();
}

unsigned
MdcRecoHitOnTrack::tdcIndex() const
{
  return mdcHit()->tdcIndex();
}
