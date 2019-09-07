#include <iostream>

#include "TCrossPart.h"
#include "TConstant.h"
using namespace rb;
void TCrossPart::SetFinalParticles(size_t n, const double *m, const int *pid){
  // set number and masses of final particles
  if(n>7){
    std::cout<<"Too many particles"<<std::endl;
    exit(1);
  }
  fNfinal = n;
  fres = new TLorentzVector*[fNfinal+2];
  if(fevent.SetDecay(fW, fNfinal, (double*)m, "Fermi")){
    for(size_t i=0;i<fNfinal;i++){
      fm[i] = m[i];
      fpid[i] = pid[i];
      fres[i] = fevent.GetDecay(i);
    }
  } else {
    std::cout<<"Energy is not enough to generate process!"<<std::endl;
    exit(1);
  }
  // include to the list of final particles two hard photons
  // (collinear and not collinear)
  fres[fNfinal] = (TLorentzVector*)&fphot->GetPhoton();
  fpid[fNfinal] = 22;
  fres[fNfinal+1] = new TLorentzVector(0,0,0,0);
  fpid[fNfinal+1] = 22;
}

double TCrossPart::Generate(){
  // Attention!!! Before invoking this method you must set fq !
  if(!fevent.SetDecay(fq, fNfinal, fm, "Fermi")) return 0;
  /* 
     for N body decay dGamma has dimension of [MeV^(2*(N-2))] from Fermi
     dependence. Meaning dGamma see paper by Kuraev et al
  */
  const double i2piN[] = 
    {1,
     2.533029591058444286096986e-2,  // = 1/pow(2*M_PI,2) 2 final particles
     1.021176138454182960196328e-4,  // = 1/pow(2*M_PI,5) 3 final particles
     4.116812173964596341083153e-7,  // = 1/pow(2*M_PI,8) 4 final particles
     1.659668869795425306504901e-9,  // = 1/pow(2*M_PI,11) 5 final particles
     6.690858462739554865983570e-12, // = 1/pow(2*M_PI,14) 6 final particles
     2.697380651233861939614714e-14  // = 1/pow(2*M_PI,17) 7 final particles
     };

  double dGamma = fevent.Generate()*i2piN[fNfinal-1];
  return dGamma;
}

TCrossPart::TCrossPart(double e, double de, double nth0){
  fe = e;
  fbeta2 = 1 - pow(me/fe,2);
  double beta = sqrt(fbeta2);
  fppz =  fe*beta;
  fpmz = -fe*beta;
  double theta0 = nth0*sqrt(me/fe);
  //    flogs = log(theta0*theta0/4);
  long double c = beta*cos(theta0);
  flogs = -log((1l+c)/(1l-c));
  
  fdelta = de/fe;
  fK = M_PI*M_PI/3 - 1./2.;
  //  fphot = new TPhoton(e,de,420);
  fphot = new TPhoton(e);
  fphot->SetThetaRange(theta0, M_PI - theta0);
  fD = new TDFun(fe);
  fsphot = new TPhotonD();
  fsphot->Init(fdelta,2*fD->GetBeta2());
  for(size_t i=0;i<NPARTMAX;i++){
    fN[i] = 0;
    fsum[i] = 0;
    fsum2[i] = 0;
    fmin[i] = 1e100;
    fmax[i] = -1e100;
    fcs[i] = 0;
    fecs[i] = 0;
  }
  inc[0] = true;
  inc[1] = true;
  inc[2] = true;
  inc[3] = true;
  inc[4] = true;

  fNRad = 25000;

  fep = fe;
  fem = fe;
  fpp.SetPxPyPzE(0,0,fppz,fep);
  fpm.SetPxPyPzE(0,0,fpmz,fem);
  fW.SetPxPyPzE(0,0,0,2*fe);
  fs = fW.M2();

  fBeamSpread = false;
}

TCrossPart::~TCrossPart(){
  delete fsphot;
  delete fD;
  delete fphot;
  delete fres[fNfinal+1];
  delete [] fres;
}

void TCrossPart::BeamSpread(){
  fep = fe + gRandom->Gaus(0,fsigmap);
  fem = fe + gRandom->Gaus(0,fsigmam);
  double betap2 = 1 - pow(me/fep,2);
  double betap = sqrt(betap2);
  double betam2 = 1 - pow(me/fem,2);
  double betam = sqrt(betam2);
  fppz =  fep*betap;
  fpmz = -fem*betam;
  fpp.SetPxPyPzE(0,0,fppz,fep);
  fpm.SetPxPyPzE(0,0,fpmz,fem);
  fW.SetPxPyPzE(0,0,0,fep+fem);
  fs = fW.M2();
}

