#include <iostream>
#include <cmath>
#if (defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
# include <mathimf.h>
#endif
#include "TEvent.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"

using namespace std;
#define INRANGE(x) if ( x < 0 ) x += 2*gConst->Pi(); else if( x > 2*gConst->Pi() ) x -= 2*gConst->Pi()

TEvent::TEvent(){
  fNSelStat = 9;
  fSelStat = new unsigned int[fNSelStat];
  Init();
}

void TEvent::Init(){
  for(unsigned int i=0;i<fNSelStat;i++){
    fSelStat[i] = 0;
  }
  fIsSmear = false;
  for(unsigned int i=0;i<6;i++){
    fdPar[i] = 0;
  }
}

TEvent::~TEvent(){
  delete [] fSelStat;
}

void lorbck(double *pcm, double *pincm, double *pinlab){
  double beta[3];
  for(int i=0;i<3;i++)
    beta[i] = pcm[i]/pcm[3];
  double gamma = 1./sqrt(1 - beta[0]*beta[0] - beta[1]*beta[1] - beta[2]*beta[2]);
  double bdotp = beta[0]*pincm[0] + beta[1]*pincm[1] + beta[2]*pincm[2];
  double fact  = gamma*bdotp/(1 + gamma) + pincm[3];

  for(int i=0; i<3; i++)
    pinlab[i] = pincm[i] + beta[i]*fact*gamma;
  pinlab[3] = gamma*(pincm[3] + bdotp);
}

bool TEvent::Selectgg(){
  // selecting two energetic photons in fiducial volume
  int in[] = { 0, 1, 2};
  for(int i=0;i<3;i++)
    for(int j=i+1;j<3;j++)
      if(fen[in[i]]<fen[in[j]]){
	int t = in[i];
	in[i] = in[j];
	in[j] = t;
      }

  int i0 = in[0], i1 = in[1];

  if ( fth[i0] < gCut->AverageThetaMin() ) return false;
  if ( fth[i0] > gCut->AverageThetaMax() ) return false;

  if ( fth[i1] < gCut->AverageThetaMin() ) return false;
  if ( fth[i1] > gCut->AverageThetaMax() ) return false;

  double delta_theta = fth[i0] + fth[i1] - gConst->Pi() + dDeltaTheta();
  if ( fabs( delta_theta ) > gCut->DeltaTheta() )  return false;

  double delta_phi = fabs(fph[i0]-fph[i1]) - gConst->Pi() + dDeltaPhi();
  if ( fabs( delta_phi ) > gCut->DeltaPhi() ) return false;

  double cos_psi = fsth[i0]*fsth[i1]*(fcph[i0]*fcph[i1] + fsph[i0]*fsph[i1]) + 
    fcth[i0]*fcth[i1];
  if ( cos_psi > gCut->CosPsi() ) return false;

  //  if(fabs(gConst->Pi()-fth[in[0]]-fth[in[1]])>0.2) return false;
  return true;
}

