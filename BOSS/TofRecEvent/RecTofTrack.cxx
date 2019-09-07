#include "TofRecEvent/RecTofTrack.h"
RecTofTrack::RecTofTrack(){}

RecTofTrack::RecTofTrack(const DstTofTrack& aTrack)
  :DstTofTrack(aTrack) {}

RecTofTrack& RecTofTrack::operator=(const DstTofTrack& aTrack)
{ 
  DstTofTrack::operator=(aTrack);
  return *this;
}
