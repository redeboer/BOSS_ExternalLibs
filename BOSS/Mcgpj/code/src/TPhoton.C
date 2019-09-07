#include <iostream>
#include "TPhoton.h"
#define DB(x) #x<<"="<<x<<", "
using namespace std;
void TPhotonI::Init(){
  // Initializing photons in Initial state
  // by distribution 1/x/(1-x)
  fP0 = gGlobal->Get_XMin()/(1-gGlobal->Get_XMin());
  fP1 = gGlobal->Get_XMax()/(1-gGlobal->Get_XMax())/fP0;
  fNorm = log(fP1);
  fP1 = fNorm; // to increase speed of exponent in GetEnergy
  fBetaI = gGlobal->Get_BetaI();
  fLnD   = gGlobal->Get_LnD();
  //  cout<<DB(fP0)<<DB(fP1)<<DB(fNorm)<<DB(fBetaI)<<DB(fLnD)<<endl<<flush;
}

void TPhotonF::Init(){
  // Initializing photons in final state 
  // by distribution 1/x
  fP0 = gGlobal->Get_XMin();
  fP1 = gGlobal->Get_XMax()/fP0;
  fNorm = log(fP1);
  fP1 = fNorm; // to increase speed of exponent in GetEnergy
  fBetaI = gGlobal->Get_BetaI();
  fLnD   = gGlobal->Get_LnD();

  fiBetaI = 1/fBetaI;
  double c1 = gGlobal->Get_CosTheta0();
  double c2 = -1;// + 0.5*(1-c1);
  fBt    = (1 + fBetaI*c1)/(1 - fBetaI*c1);
  fLnD_n = log(1./fBt*(1. + fBetaI*c2)/(1. - fBetaI*c2));

  fBt2    = 1 - fBetaI*c1;
  fLnD_n2 = log((1. - fBetaI*c2)/(1 - fBetaI*c1));
  //  cout<<DB(fBt)<<DB(fLnD_n)<<DB(fLnD)<<endl<<flush;
  //  exit(0);
}

void TPhotonS::Init(){
  // Initializing photons in final state 
  // by distribution 1/x
  Init(gGlobal->Get_XMin(), gGlobal->Get_Beta());
  fBetaI = gGlobal->Get_BetaI();
  fLnD   = gGlobal->Get_LnD();
}

void TPhotonS::Init(const double & delta, const double & beta){
  // Initializing photons in final state 
  // by distribution 1/x
  fP0 = delta;
  fP1 = 2/beta;
  fP2 = pow(2/beta*pow(delta,-beta/2),4);
  fP3 = 1.-beta/2;
  fNorm = pow(pow(delta,beta/2),4);
  fBetaI = gGlobal->Get_BetaI();
  fLnD   = gGlobal->Get_LnD();
}

void TPhotonD::Init(){
  // Initializing photons in final state 
  // by distribution 1/x
  Init(gGlobal->Get_XMin(), gGlobal->Get_Beta());
  fBetaI = gGlobal->Get_BetaI();
  fLnD   = gGlobal->Get_LnD();
}

void TPhotonD::Init(const double & delta, const double & beta){
  // Initializing photons in final state 
  // by distribution 1/x
  fP0 = delta;
  fP1 = 2/beta;
  fNorm = pow(delta,beta/2);
  fBetaI = gGlobal->Get_BetaI();
  fLnD   = gGlobal->Get_LnD();
}
