#include "EvtRecEvent/EvtRecVeeVertex.h"

EvtRecVeeVertex::EvtRecVeeVertex() :
    m_w(7, 0),
    m_Ew(7, 0),
    m_pair(-1, -1),
    m_daughters(SmartRef<EvtRecTrack>((EvtRecTrack*)0), SmartRef<EvtRecTrack>((EvtRecTrack*)0)) {
  m_vertexId = -1;
  m_vertexType = -1;
  m_chi2 = 999.9;
  m_ndof = -3;
  m_mass = 0.;
  m_nCharge = -1;
  m_nTracks = -1;
}

// copy ctor
EvtRecVeeVertex::EvtRecVeeVertex(const EvtRecVeeVertex& vtx) :
    ContainedObject(vtx),
    m_w(vtx.m_w),
    m_Ew(vtx.m_Ew),
    m_pair(vtx.m_pair),
    m_daughters(vtx.m_daughters) {
  m_vertexId = vtx.m_vertexId;
  m_vertexType = vtx.m_vertexType;
  m_chi2 = vtx.m_chi2; 
  m_ndof = vtx.m_ndof; 
  m_mass = vtx.m_mass;
  m_nCharge = vtx.m_nCharge;
  m_nTracks = vtx.m_nTracks;
}

// assignment operator
EvtRecVeeVertex& EvtRecVeeVertex::operator=(const EvtRecVeeVertex & vtx) {
  this->ContainedObject::operator=(vtx);
  m_w = vtx.m_w;
  m_Ew = vtx.m_Ew;
  m_pair = vtx.m_pair;
  m_daughters = vtx.m_daughters;
  m_vertexId = vtx.m_vertexId;
  m_vertexType = vtx.m_vertexType;
  m_chi2 = vtx.m_chi2;
  m_ndof = vtx.m_ndof;
  m_mass = vtx.m_mass;
  m_nCharge = vtx.m_nCharge;
  m_nTracks = vtx.m_nTracks;
  return *this;
}
