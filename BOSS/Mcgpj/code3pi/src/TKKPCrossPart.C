#include "TKKPCrossPart.h"
#include "TConstant.h"
#include "TUtil.h"

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

// gamma*-> K^*+ K^- -> (K^0 pi^+) K^-
// gamma*-> K^*0 K^0 -> (K^- pi^+) K^0
complex_t FK0KcPc(double s, double QK0Pi, double QKcPi){
  complex_t H = 
    R(QK0Pi,MKstar2,GKstar,mk02+mpi2,mk02-mpi2) + 
    R(QKcPi,MKstar2,GKstar,mkc2+mpi2,mkc2-mpi2);
  return (5e-3*(1/s))*H;//*(1+Amp(s));
}

// gamma*-> K^*+ K^- -> (K^+ pi^0) K^- 
// gamma*-> K^*- K^+ -> (K^- pi^0) K^+ 
complex_t FKcKcP0(double s, double QKpPi, double QKmPi){
  complex_t H = 
    R(QKpPi,MKstar2,GKstar,mkc2+mpi02,mkc2-mpi02) + 
    R(QKmPi,MKstar2,GKstar,mkc2+mpi02,mkc2-mpi02);
  return (5e-3*(1/s))*H*(1+Amp(s));
}

TKKPCrossPart::TKKPCrossPart(double e, double de, double nth0, int mode):
  TCrossPart(e,de,nth0)
{
  const double m_0[] = { mk0, mkc, mpi};
  const int pid_0[]  = { 310, -321, 211};
  const double m_1[] = { mkc, mkc, mpi0};
  const int pid_1[]  = { 321, -321, 111};
  fmode = mode;
  double emax = e;
  if(mode == 0){
    SetFinalParticles(3, m_0, pid_0);
    emax = e*(1 - pow((m_0[0]+m_0[1]+m_0[2])/(2*e),2));
    std::cout<<"Process -- e^+e^- -> K* K -> K_S K^- pi^+"<<std::endl;
  } else {
    SetFinalParticles(3, m_1, pid_1);
    emax = e*(1 - pow((m_1[0]+m_1[1]+m_1[2])/(2*e),2));
    std::cout<<"Process -- e^+e^- -> K* K -> K^+ K^- pi^0"<<std::endl;
  }

  //  emax = e/2.5;
  fphot->SetEnergyRange(de,emax);
  std::cout<<de<<"MeV <E_gamma< "<<emax<<" MeV"<<std::endl;
}

void TKKPCrossPart::SetJ(){

  TLorentzVector &qK1 = *fres[0];  //pi^+ momentum
  TLorentzVector &qK2 = *fres[1];  //pi^- momentum
  TLorentzVector &qPi = *fres[2];  //pi^0 momentum

  TLorentzVector qK1qPi = qK1 + qPi;
  TLorentzVector qK2qPi = qK2 + qPi;

  //  invariant masses
  double QK1Pi = qK1qPi.M2();
  double QK2Pi = qK2qPi.M2();

  complex_t F;
  if(fmode == 0){
    F = FK0KcPc(fq2,QK1Pi,QK2Pi);
  }else {
    F = FKcKcP0(fq2,QK1Pi,QK2Pi);
  }
  
  J3PseudoScalars();

  fJc *= F;
}

bool TKKPCrossPart::Accepted(){
  // require inv mass of 3-pi within 97% of Ecm
//   if(fq2<0.95*fs) return false;
//   const TLorentzVector &qK1 = *fres[0];  //pi^+ momentum
//   const TLorentzVector &qK2 = *fres[1];  //pi^- momentum
//   const TLorentzVector &qPi = *fres[2];  //pi^0 momentum
  
//   double qK1qK2 = qK1*qK2;
//   double qK1qPi = qK1*qPi;
//   double qK2qPi = qK2*qPi;
//   double QK12 = mpi2 + mpi02 + 2*qK2qPi;
//   double QK22 = mpi2 + mpi02 + 2*qK1qPi;
  
//   if(QK12+QK22<5e6) return false;

//   if(fabs(qK1.P()-qK2.P())>0.04*fe) return false;

//   double cosqK1 = qK1.CosTheta();
//   if(fabs(cosqK1)>0.9)return false;
  
//   double cosqK2 = qK2.CosTheta();
//   if(fabs(cosqK2)>0.9)return false;
  
  return true;
}

