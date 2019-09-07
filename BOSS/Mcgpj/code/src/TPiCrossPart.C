#include <iostream>
#define DB(x) #x<<"="<<x<<", "
#include "TPiCrossPart.h"
#include "TPiFormFactor.h"
#include "TRadGlobal.h"

TPiCrossPart::TPiCrossPart():TVCrossPart(){
  fNPart = 5;

  fK     = new TPiKFun();
  fD     = new TDFun();
  fEvent = new TEvent();

  // soft photons 
  fPhD1  = new TPhotonD();
  fPhD2  = new TPhotonD();

  // initial hard photons
  fPhI1  = new TPhotonI();
  fPhI2  = new TPhotonI();
  
  // final hard photons with spherical angle distribution
  fPhFS  = new TPhotonFS();

  // Pion Formfactor
  fFpi   = new TPiFormFactor();
  
  fBetaPi = gGlobal->Get_BetaF();

  fCompPiSV = (1+fBetaPi*fBetaPi)/(2*fBetaPi)*log((1+fBetaPi)/(1-fBetaPi))-1;

  fHardPhoton = true;
  fZeroVP     = false;
  fNoFSR      = false;
  /*
  for(int i=0;i<=1000;i++){
    double c = -1 + 2.*i/1000.;
    std::cout<<"FPI "<<c<<" "<<fK->Eval_odd(gGlobal->Get_s(),c)<<std::endl;
  }
  */
}

void TPiCrossPart::SetThetaMin(const double &th){
  fCosMin  = cos(th);
  // integral of sin^2(theta)dcos(theta) from -fCosMin to fCosMin
  fNorm    = 2*fCosMin*(1 - fCosMin*fCosMin/3)* // Theta norm
    2*gConst->Pi();                           // Phi norm  
}

double TPiCrossPart::MakeCosTheta(){
  // generate distribution sin^2(theta)dcos(theta)
  double c;
  do{
    c = (2*gRandom->Rndm()-1)*fCosMin;
  }while(1 - c*c < gRandom->Rndm());
  fSinTheta2 = 1 - c*c;
  return c;
}

double TPiCrossPart::GetCNorm(){
  return fNorm/fSinTheta2;
}

void TPiCrossPart::Init(){
  SetThetaMin(gGlobal->Get_ThetaMin());

  fD->Init();
  fEvent->Init();

  fPhD1->Init();
  fPhD2->Init();

  fPhI1->Init();
  fPhI2->Init();

  fPhFS->Init();

  fFpi->Init();

  if(fZeroVP)
    fFpi->SetUnitFF();
  else 
    fFpi->SetDefaultFF();  
}

TPiCrossPart::~TPiCrossPart(){
  delete fK;
  delete fD;
  delete fEvent;

  delete fPhD1;
  delete fPhD2;

  delete fPhI1;
  delete fPhI2;
  
  delete fPhFS;
  
  delete fFpi;
}

double r_abs_max = 0, r_rel_max = 0;
double TPiCrossPart::BornShift(const double &z1, const double &z2, const double &c){
  double Y1, y1, c1;
  fEvent->GetPPar(0,Y1,y1,c1);
  std::complex<double> Fpi = fFpi->Eval(gGlobal->Get_s()*z1*z2);
  double Fpi2 = std::abs(Fpi*std::conj(Fpi));
  double cs = Fpi2*y1*y1*y1*(1-c*c)/(z1*z1*z2*z2*(z1+z2+(z2-z1)*c*Y1/y1));
  return cs*0.25;
}

// define compensator term for initial particles
#define COMP_I(x) AP*(-(1/x-1+0.5*x)*gGlobal->Get_LnD()+0.5*x)