bool TEvent::Select(){

  //  if ( gGlobal->Get_E()*(2 - fen[Ne] - fen[Np]) < 10 ) return false;

  if ( fth[Np] < gCut->ThetaMinP() || fth[Np] > gCut->ThetaMaxP() ) {fSelStat[0]++; return false;}

  double theta_aver = 0.5*(fth[Ne] + gConst->Pi() - fth[Np]) + dATheta();
  if ( theta_aver < gCut->AverageThetaMin() || theta_aver > gCut->AverageThetaMax() ) {fSelStat[6]++; return false;}
  double delta_theta = fth[Ne] + fth[Np] - gConst->Pi() + dDeltaTheta();
  if ( fabs( delta_theta ) > gCut->DeltaTheta() ) {fSelStat[3]++; return false;}

  if ( fen[Ne] < gCut->EMin() || fen[Np] < gCut->EMin() ) {fSelStat[2]++; return false;}

  double delta_phi = fabs(fph[Ne]-fph[Np]) - gConst->Pi() + dDeltaPhi();
  if ( fabs( delta_phi ) > gCut->DeltaPhi() ) {fSelStat[4]++; return false;}

  if ( (fp[Ne]*fsth[Ne] < gCut->PCross() ) || (fp[Np]*fsth[Np] < gCut->PCross() )) {fSelStat[5]++; return false;}

  if ( fth[Ne] < gCut->ThetaMinM() || fth[Ne] > gCut->ThetaMaxM() ) {fSelStat[1]++; return false;}

  if ( fth[Np] < gCut->ThetaMinP() || fth[Np] > gCut->ThetaMaxP() ) {fSelStat[1]++; return false;}

  if ( 0.5*(fp[Ne] + fp[Np]) < gCut->PAverage() ) {fSelStat[7]++; return false;}

  //!!!!!!!!!!!
  // Old CMD cut on momenta difference
  //  if ( fabs((fp[Ne] - fp[Np])/(fp[Ne] + fp[Np])) > 0.15 ) {fSelStat[7]++; return false;}
  //!!!!!!!!!!!

  double cos_psi = (fsth[Ne]*fsth[Np]*(fcph[Ne]*fcph[Np] + fsph[Ne]*fsph[Np]) + fcth[Ne]*fcth[Np]);
  //  std::cout<<cos_psi<<" "<<gCut->CosPsi()<<std::endl;
  if ( cos_psi > gCut->CosPsi() ) {fSelStat[8]++; return false;}

  /*
  // Assymetry calculation selection 
  if ( fp[Ne]*gGlobal->Get_E() < 400 ) return false;
  if ( fp[Np]*gGlobal->Get_E() < 400 ) return false;
  double pmod1 = fp[Ne]*gGlobal->Get_E();
  double pmod2 = fp[Np]*gGlobal->Get_E();

  double ppion = sqrt(0.25*gGlobal->Get_s() - gConst->Mpi2());
  if ( pow(pmod1 - ppion, 2) + pow(pmod2 - ppion, 2) < 9. ) return false;

  double pmuon = sqrt(0.25*gGlobal->Get_s() - gConst->Mmu2());
  if ((pmod1 > pmuon-3. && pmod1 < pmuon+3.) && (fcth[Ne] > 0.65 && fcth[Ne] < 0.75)) return false;
  if ((pmod2 > pmuon-3. && pmod2 < pmuon+3.) && (fcth[Np] >-0.75 && fcth[Np] <-0.65)) return false;

  double pcm1[4], pcm2[4];
  pcm1[0] = fp[Ne]*fsth[Ne]*fcph[Ne];
  pcm1[1] = fp[Ne]*fsth[Ne]*fsph[Ne];
  pcm1[2] = fp[Ne]*fcth[Ne];
  pcm1[3] = fen[Ne];

  pcm2[0] = fp[Np]*fsth[Np]*fcph[Np];
  pcm2[1] = fp[Np]*fsth[Np]*fsph[Np];
  pcm2[2] = fp[Np]*fcth[Np];
  pcm2[3] = fen[Np];

  for(int i=0;i<4;i++){
    pcm1[i] *= gGlobal->Get_E();
    pcm2[i] *= gGlobal->Get_E();
  }

  double Px = pcm1[0] + pcm2[0];
  double Py = pcm1[1] + pcm2[1];
  double Pz = pcm1[2] + pcm2[2];
  double Xm = pow(2*gGlobal->Get_E() - (pcm1[3]+pcm2[3]), 2) - Px*Px - Py*Py - Pz*Pz;  
  if ( sqrt(fabs(Xm)) > 150. ) return false;

  double corr = 
    (fsth[Ne]+fsth[Np] - fabs(fsth[Ne]*fcth[Np]+fsth[Np]*fcth[Ne]))/
    (fsth[Ne]+fsth[Np] + fabs(fsth[Ne]*fcth[Np]+fsth[Np]*fcth[Ne]));
  if ( sqrt(corr) < 0.93 ) return false;

  double rtsp = 2*gGlobal->Get_E()*sqrt(corr);
  double eisr = 2*gGlobal->Get_E() - rtsp;
  double pisr[4];
  pisr[0] = 0.;
  pisr[1] = 0.;
  pisr[2] = -eisr*(pcm1[2] + pcm2[2])/fabs(pcm1[2] + pcm2[2]);
  pisr[3] = sqrt(rtsp*rtsp + eisr*eisr);

  double pcc1[4], pcc2[4];
  lorbck(pisr,pcm1,pcc1);
  lorbck(pisr,pcm2,pcc2);
  
  double the1 = acos(pcc1[2]/sqrt(pcc1[0]*pcc1[0]+pcc1[1]*pcc1[1]+pcc1[2]*pcc1[2]));
  double the2 = acos(pcc2[2]/sqrt(pcc2[0]*pcc2[0]+pcc2[1]*pcc2[1]+pcc2[2]*pcc2[2]));

  double theave = (the1-(the2-gConst->Pi()))/2. ;
  if(theave < gConst->Pi()/2 ) return false;
  if(theave < gCut->AverageThetaMin() || theave > gCut->AverageThetaMax()) return false;

  double xprod = 0.;
  double p1 = 0.;
  double p2 = 0.;
  for(int i=0;i<3;i++){
    xprod += pcc1[i]*pcc2[i];
    p1 += pcc1[i]*pcc1[i];
    p2 += pcc2[i]*pcc2[i];
  }
  xprod = xprod/sqrt(p1*p2);
  if (xprod > -0.997) return false;
  */  
  return true;
}

