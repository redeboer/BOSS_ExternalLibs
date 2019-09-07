#include "TKKEtaCrossPart.h"
#include "TConstant.h"
#include "TUtil.h"

#include <complex>
#include <iostream>

using namespace rb;

inline complex_t Amp(double s){
  const complex_t I(0,1);
  const double Apsi = 4*alpha*Gpsi/(3*Geepsi);
  const double phipsi = -M_PI/2;
  const double Apsip = 2*alpha*Gpsip/(3*Geepsip);
  const double phipsip = -M_PI/2;
  const double Apsipp = alpha*Gpsipp/(3*Geepsipp);
  const double phipsipp = -M_PI/2;
  complex_t res = 3*sqrt(s)/alpha*
    (
     Geepsi*(1 + Apsi*exp(I*phipsi))/(s - Mpsi2 + I*Mpsi*Gpsi) +
     Geepsip*(1 + Apsip*exp(I*phipsip))/(s - Mpsip2 + I*Mpsip*Gpsip) +
     Geepsipp*(1 + Apsipp*exp(I*phipsipp))/(s - Mpsipp2 + I*Mpsipp*Gpsipp));
  return res;
}

complex_t FK0K0Eta(double s, double Q02){
  complex_t H = R(Q02,Mphi2,Gphi,mk02+mk02,0);
  return (5e-3*(1/s))*H*(1+Amp(s));
}

complex_t FKcKcEta(double s, double Q02){
  complex_t H = R(Q02,Mphi2,Gphi,mkc2+mkc2,0);
  return (5e-3*(1/s))*H*(1+Amp(s));
}

TKKEtaCrossPart::TKKEtaCrossPart(double e, double de, double nth0, int mode):
  TCrossPart(e,de,nth0),fmode(mode)
{
  const double m_0[] = { mkc, mkc, meta};
  const int pid_0[]  = { 321, -321, 221};
  const double m_1[] = { mk0, mk0, meta};
  const int pid_1[]  = { 310,  130, 221};

  double emax = e;
  if(mode == 0){
    SetFinalParticles(3, m_0, pid_0);
    emax = e*(1 - pow((m_0[0]+m_0[1]+m_0[2])/(2*e),2));
    std::cout<<"Process -- e^+e^- -> K* K -> K^+ K^- eta"<<std::endl;
  } else {
    SetFinalParticles(3, m_1, pid_1);
    emax = e*(1 - pow((m_1[0]+m_1[1]+m_1[2])/(2*e),2));
    std::cout<<"Process -- e^+e^- -> K* K -> K_S K_L eta"<<std::endl;
  }

  //  emax = e/2.5;
  fphot->SetEnergyRange(de,emax);
  std::cout<<de<<"MeV <E_gamma< "<<emax<<" MeV"<<std::endl;
}

void TKKEtaCrossPart::SetJ(){
  TLorentzVector &qK1 = *fres[0];  //K momentum
  TLorentzVector &qK2 = *fres[1];  //K momentum

  TLorentzVector qK1qK2 = qK1 + qK2;

  //  invariant masses
  double QKK = qK1qK2.M2();

  complex_t F;
  if(fmode == 0){
    F = FKcKcEta(fq2,QKK);
  }else {
    F = FK0K0Eta(fq2,QKK);
  }
 
  J3PseudoScalars();

  fJc *= F;
}

bool TKKEtaCrossPart::Accepted(){
  // require inv mass of 3-pi within 97% of Ecm
//   if(fq2<0.95*fs) return false;
//   const TLorentzVector &qp = *fres[0];  //pi^+ momentum
//   const TLorentzVector &qm = *fres[1];  //pi^- momentum
//   const TLorentzVector &q0 = *fres[2];  //pi^0 momentum
  
//   double qpqm = qp*qm;
//   double qpq0 = qp*q0;
//   double qmq0 = qm*q0;
//   double Qp2 = mpi2 + mpi02 + 2*qmq0;
//   double Qm2 = mpi2 + mpi02 + 2*qpq0;
  
//   if(Qp2+Qm2<5e6) return false;

//   if(fabs(qp.P()-qm.P())>0.04*fe) return false;

//   double cosqp = qp.CosTheta();
//   if(fabs(cosqp)>0.9)return false;
  
//   double cosqm = qm.CosTheta();
//   if(fabs(cosqm)>0.9)return false;
  
  return true;
}

