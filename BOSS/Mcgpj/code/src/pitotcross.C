#include "TPiFormFactor.h"
#include "TRadGlobal.h"
#include "TConstants.h"
#include "TF1.h"
#include <iostream>
#include <complex>
#include <stdexcept>

using namespace std;

TPiFormFactor *fPi;
double isr(double *x, double *p){
  double nu = x[0];
  double s = p[0];
  double beta2 = p[1];
  complex<double> f = fPi->Eval(s*(1-nu));
  double fpi2 = abs(f*conj(f));
  double onemnu2 = pow(1-nu,2);
  return (1 + onemnu2)/(onemnu2*nu)*(beta2-nu)*sqrt((beta2-nu)/(1-nu))*fpi2;
}

#define Li2 TMath::DiLog
int main(unsigned int argc, char **argv){
  double E = 450;
  if(argc>1) E = atof(argv[1]);
  double Delta = 0.01;
  if(argc>2) Delta = atof(argv[2]);

  gConst = new TConstants();
  gGlobal = new TRadGlobal();
  gGlobal->Set_Type(2);
  gGlobal->Set_E(E);
  gGlobal->Set_ThetaMin(0.9);
  gGlobal->Set_dE_Abs(0.1);
  gGlobal->Set_Theta0_Rel(1);
  gGlobal->Set_ThetaInt(0.6);
  gGlobal->Set_TotalError(1.);
  gGlobal->Set_RelativeError(0.01);
  
  try{
    gGlobal->Init();
  }catch(std::logic_error lge){
    cout<<lge.what()<<endl;
    return 1;
  }
  
  fPi = new TPiFormFactor();
  fPi->Init();
  if(argc>3) fPi->SetUnitFF();
  double beta2 = pow(gGlobal->Get_BetaF(),2);
  double le = gGlobal->Get_L();
  double s = 4*E*E;
  cout<<E<<" "<<beta2<<" "<<le<<" "<<gGlobal->Get_MF2()<<endl;
  TF1 fisr("isr",isr,0,1,2);
  fisr.SetParameters(s,beta2);
  double sum = fisr.Integral(Delta, 1 - gGlobal->Get_MF2());
  complex<double> fpi = fPi->Eval(s);
  double sigmaBorn = M_PI*gConst->Alpha2()*pow(gGlobal->Get_BetaF(),3)/(3*s)*abs(fpi*conj(fpi));
  double sigmaISRhard = sum*(le-1)*gConst->Alpha3()/(3*s);
  double sigmaISRsoftvirt = 2*gConst->AlphaPi()*sigmaBorn*
    ((le-1)*log(Delta) + 3./4.*le - 1 + M_PI*M_PI/6);

  double beta = gGlobal->Get_BetaF();
  double bmp = (1.-beta)/(1.+beta);
  
  double lambda = (1.+beta2)/beta*
    (4.*Li2(bmp) + 2.*Li2(-bmp) - (3.*log(2./(1.+beta))+2.*log(beta))*log(1./bmp) ) -
    3.*log(4./(1.-beta2)) - 4.*log(beta) + 
    (5./4.*pow(1.+beta2, 2)-2.)/pow(beta,3)*log(1./bmp)+ 3./2.*(1.+beta2)/beta2;
  double sigmaFSR = 2*gConst->AlphaPi()*sigmaBorn*lambda/2;

  double hc2 = gConst->HC2()*1e7;
  cout<<hc2*sigmaISRhard<<" "<<hc2*sigmaISRsoftvirt<<" "<<hc2*sigmaFSR<<" "<<hc2*sigmaBorn<<endl;
  cout<<hc2*(sigmaISRhard + sigmaISRsoftvirt + sigmaFSR + sigmaBorn)<<endl;
  cout<<(sigmaISRhard + sigmaISRsoftvirt + sigmaFSR + sigmaBorn)/sigmaBorn<<endl;
}