bool TEvent::MakeEvent(const double &x1g, const double &cos1g, const double &phi1g, 
		       const double &x2g, const double &cos2g, const double &phi2g, 
		       const double &c, const double &phi)
{
  double pen1    = x1g;
  double pcosth1 = cos1g;
  double psinth1 = sqrt(1-pcosth1*pcosth1);
  double pphi1   = phi1g;
  double pcosph1 = cos(pphi1);
  double psinph1 = sin(pphi1);

  double pen2    = x2g;
  double pcosth2 = cos2g;
  double psinth2 = sqrt(1-pcosth2*pcosth2);
  double pphi2   = phi2g;
  double pcosph2 = cos(pphi2);
  double psinph2 = sin(pphi2);

  double px = pen1*psinth1*pcosph1 + pen2*psinth2*pcosph2;
  double py = pen1*psinth1*psinph1 + pen2*psinth2*psinph2;
  double pz = pen1*pcosth1         + pen2*pcosth2;
  double k2 = px*px+py*py+pz*pz;

  fcth[Ne] = c;
  fth[Ne]  = acos(fcth[Ne]);
  fsth[Ne] = sqrt(1-c*c);

  fph[Ne]  = phi;
  fcph[Ne] = cos(fph[Ne]);
  fsph[Ne] = sin(fph[Ne]);

  double kcospsi = (px*fcph[Ne]+py*fsph[Ne])*fsth[Ne] + pz*fcth[Ne];
  double t   = 2 - pen1 - pen2;
  double ap  = kcospsi*kcospsi-t*t;
  double b   = t*t - k2;
  double d   = 4*gGlobal->Get_MF2()*ap + b*b;
  if ( d > 0 ) {

    fen[Ne] = 0.5*( -t*b + kcospsi*sqrt(d) )/ap;
    fen[Np] = t - fen[Ne];

    fp[Ne]  = sqrt(fen[Ne]*fen[Ne]-gGlobal->Get_MF2());
    fp[Np]  = sqrt(fen[Np]*fen[Np]-gGlobal->Get_MF2());

    fcth[Np] = -(pz + fp[Ne]*fcth[Ne])/fp[Np];
    if ( fabs(fcth[Np]) <= 1){
      fsth[Np] = sqrt(1-fcth[Np]*fcth[Np]);
      fth[Np]  = acos(fcth[Np]);
      fcph[Np] = -(fp[Ne]*fsth[Ne]*fcph[Ne]+px)/(fp[Np]*fsth[Np]);
      fsph[Np] = -(fp[Ne]*fsth[Ne]*fsph[Ne]+py)/(fp[Np]*fsth[Np]);
      fph[Np] = atan2(fsph[Np],fcph[Np]);
      INRANGE(fph[Np]);
    } else 
      return false;
  } else
    return false;

  return Select();
}


