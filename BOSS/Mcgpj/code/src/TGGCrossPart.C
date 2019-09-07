#include <iostream>
#include "TGGCrossPart.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"

using namespace std;

#define DB(x) #x<<"="<<x<<", "

TGGCrossPart::TGGCrossPart():TVCrossPart(){
  fNPart = 5;

  fD     = new TDFun();
  fEvent = new TEvent();

  // soft photons 
  fPhD1  = new TPhotonD();
  fPhD2  = new TPhotonD();

  // initial hard photons
  fPhI1  = new TPhotonI();
  fPhI2  = new TPhotonI();
  
  fHardPhoton = true;
  fNoFSR      = false;
  fThetaLocked = false;
}

void TGGCrossPart::SetThetaMin(const double &th){
  if(fThetaLocked)return;
  //  cout<<"ThetaMin = "<<th<<endl;
  fCosMin  = cos(th);
  fMax     = (1 + fCosMin*fCosMin)/(1 - fCosMin*fCosMin);
  double cmax = fCosMin, cmin = -fCosMin;
  fNorm    = (cmin-cmax - log((1-cmax)/(1-cmin))+log((1+cmax)/(1+cmin))) * // Theta norm
    2*gConst->Pi(); // Phi norm  
  //  cout<<DB(fCosMin)<<DB(fMax)<<DB(fNorm)<<endl;
}

double TGGCrossPart::MakeCosTheta(){
  // generate distribution (1 + cos^2(theta))/(1 - cos^2(theta))dcos(theta)
  double c,c2;
  do{
    c = (2*gRandom->Rndm()-1)*fCosMin;
    c2 = c*c;
  }while( (1+c2) < (1-c2)*fMax*gRandom->Rndm());
  fNormTheta = fNorm*(1-c2)/(1+c2);
  return c;
}

double TGGCrossPart::GetCNorm(){
  return fNormTheta;
}

void TGGCrossPart::Init(){
  SetThetaMin(gGlobal->Get_ThetaMin());
  //  SetThetaMin(gGlobal->Get_Theta0());
  //  fThetaLocked = true;

  fD->Init();
  fEvent->Init();

  fPhD1->Init();
  fPhD2->Init();

  fPhI1->Init();
  fPhI2->Init();
}

TGGCrossPart::~TGGCrossPart(){
  delete fD;
  delete fEvent;

  delete fPhD1;
  delete fPhD2;

  delete fPhI1;
  delete fPhI2;
}

double TGGCrossPart::BornShift(const double &z1, const double &z2, 
			       const double &c){
  double cs = 2*(pow(z1*(1-c), 2) + pow(z2*(1+c), 2))/
    ((1-c*c)*pow(z1+z2+(z2-z1)*c, 2)*z1*z2);
  return 0.5*cs;
}

// define compensator term for initial particles
#define COMP_I(x) AP*(-(1/x-1+0.5*x)*gGlobal->Get_LnD()+0.5*x)
//#define COMP_I(x) 0

double TGGCrossPart::GetValue(const unsigned int ipart){
  bool IsSoftIE, IsSoftIP;
  IsSoftIE = IsSoftIP = true;

  bool CompSV, CompIE, CompIP;
  CompSV = CompIE = CompIP = false;
  double AP = gConst->AlphaPi();

  double c  = MakeCosTheta();

  switch(ipart){
  case 0:
    IsSoftIE = IsSoftIP = true;
    CompSV   = true;
    break;
  case 1:
    IsSoftIP = false;
    CompIP   = true;
    break;
  case 2:
    IsSoftIE = false;
    CompIE   = true;
    break;
  case 3:
    IsSoftIE = IsSoftIP = false;
    break;
  case 4:
    // three photons out of narrow cones around initial particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhI1,3)){
      double r3g = R_3_Gamma();
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*r3g;
      double norm = fPhI1->GetENorm()*fPhI1->GetThNorm()*
	fPhI1->GetPhiNorm()*GetCNorm();
      double res = t1*norm;
//       if(res>10000){
// 	TLorentzVector q[3];
// 	for(int i=0;i<3;i++)
// 	  q[i] = fEvent->Get4Vector(i);
// 	double CosPsi = fEvent->GetCosPsi();
// 	double gamma = q[2].E()*q[0].P()/(2 - q[2].E()*(1-CosPsi));
// 	double gamma2 = q[2].E()*q[0].E()/q[1].E();
// 	cout<<DB(res)<<DB(r3g)<<DB(t1)<<DB(norm)<<DB(CosPsi)<<DB(gamma)<<DB(q[1].E())<<DB(gamma2)<<endl;
// 	fEvent->Print();
//       }
      
      res *= 3; // The third photon always has the lowest energy

      return 0.5*res;
    }
    return 0;
    break;
  case 5:
 // Soft and virtial part for one photon cross-section (testing purpose only)
    if(fEvent->MakeEventgg(c, (double)0, (double)0)){
      double lde = gGlobal->Get_LnDelta();
      double Ae = (gGlobal->Get_L()-1)*(lde + 3./4.); 
      return BornShift(1,1,c)*(1 + AP*(2*Ae + K_SV(c)))*GetCNorm();
    }
    return 0;
    break;
  default:
    return 0;
    break;
  }

  if(!fHardPhoton) CompSV = CompIE = CompIP  = false;

  double  x1g, x2g;
  x1g = x2g = 0;

  double  z1, z2;
  z1 = z2 = 1;

  if(IsSoftIE)
    x1g = fPhD1->GetEnergy(); 
  else
    x1g = fPhI1->GetEnergy();
  z1 = 1 - x1g;
  
  if(IsSoftIP)
    x2g = fPhD2->GetEnergy(); 
  else
    x2g = fPhI2->GetEnergy();
  z2 = 1 - x2g;
  
  if(fEvent->MakeEventgg(c, x1g, x2g)){
    
    double D1, D2, t2 = 0;

    if(IsSoftIE)
      D1 = fD->EvalSoft(x1g)*fPhD1->GetENorm();
    else{
      double norm = fPhI1->GetENorm();
      D1 = fD->Eval(z1)*norm;
      if(CompIE) t2 = COMP_I(x1g)*norm;
    }
    
    if(IsSoftIP)
      D2 = fD->EvalSoft(x2g)*fPhD2->GetENorm();
    else{
      double norm = fPhI2->GetENorm();
      D2 = fD->Eval(z2)*norm;
      if(CompIP) t2 = COMP_I(x2g)*norm;
    }
    
    if(CompSV){
      t2 = 0;//AP*2*gGlobal->Get_LnDelta();
    }

    double t1 = (1 + AP*K_SV(c))*D1*D2;

    return BornShift(z1,z2,c)*(t1 + t2)*GetCNorm();
    
  }
  return 0;
}

