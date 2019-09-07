#include <iostream>
#include "TEPCrossPart.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"
#include "TEKFun.h"
#include "TLi2.h"
using namespace std;
double maxmtr = -1;

TEPCrossPart::TEPCrossPart():TVCrossPart(){
  fNPart = 18;

  fK     = new TEKFun();
  fD     = new TDFun();
  fEvent = new TEvent();

  // soft photons 
  fPhD1  = new TPhotonD();
  fPhD2  = new TPhotonD();
  fPhD3  = new TPhotonD();
  fPhD4  = new TPhotonD();

  // initial and final hard photons
  fPhI1  = new TPhotonI();
  fPhI2  = new TPhotonI();
  fPhF3  = new TPhotonF();
  fPhF4  = new TPhotonF();

  // vacuum polarization
  fVPol  = new TVacuumPol();
  fVPol->Init(gGlobal->GetDatadir(), gGlobal->GetVpolFname());

  fHardPhoton = true;
  fZeroVP     = false;
}

void TEPCrossPart::SetThetaMin(const double &th){
  fCosMin  = cos(th);
  fSinMin2 = 1 - fCosMin*fCosMin;
  //  fMax     = pow((3 + fCosMin*fCosMin),2);
  //  double c = fCosMin;
  //  double s2 = fSinMin2;
  //  fNorm    = 2*( c*(75 - 26*c*c - c*c*c*c) + 24*s2*log((1-c)/(1+c)) )/(3*s2)* // Theta norm
  fNorm = 2*fCosMin/fSinMin2* // Theta norm
    2*gConst->Pi(); // Phi norm
}

double TEPCrossPart::MakeCosTheta(){
  //  double s,r[2];
  //  do{
  //    gRandom->RndmArray(2,r);
  //    fCosTheta = 1 - fSinMin2/(1+fCosMin*(2*r[0]-1));
  //    s = 3 + fCosTheta*fCosTheta;
  //  }while(s*s < fMax*r[1]);
  fCosTheta = 1 - fSinMin2/(1+fCosMin*(2*gRandom->Rndm() - 1));
  return fCosTheta;
}

double TEPCrossPart::GetCNorm(){
  //  double s = (1 - fCosTheta)/(3 + fCosTheta*fCosTheta); 
  double s = 1 - fCosTheta;
  double norm = s*s*fNorm;
  return norm;
}

void TEPCrossPart::Init(){
  SetThetaMin((gCut->ThetaMinM()<M_PI-gCut->ThetaMaxM())?gCut->ThetaMinM():M_PI-gCut->ThetaMaxM());

  fK->Init();
  fD->Init();
  fEvent->Init();

  fPhD1->Init();
  fPhD2->Init();
  fPhD3->Init();
  fPhD4->Init();

  fPhI1->Init();
  fPhI2->Init();
  fPhF3->Init();
  fPhF4->Init();

  if(fZeroVP) 
    fVPol->SetZeroVP();
  else 
    fVPol->SetDefaultVP();
}

TEPCrossPart::~TEPCrossPart(){
  delete fK;
  delete fD;
  delete fEvent;

  delete fPhD1;
  delete fPhD2;
  delete fPhD3;
  delete fPhD4;

  delete fPhI1;
  delete fPhI2;
  delete fPhF3;
  delete fPhF4;

  delete fVPol;  
}

double TEPCrossPart::BornShift(const double &z1, const double &z2, const double &c){
  double Y1 = fEvent->GetY(0);
  double sw = gGlobal->Get_s()*z1*z2;
  double tw = -0.5*gGlobal->Get_s()*z1*Y1*(1.-c);
  double a  = z1+z2-(z1-z2)*c;
  double a2 = a*a;
  std::complex<double> vptw  = fVPol->Evalt(tw);
  std::complex<double> vpsw  = fVPol->Evals(sw);

  double p1 = z1*(1-c);
  double p12 = p1*p1;
  double p2 = z2*(1+c);
  double p22 = p2*p2;
  double cs =  
    std::abs(vptw*vptw)*(a2+p22)/(2*p12) +
    std::abs(vpsw*vpsw)*(p12+p22)/(2*a2) -
    std::real(vptw*std::conj(vpsw))*p22/(a*p1);

  cs *= 4/a2;

  return cs;
}

// define compensator term for initial particles
#define COMP_I(x) AP*((1/x-1+0.5*x)*(-gGlobal->Get_LnD())+0.5*x)

