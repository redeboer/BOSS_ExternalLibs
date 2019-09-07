#include "TMuCrossPart.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"

TMuCrossPart::TMuCrossPart():TVCrossPart(){
  fNPart = 5;

  fK     = new TMuKFun();
  fK->Init(gGlobal->GetDatadir(), gGlobal->GetIntFname());
  fD     = new TDFun();
  fEvent = new TEvent();
  std::cout<<"Coloumb factor is "<<fK->Eval_Coloumb_Factor(gGlobal->Get_s())<<std::endl;

  // soft photons 
  fPhD1  = new TPhotonD();
  fPhD2  = new TPhotonD();

  // initial hard photons
  fPhI1  = new TPhotonI();
  fPhI2  = new TPhotonI();
  
  // final hard photons with spherical angle distribution
  fPhFS  = new TPhotonFS();

  // vacuum polarization
  fVPol  = new TVacuumPol();
  fVPol->Init(gGlobal->GetDatadir(), gGlobal->GetVpolFname());

  fBetaMu = gGlobal->Get_BetaF();
  fCompMuSV = (1+fBetaMu*fBetaMu)/(2*fBetaMu)*log((1+fBetaMu)/(1-fBetaMu))-1;

  //  cout<<DB(fBetaMu)<<DB(fCompMuSV)<<endl;

  fHardPhoton = true;
  fZeroVP     = false;
  fNoFSR      = false;
}

void TMuCrossPart::SetThetaMin(const double &th){
  fCosMin  = cos(th);
  fBetaMu2 = gGlobal->Get_BetaF()*gGlobal->Get_BetaF();
  fMax     = 2 - fBetaMu2*(1 - fCosMin*fCosMin);
  fNorm    = 2*fCosMin*(2 - fBetaMu2 + fCosMin*fCosMin*fBetaMu2/3)* // Theta norm
    2*gConst->Pi(); // Phi norm  
}

double TMuCrossPart::MakeCosTheta(){
  // generate distribution (2 - beta^2 sin^2(theta))dcos(theta)
  double c,s2;
  do{
    c = (2*gRandom->Rndm()-1)*fCosMin;
    s2 = 1 - c*c;
  }while(2 - fBetaMu2*s2 < fMax*gRandom->Rndm());
  fSinTheta2 = s2;
  return c;
}

double TMuCrossPart::GetCNorm(){
  return fNorm/(2 - fBetaMu2*fSinTheta2);
}

void TMuCrossPart::Init(){
  SetThetaMin(gGlobal->Get_ThetaMin());

  fD->Init();
  fEvent->Init();

  fPhD1->Init();
  fPhD2->Init();

  fPhI1->Init();
  fPhI2->Init();

  fPhFS->Init();

  if(fZeroVP) 
    fVPol->SetZeroVP();
  else 
    fVPol->SetDefaultVP();
}

TMuCrossPart::~TMuCrossPart(){
  delete fK;
  delete fD;
  delete fEvent;

  delete fPhD1;
  delete fPhD2;

  delete fPhI1;
  delete fPhI2;

  delete fPhFS;
  
  delete fVPol;
}

double TMuCrossPart::BornShift(const double &z1, const double &z2, const double &c){
  double Y1, y1, c1;
  fEvent->GetPPar(0,Y1,y1,c1);
  double sw = gGlobal->Get_s()*z1*z2;
  std::complex<double> vpsw  = fVPol->Evals(sw);
  double yc = y1*c;
  double ym = z1*(Y1-yc);
  double yp = z2*(Y1+yc);
  double cs = std::abs(vpsw*std::conj(vpsw));
  cs*= y1*(ym*ym + yp*yp + 2*z1*z2*(Y1*Y1 - y1*y1))/(z1*z1*z1*z2*z2*z2*(z1 + z2 - (z1 - z2)*c*Y1/y1));
  //  if(cs<0)
  //#define DB(x) #x<<"="<<x<<", "
  //  std::cout<<DB(Y1)<<DB(y1)<<DB(c1)<<DB(z1)<<DB(z2)<<DB(c)<<std::endl;
  return cs*0.25;
}

// define compensator term for initial particles
#define COMP_I(x) AP*(-(1/x-1+0.5*x)*gGlobal->Get_LnD()+0.5*x)

