#include "T3piCrossPart.h"
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

// with rho0omega mixing
inline complex_t Rrho(double Q2, bool IsIsospinBreak){
  if(Q2<4*mpi2) return 0;
  const complex_t I(0,1);
  double Q = sqrt(Q2);
  complex_t prop_rho = Mrho2/(Q2 - Mrho2 + I*(Gamma(Q2,Mrho2,Grho)*Q));
  return prop_rho;
  complex_t prop_rhop = Mrhop2/(Q2 - Mrhop2 + I*(Gamma(Q2,Mrhop2,Grhop)*Q));
  complex_t prop_rhopp = Mrhopp2/(Q2 - Mrhopp2 + I*(Gamma(Q2,Mrhopp2,Grhopp)*Q));
  complex_t prop_rhoppp = Mrhoppp2/(Q2 - Mrhoppp2 + I*(Gamma(Q2,Mrhoppp2,Grhoppp)*Q));
  // rho(770), rho(1450), rho(1700) in 2-pi inv mass
  // beta and gamma from Ph.D. thesis of Fedor Ignatov (ignatov@inp.nsk.su)

  //  const complex_t beta(0.41*cos(-2.02),0.41*sin(-2.02));
  const complex_t beta(-0.178042,-0.369325);

  //  const complex_t gamma(0.074*cos(-3.53),0.074*sin(-3.53));
  const complex_t gamma(-0.068488,0.0280249);

  const complex_t xi(0.05,0.0);
  if(IsIsospinBreak){
    // rho-omega mixing
    const complex_t delta(0.002,0);
    complex_t prop_omega = Momega2/(Q2 - Momega2 + I*(Gomega*Momega));

    return (prop_rho*(1-delta*Q2/Momega2*prop_omega) + beta*prop_rhop  + gamma*prop_rhopp + xi*prop_rhoppp)/(1+beta+gamma+xi);
  } else {
    return (prop_rho + beta*prop_rhop  + gamma*prop_rhopp + xi*prop_rhoppp)/(1+beta+gamma+xi);
  }
}

inline complex_t Amp(double s){
  const complex_t I(0,1);
  const double Apsi = 4*alpha*Gpsi/(3*Geepsi);
  const double phipsi = -M_PI/2;
  const double Apsip = alpha*Gpsip/(3*Geepsip);
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

inline complex_t AmpOmegaPhi(double s){
  //  const double eta = 3.4*M_PI/180;
  //  const double theta = eta + asin(1/sqrt(3));
  //  const double comega = sin(theta)*cos(eta);
  //  const double cphi = -cos(theta)*sin(eta);
  const double comega = 6.23657803940740774e-01;
  const double cphi = -4.63075347511148622e-02;
  complex_t Romega = Momega2/complex_t(s-Momega2,Gomega*Momega);
  complex_t Rphi = Mphi2/complex_t(s-Mphi2,Gphi*Mphi);
  return (comega*Romega + cphi*Rphi);
}

complex_t F3pi(double s, double Q02, double Qp2, double Qm2){
  // Rrho has rho-omega mixing otherwise use Rrho
  complex_t H = Rrho(Q02,true) + Rrho(Qm2,false) + Rrho(Qp2,false);
  return (5e-3*(1/s))*H*(1+Amp(s));
  //  const double f_pi3 = 93*93*93;// [MeV^3]
  //  return complex_t(1000/(s*sqrt(s)),0);
  //  const double alphaK = 0.5;
  //  return 2e5/(s*s)*(1+Amp(s))*(1-3*alphaK-alphaK*H);
  //  return 0.325*1e-9*Amp(s)*H;
  //  return sqrt(3)/(4*M_PI*M_PI*f_pi3)*(AmpOmegaPhi(s)+Amp(s))*(1-3*alphaK-alphaK*H);
  //  return sqrt(3)/(4*M_PI*M_PI*f_pi3)*(1)/(s)*(1-3*alphaK-alphaK*H);
}

T3piCrossPart::T3piCrossPart(double e, double de, double nth0):
  TCrossPart(e,de,nth0)
{
  const double m[] = { mpi, mpi, mpi0};
  const int pid[]  = { 211, -211, 111};
  double emax = e*(1 - pow((m[0]+m[1]+m[2])/(2*e),2));
  //  emax = e/2.5;
  fphot->SetEnergyRange(de,emax);
  std::cout<<de<<"MeV <E_gamma< "<<emax<<" MeV"<<std::endl;
  //  fphot->SetEnergyRange(de,0.5*e);
  SetFinalParticles(3, m, pid);
  std::cout<<"Process -- e^+e^- -> rho pi -> pi^+pi^-pi^0"<<std::endl;
}

void T3piCrossPart::SetJ(){

  TLorentzVector &qp = *fres[0];  //pi^+ momentum
  TLorentzVector &qm = *fres[1];  //pi^- momentum
  TLorentzVector &q0 = *fres[2];  //pi^0 momentum

  double qpqm = qp*qm;
  double qpq0 = qp*q0;
  double qmq0 = qm*q0;

  // dipion invariant masses
  double Q02 = 2*(mpi2 + qpqm);
  double Qp2 = mpi2 + mpi02 + 2*qmq0;
  double Qm2 = mpi2 + mpi02 + 2*qpq0;

  complex_t F = F3pi(fq2,Q02,Qp2,Qm2);

  J3PseudoScalars();

  fJc *= F;
}

bool T3piCrossPart::Accepted(){
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