double TGGCrossPart::R_3_Gamma(){
  TLorentzVector q[3];
  for(int i=0;i<3;i++) {
    q[i] = fEvent->Get4Vector(i);
    //    cout<<i<<" "<<q[i].E()<<" "<<q[i].P()<<" "<<q[i].Theta()<<" "<<q[i].Phi()<<endl;
  }
  TLorentzVector pm(0,0, gGlobal->Get_BetaI(),1.);
  TLorentzVector pp(0,0,-gGlobal->Get_BetaI(),1.);
  double chi[3], chi_p[3];
  for(int i=0;i<3;i++){
    chi[i]   = q[i]*pm;
    chi_p[i] = q[i]*pp;
    //    cout<<i<<" "<<chi[i]<<" "<<chi_p[i]<<endl;
  }

  double res = 0;
  double s = 4;
  double me2 = gGlobal->Get_MI2();
  double cycper[3];
  for(int i=0;i<3;i++){ // cyclic permutations
    int i0 = (0 + i)%3, i1 = (1 + i)%3, i2 = (2 + i)%3;
    
    double chi22 = pow(chi[i2],2);
    double chi_p22 = pow(chi_p[i2],2);
    cycper[i]  = s*(chi22 + chi_p22)/(chi[i0]*chi[i1]*chi_p[i0]*chi_p[i1]);
    cycper[i] -= 2*me2*
      ((pow(chi[i0],2)+pow(chi[i1],2))/(chi[i0]*chi[i1]*chi_p22)+
       (pow(chi_p[i0],2)+pow(chi_p[i1],2))/(chi_p[i0]*chi_p[i1]*chi22));
    res += cycper[i];
    //	cout<<DB(i)<<DB(j)<<DB(k)<<endl;
    
  }
  //  cout<<res<<endl;
  double CosPsi = fEvent->GetCosPsi();
  double gamma = q[2].E()*q[0].P()/(2 - q[2].E()*(1-CosPsi));
  //  double gamma = q[2].E()*q[0].E()/q[1].E();
//   if(res/3*gamma*0.25>1e7){
//     cout<<DB(CosPsi)<<DB(gamma)<<endl;
//     cout<<DB(cycper[0])<<DB(cycper[1])<<DB(cycper[2])<<endl;
//     cout<<DB(chi[0])<<DB(chi[1])<<DB(chi[2])<<endl;
//     cout<<DB(chi_p[0])<<DB(chi_p[1])<<DB(chi_p[2])<<endl;
//     for(int i=0;i<3;i++)cout<<q[i][0]<<" "<<q[i][1]<<" "<<q[i][2]<<" "<<q[i][3]<<endl;
//     cout<<q[0].Theta()<<" "<<q[1].Theta()<<" "<<q[2].Theta()<<endl;
//   }
  
  return res/3*gamma*0.25;
}

double TGGCrossPart::K_SV(const double &c){
  double R = (1+c)/(1-c);
  double iR = 1/R;
  double lmc = log(0.5*(1-c));
  double lpc = log(0.5*(1+c));
  return gConst->Pi2()/3 + (1-c*c)/(2*(1+c*c))*
    ( (1 + 1.5*R)*lmc + (1 + iR + 0.5*R)*lmc*lmc +
      (1 + 1.5*iR)*lpc + (1 + R + 0.5*iR)*lpc*lpc );
}