double TMuCrossPart::GetValue(const unsigned int ipart){
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
    // one photon out of the narrow cones with singularity around initial particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhI1,2)){
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*R_e_Gamma();
      double norm = fPhI1->GetENorm()*fPhI1->GetThNorm()*fPhI1->GetPhiNorm()*GetCNorm();
      return t1*norm;
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
  
  if(fEvent->MakeEvent(c, x1g, x2g)){
    
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
    
    if(CompSV && !fNoFSR ){
      t2 = 2*AP*gGlobal->Get_LnDelta()*(fCompMuSV + 2*log((1-fBetaMu*c)/(1+fBetaMu*c)));
    }

    double kfactor, coloumbfactor;
    if(fNoFSR){
      kfactor = 2*AP*fK->Eval_a();
      coloumbfactor = 1;
    }else{
      kfactor = 2*AP*fK->Eval(z1*z2*gGlobal->Get_s(),c);
      coloumbfactor = fK->Eval_Coloumb_Factor(z1*z2*gGlobal->Get_s());
    }
    double t1 = coloumbfactor*(1 + kfactor)*D1*D2;

    return BornShift(z1,z2,c)*(t1+t2)*GetCNorm();
    
  }
  return 0;
}

double TMuCrossPart::R_e_Gamma(){
  // Equation 2.12

  double em,pm,cm, ep,pp,cp, eg,pg,cg;
  fEvent->GetPPar(0,em,pm,cm);
  fEvent->GetPPar(1,ep,pp,cp);
  fEvent->GetPPar(2,eg,pg,cg);

  double s, s1,  t, t1,  u, u1;
  s  = 4;
  s1 = 4*(1-eg);

  double BetaI = gGlobal->Get_BetaI();
  t  = -2*(em-BetaI*pm*cm);
  t1 = -2*(ep+BetaI*pp*cp);

  u  = -2*(ep-BetaI*pp*cp);
  u1 = -2*(em+BetaI*pm*cm);
      
  double chi_m  = eg*(1-BetaI*cg);
  double chi_p  = eg*(1+BetaI*cg);
  double ichi_m = 1/chi_m;
  double ichi_p = 1/chi_p;

  double chi_m_p=0, chi_p_p=0, ichi_m_p=0, ichi_p_p=0;
  if(!fNoFSR){
    chi_m_p = 2*(1-ep);
    chi_p_p = 2*(1-em);
    ichi_m_p = 1/chi_m_p;
    ichi_p_p = 1/chi_p_p;
  }

  double is  = 1/s;
  double is1 = 1/s1;

  double mmu2 = gGlobal->Get_MF2();
  double me2  = gGlobal->Get_MI2();

  double B = (u*u + u1*u1 + t*t + t1*t1)*(0.25*is*is1);
  double D_s1_s1 = ((t+u)*(t+u)+(t1+u1)*(t1+u1))*(0.5*ichi_m*ichi_p);
  double R_e = s*B*(ichi_m*ichi_p) 
    - me2*(0.5*ichi_m*ichi_m*is1*is1)*(t1*t1 + u1*u1 + 2*mmu2*s1) 
    - me2*(0.5*ichi_p*ichi_p*is1*is1)*(t*t   + u*u   + 2*mmu2*s1)
    + mmu2*(is1*is1)*D_s1_s1;

  double R_mu   = 0;
  double R_e_mu = 0;
  if(!fNoFSR){
    double D_s_s  = -(u*u + t1*t1 + 2*s*mmu2)*(0.5*ichi_m_p*ichi_m_p)
      -(u1*u1 + t*t + 2*s*mmu2)*(0.5*ichi_p_p*ichi_p_p)
      +(s*s1 - s*s + t*u + t1*u1 - 2*s*mmu2)*(ichi_m_p*ichi_p_p);
    R_mu   = s1*B*(ichi_m_p*ichi_p_p)+mmu2*(is*is)*D_s_s;
    
    double C      = 
      u*(ichi_m*ichi_p_p) + u1*(ichi_p*ichi_m_p) -
      t*(ichi_m*ichi_m_p) - t1*(ichi_p*ichi_p_p);
    double D_s_s1 = 0.5*(s+s1)*C + 2*(u-t1)*ichi_m_p + 2*(u1-t)*ichi_p_p;
    R_e_mu = B*C + mmu2*(is*is1)*D_s_s1;
  }

  double CosPsi = fEvent->GetCosPsi();
  double gamma = eg*pm/(2-eg*(1-em*CosPsi/pm));

  std::complex<double> vp_s1 = fVPol->Evals(gGlobal->Get_s()*(1-eg));
  double vp_s1_s1 = std::abs(vp_s1*vp_s1);
  
  double vp_s_s   = 0;
  double vp_s1_cs = 0;
  if(!fNoFSR){
    std::complex<double> vp_s  = fVPol->Evals(gGlobal->Get_s());
    vp_s_s  = std::abs(vp_s*vp_s);
    vp_s1_cs= std::real(vp_s1*std::conj(vp_s));
  }

  return (vp_s1_s1*R_e + vp_s1_cs*R_e_mu + vp_s_s*R_mu)*gamma;
}

