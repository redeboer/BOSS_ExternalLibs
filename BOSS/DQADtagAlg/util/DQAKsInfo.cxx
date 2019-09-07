#include "DQADtagAlg/util/DQAKsInfo.h" 

DQAKsInfo::DQAKsInfo(): m_track0(0),m_track1(0)
{
	m_vtxFit = VertexFit::instance();
	m_2ndVtxFit = SecondVertexFit::instance();
}

DQAKsInfo::DQAKsInfo(EvtRecTrack* track0, EvtRecTrack* track1,VertexParameter vtxpar, VertexParameter privtxpar)
{
	m_vtxFit = VertexFit::instance();
	m_2ndVtxFit = SecondVertexFit::instance();
	m_track0 = track0;
	m_track1 = track1;
	m_vtxpar = vtxpar;
	m_privtxpar = privtxpar;
}
DQAKsInfo::~DQAKsInfo()
{
	m_track0 = 0;
	m_track1 = 0;
	m_p4 = HepLorentzVector(0,0,0,0);
	m_vmks = 0;
}
bool DQAKsInfo::calculate()
{
  if (m_track0 == 0 || m_track1 == 0 ) return false;
  RecMdcKalTrack* pion1Trk = m_track0->mdcKalTrack();
  RecMdcKalTrack* pion2Trk = m_track1->mdcKalTrack();
  HepLorentzVector pion1p4 = pion1Trk->p4(Ksinfo_mpi);
  HepLorentzVector pion2p4 = pion2Trk->p4(Ksinfo_mpi);
  HepLorentzVector ksp4 = pion1p4 + pion2p4;
  m_mksp4 = ksp4.m();

  WTrackParameter wvTrk1(Ksinfo_mpi,     pion1Trk->getZHelix(),  pion1Trk->getZError());
  WTrackParameter wvTrk2(Ksinfo_mpi,     pion2Trk->getZHelix(),  pion2Trk->getZError());

  m_vtxFit->init();
  m_vtxFit->setChisqCut(100);
  m_vtxFit->AddTrack(0, wvTrk1);
  m_vtxFit->AddTrack(1, wvTrk2);
  m_vtxFit->AddVertex(0, m_vtxpar, 0, 1);
  if ( ! m_vtxFit->Fit(0) ) return false;
  m_vtxFit->Swim(0);
  m_vtxFit->BuildVirtualParticle(0);
  WTrackParameter wtKs = m_vtxFit->wVirtualTrack(0);
  VertexParameter vtKs = m_vtxFit->vpar(0);

  m_2ndVtxFit->init();
  m_2ndVtxFit->setChisqCut(100);
  m_2ndVtxFit->setPrimaryVertex(m_privtxpar);
  m_2ndVtxFit->AddTrack(0, wtKs);
  m_2ndVtxFit->setVpar(vtKs);
  if ( ! m_2ndVtxFit->Fit() ) return false;
  HepLorentzVector vp4Ks = m_2ndVtxFit->p4par();
  m_p4 = vp4Ks;
  m_vmks = vp4Ks.m();
  m_vchi2ks1 = m_vtxFit->chisq(0);
  m_vchi2ks2 = m_2ndVtxFit->chisq();
  m_vlks = m_2ndVtxFit->decayLength();
  m_vrks = m_2ndVtxFit->decayLength() / m_2ndVtxFit->decayLengthError();
  return true;
}
