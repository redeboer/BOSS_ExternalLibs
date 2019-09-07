#include "T2piCrossPart.h"
#include "TConstant.h"

#include <complex>
#include <iostream>
using namespace rb;
typedef std::complex<double> complex_t;

inline complex_t operator+(const double &x, const complex_t &y) {
  return y + x;
}

inline complex_t operator-(const double &x, const complex_t &y) {
  return -(y - x);
}

inline double Gamma(const double Q2, const double M2, const double G){
  double beta2 = (Q2-4*mpi2)/(M2-4*mpi2);
  double beta = sqrt(beta2);
  return G*M2/Q2*beta2*beta;  
}

inline complex_t Rrho0(double Q2){
  if(Q2<4*mpi2) return 0;
  const complex_t I(0,1);
  double Q = sqrt(Q2);
  complex_t prop_rho = Mrho2/(Q2 - Mrho2 + I*(Gamma(Q2,Mrho2,Grho)*Q));
  complex_t prop_rhop = Mrhop2/(Q2 - Mrhop2 + I*(Gamma(Q2,Mrhop2,Grhop)*Q));
  complex_t prop_rhopp = Mrhopp2/(Q2 - Mrhopp2 + I*(Gamma(Q2,Mrhopp2,Grhopp)*Q));
  // rho(770), rho(1450), rho(1700) in 2-pi inv mass
  // beta and gamma from Ph.D. thesis of Fedor Ignatov (ignatov@inp.nsk.su)

  //  const complex_t beta(0.41*cos(-2.02),0.41*sin(-2.02));
  const complex_t beta(-0.178042,-0.369325);

  //  const complex_t gamma(0.074*cos(-3.53),0.074*sin(-3.53));
  const complex_t gamma(-0.068488,0.0280249);

  // rho-omega mixing
  const complex_t delta(0.0015,0);
  complex_t prop_omega = Momega2/(Q2 - Momega2 + I*(Gomega*Momega));

  return (prop_rho*(1-delta*Q2/Momega2*prop_omega) + beta*prop_rhop  + gamma*prop_rhopp)/(1+beta+gamma);
}

complex_t F2pi(double s){
  return complex_t(1,0);
  //  return Rrho0(s);
}

T2piCrossPart::T2piCrossPart(double e, double de, double nth0):
  TCrossPart(e,de,nth0)
{
  const double m[] = { mpi, mpi};
  const int pid[]  = { 211, -211};
  fphot->SetEnergyRange(de,e - m[0]*m[0]/e);
  //  fphot->SetEnergyRange(de,0.5*e);
  SetFinalParticles(2, m, pid);
  std::cout<<"Process -- e^+e^- -> pi^+pi^-"<<std::endl;
}

// The hadronic current 
void T2piCrossPart::SetJ(){
  TLorentzVector &qp = *fres[0];  //pi^+ momentum
  TLorentzVector &qm = *fres[1];  //pi^- momentum
  
  double Jx = qp.X()-qm.X();
  double Jy = qp.Y()-qm.Y();
  double Jz = qp.Z()-qm.Z();
  double Jt = qp.T()-qm.T();

  complex_t F = F2pi(fq2);

  fJc.SetPxPyPzE(F*Jx,F*Jy,F*Jz,F*Jt);
}

bool T2piCrossPart::Accepted(){
  // require inv mass of 3-pi within 97% of Ecm
  //  if(fq2<0.5*fs) return false;
  //   const TLorentzVector &qp = *fres[0];  //pi^+ momentum
  //   const TLorentzVector &qm = *fres[1];  //pi^- momentum
  
//   if(fabs(qp.P()-qm.P())>0.04*fe) return false;

//   double cosqp = qp.CosTheta();
//   if(fabs(cosqp)>0.9)return false;
  
//   double cosqm = qm.CosTheta();
//   if(fabs(cosqm)>0.9)return false;
  
  return true;
}