double TMuCrossPart::R_f_Gamma(){
  double em,pm,cm, ep,pp,cp, eg,pg,cg;
  fEvent->GetPPar(0,em,pm,cm);
  fEvent->GetPPar(1,ep,pp,cp);
  fEvent->GetPPar(2,eg,pg,cg);

  double s, s1,  t, t1,  u, u1;
  s  = 4;
  s1 = 4*(1-eg);

  double BetaI = gGlobal->Get_BetaI();
  t  = -2*(em-BetaI*pm*cm);
  t1 = -2*(ep+BetaI*pp*cp);

  u  = -2*(ep-BetaI*pp*cp);
  u1 = -2*(em+BetaI*pm*cm);

  double chi_m, chi_p, chi_m_p, chi_p_p;
  chi_m   = eg*(1-BetaI*cg);
  chi_p   = eg*(1+BetaI*cg);
  chi_m_p = 2*(1-ep);
  chi_p_p = 2*(1-em);

  double mmu2 = gGlobal->Get_MF2();
  double B = (u*u + u1*u1 + t*t + t1*t1)/(4*s*s1);
  double D_s_s = -(u*u + t1*t1 + 2*s*mmu2)/(2*chi_m_p*chi_m_p)
    -(u1*u1 + t*t + 2*s*mmu2)/(2*chi_p_p*chi_p_p)
    +(s*s1 - s*s + t*u + t1*u1 - 2*s*mmu2)/(chi_m_p*chi_p_p);
  double R_mu = s1*B/(chi_m_p*chi_p_p)+mmu2/(s*s)*D_s_s;
  
  double C = u/(chi_m*chi_p_p) + u1/(chi_p*chi_m_p) - t/(chi_m*chi_m_p) - t1/(chi_p*chi_p_p);
  double D_s_s1 = 0.5*(s+s1)*C + 2*(u-t1)/chi_m_p + 2*(u1-t)/chi_p_p;
  double R_e_mu = B*C + mmu2/(s*s1)*D_s_s1;


  double CosPsi = fEvent->GetCosPsi();
  double gamma = eg*pm/(2-eg*(1-em*CosPsi/pm));

  std::complex<double> vp_s  = fVPol->Evals(gGlobal->Get_s());
  double vp_s_s  = std::abs(vp_s*vp_s);
  std::complex<double> vp_s1 = fVPol->Evals(gGlobal->Get_s()*s1*0.25);
  double vp_s1_cs= std::real(vp_s1*std::conj(vp_s));

  double RG_f = gamma*(vp_s_s*R_mu + vp_s1_cs*R_e_mu);
  return RG_f;  
}

double TMuCrossPart::R_e_Gamma_t(){
  double em,pm,cm, ep,pp,cp, eg,pg,cg;

  fEvent->GetPPar(0,em,pm,cm);
  fEvent->GetPPar(1,ep,pp,cp);
  fEvent->GetPPar(2,eg,pg,cg);
  
  double betam = pm/em;
  double betap = pp/ep;

  double mmu2 = gGlobal->Get_MF2();
  double egamma2 = 1/gGlobal->Get_MI2();

  double Ae = mmu2*(1-eg+eg*eg/4*(1+cg*cg))+ep*em*(1-(1-eg)*betap*cp*betam*cm)
    +(1-eg)*eg*cg*eg*cg/2+(1-eg/2)*((2-eg)*(1-eg)-2*ep*em);

  double Be =-1/(1+egamma2*(1-cg*cg))*((1-eg)*(mmu2*ep*em*(1-betap*cp*betam*cm)-2)+
				       ep*ep*(1+cg*betap*cp)+
				       em*em*(1+cg*betam*cm));

  double CosPsi = fEvent->GetCosPsi();
  double gamma = eg*pm/(2-eg*(1-em*CosPsi/pm));

  double BetaI = gGlobal->Get_BetaI();
  double RG_i = (Ae+Be)*gamma/((eg*eg*(1-eg)*(1-eg))*(1-BetaI*BetaI*cg*cg));
  
  std::complex<double> vp_s1 = fVPol->Evals(gGlobal->Get_s()*(1-eg));
  double vp_s1_s1 = std::abs(vp_s1*vp_s1);
  
  return vp_s1_s1*RG_i;
}

