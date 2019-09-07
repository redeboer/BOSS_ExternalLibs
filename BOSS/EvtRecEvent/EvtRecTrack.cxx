#include "EvtRecEvent/EvtRecTrack.h"

EvtRecTrack::EvtRecTrack() :
   m_trackId(-1),
   m_partId(-1),
   m_quality(-1)
{
   m_originMdcTrack = 0;
   m_originMdcKalTrack = 0;
   m_originMdcDedx = 0;
   m_originTofTrack.clear();
   m_originEmcShower = 0;
   m_originMucTrack = 0;
   m_originExtTrack = 0;
}

EvtRecTrack::EvtRecTrack(const EvtRecTrack & trk) : ContainedObject(trk),
   m_trackId(trk.m_trackId),
   m_partId(trk.m_partId),
   m_quality(trk.m_quality),
   m_originMdcTrack(trk.m_originMdcTrack),
   m_originMdcKalTrack(trk.m_originMdcKalTrack),
   m_originMdcDedx(trk.m_originMdcDedx),
   m_originTofTrack(trk.m_originTofTrack),
   m_originEmcShower(trk.m_originEmcShower),
   m_originMucTrack(trk.m_originMucTrack),
   m_originExtTrack(trk.m_originExtTrack)
{
}

EvtRecTrack & EvtRecTrack :: operator = (const EvtRecTrack & trk) { 
   m_trackId           = trk.m_trackId;
   m_partId            = trk.m_partId;
   m_quality           = trk.m_quality;
   m_originMdcTrack    = trk.m_originMdcTrack;
   m_originMdcKalTrack = trk.m_originMdcKalTrack;
   m_originMdcDedx     = trk.m_originMdcDedx;
   m_originTofTrack    = trk.m_originTofTrack;
   m_originEmcShower   = trk.m_originEmcShower;
   m_originMucTrack    = trk.m_originMucTrack;
   m_originExtTrack    = trk.m_originExtTrack;
   return *this;
}
