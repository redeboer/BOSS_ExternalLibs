#include <iostream>

#include "T5piCrossPart.h"
#include "TConstant.h"
#include "TUtil.h"
#include <complex>

typedef std::complex<double> complex_t;
using namespace std;
using namespace rb;

inline complex_t Rrho(double s, bool IsIsospinBreak){
  complex_t Drho = complex_t(Mrho2-s, -Mrho*Grho);
  complex_t prop_rho = Mrho2/Drho;
  return prop_rho;
}

inline complex_t bw_om(double s){
  complex_t Domega = complex_t(Momega2-s,-Momega*Gomega);
  complex_t bw = Momega2/Domega;
  return bw;
}

inline complex_t bw_b1(double s){
  complex_t Db1 = complex_t(Mb12-s,-Mb1*Gb1);
  complex_t bw = Mb12/Db1;
  return bw;
}

T5piCrossPart::T5piCrossPart(double e, double de, double nth0, int mode):
  TCrossPart(e,de,nth0),fmode(mode)
{
  const double m_0[] = { mpi, mpi, mpi, mpi, mpi0};
  const int pid_0[]  = { 211, -211, 211, -211, 111};
  const double m_1[] = { mpi, mpi, mpi0, mpi0, mpi0};
  const int pid_1[]  = { 211, -211, 111, 111, 111};
  double emax = e;
  if(fmode==0) {
    emax *= 1 - pow((m_0[0]+m_0[1]+m_0[2]+m_0[3]+m_0[4])/(2*e),2);
    SetFinalParticles(5, m_0, pid_0);
    std::cout<<"Process -- e^+e^- -> b1^+- pi^-+"<<std::endl;
    std::cout<<"                      |-> omega pi^+-"<<std::endl;
    std::cout<<"                            |-> rho^+-0 pi^-+0"<<std::endl;
    std::cout<<"                                  |-> pi pi"<<std::endl;
  }else{
    emax *= 1 - pow((m_1[0]+m_1[1]+m_1[2]+m_1[3]+m_1[4])/(2*e),2);
    SetFinalParticles(5, m_1, pid_1);
    std::cout<<"Process -- e^+e^- -> b1^0 pi^0"<<std::endl;
    std::cout<<"                      |-> omega pi^0"<<std::endl;
    std::cout<<"                            |-> rho^+-0 pi^-+0"<<std::endl;
    std::cout<<"                                  |-> pi pi"<<std::endl;
  }
  fphot->SetEnergyRange(de,emax);
}

// TLorentzVectorC J5pi(const TLorentzVector &q1,
// 		     const TLorentzVector &q2,
// 		     const TLorentzVector &q3, 
// 		     const TLorentzVector &q4,
// 		     const TLorentzVector &q5){
//   TLorentzVector qom = q3 + q4 + q5; 
//   TLorentzVector qb1 = qom + q2;
//   complex_t pom = bw_om(qom.M2());
//   TLorentzVector tom = PseudoScalars3(q3,q4,q5);

//   complex_t pb1 = bw_b1(qb1.M2());

//   complex_t prop = pom*pb1;

//   TLorentzVector res0
//     = qom*(( q1*q2 )*(qb1*tom) - (qb1*q2 )*( q1*tom))
//     +  q2*((qb1*qom)*( q1*tom) - ( q1*qom)*(qb1*tom))
//     + tom*(( q1*qom)*(qb1*q2 ) - (qb1*qom)*( q1*q2 ));

//   return TLorentzVectorC(res0.X()*prop,
// 			 res0.Y()*prop,
// 			 res0.Z()*prop,
// 			 res0.T()*prop);
// }
TLorentzVectorC J5pi(const TLorentzVector &q1,
		     const TLorentzVector &q2,
		     const TLorentzVector &q3, 
		     const TLorentzVector &q4,
		     const TLorentzVector &q5){
  TLorentzVector qom = q3 + q4 + q5;
  TLorentzVector qp = q3 + q5;
  TLorentzVector qm = q4 + q5;
  TLorentzVector q0 = q3 + q4;
  TLorentzVector qb1_0 = qom + q2;
  TLorentzVector qb1_1 = qom + q1;
  complex_t pom = bw_om(qom.M2());
  pom *= Rrho(qp.M2(),false) + Rrho(qm.M2(),false) + Rrho(q0.M2(),true);

  TLorentzVector tom = PseudoScalars3(q3,q4,q5);

  complex_t pb1_0 = bw_b1(qb1_0.M2());
  complex_t pb1_1 = bw_b1(qb1_1.M2());

  complex_t prop_0 = pom*pb1_0;
  complex_t prop_1 = pom*pb1_1;
  double    q1q2   =    q1*q2;
  double    q1tom  =    q1*tom;
  double qb1_0qom  = qb1_0*qom;
  double qb1_0tom  = qb1_0*tom;
  double qb1_0q2   = qb1_0*q2;
  double    q1qom  =    q1*qom;
  TLorentzVector res0
    = qom*((   q1q2 )*(qb1_0tom) - (qb1_0q2 )*(   q1tom))
    +  q2*((qb1_0qom)*(   q1tom) - (   q1qom)*(qb1_0tom))
    + tom*((   q1qom)*(qb1_0q2 ) - (qb1_0qom)*(   q1q2 ));

  double    q2tom  =    q2*tom;
  double qb1_1qom  = qb1_1*qom;
  double qb1_1tom  = qb1_1*tom;
  double qb1_1q1   = qb1_1*q1;
  double    q2qom  =    q2*qom;
  TLorentzVector res1
    = qom*((   q1q2 )*(qb1_1tom) - (qb1_1q1 )*(   q2tom))
    +  q1*((qb1_1qom)*(   q2tom) - (   q2qom)*(qb1_1tom))
    + tom*((   q2qom)*(qb1_1q1 ) - (qb1_1qom)*(   q1q2 ));
  
  return TLorentzVectorC(res0.X()*prop_0 + res1.X()*prop_1,
			 res0.Y()*prop_0 + res1.Y()*prop_1,
			 res0.Z()*prop_0 + res1.Z()*prop_1,
			 res0.T()*prop_0 + res1.T()*prop_1);
}

void T5piCrossPart::SetJ(){
  TLorentzVector &q1 = *fres[0];  //pi^+ momentum
  TLorentzVector &q2 = *fres[1];  //pi^- momentum
  TLorentzVector &q3 = *fres[2];  //pi^+ momentum
  TLorentzVector &q4 = *fres[3];  //pi^- momentum
  TLorentzVector &q5 = *fres[4];  //pi^0 momentum
  
  if(fmode == 0){
    fJc  = J5pi(q1,q2,q3,q4,q5);
    fJc += J5pi(q3,q2,q1,q4,q5);
    fJc += J5pi(q3,q4,q1,q2,q5);
    fJc += J5pi(q1,q4,q3,q2,q5);
  } else {
    fJc  = J5pi(q3,q4,q1,q2,q5);
    fJc += J5pi(q5,q3,q1,q2,q4);
    fJc += J5pi(q5,q4,q1,q2,q3);
  }
  double fac3 = 1.45e-19;
  fJc *= fac3/fq2;
}

bool T5piCrossPart::Accepted(){
  //  if(fq2<0.5*fs) return false;
  return true;
}