bool TEvent::MakeEvent( const double &c, TPhoton *Ph, const unsigned int SType){
#ifdef FIXP
  double c1g = 0.98;
  Ph->SetCosTheta(c1g);
#else
  double c1g;
  if(SType == 0)
    c1g = Ph->GetCosThetaF();
  else 
    c1g = Ph->GetCosTheta();
#endif
  if( SType == 1 && fabs(c1g)<gGlobal->Get_CosThetaInt()) return false; 
  double s1g = sqrt(1-c1g*c1g);

#ifdef FIXP
  double p1g = 1.2;
#else
  double p1g = Ph->GetPhi();
#endif
  double cp1g, sp1g;
  //  __sincos(p1g, &sp1g, &cp1g);
  sincos(p1g, &sp1g, &cp1g);

  fcth[Ne] = c;
  fsth[Ne] = sqrt(1-fcth[Ne]*fcth[Ne]);

#ifdef FIXP
  fph[Ne]  = 0.;
#else
  fph[Ne]  = GetPhi();
#endif
  //  __sincos(fph[Ne], &fsph[Ne], &fcph[Ne]);
  sincos(fph[Ne], &fsph[Ne], &fcph[Ne]);
  double nx,ny,nz;
  if( SType == 0){
    //singularities around final particles
    //    cout<<fcth[Ne]<<" "<<c1g<<" "<<fsth[Ne]<<" "<<s1g<<" "<<cp1g<<endl;
    nz = fcth[Ne]*c1g + fsth[Ne]*s1g*cp1g;
    //    cout<<DB(nz)<<endl;
    if ( fabs(nz) > gGlobal->Get_CosThetaInt() ) return false;
    fcth[Ng1] = nz;
    fsth[Ng1] = sqrt(1-fcth[Ng1]*fcth[Ng1]);

    double nxp = fsth[Ne]*c1g - fcth[Ne]*s1g*cp1g;
    double nyp =-s1g*sp1g;
    nx = fcph[Ne]*nxp - fsph[Ne]*nyp;
    ny = fsph[Ne]*nxp + fcph[Ne]*nyp;
    //    double norm = 1/sqrt(nx*nx + ny*ny);
    double norm = 1/fsth[Ng1];
    fcph[Ng1] = nx*norm;
    fsph[Ng1] = ny*norm;
    //    cout<<DB(nx)<<DB(ny)<<endl;
    fCosPsi = c1g;
  }else{
    // singularities around initial particles;
    fcth[Ng1] = c1g;
    fsth[Ng1] = s1g;
    
    fph[Ng1]  = p1g;
    fcph[Ng1] = cp1g;
    fsph[Ng1] = sp1g;

    nx = fsth[Ng1]*fcph[Ng1];
    ny = fsth[Ng1]*fsph[Ng1];
    nz = fcth[Ng1];
    fCosPsi = (nx*fcph[Ne]+ny*fsph[Ne])*fsth[Ne] + nz*fcth[Ne];
    if(SType == 1){ // if Bhabha Excluding theta0 cone around final electron 
      if ( fCosPsi > gGlobal->Get_CosTheta0() ) return false;
    }
  }

#ifdef FIXP
  fen[Ng1]  = fp[Ng1] = 0.1;
  Ph->SetEnergy(fen[Ng1]);
#else
  fen[Ng1]  = fp[Ng1] = Ph->GetEnergy();
  if (SType == 3 && fen[Ng1] > 0.5 ) return false;
#endif

  double k2 = fen[Ng1]*fen[Ng1];
  double t  = 2 - fen[Ng1];
  double b  = t*t - k2;
  // to build real final electro-positron system we must have enough
  // energy
  //  cout<<fen[Ng1]<<" "<<t<<" "<<k2<<" "<<gGlobal->Get_MF2()<<endl;
  if ( b<4*gGlobal->Get_MF2() ) return false; 

  double kcospsi = fen[Ng1]*fCosPsi;
  double ap = kcospsi*kcospsi-t*t;
  double d  = 4*gGlobal->Get_MF2()*ap + b*b;
  //  cout<<"kuku0 "<<b<<endl;
  if ( d < 0 ) return false; // equation must have real roots
			     // (determinant>=0)

  fen[Ne]  = 0.5*( -t*b + kcospsi*sqrt(d) )/ap;
  fen[Np]  = t - fen[Ne];
  if(SType == 3 && fen[Ne] < gGlobal->Get_XMin()) return false;
  if(SType == 3 && fen[Np] < gGlobal->Get_XMin()) return false;

  fp[Ne]   = sqrt(fen[Ne]*fen[Ne] - gGlobal->Get_MF2());
  fp[Np]   = sqrt(fen[Np]*fen[Np] - gGlobal->Get_MF2());
  
  fcth[Np] = -(fen[Ng1]*nz + fp[Ne]*fcth[Ne])/fp[Np];
  //  cout<<"kuku1 "<<fcth[Np]<<endl;

  //  if(SType<3){
    // positron must be in detector volume
  if ( SType != 3 && fabs(fcth[Np]) > gGlobal->Get_CosThetaMin() ) return false; 
  if ( SType == 3 && fabs(fcth[Np]) > gGlobal->Get_CosTheta0() ) return false; 
    //  } else {
    // outside narrow cone of initial particles
    //    if ( fabs(fcth[Np]) > gGlobal->Get_CosTheta0() ) return false; 
    //  }

  fsth[Np] = sqrt(1-fcth[Np]*fcth[Np]);

  double inv = 1/(fp[Np]*fsth[Np]);
  fcph[Np] = -(fp[Ne]*fsth[Ne]*fcph[Ne] + fen[Ng1]*nx)*inv;
  //  cout<<"kuku2 "<<fabs(fcph[Np])-1<<endl;
  if ( fabs(fcph[Np]) > 1 + 1e-10 ) return false;
  fsph[Np] = -(fp[Ne]*fsth[Ne]*fsph[Ne] + fen[Ng1]*ny)*inv;
  //  cout<<"kuku3 "<<fabs(fsph[Np])-1<<endl;
  if ( fabs(fsph[Np]) > 1 + 1e-10 ) return false;

  if(SType == 1 || SType == 0){ // if Bhabha Excluding theta0 cone
				// around final positron
    double cos_psi_p = (nx*fcph[Np]+ny*fsph[Np])*fsth[Np] + nz*fcth[Np];
    //    cout<<"kuku4 "<<cos_psi_p<<endl;
    if ( cos_psi_p > gGlobal->Get_CosTheta0() ) {
      //      cout<<DB(cos_psi_p)<<endl;
      return false;
    }
  }

  fth[Ne]  = acos(fcth[Ne]);
  fth[Np]  = acos(fcth[Np]);
  fth[Ng1] = acos(fcth[Ng1]);

  fph[Np]  = atan2(fsph[Np],fcph[Np]);
  fph[Ng1] = atan2(fsph[Ng1],fcph[Ng1]);

  //  cout<<DB(fph[Np])<<endl;
  INRANGE(fph[Np]);
  //  Print();
  return (SType<3)?Select():Selectgg();
}

