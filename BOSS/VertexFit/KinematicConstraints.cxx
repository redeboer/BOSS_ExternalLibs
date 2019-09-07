// #include <iostream>
#include "VertexFit/KinematicConstraints.h"

const int KinematicConstraints::Resonance = 1;
const int KinematicConstraints::TotalEnergy = 2;
const int KinematicConstraints::TotalMomentum = 4;
const int KinematicConstraints::Position = 8;
const int KinematicConstraints::ThreeMomentum = 16;
const int KinematicConstraints::FourMomentum = 32;
const int KinematicConstraints::EqualMass = 64;

KinematicConstraints::KinematicConstraints(){
  m_Ec.clear();
  m_Dc.clear();
  m_dc.clear();
  m_Vm.clear();
  m_lambda.clear();
  m_ltrk.clear();
  m_nequal.clear();
  m_nc=0;
  m_mres = -1;
  m_etot = -1;
  m_ptot = -1;
  m_p3 = Hep3Vector(0, 0, 0);
  m_p4 = HepLorentzVector(0, 0, 0, 0);
  m_phipre = 0;
  m_lambdapre = 0;
  m_Vre = HepSymMatrix(1,0);
  m_Vme = HepSymMatrix(4,0);
  m_Vne = HepSymMatrix(1,0);
  m_Vpe = HepSymMatrix(2,0);
}

void KinematicConstraints::ResonanceConstraints(const double mres, std::vector<int> tlis, HepSymMatrix Vre) 
{
  m_ltrk = tlis;
  m_nc = 1;
  setType(typeRes());
  setMres(mres);
  setVre(Vre);
  setVmeasure(Vre);
}

void KinematicConstraints::TotalEnergyConstraints(const double etot, std::vector<int> tlis) 
{
  m_ltrk = tlis;
  m_nc = 1;
  setType(typeEtot());
  setEtot(etot);
  HepSymMatrix tmp(1,0);
  setVmeasure(tmp);
}

void KinematicConstraints::TotalMomentumConstraints(const double ptot, std::vector<int> tlis)
{
  m_ltrk = tlis;
  m_nc = 1;
  setType(typePtot());
  setPtot(ptot);
  HepSymMatrix tmp(1,0);
  setVmeasure(tmp);
}

void KinematicConstraints::ThreeMomentumConstraints(const Hep3Vector p3, std::vector<int> tlis)
{
  m_ltrk = tlis;
  m_nc = 3;
  setType(typeP3());
  setP3(p3);
  HepSymMatrix tmp(3,0);
  setVmeasure(tmp);
}

void KinematicConstraints::FourMomentumConstraints(const HepLorentzVector p4, std::vector<int> tlis, HepSymMatrix Vme)
{
  m_ltrk = tlis;
  m_nc = 4;
  setType(typeP4());
  setP4(p4);
  setVme(Vme);
  m_Vm.push_back(Vme);
  setVmeasure(Vme);
}

void KinematicConstraints::EqualMassConstraints(std::vector<int> tlis1, std::vector<int> tlis2, HepSymMatrix Vne) 
{
  m_ltrk.clear();
  for(unsigned int i = 0; i < tlis1.size(); i++)
    m_ltrk.push_back(tlis1[i]);
  for(unsigned int i = 0; i < tlis2.size(); i++)
    m_ltrk.push_back(tlis2[i]);
  m_nc = 1;
  setType(typeEqMass());
  setVne(Vne);
  m_Vm.push_back(Vne);
  m_nequal.clear();
  m_nequal.push_back(tlis1.size());
  m_nequal.push_back(tlis2.size());
  setVmeasure(Vne);
 
}
/*
void KinematicConstraints::PositionConstraints(const double phipre, const double lambdapre, std::vector<int> tlis_V, HepSymMatrix Vpe) {
  m_ltrk_V = tlis_V;
  m_nc = 2;
  setType(typePoint());
  setPhipre(phipre);
  setLambdapre(lambdapre);
  setVpe(Vpe);
  m_Vm.push_back(Vpe);
  setVmeasure(Vpe);
}
*/
