#include "TRadGlobal.h"
#include <iostream>
#include <stdexcept>

TRadGlobal::TRadGlobal(){
  // Set error
  fRelativeError = 0.1;
  fTotalError = -1;

  // Set general parameters
  fE      = -1;
  fThetaMin = -1;
  fType   = -1;
  // Set auxiliary parameters
  fdE     = -1;
  fdE_Rel = -1;

  fTheta0 = -1;
  fTheta0_Rel = -1;

  fThetaInt = -1;
  fInfraRedCut = 1e-10;
}

TRadGlobal::~TRadGlobal(){}

void TRadGlobal::Default(){
  // Set general parameters
  fThetaMin = 0.975;

  // Set auxiliary parameters
  fdE     = 2.;
  fTheta0 = 2;
  fThetaInt = 0.473;
}

void TRadGlobal::Init(){
  // Set masses of inital and final particles
  fMI     = gConst->Me();
  switch(fType){
  case 0:
    fMF     = gConst->Me();
    break;
  case 1:
    fMF     = gConst->Mmu();
    break;
  case 2:
    fMF     = gConst->Mpi();
    break;
  case 3:
    fMF     = gConst->MK0();
    break;
  case 4:
    fMF     = gConst->MKC();
    //    throw std::logic_error("Charged kaon is not really yet implemented! Sorry...");
    break;
  case 5:
    fMF     = gConst->Mtau();
    break;
  case 6:
    fMF     = 0;
    break;
  default:
    throw std::logic_error("Wrong or no process type is defined!");
    break;
  }

  if(fE<=fMF) 
    throw std::logic_error("Energy is not enough to produce final particles!");

  // Test for general parameters
  if(0>fE) throw std::logic_error("Energy is not defined!");

  if(0>fThetaMin) throw std::logic_error("ThetaMin is not defined!");

  // Test for auxiliary parameters
  if(0>fdE)
    if(0>fdE_Rel) 
      throw std::logic_error("dE is not defined!");
    else
      fdE = fdE_Rel*fE;

  if(0>fTheta0)
    if(0>fTheta0_Rel)
      throw std::logic_error("Theta0 is not defined!");
    else
      fTheta0 = fTheta0_Rel*sqrt(fMI/fE);
  
  if(0>fThetaInt)
      throw std::logic_error("Theta Intermediate is not defined!");
  
  if(0>fTotalError)
      throw std::logic_error("Total error of cross section is not defined!");
  
  fs = 4.*fE*fE;

  fMI2    = 4.*fMI*fMI/fs;
  fBetaI  = sqrt(fE+fMI)*sqrt(fE-fMI)/fE;
  fGammaI = fE/fMI;
	  
  fMF2    = 4.*fMF*fMF/fs;
  fBetaF  = sqrt(fE+fMF)*sqrt(fE-fMF)/fE;
  fGammaF = fE/fMF;

  fXMin   = fdE/fE;
  fLnXMin = log(fXMin);
  
  if(fMF<gConst->Me())
    fXMax   = 1 - pow(gConst->Me()/fE,2);
  else
    fXMax   = 1 - fMF2;
    
  //  fXMax   = 0.999;
  fLnXMax = log(fXMax);
  
  fLnR    = log(fXMax/fXMin);

  fCosTheta0 = cos(fTheta0);
  long double c = fBetaI*fCosTheta0;
  fLnD  = log((1l+c)/(1l-c));

  fL    = 2.*log(2.*fE/fMI);
  fBeta = 2.*gConst->AlphaPi()*(fL-1.);
  //  fD0   = 1.+3./8.*fBeta-fBeta*fBeta/48.*(gConst->Pi2() + fL/3. - 47./8.);
  fD0   = 1.+3./8.*fBeta+fBeta*fBeta/16.*(9./8. - gConst->Pi2()/3);
  fb    = pow(fXMin,0.5*fBeta)*fD0;

  fbRed = pow(fInfraRedCut,0.5*fBeta)*fD0;
  
  fCosThetaInt = cos(fThetaInt);
  fCosThetaMin = cos(fThetaMin);
}

void TRadGlobal::Print(void){
  using namespace std;
  cout
    <<"E                = "<<fE<<" MeV"<<endl
    <<"Abs dE           = "<<fdE<<" MeV"<<endl
    <<"Rel dE           = "<<fdE/fE<<"*E MeV"<<endl 
    <<"Abs Theta0       = "<<fTheta0<<" rad"<<endl
    <<"Rel Theta0       = "<<fTheta0/sqrt(fMI/fE)<<"*sqrt(Me/E) rad"<<endl
    <<"Minimal Theta    = "<<fThetaMin<<" rad"<<endl
    <<"Beta             = "<<fBeta<<endl
    <<"b                = "<<fb<<endl
    <<"L                = "<<fL<<endl
    <<"LnD              = "<<fLnD<<endl
    <<"XMax             = "<<fXMax<<endl
    <<flush;
}
