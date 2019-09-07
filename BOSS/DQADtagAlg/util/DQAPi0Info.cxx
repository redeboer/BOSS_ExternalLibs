#include "DQADtagAlg/util/DQAPi0Info.h" 

DQAPi0Info::DQAPi0Info(): m_shower0(0),m_shower1(0),m_p4(0),m_mpi0(0)
{
	m_energyThreshold_e = 0.05;
	m_energyThreshold_b = 0.025;
	m_costheta_b = 0.82;
	m_costheta_e1 = 0.85;
	m_costheta_e2 = 0.92;
}

DQAPi0Info::~DQAPi0Info()
{
	m_shower0 = 0;
	m_shower1 = 0;
	m_p4 = HepLorentzVector(0,0,0,0);
	m_mpi0 = 0;
}