// define compensator term for final particles
#define COMP_F(x) AP*((1/x-1+0.5*x)*(-gGlobal->Get_LnD()+2*log(1-x))+0.5*x)

double TEPCrossPart::GetValue(const unsigned int ipart){
  bool IsSoftIE, IsSoftIP, IsSoftFE, IsSoftFP;
  IsSoftIE = IsSoftIP = IsSoftFE = IsSoftFP = true;

  bool CompSV, CompIE, CompIP, CompFE, CompFP;
  CompSV = CompIE = CompIP = CompFE = CompFP = false;
#ifdef OLDSTYLE
  double b  = gGlobal->Get_b();
#endif
  double AP = gConst->AlphaPi();

#ifdef FIXP
  double c = 0.3;
  fCosTheta = c;
#else
  double c = MakeCosTheta();
#endif

  switch(ipart){
  case 0:
    IsSoftIE = IsSoftIP = IsSoftFE = IsSoftFP = true;
    CompSV   = true;
    break;
  case 1:
    IsSoftFP = false;
    CompFP   = true;
    break;
  case 2:
    IsSoftFE = false;
    CompFE   = true;
    break;
  case 3:
    IsSoftFP = IsSoftFE = false;
    break;
  case 4:
    IsSoftIP = false;
    CompIP   = true;
    break;
  case 5:
    IsSoftIP = IsSoftFP = false;
    break;
  case 6:
    IsSoftIP = IsSoftFE = false;
    break;
  case 7:
    IsSoftIP = IsSoftFE = IsSoftFP = false;
    break;
  case 8:
    IsSoftIE = false;
    CompIE   = true;
    break;
  case 9:
    IsSoftIE = IsSoftFP = false;
    break;
  case 10:
    IsSoftIE = IsSoftFE = false;
    break;
  case 11:
    IsSoftIE = IsSoftFE = IsSoftFP = false;
    break;
  case 12:
    IsSoftIE = IsSoftIP = false;
    break;
  case 13:
    IsSoftIE = IsSoftIP = IsSoftFP = false;
    break;
  case 14:
    IsSoftIE = IsSoftIP = IsSoftFE = false;
    break;
  case 15:
    IsSoftIE = IsSoftIP = IsSoftFE = IsSoftFP = false;
    break;
  case 16:
    // one photon out of the narrow cones with singularity about initial particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhI1,1)){
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*RGamma();
      double norm = fPhI1->GetENorm()*fPhI1->GetThNorm()*fPhI1->GetPhiNorm()*GetCNorm();
      return t1*norm;
    }
    return 0;
    break;
  case 17:
    // one photon out of the narrow cones with singularity about final particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhF3,0)){
      //      fEvent->CosPrint();
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*RGamma();
      double norm = fPhF3->GetENorm()*fPhF3->GetThNormF()*fPhF3->GetPhiNorm()*GetCNorm();
      //      cout<<DB(RGamma())<<DB(GetCNorm())<<DB(fPhF3->GetENorm())<<DB(fPhF3->GetThNorm())<<endl;
      /*      
      if(t1*norm>maxmtr){
	maxmtr = t1*norm;
	fEvent->Print();
	cout<<maxmtr<<" "<<t1<<" "<<norm<<endl<<flush;
      }
      */
      return t1*norm;
    }
    return 0;
    break;
  case 18:
    // Soft and virtial part for one photon cross-section (testing purpose only)
    if(fEvent->MakeEvent(c,0,0,0,0)){
      double lde = gGlobal->Get_LnDelta();
      return BornShift(1,1,c)*
	(1
	 + 2*AP*(gGlobal->Get_L()-1)*(2*lde + 1.5)
	 - 4*AP*log((1+c)/(1-c))*lde 
	 + AP*fK->Eval(1,1,c)
	 )*GetCNorm();
    }
    return 0;
    break;
  case 19:
    // preprint INP 78-82 soft and virtual part
    if(fEvent->MakeEvent(c,0,0,0,0)){
      double lde = gGlobal->Get_LnDelta();
      double rho = gGlobal->Get_L();
      double z = c;
      double z2 = z*z;
      double z3 = z2*z;
      double z4 = z3*z;
#define F(x) TLi2::Eval(x)
      return BornShift(1,1,c)*
	(1
	 + 2*AP*
	 (
	  2*(rho - 1 - log((1+z)/(1-z)))*lde - F(0.5*(1+z)) + F(0.5*(1-z)) - 23./9. + 
	  11./6.*rho + 
	  0.5*pow(3+z2,-2)*(
			    M_PI*M_PI/3.*(2*z4 - 3*z3 - 15*z) + 
			    0.5*(2*z4-3*z3+9*z2+3*z+21)*pow(log(0.5*(1-z)),2)-
			    (z4+z2-2*z)*pow(log(0.5*(1+z)),2) - 
			    (z3+4*z2+5*z+6)*pow(log((1+z)/(1-z)),2)+
			    1./3.*(11*z3+33*z2+21*z+111)*log(0.5*(1-z))+
			    (z3-3*z2+7*z-5)*log(0.5*(1+z))
			    )
	  )
	 )*GetCNorm();
#undef F
    }
    
    return 0;
    break;
  case 20:
    // one photon out of the narrow cones with singularity about initial particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhI1,1)){
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*RGamma_kuraev_eidelman();
      double norm = fPhI1->GetENorm()*fPhI1->GetThNorm()*fPhI1->GetPhiNorm()*GetCNorm();
      return t1*norm;
    }
    return 0;
    break;
  case 21:
    // one photon out of the narrow cones with singularity about final particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhF3,0)){
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*RGamma_kuraev_eidelman();
      double norm = fPhF3->GetENorm()*fPhF3->GetThNormF()*fPhF3->GetPhiNorm()*GetCNorm();

      if(t1*norm>maxmtr){
	maxmtr = t1*norm;
	fEvent->Print();
	RGamma_kuraev_eidelman(true);
	cout<<maxmtr<<" "<<t1<<" "<<norm<<endl<<flush;
      }

      return t1*norm;
    }
    return 0;
    break;
  default:
    return 0;
    break;
  }

  if(!fHardPhoton) CompSV = CompIE = CompIP = CompFE = CompFP = false;

  double RedCut = 1e-10;//gGlobal->Get_InfraRedCut();

  double  x1g, x2g, x3g, x4g;
  x1g = x2g = x3g = x4g = 0;

  double  z1, z2, z3, z4;
  z1 = z2 = z3 = z4 = 1;

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
  
  if(fEvent->MakeEvent(c,x1g,x2g,0,0)){

    double Beta2_m, D0_m;
    if(IsSoftFE){
      gGlobal->Get_VarPar(1, Beta2_m, D0_m);
      //      gGlobal->Get_VarPar(fEvent->GetY(0), Beta2_m, D0_m);
      x3g = fPhD3->GetEnergy(gGlobal->Get_XMin(), Beta2_m); 
    }else
      x3g = fPhF3->GetEnergy();
    z3 = 1 - x3g;
    
    double Beta2_p, D0_p;
    if(IsSoftFP){
      gGlobal->Get_VarPar(1, Beta2_p, D0_p);
      //      gGlobal->Get_VarPar(fEvent->GetY(1), Beta2_p, D0_p);
      x4g = fPhD4->GetEnergy(gGlobal->Get_XMin(), Beta2_p);
    }else
      x4g = fPhF4->GetEnergy();
    z4 = 1 - x4g;
      
    if(fEvent->MakeEventN(x3g,x4g)){

      double D1, D2, D3, D4, t2 = 0;

      if(IsSoftIE){
#ifdef OLDSTYLE
	D1 = b;
#else
	if(x1g>RedCut)
	  D1 = fD->EvalSoft(x1g)*fPhD1->GetENorm();
	else
	  D1 = fD->EvalSoft(RedCut)*fPhD1->GetENorm();
#endif
      }else{
	double norm = fPhI1->GetENorm();
	D1 = fD->Eval(z1)*norm;
	if(CompIE) t2 = COMP_I(x1g)*norm;
      }

      if(IsSoftIP)
#ifdef OLDSTYLE
	D2 = b;
#else
	if(x2g>RedCut)
	  D2 = fD->EvalSoft(x2g)*fPhD2->GetENorm();
	else
	  D2 = fD->EvalSoft(RedCut)*fPhD2->GetENorm();
#endif
      else{
	double norm = fPhI2->GetENorm();
	D2 = fD->Eval(z2)*norm;
	if(CompIP) t2 = COMP_I(x2g)*norm;
      }

      if(IsSoftFE)
#ifdef OLDSTYLE
	D3 = b;
#else
	if(x3g>RedCut)
	  D3 = fD->EvalSoft(x3g, Beta2_m, D0_m)*fPhD3->GetENorm();
	else
	  D3 = fD->EvalSoft(RedCut, Beta2_m, D0_m)*fPhD3->GetENorm();
#endif
      else{
	gGlobal->Get_VarPar(fEvent->GetY(0), Beta2_m, D0_m);
	double norm = fPhF3->GetENorm();
	D3 = fD->Eval(z3, Beta2_m, D0_m)*norm;
	if(CompFE) t2 = COMP_F(x3g)*norm;
      }

      if(IsSoftFP){
#ifdef OLDSTYLE
	D4 = b;
#else
	if(x4g>RedCut)
	  D4 = fD->EvalSoft(x4g, Beta2_p, D0_p)*fPhD4->GetENorm();
	else
	  D4 = fD->EvalSoft(RedCut, Beta2_p, D0_p)*fPhD4->GetENorm();
#endif
      }else{
	gGlobal->Get_VarPar(fEvent->GetY(1), Beta2_p, D0_p);
	double norm = fPhF4->GetENorm();
	D4 = fD->Eval(z4, Beta2_p, D0_p)*norm;
	if(CompFP) t2 = COMP_F(x4g)*norm;
      }

      if(CompSV)
	t2 = -4*AP*log((1+c)/(1-c))*gGlobal->Get_LnDelta();
      double Ksv;
      if(fHardPhoton)
	Ksv = fK->Eval(z1,z2,c);
      else
	Ksv = 0;
      double t1  = (1 + AP*Ksv)*D1*D2*D3*D4;

      return BornShift(z1,z2,c)*(t1+t2)*GetCNorm();
    }
  }
  return 0;
}

