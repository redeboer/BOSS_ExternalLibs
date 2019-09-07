#ifndef CLASS_DQAKSINFO_H
#define CLASS_DQAKSINFO_H

//#include "GaudiKernel/ObjectVector.h"
//#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/NTuple.h"
//#include "GaudiKernel/IDataProviderSvc.h"

#include "EvtRecEvent/EvtRecTrack.h"
//#include "EvtRecEvent/EvtRecDTag.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <iostream>
using namespace std;
const double Ksinfo_mpi = 0.139570;

class DQAKsInfo{
public: 
DQAKsInfo();
DQAKsInfo(EvtRecTrack*,EvtRecTrack*,VertexParameter,VertexParameter);
~DQAKsInfo();

void setchild(int n, EvtRecTrack* track)
{
  if (n==0)
  m_track0 = track;
  else if (n==1)
  m_track1 = track;
}
void setchilds(EvtRecTrack* track0, EvtRecTrack* track1)
{
  m_track0 = track0;
  m_track1 = track1;
}
EvtRecTrack* getchild(int n)
{
  if (n==0)
	return m_track0;
  else if (n==1)
	return m_track1;
}
double m()
{
  return m_vmks;
}
double mksp4()
{
  return m_mksp4;
}
double vtxChi2()
{
	return m_vchi2ks1;
}
double chi2()
{
	return m_vchi2ks2;
}
double decayLength()
{
	return m_vlks;
}
double decayLengthRatio()
{
	return m_vrks;
}
HepLorentzVector p4()
{
  return m_p4;
}
void setVtxPar(VertexParameter vtxpar)
{
	m_vtxpar = vtxpar;
}
void setPrimaryVtxPar(VertexParameter privtxpar)
{
	m_privtxpar = privtxpar;
}
bool init()
{
	m_vtxFit->init();
	m_2ndVtxFit->init();
	return true;
}
bool calculate();

private:
EvtRecTrack*	m_track0;
EvtRecTrack*	m_track1;
HepLorentzVector m_p4;
VertexFit*       m_vtxFit;
SecondVertexFit* m_2ndVtxFit;
VertexParameter  m_vtxpar;
VertexParameter  m_privtxpar;
double m_mksp4;
double m_vmks;
double m_vchi2ks1;
double m_vchi2ks2;
double m_vlks;
double m_vrks;
};
#endif
