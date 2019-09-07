#ifndef CLASS_DQAPi0INFO_H
#define CLASS_DQAPI0INFO_H

//#include "GaudiKernel/ObjectVector.h"
//#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/NTuple.h"
//#include "GaudiKernel/IDataProviderSvc.h"

#include "EvtRecEvent/EvtRecTrack.h"
//#include "EvtRecEvent/EvtRecDTag.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <iostream>

using namespace std;
class DQAPi0Info{
public: 
DQAPi0Info();
~DQAPi0Info();

void setchild(int n, EvtRecTrack* shower)
{
  if (n==0)
  m_shower0 = shower;
  else if (n==1)
  m_shower1 = shower;
}
void setchilds(EvtRecTrack* shower0, EvtRecTrack* shower1)
{
  m_shower0 = shower0;
  m_shower1 = shower1;
}
EvtRecTrack* getchild(int n)
{
  if (n==0)
	return m_shower0;
  else if (n==1)
	return m_shower1;
}
double m()
{
  return m_mpi0;
}
HepLorentzVector p4()
{
  return m_p4;
}
void setEnergyThreshold(double energyThreshold_b, double energyThreshold_e)
{
  m_energyThreshold_b = energyThreshold_b;
  m_energyThreshold_e = energyThreshold_e;
}
void setCosTheta(double costheta_b, double costheta_e1, double costheta_e2)
{
	m_costheta_b = costheta_b;
	m_costheta_e1 = costheta_e1;
	m_costheta_e2 = costheta_e2;
}
bool calculate()
{
  if (m_shower0 == 0 || m_shower1 == 0 ) return false;
  RecEmcShower* photon1= m_shower0->emcShower();
  RecEmcShower* photon2= m_shower1->emcShower();
  double eraw1 = photon1->energy();
  double phiemc1 = photon1->phi(); 
  double theta1 = photon1->theta();
  double eraw2 = photon2->energy();
  double phiemc2 = photon2->phi(); 
  double theta2 = photon2->theta();
  double costheta1 = fabs(cos(theta1));
  double costheta2 = fabs(cos(theta2));
  if (!((costheta1 < m_costheta_b && eraw1 > m_energyThreshold_b ) || 
  (costheta1 > m_costheta_e1 && costheta1 < m_costheta_e2 && eraw1 > m_energyThreshold_e )))
  return false;
  if (!((costheta2 < m_costheta_b && eraw2 > m_energyThreshold_b ) ||
  (costheta2 > m_costheta_e1 && costheta2 < m_costheta_e2 && eraw2 > m_energyThreshold_e )))
  return false;

  HepLorentzVector pemc1;
  pemc1.setPx(eraw1*sin(theta1)*cos(phiemc1));
  pemc1.setPy(eraw1*sin(theta1)*sin(phiemc1));
  pemc1.setPz(eraw1*cos(theta1));
  pemc1.setE(eraw1);

  HepLorentzVector pemc2;
  pemc2.setPx(eraw2*sin(theta2)*cos(phiemc2));
  pemc2.setPy(eraw2*sin(theta2)*sin(phiemc2));
  pemc2.setPz(eraw2*cos(theta2));
  pemc2.setE(eraw2);

  HepLorentzVector ppi0=pemc1+pemc2;
  m_p4 = ppi0;
  m_mpi0 = ppi0.m();
  return true;
}
private:
EvtRecTrack*	m_shower0;
EvtRecTrack*	m_shower1;
HepLorentzVector m_p4;
double m_mpi0;
double m_energyThreshold_b;
double m_energyThreshold_e;
double m_costheta_b;
double m_costheta_e1;
double m_costheta_e2;
};
#endif