bool TEvent::MakeEvent(const double &c, const double &x1g, const double &x2g){
  // kinematics in collinear region for initial particles, final
  // particles have non-zero mass.

  fen[Ng1] = 0;
  fxg[0] = x1g;
  fxg[1] = x2g;
  fxg[2] = 0;
  fxg[3] = 0;

  fcth[Ne] = c;
  fsth[Ne] = sqrt(1-fcth[Ne]*fcth[Ne]);

  double pz = x1g - x2g;
  double kcospsi = pz*fcth[Ne];
  double t   = 2 - x1g - x2g;
  double ap  = kcospsi*kcospsi-t*t;
  double b   = t*t - pz*pz;
  double d   = 4*gGlobal->Get_MF2()*ap + b*b;

  if ( d < 0 ) return false;

  fY[Ne] = fen[Ne] = 0.5*( -t*b + kcospsi*sqrt(d) )/ap;
  fY[Np] = fen[Np] = t - fen[Ne];

  fp[Ne]  = sqrt(fen[Ne]*fen[Ne]-gGlobal->Get_MF2());
  fp[Np]  = sqrt(fen[Np]*fen[Np]-gGlobal->Get_MF2());

  fcth[Np] = -(pz + fp[Ne]*fcth[Ne])/fp[Np];
  if ( fabs(fcth[Np]) > gGlobal->Get_CosThetaMin() ) return false;

  fsth[Np] = sqrt(1-fcth[Np]*fcth[Np]);
  fth[Ne]  = acos(fcth[Ne]);
  fth[Np]  = acos(fcth[Np]);

  fph[Ne]  = GetPhi();
  //  __sincos(fph[Ne], &fsph[Ne], &fcph[Ne]);
  sincos(fph[Ne], &fsph[Ne], &fcph[Ne]);
  
  fcph[Np] = -fcph[Ne];
  fsph[Np] = -fsph[Ne];
  fph[Np] = fph[Ne] + gConst->Pi();
  INRANGE(fph[Np]);

  return Select();
}