double TPiCrossPart::GetValue(const unsigned int ipart){
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
    // one photon out of the narrow cones with singularity about initial particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhI1,2)){
      //#define TEST
#ifdef TEST
      double r0 = R_Gamma();
      //      double r1 = R_Gamma_t();
      double r1 = R_Gamma_kuraev_eidelman();
      if(r_abs_max<fabs(r1-r0)||r_rel_max<fabs((r1-r0)/r0)){
	std::cout<<DB(r0)<<DB(r1)<<DB(r1-r0)<<DB((r1-r0)/r0)<<std::endl;
	if(r_abs_max<fabs(r1-r0))r_abs_max = fabs(r1-r0);
	if(r_rel_max<fabs((r1-r0)/r0))r_rel_max = fabs((r1-r0)/r0);
      }
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*r1;
#else
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*R_Gamma();
#endif
      double norm = fPhI1->GetENorm()*fPhI1->GetThNorm()*fPhI1->GetPhiNorm()*GetCNorm();
      return t1*norm;
    }
    return 0;
    break;
  case 5:
    // Soft and virtial part for one photon cross-section (testing purpose only)
    if(fEvent->MakeEvent(c,(double)0.,(double)0.)){
      double lde = gGlobal->Get_LnDelta();
      double Ae = (gGlobal->Get_L()-1)*(lde + 3./4.); 
      // + (M_PI*M_PI/6 - 1) -- this part in Api
      double Api = (fCompPiSV + 2*log((1 - fBetaPi*c)/(1 + fBetaPi*c)))*lde
	+ fK->Eval(gGlobal->Get_s(),c); // fK contains charge-(even+odd) part
      //      std::cout<<DB(lde)<<DB(Ae)<<DB(Api)<<std::endl;
      return BornShift(1,1,c)*(1 + 2*AP*(Ae + Api))*GetCNorm();
    }
    return 0;
    break;
  case 6:
    // Soft and virtial part for one photon cross-section (testing purpose only)
    if(fEvent->MakeEvent(c,(double)0.,(double)0.)){
      double lde = gGlobal->Get_LnDelta();
      double rho_e = gGlobal->Get_L();
      return BornShift(1,1,c)*
	(1 + 2*AP*((rho_e-1)*lde+13./12.*rho_e - 14./9. + M_PI*M_PI/6.))*GetCNorm();
    }
    return 0;
    break;
  case 7:
    // one photon out of the narrow cones with singularity about initial particles
    if(fHardPhoton && fEvent->MakeEvent(c,fPhI1,2)){
      double t1 = gConst->Alpha()/(2*gConst->Pi2())*R_Gamma_kuraev_eidelman();
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
    
    if(CompSV && !fNoFSR) t2 = 2*AP*gGlobal->Get_LnDelta()*
    		 (fCompPiSV + 2*log((1 - fBetaPi*c)/(1 + fBetaPi*c)));

    double Ksv = 0;
    if(fNoFSR)
      Ksv = fK->Eval_a();
    else
      Ksv = fK->Eval(z1*z2*gGlobal->Get_s(),c);

    double t1 = (1 + 2*AP*Ksv)*D1*D2;
    
    return BornShift(z1,z2,c)*(t1+t2)*GetCNorm();
    
  }
  return 0;
}

