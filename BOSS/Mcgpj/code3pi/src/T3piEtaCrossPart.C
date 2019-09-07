#include <iostream>

#include "T3piEtaCrossPart.h"
#include "TConstant.h"
#include <complex>

using namespace std;
using namespace rb;
typedef complex<double> complex_t;
inline complex_t operator+(const double &x, const complex_t &y) {
  return y + x;
}

inline complex_t operator-(const double &x, const complex_t &y) {
  return -(y - x);
}

inline complex_t Amp(double s){
  const complex_t I(0,1);
  const double Apsi = alpha*Gpsi/(3*Geepsi);
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

inline complex_t bw_rho(double s){
  const double sig = -0.1;
  complex_t Drho = complex_t(Mrho*Mrho-s,-Mrho*Grho);
  complex_t Drhop = complex_t(Mrhop*Mrhop-s,-Mrhop*Grhop);
  complex_t bw = (1./Drho + sig/Drhop);
//
// dumping factor added in omega part: to be substituted with a formfactor
//
  const double Et2 = (2.2*1e3)*(2.2*1e3);
  if(s>Et2){
    double r = Et2/s;
    bw *= r*r;
  }
  return bw;
}

inline complex_t bw_om(double s){
  complex_t Domega = complex_t(Momega*Momega-s,-Momega*Gomega);
  complex_t bw = 1./Domega;
  return bw;
}

T3piEtaCrossPart::T3piEtaCrossPart(double e, double de, double nth0):
  TCrossPart(e,de,nth0)
{
  const double m[] = { mpi, mpi, mpi0, meta};
  const int pid[]  = { 211, -211, 111, 221};
  fphot->SetEnergyRange(de,0.5*e);
  SetFinalParticles(4, m, pid);
  cout<<"Process -- e^+e^- -> omega eta -> pi^+pi^-pi^0 eta"<<endl;
}

void T3piEtaCrossPart::SetJ(){
  TLorentzVector &q3 = *fres[0];  //pi^+ momentum
  TLorentzVector &q4 = *fres[1];  //pi^- momentum
  TLorentzVector &q1 = *fres[2];  //pi^0 momentum
  TLorentzVector &q2 = *fres[3];  //eta momentum
  
  TLorentzVector q134 = q1 + q3 + q4; 

  double q1_134 = q1*q134;
  double q3_134 = q3*q134;
  double q4_134 = q4*q134;
  double q12 = q1*q2;
  double q13 = q1*q3;
  double q14 = q1*q4;
  double q23 = q2*q3;
  double q24 = q2*q4;
  double q34 = q3*q4;
  double q134_2 = q134*q134;
//
//  from here Omega current
//
  complex_t bw = bw_om(q134_2);

  complex_t f0 = bw*(q3_134*q24 - q4_134*q23);
  complex_t f1 = complex_t(0,0);
  complex_t f2 = bw*(q4_134*q12 - q1_134*q24);
  complex_t f3 = bw*(q1_134*q23 - q3_134*q12);

  complex_t Jx = q1.X()*f0 + q2.X()*f1 + q3.X()*f2 + q4.X()*f3;
  complex_t Jy = q1.Y()*f0 + q2.Y()*f1 + q3.Y()*f2 + q4.Y()*f3;
  complex_t Jz = q1.Z()*f0 + q2.Z()*f1 + q3.Z()*f2 + q4.Z()*f3;
  complex_t Jt = q1.T()*f0 + q2.T()*f1 + q3.T()*f2 + q4.T()*f3;

  fJc.SetPxPyPzE(Jx,Jy,Jz,Jt);

  fJc *= 1e-10*(1+Amp(fq2));
}

bool T3piEtaCrossPart::Accepted(){
  if(fq2<0.5*fs) return false;
  return true;
}