bool TEvent::MakeEventgg(const double &c, const double &x1g, const double &x2g){
  // kinematics in collinear region for initial particles, final
  // particles are photons.

  fen[Ng1] = 0;
  fxg[0] = x1g;
  fxg[1] = x2g;
  fxg[2] = 0;
  fxg[3] = 0;

  fcth[Ne] = c;
  fsth[Ne] = sqrt(1-fcth[Ne]*fcth[Ne]);

  double pz = x1g - x2g;
  double kcospsi = pz*fcth[Ne];
  double t   = 2 - x1g - x2g;
  double ap  = kcospsi*kcospsi-t*t;
  double b   = t*t - pz*pz;

  fp[Ne] = fY[Ne] = fen[Ne] = 0.5*b*(kcospsi - t)/ap;
  fp[Np] = fY[Np] = fen[Np] = t - fen[Ne];

  fcth[Np] = -(pz + fp[Ne]*fcth[Ne])/fp[Np];
  if ( fabs(fcth[Np]) > gGlobal->Get_CosThetaMin() ) return false;

  fsth[Np] = sqrt(1-fcth[Np]*fcth[Np]);
  fth[Ne]  = acos(fcth[Ne]);
  fth[Np]  = acos(fcth[Np]);

  fph[Ne]  = GetPhi();
  //  __sincos(fph[Ne], &fsph[Ne], &fcph[Ne]);
  sincos(fph[Ne], &fsph[Ne], &fcph[Ne]);
  
  fcph[Np] = -fcph[Ne];
  fsph[Np] = -fsph[Ne];
  fph[Np] = fph[Ne] + gConst->Pi();
  INRANGE(fph[Np]);

  return Selectgg();
}

