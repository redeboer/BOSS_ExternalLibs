#include <iostream>
#include "TKcFormFactor.h"
#include "TConstants.h"

TKcFormFactor::TKcFormFactor():TFormFactor(){
  fM_rho  = 775.8;
  fM_rho2 = fM_rho*fM_rho;
  fG_rho  = 150.3;
  fB_rho_ee = 4.67e-5;

  fM_omega = 782.59;
  fM_omega2 = fM_omega*fM_omega;
  fG_omega = 8.49;
  fB_omega_ee = 7.14e-5;

  fM_phi = 1019.456;
  fM_phi2 = fM_phi*fM_phi;
  fG_phi = 4.26;
  fB_phi_ee = 2.98e-4;

  fB_phi[0] = 0.492;   // phi->K+ K-
  fB_phi[1] = 0.337;   // phi->K_S K_L
  fB_phi[2] = 0.155;   // phi->pi+ pi- pi0
  fB_phi[3] = 0.01299; // phi->eta pi0
  fB_phi[4] = 1.23e-3; // phi->pi0 gamma
  fB_phi[5] = 7.3e-5;  // phi->pi+ pi-

  fB_omega[0] = 0.891;  // omega->pi+ pi- pi0
  fB_omega[1] = 0.087;  // omega->pi0 gamma
  fB_omega[2] = 0.017;  // omega->pi+ pi-
  fB_omega[3] = 4.9e-4; // omega->eta gamma

  fB_rho[0] = 0.9987386; // rho->pi+ pi-
  fB_rho[1] = 6e-4;      // rho->pi0 gamma
  fB_rho[2] = 3e-4;      // rho->eta gamma

  // phase space parametrization of 3pi from A.S.Popov
  double lm[] = {1.1, 0.875, 0.75, 0.62, 0.52, 0.46};
  for(int i=0;i<6;i++)f_range[i] = lm[i];

  double b[]  = { 5.29354148,   -7.90990714,   -2.26007613,  5.21453902};
  double a[]  = {-6.12394622,   25.0341405,   -34.1311022,  15.5413717};
  double a1[] = {-4.91624401,   19.8655606,   -26.9136128,  12.2412286};
  double c[]  = {-0.56436115,    2.69953793,   -4.32966739,  2.33116866};
  double d[]  = {-0.0548334238,  0.31600391,   -0.609523718, 0.393667808};
  double d1[] = {-0.00794774189, 0.0522269164, -0.114526409, 0.0838126536};
  
  for(int i=0;i<4;i++){
    f_array[0][i] = b[i];
    f_array[1][i] = a[i];
    f_array[2][i] = a1[i];
    f_array[3][i] = c[i];
    f_array[4][i] = d[i];
    f_array[5][i] = d1[i];
  }

  for(int i=0;i<5;i++){
    double *t = &f_array[5-i][0];
    double temp1 = t[0] + (t[1] + (t[2] + t[3]*lm[4-i])*lm[4-i])*lm[4-i];
    double *r = &f_array[4-i][0];
    double temp2 = (r[1] + (r[2] + r[3]*lm[4-i])*lm[4-i])*lm[4-i];
    r[0] = temp1 - temp2;
  }
  
}

std::complex<double> TKcFormFactor::ff(const double &s){
  std::complex<double> arho = sqrt(phiwidth(fM_phi2)*rhowidth(fM_rho2)*pow(fM_rho,3)*fM_phi2*6*M_PI*fB_rho_ee*fB_phi[1])/std::complex<double>(fM_rho2-s, -sqrt(s)*rhowidth(s));
  std::complex<double> aomega = sqrt(phiwidth(fM_phi2)*omegawidth(fM_omega2)*pow(fM_omega,3)*fM_phi2*6*M_PI*fB_omega_ee*fB_phi[1])/std::complex<double>(fM_omega2-s, -sqrt(s)*omegawidth(s));
  double smax = 2043.0;
  std::complex<double> aphi = phiwidth(fM_phi2)*pow(fM_phi,3)*sqrt(smax/0.389379292e12*fM_phi)/std::complex<double>(fM_phi2-s,-sqrt(s)*phiwidth(s));
  std::complex<double> atot = -aphi - aomega - arho;
  double cs = csborn(s);
  //  std::cout<<phiwidth(fM_phi2)<<" "<<omegawidth(fM_omega2)<<std::endl;
  //  std::cout<<atot<<" "<<aphi<<" "<<aomega<<" "<<arho<<std::endl;
  //  std::cout<<pow(sqrt(s),5)<<" "<<qkpkm(s)<<" "<<qkpkm(fM_phi2)<<" "<<z(s)<<" "<<z(fM_phi2)<<" "<<cs<<std::endl;
  //  std::cout<<0.389379e12/pow(sqrt(s),5)*qkpkm(s)/qkpkm(fM_phi2)*z(s)/z(fM_phi2)/cs<<std::endl;
  if(cs>0){
    std::complex<double> res = atot*sqrt(0.389379e12/pow(sqrt(s),5)*qkpkm(s)/qkpkm(fM_phi2)*z(s)/z(fM_phi2)/cs);
    //    std::cout<<arho<<" "<<aomega<<" "<<aphi<<" "<<z(fM_phi2)<<" "<<cs<<" "<<qkpkm(s)<<" "<<z(s)<<std::endl;
    //    std::cout<<sqrt(s)/2<<" "<<res*z(fM_phi2)*cs<<std::endl;
    //    exit(0);
    return res;
  } else {
    std::complex<double> res = atot*sqrt(0.389379e12/pow(sqrt(s+1),5)*qkpkm(s+1)/qkpkm(fM_phi2)*z(s+1)/z(fM_phi2)/csborn(s+1));
    //    std::cout<<arho<<" "<<aomega<<" "<<aphi<<" "<<z(fM_phi2)<<" "<<cs<<" "<<qkpkm(s)<<" "<<z(s)<<std::endl;
    //    std::cout<<sqrt(s)/2<<" "<<res*z(fM_phi2)*cs<<std::endl;
    return res;    
  }
}

