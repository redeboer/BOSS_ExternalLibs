#include "TPhoton_o.h"
#include "TConstant.h"
using namespace rb;
TPhoton::TPhoton(double E){
  double gamma = E/me;
  fE = E;
  fBetaI = sqrt(gamma-1)*sqrt(gamma+1)/gamma;
  fiBetaI = 1/fBetaI;
  double c1 = -1, c2 = 1;
  SetCosThetaRange(c1,c2);
}

TPhoton::TPhoton(double E, double Emin, double Emax){
  double gamma = E/me;
  fE = E;
  fBetaI = sqrt(gamma-1)*sqrt(gamma+1)/gamma;
  fiBetaI = 1/fBetaI;
  double c1 = -1, c2 = 1;
  SetCosThetaRange(c1,c2);
  SetEnergyRange(Emin,Emax);
}

void TPhoton::SetEnergyRange(double Emin, double Emax){
  fP0 = Emin/fE;
  fP1 = Emax/fE/fP0;
  fNorm = log(fP1);
  fP1 = fNorm; // to increase speed of exponent in GetEnergy
  //    cout<<fP0<<" "<<fP1<<" "<<fNorm<<" "<<fLnD_n<<" "<<fBt<<endl;
}

void TPhoton::SetThetaRange(double thmin, double thmax){
  double c1 = cos(thmax), c2 = cos(thmin);
  SetCosThetaRange(c1,c2);
}

void TPhoton::SetCosThetaRange(double c1, double c2){
  fLnD_n = log((1-fBetaI*c1)/(1+fBetaI*c1)*(1+fBetaI*c2)/(1-fBetaI*c2));
  fBt = (1+fBetaI*c1)/(1-fBetaI*c1);
    //    cout<<c1<<" "<<c2<<" "<<fLnD_n<<" "<<fBt<<endl;
}

const TLorentzVector &TPhoton::GetNewPhoton(){
  double E = fE*GetEnergy();
  double c = GetCosThetaF();
  double s = sqrt(1-c*c);
  double phi = GetPhi();
  double cp,sp;
  sincos(phi,&sp,&cp);
  //    cout<<E<<" "<<c<<" "<<phi<<endl;
  fk.SetPxPyPzE(E*cp*s,E*sp*s,E*c,E);
  fknorm = GetENorm()*GetThNormF()*GetPhiNorm();
  return fk;
}