bool TEvent::MakeEvent(const double &c,
		       const double &x1g, const double &x2g, 
		       const double &x3g, const double &x4g){
  // kinematics in massless approach for final and initial particles.
  fen[Ng1] = 0;
  fxg[0] = x1g;
  fxg[1] = x2g;
  fxg[2] = x3g;
  fxg[3] = x4g;

  fcth[Ne] = c;
  
  double e = 2 - x1g - x2g;
  fY[Ne] = 2*(1-x1g)*(1-x2g)/(e-fcth[Ne]*(x2g-x1g));
  fY[Np] = e - fY[Ne];

  fen[Ne] = fY[Ne]*(1 - x3g);
  fen[Np] = fY[Np]*(1 - x4g);
  
  if( fen[Ne] < 0 || fen[Np] < 0 ) return false;
  fp[Ne]  = fen[Ne];
  fp[Np]  = fen[Np];

  fsth[Ne] = sqrt(1-fcth[Ne]*fcth[Ne]);
  fth[Ne]  = acos(fcth[Ne]);
  
  fph[Ne]  = GetPhi();
  fcph[Ne] = cos(fph[Ne]);
  fsph[Ne] = sin(fph[Ne]);
  
  double inv = 1/fY[Np];
  fcth[Np] = (x2g-x1g-fY[Ne]*fcth[Ne])*inv;
  fsth[Np] = fsth[Ne]*fY[Ne]*inv;
  fth[Np]  = acos(fcth[Np]);
  
  fph[Np]  = fph[Ne] + gConst->Pi();

  INRANGE(fph[Np]);

  fcph[Np] = -fcph[Ne];
  fsph[Np] = -fsph[Ne];

  if(fIsSmear)
    return 1;
  else
    return Select();
}

bool TEvent::MakeEventN(const double &x3g, const double &x4g){
  // kinematics in massless approach for final particles radiation.
  fen[Ng1] = 0;
  fxg[2] = x3g;
  fxg[3] = x4g;

  fen[Ne] = fY[Ne]*(1 - x3g);
  fen[Np] = fY[Np]*(1 - x4g);
  
  fp[Ne]  = fen[Ne];
  fp[Np]  = fen[Np];
  /*
  if ( fen[Ne] < gCut->EMin() || fen[Np] < gCut->EMin() ) {fSelStat[2]++; return false;}

  if ( (fp[Ne]*fsth[Ne] < gCut->PCross() ) || (fp[Np]*fsth[Np] < gCut->PCross() )) {fSelStat[5]++; return false;}

  if ( 0.5*(fp[Ne] + fp[Np]) < gCut->PAverage() ) {fSelStat[7]++; return false;}
  */
  return Select();
}

void TEvent::Print(){
  printf("Electron en=%f, p=%f, th=%f, phi=%f\n",fen[Ne],fp[Ne],fth[Ne],fph[Ne]);
  printf("Positron en=%f, p=%f, th=%f, phi=%f\n",fen[Np],fp[Np],fth[Np],fph[Np]);
  printf("Gamma    en=%f, p=%f, th=%f, phi=%f\n",fen[Ng1],fp[Ng1],fth[Ng1],fph[Ng1]);
  printf("Sum      en=%g, px=%g, py=%g, pz=%g\n",fen[Ne]+fen[Np]+fen[Ng1],
	 fp[Ne]*sin(fth[Ne])*cos(fph[Ne])+fp[Np]*sin(fth[Np])*cos(fph[Np])+fp[Ng1]*sin(fth[Ng1])*cos(fph[Ng1]),
	 fp[Ne]*sin(fth[Ne])*sin(fph[Ne])+fp[Np]*sin(fth[Np])*sin(fph[Np])+fp[Ng1]*sin(fth[Ng1])*sin(fph[Ng1]),
	 fp[Ne]*cos(fth[Ne])+fp[Np]*cos(fth[Np])+fp[Ng1]*cos(fth[Ng1]));
}

bool TEvent::MakeEvent(const double &c){
  // draw kinematic for elastic collision 
  fen[Ng1] = 0;
  fxg[0] = 0;
  fxg[1] = 0;
  fxg[2] = 0;
  fxg[3] = 0;

  fY[Ne]   = 1;
  fen[Ne]  = 1;
  fp[Ne]   = gGlobal->Get_BetaF();
  fcth[Ne] = c;
  fsth[Ne] = sqrt(1-fcth[Ne]*fcth[Ne]);
  fth[Ne]  = acos(fcth[Ne]);
  fph[Ne]  = GetPhi();
  fcph[Ne] = cos(fph[Ne]);
  fsph[Ne] = sin(fph[Ne]);
  
  fY[Np]   = 1;
  fen[Np]  = 1;
  fp[Np]   = gGlobal->Get_BetaF();
  fcth[Np] = -fcth[Ne];
  fsth[Np] = fsth[Ne];
  fth[Np]  = gConst->Pi() - fth[Ne];
  fph[Np]  = fph[Ne] + gConst->Pi();
  INRANGE(fph[Np]);
  fcph[Np] = -fcph[Ne];
  fsph[Np] = -fsph[Ne];

  return Select();
}