double TKcFormFactor::rhowidth(const double& s){
  return fG_rho*fM_rho2/s*
    fB_rho[0]*pow(sqrt((s/4.0-gConst->Mpi2())/(fM_rho2/4.0-gConst->Mpi2())),3) + 
    fG_rho*fB_rho[1]*qpgamma(s,1)/qpgamma(fM_rho2,1) +
    fG_rho*fB_rho[2]*qpgamma(s,2)/qpgamma(fM_rho2,2) +
    0.5*phiwidth(fM_phi2)*fM_phi2*
    (
     fB_phi[0]*qkpkm(s)/qkpkm(fM_phi2) +
     fB_phi[1]*qklks(s)/qklks(fM_phi2)
     )/s;
}

double TKcFormFactor::omegawidth(const double& s){

  double temp5 = pow(0.5*sqrt(s - 4*gConst->Mpi2()),3);
  double temp6 = pow(0.5*sqrt(fM_omega2 - 4*gConst->Mpi2()),3);

  return 
    fG_omega*
    (
     fB_omega[0]*phase_space_3pi(s)/phase_space_3pi(fM_omega2) +
     fB_omega[1]*qpgamma(s,1)/qpgamma(fM_omega2,1) +
     fB_omega[2]*temp5/temp6*fM_omega2/s +
     fB_omega[3]*qpgamma(s,2)/qpgamma(fM_omega2,2)
     ) +
    0.5*phiwidth(fM_phi2)*fM_phi2*
    (
     fB_phi[0]*qkpkm(s)/qkpkm(fM_phi2)/s +
     fB_phi[1]*qklks(s)/qklks(fM_phi2)/s
     );
}

double TKcFormFactor::phiwidth(const double& s){
  double w1 = phase_space_3pi(s);
  double w2 = phase_space_3pi(fM_phi2);
  return fM_phi2*fG_phi*
    (
     fB_phi[0]*qkpkm(s)/qkpkm(fM_phi2) + 
     fB_phi[1]*qklks(s)/qklks(fM_phi2) +
     fB_phi[5]*pow(sqrt((s/4-gConst->Mpi2())/(fM_phi2/4 - gConst->Mpi2())),3)
     )/s + 
    fG_phi*
    (
     fB_phi[2]*w1/w2 + 
     fB_phi[4]*qpgamma(s,1)/qpgamma(fM_phi2,1) +
     fB_phi[3]*qpgamma(s,2)/qpgamma(fM_phi2,2)
     );
}

double TKcFormFactor::qklks(const double& s){
  double M2 = 4*gConst->MK02();
  return (s<M2)?0:pow(0.5*sqrt(s - M2),3);
}

double TKcFormFactor::qkpkm(const double& s){
  double M2 = 4*gConst->MKC2();
  return (s<M2)?0:pow(0.5*sqrt(s - M2),3);
}

double TKcFormFactor::qpgamma(const double& s, const int& mode){
  //====================================================
  //  MODE is a type of Pseudo-Scalar meson :
  //       = 1 means pi0
  //       = 2 means eta
  //====================================================
  switch (mode){
  case 1:
    return (s>gConst->Mpi2())?pow((1-gConst->Mpi2()/s)*sqrt(s)/2,3):0;
    break;
  case 2:
    return (s>gConst->Meta2())?pow((1-gConst->Meta2()/s)*sqrt(s)/2,3):0;
    break;
  default:
    return 0;
    break;    
  }
}

double TKcFormFactor::z(const double& s){
  double mkc24 = 4*gConst->MKC2();
  double is = 1/s;
  return (s<=mkc24)?0:1 + 0.5*gConst->Alpha()*M_PI*(2 - mkc24*is)/sqrt(1 - mkc24*is);
}

double TKcFormFactor::csborn(const double& s){
  double beta = sqrt(1 - 4*gConst->MKC2()/s);
  return M_PI*gConst->Alpha2()*pow(beta,3)/(3*s)*0.389379e12;
}

double TKcFormFactor::phase_space_3pi(const double& s){
  double x = sqrt(s)*1e-3;

  int k = 0;
  if (f_range[0] <= x ){
    k = 0;
  } else if( x < f_range[5] ){
    k = 5;
  } else {
    for(int i=0;i<4;i++){
      if( f_range[i+1] <= x && x < f_range[i] ){
	k = i+1;
	break;
      }
    }
  }
  double pol = 0;
  for(int i=0;i<4;i++)
    pol += pow(x,i)*f_array[k][i];
  return pol/0.393728*(0.00749/0.0361478); // norm to omega
}
