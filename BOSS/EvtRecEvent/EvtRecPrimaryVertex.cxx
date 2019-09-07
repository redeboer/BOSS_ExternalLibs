#include "EvtRecEvent/EvtRecPrimaryVertex.h"

EvtRecPrimaryVertex::EvtRecPrimaryVertex() :
    m_vtx(3, 0), m_Evtx(3, 0) {
  m_isValid = false;
  m_nTracks = -1; 
  m_chi2 = -1; 
  m_ndof = -3; 
  m_fitMethod = -1; 
}

// copy ctor
EvtRecPrimaryVertex::EvtRecPrimaryVertex(const EvtRecPrimaryVertex& vtx) :
    DataObject(vtx),
    m_trackIdList(vtx.m_trackIdList),
    m_vtx(vtx.m_vtx),
    m_Evtx(vtx.m_Evtx) {
  m_isValid = vtx.m_isValid;
  m_nTracks = vtx.m_nTracks; 
  m_chi2 = vtx.m_chi2; 
  m_ndof = vtx.m_ndof; 
  m_fitMethod = vtx.m_fitMethod; 
}

// assignment operator
EvtRecPrimaryVertex& EvtRecPrimaryVertex:: operator = (const EvtRecPrimaryVertex & vtx) {
  m_isValid = vtx.m_isValid;
  m_nTracks = vtx.m_nTracks;
  m_trackIdList = vtx.m_trackIdList; // FIXME
  m_chi2 = vtx.m_chi2;
  m_ndof = vtx.m_ndof;
  m_fitMethod = vtx.m_fitMethod;
  m_vtx = vtx.m_vtx;
  m_Evtx = vtx.m_Evtx;
  return *this;
}