double TEPCrossPart::RGamma(){
  double em,pm,cm, ep,pp,cp, eg,pg,cg;
  fEvent->GetPPar(0,em,pm,cm);
  fEvent->GetPPar(1,ep,pp,cp);
  fEvent->GetPPar(2,eg,pg,cg);

  double  s, s1,  t, t1,  u, u1;
  s  = 4;
  s1 = 4*(1 - eg);

  double BetaI = gGlobal->Get_BetaI();
  t  = -2*(em - BetaI*pm*cm);
  t1 = -2*(ep + BetaI*pp*cp);

  u  = -2*(ep - BetaI*pp*cp);
  u1 = -2*(em + BetaI*pm*cm);

  double is  = 1/s;
  double is1 = 1/s1;
  double it  = 1/t;
  double it1 = 1/t1;

  double s_ev = gGlobal->Get_s()*0.25;
  std::complex<double> vpt,vpt1,vps,vps1;
  vpt  = fVPol->Evalt(s_ev*t);
  vpt1 = fVPol->Evalt(s_ev*t1);
  vps  = fVPol->Evals(s_ev*s);
  vps1 = fVPol->Evals(s_ev*s1);

  //  cout<<DB(vpt)<<DB(vpt1)<<DB(vps)<<DB(vps1)<<endl;
  double vpss, vps1s1, vptt, vpt1t1, vptct1, vpscs1, vptcs, vpt1cs1, vpt1cs, vptcs1;
  vpss    = std::real(vps*std::conj(vps));
  vps1s1  = std::real(vps1*std::conj(vps1));
  vptt    = std::real(vpt*std::conj(vpt));
  vpt1t1  = std::real(vpt1*std::conj(vpt1));

  vptct1  = std::real(vpt*std::conj(vpt1));
  vpscs1  = std::real(vps*std::conj(vps1));
  vptcs   = std::real(vpt*std::conj(vps));
  vpt1cs1 = std::real(vpt1*std::conj(vps1));
  vpt1cs  = std::real(vpt1*std::conj(vps));
  vptcs1  = std::real(vpt*std::conj(vps1));

  double chi_m,chi_p,chi_m_p,chi_p_p;
  chi_m   = eg*(1 - BetaI*cg);
  chi_p   = eg*(1 + BetaI*cg);
  chi_m_p = 2*(1 - ep);
  chi_p_p = 2*(1 - em);
  //  cout<<DB(chi_m)<<DB(chi_p)<<DB(chi_m_p)<<DB(chi_p_p)<<endl;
  double ichi_m   = 1/chi_m;
  double ichi_p   = 1/chi_p;
  double ichi_m_p = 1/chi_m_p;
  double ichi_p_p = 1/chi_p_p;

  double SS,S1S1,SS1,TT,T1T1,TT1,TS,TS1,T1S,T1S1;
  SS   = t*t + t1*t1 + u*u + u1*u1;
  S1S1 = SS;
  SS1  = SS*(t*chi_p*chi_p_p + t1*chi_m*chi_m_p - u*chi_p*chi_m_p - u1*chi_m*chi_p_p);
  TT   = s*s + s1*s1 + u*u + u1*u1;
  T1T1 = TT;
  TT1  = TT*(u*chi_p*chi_m_p + u1*chi_m*chi_p_p + s*chi_m_p*chi_p_p + s1*chi_m*chi_p);
  TS   = -0.5*( u*u + u1*u1 )*( s *(t  + s1) + t *(s  + t1) - u*u1 );
  TS1  = -0.5*( u*u + u1*u1 )*( t *(s1 + t1) + s1*(s  + t ) - u*u1 );
  T1S  =  0.5*( u*u + u1*u1 )*( t1*(s  + t ) + s *(s1 + t1) - u*u1 );
  T1S1 =  0.5*( u*u + u1*u1 )*( s1*(s  + t1) + t1*(s1 + t ) - u*u1 );
  //  cout<<DB(SS)<<DB(TT)<<DB(TS)<<DB(TS1)<<DB(T1S)<<DB(T1S1)<<endl;
  /*
  double R = 
    vpss*SS/(s*chi_m_p*chi_p_p) + 
    vps1s1*S1S1/(s1*chi_m*chi_p) -
    vptt*TT/(t*chi_p*chi_p_p) -
    vpt1t1*T1T1/(t1*chi_m*chi_m_p) + 
    (vptct1*TT1/(t*t1)-vpscs1*SS1/(s*s1))/(chi_m*chi_m_p*chi_p*chi_p_p) +
    vptcs*TS/(t*s*chi_m_p*chi_p*chi_p_p) +
    vpt1cs1*T1S1/(t1*s1*chi_m*chi_m_p*chi_p) -
    vpt1cs*T1S/(t1*s*chi_m*chi_m_p*chi_p_p) -
    vptcs1*TS1/(t*s1*chi_m*chi_p*chi_p_p);
  cout<<DB(R)<<endl;
  */
  double R = 
    vpss*SS*(is*ichi_m_p*ichi_p_p) + 
    vps1s1*S1S1*(is1*ichi_m*ichi_p) -
    vptt*TT*(it*ichi_p*ichi_p_p) -
    vpt1t1*T1T1*(it1*ichi_m*ichi_m_p) + 
    (vptct1*TT1*(it*it1) - vpscs1*SS1*(is*is1))*(ichi_m*ichi_m_p*ichi_p*ichi_p_p) +
    vptcs*TS*(it*is*ichi_m_p*ichi_p*ichi_p_p) +
    vpt1cs1*T1S1*(it1*is1*ichi_m*ichi_m_p*ichi_p) -
    vpt1cs*T1S*(it1*is*ichi_m*ichi_m_p*ichi_p_p) -
    vptcs1*TS1*(it*is1*ichi_m*ichi_p*ichi_p_p);
  /*
  double vptt1   = std::abs(vpt*vpt1);
  double vprtt1  = std::real(vpt*vpt1);
  double Rss1  = -vptt*TT/(t*chi_p*chi_p_p) - vpt1t1*T1T1/(t1*chi_m*chi_m_p)
    + vprtt1*TT/(t*t1)*(u/(chi_m*chi_p_p) + u1/(chi_p*chi_m_p)) 
    - vpscs1*SS/(s*s1)*
    (t/(chi_m*chi_m_p) + t1/(chi_p*chi_p_p) - 
     u/(chi_m*chi_p_p) - u1/(chi_p*chi_m_p)) +
    (vptcs*chi_m*TS/(t*s) + vpt1cs1*chi_p_p*T1S1/(t1*s1) - 
     vpt1cs*chi_p*T1S/(t1*s) - vptcs1*chi_m_p*TS1/(t*s1))/(chi_p*chi_m*chi_p_p*chi_m_p);
  double Rss   = (vpss*SS/s + vptt1*TT*s1/(t*t1))/(chi_p_p*chi_m_p);
  double Rs1s1 = (vps1s1*SS/s1 + vptt1*TT*s/(t*t1))/(chi_p*chi_m);
  double R1 = Rss + Rss1 + Rs1s1;
  */

  double me2 = gGlobal->Get_MI2();
  double st2 = (s+t)*(s+t);
  double b1 = 0.5*(vptt  *( s*s  +  st2)*it *it  + vpss  *( st2  +  t*t )* is*is ) + vptcs  *st2 *is *it;
  double st12 = (s+t1)*(s+t1);
  double b2 = 0.5*(vpt1t1*( s*s  + st12)*it1*it1 + vpss  *( st12 + t1*t1)* is*is ) + vptcs  *st12*is *it1;
  double s1t2 = (s1+t)*(s1+t);
  double b3 = 0.5*(vptt  *(s1*s1 + s1t2)*it *it  + vps1s1*(s1t2  +  t*t )*is1*is1) + vptcs1 *s1t2*is1*it;
  double s1t12 = (s1+t)*(s1+t1);
  double b4 = 0.5*(vpt1t1*(s1*s1 +s1t12)*it1*it1 + vps1s1*(s1t12 + t1*t1)*is1*is1) + vpt1cs1*s1t12*is1*it1;
  R -= 4*me2*(
	      (ichi_p_p*ichi_p_p)*b1 +
	      (ichi_m_p*ichi_m_p)*b2 +
	      (ichi_p  *ichi_p  )*b3 +
	      (ichi_m  *ichi_m  )*b4
	      );

  double CosPsi = fEvent->GetCosPsi();
  double gamma  = eg*pm/(2 - eg*(1 - em*CosPsi/pm));

  return R*gamma*0.25;
}