#define swap(x) {double t = x[Ne]; x[Ne] = x[Np]; x[Np] = t;}
void TEvent::Swap(){
  swap(fen);
  swap(fp);
  swap(fY);

  swap(fth);
  swap(fcth);
  swap(fsth);

  swap(fph);
  swap(fcph);
  swap(fsph);

  //  fCosPsi = (fcph[Ng1]*fcph[Ne]+fsph[Ng1]*fsph[Ne])*fsth[Ne] + fcth[Ng1]*fcth[Ne];
}
#undef swap

void TEvent::CosPrint(){
  double pm[3],pp[3],pg[3];

  pm[0] = fp[Ne]*fcph[Ne]*fsth[Ne];
  pm[1] = fp[Ne]*fsph[Ne]*fsth[Ne];
  pm[2] = fp[Ne]*fcth[Ne];

  pp[0] = fp[Np]*fcph[Np]*fsth[Np];
  pp[1] = fp[Np]*fsph[Np]*fsth[Np];
  pp[2] = fp[Np]*fcth[Np];

  pg[0] = fp[Ng1]*fcph[Ng1]*fsth[Ng1];
  pg[1] = fp[Ng1]*fsph[Ng1]*fsth[Ng1];
  pg[2] = fp[Ng1]*fcth[Ng1];

  double cos_m = (pm[0]*pg[0] + pm[1]*pg[1] + pm[2]*pg[2])/(fp[Ne]*fp[Ng1]);
  double cos_p = (pp[0]*pg[0] + pp[1]*pg[1] + pp[2]*pg[2])/(fp[Np]*fp[Ng1]);

  std::cout<<"cos "<<cos_m<<" "<<cos_p<<std::endl;

}

void TEvent::GetEvent(double *mom, int &npart){
  const double MeV2GeV = 0.001;
  npart = 0;

  mom[4*npart+0] = fsth[Ne]*fcph[Ne];
  mom[4*npart+1] = fsth[Ne]*fsph[Ne];
  mom[4*npart+2] = fcth[Ne];
  mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fp[Ne];
  npart++;

  mom[4*npart+0] = fsth[Np]*fcph[Np];
  mom[4*npart+1] = fsth[Np]*fsph[Np];
  mom[4*npart+2] = fcth[Np];
  mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fp[Np];
  npart++;

  if(fen[Ng1]>0){
    mom[4*npart+0] = fsth[Ng1]*fcph[Ng1];
    mom[4*npart+1] = fsth[Ng1]*fsph[Ng1];
    mom[4*npart+2] = fcth[Ng1];
    mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fp[Ng1];
    npart++;
  } else {
    if(fxg[0]>0){
      mom[4*npart+0] = 0;
      mom[4*npart+1] = 0;
      mom[4*npart+2] = 0.999999;
      mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fxg[0];
      npart++;      
    }
    if(fxg[1]>0){
      mom[4*npart+0] = 0;
      mom[4*npart+1] = 0;
      mom[4*npart+2] = -0.999999;
      mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fxg[1];
      npart++;      
    }
    if(fxg[2]>0){
      mom[4*npart+0] = fsth[Ne]*fcph[Ne];
      mom[4*npart+1] = fsth[Ne]*fsph[Ne];
      mom[4*npart+2] = fcth[Ne];
      mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fY[Ne]*fxg[2];
      npart++;
    }
    if(fxg[3]>0){
      mom[4*npart+0] = fsth[Np]*fcph[Np];
      mom[4*npart+1] = fsth[Np]*fsph[Np];
      mom[4*npart+2] = fcth[Np];
      mom[4*npart+3] = MeV2GeV*gGlobal->Get_E()*fY[Np]*fxg[3];
      npart++;
    }
  }
}