double TMuCrossPart::R_e_mu_Gamma_t(){
  TLorentzVector pm(0,0,gGlobal->Get_BetaI(),1.);
  TLorentzVector pp(0,0,-gGlobal->Get_BetaI(),1.);
  TLorentzVector qm = fEvent->Get4Vector(0);
  TLorentzVector qp = fEvent->Get4Vector(1);
  TLorentzVector k  = fEvent->Get4Vector(2);

  double s  = (pp + pm)*(pp + pm);
  double s1 = (qp + qm)*(qp + qm);

  double t  = -2*pm*qm;
  double t1 = -2*pp*qp;
  double u  = -2*pm*qp;
  double u1 = -2*pp*qm;

  double chi_p   = pp*k;
  double chi_m   = pm*k;
  double chi_p_p = qp*k;
  double chi_m_p = qm*k;
  
  double ichi_m   = 1/chi_m;
  double ichi_p   = 1/chi_p;
  double ichi_m_p = 1/chi_m_p;
  double ichi_p_p = 1/chi_p_p;

  TLorentzVector Q = qm*ichi_m_p - qp*ichi_p_p;
  TLorentzVector P = pp*ichi_p   - pm*ichi_m;

  double mmu2 = gGlobal->Get_MF2();
  double me2  = gGlobal->Get_MI2();

  double cA  = 2*mmu2*s     + u*u1       + t*t1;
  double cPp = 2*mmu2*chi_p - u1*chi_p_p - t1*chi_m_p;
  double cPm = 2*mmu2*chi_m - u *chi_m_p - t *chi_p_p;

  // Equation 2.9
  double A_e = 
    0.5*s*ichi_p*ichi_m*cA + ichi_m*cPp + ichi_p*cPm -
    0.5*(u1*ichi_p - t*ichi_m)*(u-t1) -
    0.5*(t1*ichi_p - u*ichi_m)*(t-u1) -
    0.5*s*ichi_p*(2*mmu2 + ichi_m*cPp) -
    0.5*s*ichi_m*(2*mmu2 + ichi_p*cPm);

  double B_e = -me2*(0.5*(ichi_p*ichi_p + ichi_m*ichi_m)*cA -
  		     ichi_p*ichi_p*cPm - ichi_m*ichi_m*cPp);

  double R_e = A_e + B_e; R_e *= s/(s1*s1);

  // Equation 2.10
  double A_mu = 
    ((cA - (u1 + t1)*chi_m - (u + t)*chi_p)*qm*qp - 
     4*mmu2*chi_p*chi_m)*(ichi_p_p*ichi_m_p) - 
    (t1*chi_m + u*chi_p)*ichi_m_p - 
    (u1*chi_m + t*chi_p)*ichi_p_p +
    0.5*(t1*ichi_p_p - u1*ichi_m_p)*(t-u) + 
    0.5*(u*ichi_p_p - t*ichi_m_p)*(u1-t1);

  double B_mu = 
    -0.5*cA*mmu2*(ichi_p_p*ichi_p_p + ichi_m_p*ichi_m_p) +
    mmu2*((t1*chi_m + u*chi_p)*(ichi_m_p*ichi_m_p) + 
	  (u1*chi_m + t*chi_p)*(ichi_p_p*ichi_p_p));
  /*
  // Equation 2.13
  double C_mu = me2*( 2*mmu2*s1*ichi_p_p*ichi_m_p 
		      + (t + u1)*ichi_m_p + (t1 + u)*ichi_p_p 
		      + 4*mmu2*(ichi_m_p + ichi_p_p));
  */
  double C_mu = 0;
  double R_mu = A_mu + B_mu + C_mu; R_mu /= s;

  double C = u*(ichi_m*ichi_p_p) + u1*(ichi_p*ichi_m_p) - 
    t*(ichi_m*ichi_m_p) - t1*(ichi_p*ichi_p_p);

  // Equation 2.11
  double R_e_mu = 0.5*cA*C - 
    2*( -t - t1 + u + u1 - 
	0.5*u1*(pm*ichi_m - qp*ichi_p_p )*(Q*chi_p_p + P*chi_m) - 
	0.5*t1*(pm*ichi_m - qm*ichi_m_p )*(Q*chi_m_p - P*chi_m) - 
	mmu2*(chi_p + chi_m )*(Q*P) + 
	mmu2*(ichi_p_p - ichi_m_p )*(chi_m - chi_p) - 
	0.5*t*(qp*ichi_p_p - pp*ichi_p )*(Q*chi_p_p - P*chi_p) -
	0.5*u*(qm*ichi_m_p - pp*ichi_p )*(Q*chi_m_p + P*chi_p) );
  R_e_mu /= s1;

  double CosPsi = fEvent->GetCosPsi();
  double gamma = k.E()*qm.P()/(2-k.E()*(1-qm.E()*CosPsi/qm.P()));

  std::complex<double> vp_s1 = fVPol->Evals(gGlobal->Get_s()*s1*0.25);
  std::complex<double> vp_s  = fVPol->Evals(gGlobal->Get_s());
  double vp_s1_s1 = std::abs(vp_s1*vp_s1);
  double vp_s_s   = std::abs(vp_s*vp_s);
  double vp_s1_cs = std::real(vp_s1*std::conj(vp_s));

  //  cout<<DB(gamma*R_e_mu*0.25)<<endl;
  //  return gamma*R_e_mu*vp_s1_cs*0.25;

  return gamma*(R_e*vp_s1_s1 + R_mu*vp_s_s + R_e_mu*vp_s1_cs)*0.25;
}