double TCrossPart::GetValue(const unsigned int npart){
  double m2 = 0;
  const double AP = alpha/M_PI;
  if(!inc[npart]) return 0;
  if(fBeamSpread) BeamSpread();
  switch ( npart ){
  case 4:
    {
      const TLorentzVector &k = fphot->GetNewPhoton();
      double d3k_om = k.E()*fphot->GetPhotNorm()*fe;
      m2 = radB(k)*d3k_om;
    }
    break;
  case 1:
    {
      fxp = fphot->GetEnergy();
      double normp = fphot->GetENorm();
      fxm = fsphot->GetEnergy();
      double normm = fsphot->GetENorm();
      
      double comp = ((1-fxp+0.5*fxp*fxp)*flogs+0.5*fxp*fxp)/fxp;
      //	double comp = 0;
      m2 = rad(1-fxp, 1-fxm)*
	((1+AP*fK)*
	 fD->Eval(1-fxp)*
	 fD->EvalSoft(fxm)*normm
	 + AP*comp)*normp;
    }
    break;
  case 2:
    {
      fxm = fphot->GetEnergy();
      double normm = fphot->GetENorm();
      fxp = fsphot->GetEnergy();
      double normp = fsphot->GetENorm();
      
      double comp = ((1-fxm+0.5*fxm*fxm)*flogs+0.5*fxm*fxm)/fxm;
      //	double comp = 0;
      m2 = rad(1-fxp, 1-fxm)*
	((1+AP*fK)*
	 fD->Eval(1-fxm)*
	 fD->EvalSoft(fxp)*normp
	 + AP*comp)*normm;
    }
    break;
  case 0:
    {
      //      return 0;
      fxm = fsphot->GetEnergy();
      double normm = fsphot->GetENorm();
      fxp = fsphot->GetEnergy();
      double normp = fsphot->GetENorm();
      
      double cs = rad(1-fxp, 1-fxm);
      m2 = cs*((1+AP*fK)*
	       fD->EvalSoft(fxm)*normm*
	       fD->EvalSoft(fxp)*normp);
    }
    break;
  case 3:
    {
      fxm = fphot->GetEnergy();
      double normm = fphot->GetENorm();
      fxp = fphot->GetEnergy();
      double normp = fphot->GetENorm();
      
      double cs = rad(1-fxp, 1-fxm);
      m2 = cs*(1+AP*fK)*
	fD->Eval(1-fxm)*normm*
	fD->Eval(1-fxp)*normp;
    }
    break;
  default:
    break;
  }
  return m2;
}

void TCrossPart::MakeParts(double err){
  double dcs = pow(err,2)/NPARTMAX;
  for(size_t npart = 0; npart<NPARTMAX; npart++){
    //      if(inc[npart]) continue;
    double err2;
    do {
      for(size_t i=0;i<fNRad;i++){
	double m2 = GetValue(npart);
	fsum[npart] += m2; 
	fsum2[npart] += m2*m2; 
	fN[npart]++;
	if(m2>fmax[npart]) fmax[npart] = m2;
	if(m2<fmin[npart]) fmin[npart] = m2;
      }
      double iN = 1./fN[npart];
      err2 = (fsum2[npart]*iN - pow(fsum[npart]*iN,2))*iN;
      std::cout<<npart<<" "<<fsum[npart]/fN[npart]<<" "<<sqrt(err2)<<std::endl;
    } while(err2 > dcs);
  }
  
  for(size_t i=0;i<NPARTMAX;i++){
    if(fN[i]>0) fcs[i] =  fsum[i]/fN[i];
    if(fN[i]>0) fecs[i] = (fsum2[i]/fN[i] - pow(fcs[i],2))/fN[i];
  }
  
  double cs_sum = 0, ecs_sum = 0;
  for(size_t i=0;i<NPARTMAX;i++){
    cs_sum += fcs[i];
    ecs_sum += fecs[i];
    std::cout<<"Part = "<<i<<" "
	<<"CS = "<<fcs[i]<<" +- "<<sqrt(fecs[i])<<" nb "
	<<"Min = "<<fmin[i]<<" "
	<<"Max = "<<fmax[i]<<" "
	<<"N = "<<fN[i]<<" "
	<<std::endl;
  }
  
  facs[0] = fcs[0];
  for(size_t i=1;i<NPARTMAX;i++){
    facs[i] = facs[i-1] + fcs[i];
  }
  for(size_t i=0;i<NPARTMAX;i++){
    facs[i] /= cs_sum;
    std::cout<<facs[i]<<" ";
  }
  std::cout<<std::endl;
  
  ecs_sum = sqrt(ecs_sum);
  std::cout<<"sigma = "<<cs_sum<<" +- "<<ecs_sum<<" nb, "
	   <<"relative error is "<<ecs_sum/cs_sum*100<<" %"
	   <<std::endl;
}
  
size_t TCrossPart::GenUnWeightedEvent(){
  double r = gRandom->Rndm();
  size_t ipart = 0;
  while(r>facs[ipart]) ipart++;
  while (true){
    double m = GetValue(ipart);
    if ( m != 0  &&  m>fmax[ipart]*gRandom->Rndm() ) break;
  }

  if(ipart==4){
    fres[fNfinal+1]->SetPxPyPzE(0,0,0,0);
  } else {
    fres[fNfinal]->SetPxPyPzE(0,0,-fxm*fem,fxm*fem);
    fres[fNfinal+1]->SetPxPyPzE(0,0, fxp*fep,fxp*fep);
  }
  return ipart;
}