double TPiCrossPart::R_Gamma(){
  double em,pm,cm, ep,pp,cp, eg,pg,cg;
  fEvent->GetPPar(0,em,pm,cm);
  fEvent->GetPPar(1,ep,pp,cp);
  fEvent->GetPPar(2,eg,pg,cg);
  //  cout<<DB(eg)<<DB(cg)<<endl;
  double s, s1,  t, t1,  u, u1;
  s  = 4;
  s1 = 4*(1-eg);

  double BetaI = gGlobal->Get_BetaI();
  t  = -2*(em-BetaI*pm*cm);
  t1 = -2*(ep+BetaI*pp*cp);

  u  = -2*(ep-BetaI*pp*cp);
  u1 = -2*(em+BetaI*pm*cm);
      
  double chi_m,chi_p,chi_m_p,chi_p_p;
  chi_m   = eg*(1-BetaI*cg);
  chi_p   = eg*(1+BetaI*cg);
  chi_m_p = 2*(1-ep);
  chi_p_p = 2*(1-em);

  double ichi_m   = 1/chi_m;
  double ichi_p   = 1/chi_p;
  double ichi_m_p = 1/chi_m_p;
  double ichi_p_p = 1/chi_p_p;
  double is       = 1/s;
  double is1      = 1/s1;
  
  //  cout<<DB(eg)<<DB(ichi_m)<<DB(ichi_p)<<endl;

  double mpi2 = gGlobal->Get_MF2();
  double me2  = gGlobal->Get_MI2();

  double D_s1_s1, D_s_s, D_s_s1;
  D_s1_s1 = D_s_s = D_s_s1 = 0;
  double R_s1_s1, R_s_s, R_s_s1;
  R_s1_s1 = R_s_s = R_s_s1 = 0;

  double A = (t*u + t1*u1)*(is*is1);
  D_s1_s1 =-4*( (t+u)*(t+u) + (t1+u1)*(t1+u1) )*(is1*is1*ichi_m*ichi_p);
  R_s1_s1 = 4*s*A*(ichi_m*ichi_p) + mpi2*D_s1_s1
    -8*me2*(is1*is1)*( t1*u1*(ichi_m*ichi_m) + t*u*(ichi_p*ichi_p))
    +8*mpi2*me2*is1*((ichi_m*ichi_m) + (ichi_p*ichi_p));
  //    +4*me2*is*is1*is1*(s*s1-(t-u)*(t1-u1))*((t1+u1)*(ichi_m*ichi_m)+(t+u)*(ichi_p*ichi_p))
  //    -8*mpi2*me2*(is1*is1)*((t1+u1)*(ichi_m*ichi_m)+(t+u)*(ichi_p*ichi_p));
  //    - 8*me2*(is1*is1)*( t1*u1*(ichi_m*ichi_m) + t*u*(ichi_p*ichi_p) );
  //  R_s1_s1 = 4*(ichi_m*ichi_p*is1*is1)*
  //    (s1*(t*u + t1*u1) - mpi2*( (t+u)*(t+u) + (t1+u1)*(t1+u1)) 
  //     -2*me2*( t1*u1*(ichi_m*chi_p) + t*u*(ichi_p*chi_m)));

  if(!fNoFSR){
    D_s_s = 2*mpi2*(s-s1)*(s-s1)*is*(ichi_m_p*ichi_m_p*ichi_p_p*ichi_p_p)+
      8*is*is*(t*t1+u*u1-s*s-s*s1)*(ichi_m_p*ichi_p_p);
    R_s_s = 4*s1*A*(ichi_m_p*ichi_p_p) + mpi2*D_s_s
      -8*mpi2*(is*is)*( t*u1*(ichi_p_p*ichi_p_p) + t1*u*(ichi_m_p*ichi_m_p) );
    
    double C = 
      t*ichi_m*ichi_m_p + t1*ichi_p*ichi_p_p - 
      u*ichi_m*ichi_p_p - u1*ichi_p*ichi_m_p;
    D_s_s1 = 8*is*is1*((2*(t1 - u) + u1 - t)*chi_m_p + (2*(t - u1) + u - t1)*ichi_p_p +
		       0.5*(u1 + t1 - s)*ichi_m*( u*ichi_p_p -  t*ichi_m_p) +
		       0.5*(u  + t  - s)*ichi_p*(u1*ichi_m_p - t1*ichi_p_p));
    R_s_s1 = -4*A*C + mpi2*D_s_s1;
  }

  double CosPsi = fEvent->GetCosPsi();
  double gamma = eg*pm/( 2 - eg*(1 - em*CosPsi/pm) );

  std::complex<double> Fpi_s1 = fFpi->Eval(gGlobal->Get_s()*(1-eg));
  double Fpi_s1_s1 = std::abs(Fpi_s1*Fpi_s1);

  double Fpi_s_s, Fpi_s_cs1;
  Fpi_s_s = Fpi_s_cs1 = 0;
  if(!fNoFSR){
    std::complex<double> Fpi_s  = fFpi->Eval(gGlobal->Get_s());
    Fpi_s_s  = std::abs(Fpi_s*Fpi_s);
    Fpi_s_cs1= std::real(Fpi_s*std::conj(Fpi_s1));
  }
  //  Fpi_s1_s1 = 0; Fpi_s_s = 0;  Fpi_s_cs1 = 1;

  return (Fpi_s1_s1*R_s1_s1 + Fpi_s_s*R_s_s + Fpi_s_cs1*R_s_s1)*gamma/16;
}