#define DB(x) #x<<"="<<x<<", "
double TEPCrossPart::RGamma_kuraev_eidelman(const bool &print){
  // preprint INP 78-82
  double em,pm,cm, ep,pp,cp, eg,pg,cg;
  fEvent->GetPPar(0,em,pm,cm);
  fEvent->GetPPar(1,ep,pp,cp);
  fEvent->GetPPar(2,eg,pg,cg);

  double s  = 4;
  double s1 = s*(1 - eg);

  double u1 = s*(-1. + 0.5*em*(1.-cm));
  double t1 = -0.5*s*em*(1.+cm);
  double t  = -0.5*s*em*(1.-cm);
  double u  = s*(-1 + eg + 0.5*em*(1+cm));
  double utilde  = s*(-1 + 0.5*em*(1+cm));
  double t1tilde = s*(-1 + eg +0.5*em*(1-cm));

  double BetaI   = gGlobal->Get_BetaI();
  double chi_m   = eg*(1 - BetaI*cg);
  double chi_p   = eg*(1 + BetaI*cg);
  double chi_m_p = 2*(1 - ep);
  double chi_p_p = 2*(1 - em);
  double me2  = gGlobal->Get_MI2();
  double R_i = 
    0.5*(u     *u1*pow(1/t1+1/s1,2) + s*s1/pow(t1     ,2) + 
	 t*t1     /pow(s1,2))*s*(-me2*(1-eg)/pow(chi_m,2) + (1+pow(1-eg,2))/(eg*chi_m)) +
    0.5*(utilde*u1*pow(1/t +1/s1,2) + s*s1/pow(t1tilde,2) + 
	 t*t1tilde/pow(s1,2))*s*(-me2*(1-eg)/pow(chi_p,2) + (1+pow(1-eg,2))/(eg*chi_p));

  double R_f = pow((3+cm*cm)/(1-cm),2)*s*(-me2/(4*pow(chi_p_p,2))-me2/(4*pow(chi_m_p,2)) + 
					  (1+pow(1-eg,2))/(4*eg)*(1/chi_m_p + 1/chi_p_p));
  
  double R = R_i + R_f;
  
  double CosPsi = fEvent->GetCosPsi();
  double gamma  = eg*pm/(2 - eg*(1 - em*CosPsi/pm));
  if(print)cout<<DB(R_i)<<DB(R_f)<<DB(gamma)<<DB(u)<<DB(u1)<<DB(utilde)<<DB(t)<<DB(t1)<<DB(t1tilde)<<DB(chi_m)<<DB(chi_p)<<DB(chi_m_p)<<DB(chi_p_p)<<endl<<flush;
  return R*gamma*0.25;
}