size_t TCrossPart::GenWeightedEvent(double &w){
  double m = 0;
  size_t ipart = 0;
  do {
    double r = gRandom->Rndm();
    ipart = 0;
    while(r>facs[ipart]) ipart++;
    m = GetValue(ipart);
    if ( m > 0 ) break;
  } while (m<=0);

  if(ipart==4){
    fres[fNfinal+1]->SetPxPyPzE(0,0,0,0);
  } else {
    fres[fNfinal]->SetPxPyPzE(0,0,-fxm*fem,fxm*fem);
    fres[fNfinal+1]->SetPxPyPzE(0,0, fxp*fep,fxp*fep);
  }

  w = m/fcs[ipart];
  return ipart;
}

// the bremsstrahlung part
double TCrossPart::radB(const TLorentzVector &k){ // photon momentum
  fq.SetPxPyPzE(-k.Px(),-k.Py(),-k.Pz(),fep+fem-k.E()); // photon virtuality
  fq2 = fq*fq;//fq.M2();
  double dGamma = Generate();
  if(!Accepted()) return 0;              // cut which can be removed

  SetJ(); // Set hadronic current J

  std::complex<double> ppJ = fJc*fpp;
  std::complex<double> pmJ = fJc*fpm;
  double J2 = std::real(fJc*fJc);

  double chip = k*fpp; //chi1 in Kuraev's paper
  double chim = k*fpm; //chi2 in Kuraev's paper

  double T = -0.5*(fs*fq2 + 2*chip*chip + 2*chim*chim)*J2 -
    fq2*(std::real(pmJ*std::conj(pmJ))+std::real(ppJ*std::conj(ppJ))); // folded bremsstrahlung and hadronic tensors

  const double alpha3 = alpha*alpha*alpha;
  double cs = 2*alpha3/(fs*fq2*fq2*chim*chip)*T*dGamma*MeV2nb;
  return cs;
}

// shifted Born xection collinear emission 
// e+(e-) are not far from mass shell (virtuality is small)
double TCrossPart::rad(double zp, double zm){
  fq.SetPxPyPzE(0,0,zp*fppz+zm*fpmz,zp*fep+zm*fem); // photon virtuality 
  fq2 = fq*fq;//fq.M2(); // s' after photon emission
  //  TLorentzVector q = zp*fpp + zm*fpm;
  double dGamma = Generate();
  if(!Accepted()) return 0;             // cut which can be removed
  
  SetJ(); // Set hadronic current J
  
  std::complex<double> ppJ = fJc*fpp; ppJ *= zp;
  std::complex<double> pmJ = fJc*fpm; pmJ *= zm;
  double J2 = std::real(fJc*fJc);

  double T = 2*std::real(ppJ*std::conj(pmJ)) - 0.5*fq2*J2; // folded leptonic and hadronic tensors

  const double AP28 = 8*alpha*M_PI*alpha*M_PI;
  double cs = AP28/(fq2*fq2*fq2)*T*dGamma*MeV2nb;

  return cs;
}

void TCrossPart::J3PseudoScalars(){
  // The hadronic current 
  // here for 3pi J_\mu=\epsilon_{a b g \mu}q+_{a}q-_{b}q0_{g}

  TLorentzVector &qp = *fres[0];  //p1 momentum
  TLorentzVector &qm = *fres[1];  //p2 momentum
  TLorentzVector &q0 = *fres[2];  //p3 momentum
  
  double Jx = qp.Y()*qm.Z()*q0.T()
    -    qp.Y()*qm.T()*q0.Z()
    +    qp.T()*qm.Y()*q0.Z()
    -    qp.T()*qm.Z()*q0.Y()
    +    qp.Z()*qm.T()*q0.Y()
    -    qp.Z()*qm.Y()*q0.T();
  
  double Jy = qp.X()*qm.Z()*q0.T()
    -    qp.X()*qm.T()*q0.Z()
    +    qp.T()*qm.X()*q0.Z()
    -    qp.T()*qm.Z()*q0.X()
    +    qp.Z()*qm.T()*q0.X()
    -    qp.Z()*qm.X()*q0.T();

  double Jz = qp.Y()*qm.X()*q0.T()
    -    qp.Y()*qm.T()*q0.X()
    +    qp.T()*qm.Y()*q0.X()
    -    qp.T()*qm.X()*q0.Y()
    +    qp.X()*qm.T()*q0.Y()
    -    qp.X()*qm.Y()*q0.T();

  double Jt = qp.Y()*qm.Z()*q0.X()
    -    qp.Y()*qm.X()*q0.Z()
    +    qp.X()*qm.Y()*q0.Z()
    -    qp.X()*qm.Z()*q0.Y()
    +    qp.Z()*qm.X()*q0.Y()
    -    qp.Z()*qm.Y()*q0.X();

  fJc.SetPxPyPzE(Jx,Jy,Jz,Jt);
}