#define Prod(a,b,c,d)  ( (a##b)*(c##d) + (a##d)*(b##c) - (a##c)*(b##d) )
#define Dot(a,b) double a##b = a*b; double b##a = a##b
double TPiCrossPart::R_Gamma_t(){
  // RGamma for testing purpose only
  TLorentzVector pm(0,0,gGlobal->Get_BetaI(),1.);
  TLorentzVector pp(0,0,-gGlobal->Get_BetaI(),1.);
  TLorentzVector qm = fEvent->Get4Vector(0);
  TLorentzVector qp = fEvent->Get4Vector(1);
  TLorentzVector k  = fEvent->Get4Vector(2);

  double s  = (pp + pm)*(pp + pm);
  double s1 = (qp + qm)*(qp + qm);

  double chi_p   = pp*k;
  double chi_m   = pm*k;
  double chi_p_p = qp*k;
  double chi_m_p = qm*k;
  
  double ichi_m   = 1/chi_m;
  double ichi_p   = 1/chi_p;
  double ichi_m_p = 1/chi_m_p;
  double ichi_p_p = 1/chi_p_p;

  //  cout<<DB(ichi_m)<<DB(ichi_p)<<endl;
  Dot(pp,pm);
  double ipppm = 1/pppm;

  Dot(qp,qm);

  TLorentzVector Q  = 0.5*(qp - qm);
  TLorentzVector kp = k - pp*chi_m*ipppm;
  TLorentzVector km = k - pm*chi_p*ipppm;

  double QQ = Q*Q;
  double kk = k*k;
  Dot(k,Q);

  Dot(pp,Q);
  Dot(pm,Q);
  
  Dot(kp,Q);
  Dot(km,Q);

  Dot(qp,Q);
  Dot(qm,Q);

  Dot(qp,k);
  Dot(qm,k);

  Dot(pp,k);
  Dot(pm,k);

  Dot(pp,km);
  Dot(pm,kp);

  Dot(pm,qp);
  Dot(pp,qp);

  Dot(pm,qm);
  Dot(pp,qm);

  double mpi2 = gGlobal->Get_MF2();
  double me2  = gGlobal->Get_MI2();
  double is  = 1/s;
  double is1 = 1/s1;

  std::complex<double> Fpi_s1 = fFpi->Eval(gGlobal->Get_s()*s1*0.25);
  double Fpi_s1_s1 = std::abs(Fpi_s1*Fpi_s1);
  double Fpi_s_s, Fpi_s_cs1;
  Fpi_s_s = Fpi_s_cs1 = 0;
  if(!fNoFSR){
    std::complex<double> Fpi_s  = fFpi->Eval(gGlobal->Get_s());
    Fpi_s_s  = std::abs(Fpi_s*Fpi_s);
    Fpi_s_cs1= std::real(Fpi_s*std::conj(Fpi_s1));
  }

  //  Fpi_s1_s1 = 0; Fpi_s_s = 0;  Fpi_s_cs1 = 1;
  /* //from article in JHEP
  double R_1 = s*(is1*is1)*Fpi_s1_s1*Prod(pp,Q,pm,Q)*
    (
     (pppm*ichi_p*ichi_m - 2*ichi_m - me2*(ichi_m*ichi_m) + chi_p*ipppm*ichi_m*(1 + me2*ichi_m)) +
     (pppm*ichi_p*ichi_m - 2*ichi_p - me2*(ichi_p*ichi_p) + chi_m*ipppm*ichi_p*(1 + me2*ichi_p))
     );

  */
  double R_1 = s*(is1*is1)*Fpi_s1_s1*
    ( (Prod(pp,Q,pm,Q)*(pppm*ichi_p*ichi_m - 2*ichi_m + 2*chi_p*is*ichi_m) + Prod(pp,Q,k,Q)*me2*ichi_m*ichi_m) + 
      (Prod(pm,Q,pp,Q)*(pppm*ichi_p*ichi_m - 2*ichi_p + 2*chi_m*is*ichi_p) + Prod(pm,Q,k,Q)*me2*ichi_p*ichi_p)
      );

  double R_2 = Prod(pp,Q,pm,Q)*
    (
     is*Fpi_s_s*
     (
      (qpqm*(ichi_p_p*ichi_m_p) - mpi2*(ichi_p_p*ichi_p_p)) +
      (qpqm*(ichi_p_p*ichi_m_p) - mpi2*(ichi_m_p*ichi_m_p))
      ) 
     
     + 
     
     2*is1*Fpi_s_cs1*(pp*ichi_p - pm*ichi_m) * (qp*ichi_p_p - qm*ichi_m_p)
     );
  
  double R_3 = 
    s*is1*is1*Fpi_s1_s1*
    (Prod(pp,Q,km,Q)*ichi_m + Prod(pm,Q,kp,Q)*ichi_p + 2*Qk*(ichi_m*ichi_p)*Prod(pp,Q,pm,k))

    +
    
    is*Fpi_s_s*
    (
     -0.5*chi_p*chi_m*(ichi_p_p*ichi_m_p)*qpqm - 0.25*mpi2*(ichi_p_p*ichi_p_p)*
     (Prod(pp,k,pm,k) + 2*Prod(pp,Q,pm,k) + 2*Prod(pp,k,pm,Q)) + 
     ichi_p_p*(chi_p*pmqp + chi_m*ppqp + 2*Prod(pm,Q,pp,qp))
     
     +
     
     -0.5*chi_p*chi_m*(ichi_p_p*ichi_m_p)*qpqm - 0.25*mpi2*(ichi_m_p*ichi_m_p)*
     (Prod(pp,k,pm,k) - 2*Prod(pp,Q,pm,k) - 2*Prod(pp,k,pm,Q)) + 
     ichi_m_p*(chi_p*pmqm + chi_m*ppqm - 2*Prod(pm,Q,pp,qm))
     )
    
    +
    
    is1*Fpi_s_cs1*
    (
     (
      Prod(pp,Q,pm,k)*ichi_p_p*(qppp*ichi_p - qppm*ichi_m) + 2*(ppQ - pmQ) 
      - ichi_p_p*(Prod(pp,Q,qp,k)-Prod(pm,Q,qp,k)) 
      + 2*Qk*Qpp*pmqp*(ichi_p_p*ichi_m) -  2*Qpm*Qk*ppqp*(ichi_p_p*ichi_p)
      - QQ*chi_p*qppm*(ichi_p_p*ichi_m) + QQ*ppqm*ichi_p_p + QQ*chi_m*ppqp*(ichi_p_p*ichi_p) 
      - QQ*pmqm*ichi_p_p
      )
     
     -
     
     (
      -Prod(pp,Q,pm,k)*ichi_m_p*(qmpp*ichi_p - qmpm*ichi_m) - 2*(ppQ - pmQ) 
      + ichi_m_p*(Prod(pp,Q,qm,k)-Prod(pm,Q,qm,k)) 
      + 2*Qk*Qpp*pmqm*(ichi_m_p*ichi_m) -  2*Qpm*Qk*ppqm*(ichi_m_p*ichi_p)
      - QQ*chi_p*qmpm*(ichi_m_p*ichi_m) + QQ*ppqp*ichi_m_p + QQ*chi_m*ppqm*(ichi_m_p*ichi_p) 
      - QQ*pmqp*ichi_m_p
      )
     )
    ;

  double CosPsi = fEvent->GetCosPsi();
  double gamma = k.E()*qm.P()/(2-k.E()*(1-qm.E()*CosPsi/qm.P()));

  return gamma*(R_1 + R_2 + R_3 )*0.25;
}

