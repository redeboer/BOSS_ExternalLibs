#include <iostream>

#include "T4piCrossPart.h"
#include "TConstant.h"
#include <complex>

typedef std::complex<double> complex_t;

using namespace std;
using namespace rb;

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

inline complex_t Rrho(double s, bool IsIsospinBreak){
  complex_t Drho = complex_t(Mrho2-s, -Mrho*Grho);
  complex_t prop_rho = Mrho2/Drho;
  return prop_rho;
}

inline complex_t bw_om(double s){
  complex_t Domega = complex_t(Momega*Momega-s,-Momega*Gomega);
  complex_t bw = 1./Domega;
  return bw;
}

T4piCrossPart::T4piCrossPart(double e, double de, double nth0):
  TCrossPart(e,de,nth0)
{
  const double m[] = { mpi, mpi, mpi0, mpi0};
  const int pid[]  = { 211, -211, 111, 111};
  double emax = e*(1 - pow((m[0]+m[1]+m[2]+m[3])/(2*e),2));
  fphot->SetEnergyRange(de,emax);
  SetFinalParticles(4, m, pid);
  std::cout<<"Process -- e^+e^- -> omega pi^0"<<std::endl;
  std::cout<<"                       |-> rho pi"<<std::endl;
  std::cout<<"                            |-> pi pi"<<std::endl;
}

void T4piCrossPart::SetJ(){
  TLorentzVector &q3 = *fres[0];  //pi^+ momentum
  TLorentzVector &q4 = *fres[1];  //pi^- momentum
  TLorentzVector &q1 = *fres[2];  //pi^0 momentum
  TLorentzVector &q2 = *fres[3];  //pi^0 momentum
  
  TLorentzVector q134 = q1 + q3 + q4; 
  TLorentzVector q234 = q2 + q3 + q4; 

  double q1_134 = q1*q134;
  double q3_134 = q3*q134;
  double q4_134 = q4*q134;
  double q2_234 = q2*q234;
  double q3_234 = q3*q234;
  double q4_234 = q4*q234;
  double q12 = q1*q2;
  double q13 = q1*q3;
  double q14 = q1*q4;
  double q23 = q2*q3;
  double q24 = q2*q4;
  double q34 = q3*q4;
  double q134_2 = q134*q134;
  double q234_2 = q234*q234;
//
//  from here Omega current
//
  complex_t Rrho34 = Rrho(q34,false);

  complex_t H1 = Rrho(q13,false) + Rrho(q14,false) + Rrho34;
  complex_t H2 = Rrho(q23,false) + Rrho(q24,false) + Rrho34;
  complex_t bwr = 1e-8;//bw_rho(fq2);
  complex_t bw2 = bwr*bw_om(q234_2)*H2;
  complex_t bw1 = bwr*bw_om(q134_2)*H1;
  //  cout<<qq2<<" "<<q134_2<<" "<<q234_2<<" "<<bw1<<" "<<bw2<<endl;

  complex_t f0 = bw1*(q3_134*q24 - q4_134*q23);
  complex_t f1 = bw2*(q3_234*q14 - q4_234*q13);
  complex_t f2 = bw1*(q4_134*q12 - q1_134*q24) + bw2*(q4_234*q12 - q2_234*q14);
  complex_t f3 = bw1*(q1_134*q23 - q3_134*q12) + bw2*(q2_234*q13 - q3_234*q12);

  complex_t Jx = q1.X()*f0 + q2.X()*f1 + q3.X()*f2 + q4.X()*f3;
  complex_t Jy = q1.Y()*f0 + q2.Y()*f1 + q3.Y()*f2 + q4.Y()*f3;
  complex_t Jz = q1.Z()*f0 + q2.Z()*f1 + q3.Z()*f2 + q4.Z()*f3;
  complex_t Jt = q1.T()*f0 + q2.T()*f1 + q3.T()*f2 + q4.T()*f3;
  //  ptlv(fJc);
  fJc.SetPxPyPzE(Jx,Jy,Jz,Jt);
  double fac3 = 5.5e3;
  fJc *= fac3/fq2;
}

bool T4piCrossPart::Accepted(){
  //  if(fq2<0.5*fs) return false;
  return true;
}