#ifdef Dot
#  undef Dot
#endif

double TPiCrossPart::R_Gamma_kuraev_eidelman(){
  // RGamma from preprint INP 78-82 
  // equation (30) page 13
  TLorentzVector pm(0,0, gGlobal->Get_BetaI(),1.);
  TLorentzVector pp(0,0,-gGlobal->Get_BetaI(),1.);
  TLorentzVector qm = fEvent->Get4Vector(0);
  TLorentzVector k  = fEvent->Get4Vector(2);
  double x = k.E();

  double s  = (pp + pm)*(pp + pm);
  double s1 = s*(1-x);
  double is1 = 1/s1;

  double ichi_m   = 1/(pp*k);
  double ichi_p   = 1/(pm*k);

  double mpi2 = gGlobal->Get_MF2();
  double me2  = gGlobal->Get_MI2();

  double z      = pm.Vect().Dot(qm.Vect())/(pm.P()*qm.P());
  double xm     = qm.E();
  double betam  = qm.P();
  double t      = mpi2 - s*(1 - x - xm*0.5*(1+betam*z));
  double u      = mpi2 - s*(1     - xm*0.5*(1-betam*z));
  double t1     =  mpi2 - s*xm*0.5*(1-betam*z);
  double u1     =  mpi2 - s*xm*0.5*(1+betam*z);
  double utilde = u + s*x;
  double ttilde = t - s*x;

  std::complex<double> Fpi_s1 = fFpi->Eval(gGlobal->Get_s()*s1*0.25);
  double Fpi_s1_s1 = std::abs(Fpi_s1*Fpi_s1);

  double R_pi = Fpi_s1_s1*0.125*is1*is1*
    ((s*(s1-4*mpi2)-(u1-t)*(u-t1))*
     (-me2*s*(1-x)*ichi_m*ichi_m + s*ichi_m*(1 + pow(1 - x, 2))/x) +
     (s*(s1-4*mpi2)-(u1-ttilde)*(utilde-t1))*
     (-me2*s*(1-x)*ichi_p*ichi_p + s*ichi_p*(1 + pow(1 - x, 2))/x));

  double CosPsi = fEvent->GetCosPsi();
  double gamma = k.E()*qm.P()/(2-k.E()*(1-qm.E()*CosPsi/qm.P()));

  return gamma*R_pi*0.25;
}
