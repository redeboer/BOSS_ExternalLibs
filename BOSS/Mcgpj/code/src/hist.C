/*
  This program calculates cross section of 
  the ee->ee, mumu, pipi, K_SK_L, K+K- processes
*/

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "TROOT.h"
#include "TH2.h"
#include "TFile.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TStopwatch.h"
#if (defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
# include <mathimf.h>
#endif

#include "TRadCor.h"
#include "TEPCrossPart.h"
#include "TMuCrossPart.h"
#include "TPiCrossPart.h"
#include "TKnCrossPart.h"
#include "TKcCrossPart.h"
#include "TGGCrossPart.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"

using namespace std; 

typedef struct {
  Float_t Tptot[3];
  Float_t Tth[3];
  Float_t Tphi[3];
  Float_t Tptott[3];
  Float_t Ttht[3];
  Float_t Tphit[3];
  Float_t Vz;
  Int_t Type;
} EVENT;

// Masses of final particle (in MeV)
Double_t mass[] = {
  M_ME,   // electron mass
  M_MMU,  // muon mass
  M_MPI,  // charged pions mass
  M_MKC,  // charged kaons mass
  M_MK0,  // neutral kaons mass
  M_MTAU  // tau mass
};  

char *proc_name[] = {
  "ee",  // Bhabha process with radiative photons
  "mm",  // e^+ e^- into \mu^+ \mu^- (\gamma)
  "pp",  // e^+ e^- into \pi^+ \pi^- (\gamma)
  "kc",  // e^+ e^- into K^+ K^- (\gamma)
  "kn",  // e^+ e^- into K_S K_L (\gamma)
  "tt",  // e^+ e^- into \tau^+ \tau^- (\gamma)
  "gg"   // e^+ e^- into \gamma \gamma (\gamma)
};

TRadCor *gRad;
TRadGlobal *gGlobal;
TKinemCut *gCut;
TConstants *gConst;
TStopwatch *timer;

Double_t EBeam;
Int_t NSim, NRad;
Int_t IsTuple, IsSmear, IsBrem, IsHardPhoton, IsNoVacPol, IsVerbose, IsFSR, IsAngleHist, IsDTSmear;
string fname, RandomFileName;
double smear_par[9];
int type;
int GetOption(unsigned int, char**);
void hist_sample(Int_t i=0);
void hist_sample_angle(Int_t i=0);
void hist_sample_comp();
const int MaxList = 22;
bool InList[MaxList];
TH1D *gH[MaxList], *gH2[MaxList];

int main(int argc, char **argv){
  int proc;
  timer = new TStopwatch();
  timer->Start(kFALSE);
  
  gConst  = new TConstants();
  gGlobal = new TRadGlobal();
  gCut    = new TKinemCut();

  try{
    proc = GetOption(argc, argv);
  }catch(std::logic_error lge){
    cout<<lge.what()<<endl;
    return 1;
  }

  if(gRandom) delete gRandom;
  gRandom = new TRandom3();
  struct stat stbuf;
  if( stat(RandomFileName.c_str(),&stbuf) == 0 ){
    cout<<"Reading random seed from file \""<<RandomFileName<<"\"."<<endl;
    TFile *frndm = new TFile(RandomFileName.c_str(),"read");
    frndm->ReadTObject(gRandom,gRandom->GetName());
    delete frndm;
    //    gRandom->ReadRandom(RandomFileName.c_str());
  } else {
    cout<<strerror(errno)<<endl;
  }

  TVCrossPart *MatrixElements;
  if(proc==0)
    MatrixElements = new TEPCrossPart();
  else if(proc==1 || proc==5)
    MatrixElements = new TMuCrossPart();
  else if(proc==2)
    MatrixElements = new TPiCrossPart();
  else if(proc==3)
    MatrixElements = new TKnCrossPart();
  else if(proc==4)
    MatrixElements = new TKcCrossPart();
  else if(proc==6)
    MatrixElements = new TGGCrossPart();
  else
    return 1;

  if(IsNoVacPol)
    MatrixElements->SetZeroVP();

  if(!IsFSR)
    MatrixElements->SetNoFSR();

  gRad = new TRadCor(MatrixElements);
  gRad->SetNEvents(NRad);
  gRad->SetPartList(InList);
  gRad->Init();

  MatrixElements->SetHardPhoton(IsHardPhoton);
  if(IsDTSmear){
    gRad->SetSmear(true);
    MatrixElements->GetEvent()->SetdPar(smear_par);
  }

#ifdef HIST
  TFile *f = NULL;
  if(fname == "") return -1;
  cout<<"Try to open output file."<<endl<<flush;
  cout<<fname<<endl<<flush;
  f = new TFile(fname.c_str(),"RECREATE");

  int nelem = MatrixElements->GetNPart();
  for(int i=0;i<nelem+1;i++)
    if(InList[i]||i==nelem){
      ostringstream hname;
      hname.str(""); hname<<"dth_"<<i;
      gH[i] = new TH1D(hname.str().c_str(),"#Delta#theta",34*5,0,34*0.03);
      gH[i]->Sumw2();
      hname.str(""); hname<<"dth2_"<<i;
      gH2[i] = new TH1D(hname.str().c_str(),"#Delta#theta",34*5,0,34*0.03);
      gH2[i]->Sumw2();
    }
#endif
  gRad->MakeCrossSection();
#ifdef HIST
  for(int i=0;i<nelem;i++)
    if(InList[i]){
      gH[nelem]->Add(gH[i],1./gRad->GetWN(i));
      gH2[nelem]->Add(gH2[i],gRad->GetMax(i)/gRad->GetN(i));
    }

  cout<<"Histogram has been sampled!"<<timer->RealTime()<<endl<<flush;
  f->Write();
  delete f;
  cout<<"File has been closed!"<<timer->RealTime()<<endl<<flush;
#endif
  if(proc==2)((TPiCrossPart*)MatrixElements)->GetFormFactor()->Print();
  cout<<"Born Cross Section is "<<2*M_PI*gGlobal->Get_Norm()*MatrixElements->BornCrossSection(gCut->AverageThetaMin())<<" nb"<<endl;
  if ( fname != "" && NSim > 0 ) {
    //    hist_sample_comp();
    if(IsAngleHist)
      hist_sample_angle(proc);
    else
      hist_sample(proc);
  }

  delete MatrixElements;
  delete gRad;
  delete gCut;
  delete gGlobal;
  if( stat(RandomFileName.c_str(),&stbuf) == 0 ){
    remove(RandomFileName.c_str());
  }
  cout<<"Writing random seed to file \""<<RandomFileName<<"\"."<<endl;
  //  gRandom->WriteRandom(RandomFileName.c_str());
  TFile *frndm = new TFile(RandomFileName.c_str(),"recreate");
  frndm->WriteTObject(gRandom,gRandom->GetName());
  delete frndm;

  delete gRandom;
  delete timer;

  return 0;
}

int GetOption(unsigned int argc, char **argv){
  EBeam        = 185.;
  NRad         = 1000;
  NSim         = 0;
  IsTuple      = 0;
  IsSmear      = 0;
  IsBrem       = 0;
  IsHardPhoton = 1;
  IsNoVacPol   = 0;
  IsVerbose    = 0;
  IsFSR        = 1;
  IsAngleHist  = 0;
  IsDTSmear    = 0;
  type  = 0;
  fname = "";
  RandomFileName = ".random_stat.root";
  int proc   = 0;
  double pc  = 0.;
  double de  = -1.;
  double nt0 = -1.;
  double dt  = 0.25;
  double dp  = 0.15;
  double at  = 1.1;
  double td  = -1.;
  double am  = 90.;
  double cm  = 90.;
  double em  = 50.;
  double ti  = 0.473;
  double al  = 1.;
  double thm_min = -1.;
  double thp_min = -1.;
  double thm_max = -1.;
  double thp_max = -1.;
  double te  = 0.1;
  double re  = 0.1;
  for(int j =  0; j<18; j++) InList[j] = true;
  for(int j = 18; j<MaxList; j++) InList[j] = false;

  for (unsigned int i = 1; i < argc; i++){
    if(strlen(argv[i])>1)
      if (!strcmp(argv[i], "-e")){
	EBeam = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-ns")){
	NSim = atoi(argv[++i]);

      }else if (!strcmp(argv[i], "-type")){
	type = atoi(argv[++i]);

      }else if (!strcmp(argv[i], "-v")){
        IsVerbose = 1;

      }else if (!strcmp(argv[i], "-nr")){
	NRad = atoi(argv[++i]);

      }else if (!strcmp(argv[i], "-de")){
	de = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-nt0")){
	nt0 = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-dt")){
	dt = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-tc")){
	at = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-dp")){
	dp = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-td")){
	td = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-ti")){
	ti = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-al")){
	al = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-am")){
	am = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-cm")){
	cm = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-em")){
	em = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-pc")){
	pc = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-thm")){
	thm_min = atof(argv[++i]);
	thm_max = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-thp")){
	thp_min = atof(argv[++i]);
	thp_max = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-p")){
	proc = atoi(argv[++i]);

      }else if (!strcmp(argv[i], "-er")){
	te = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-rer")){
	re = atof(argv[++i]);

      }else if (!strcmp(argv[i], "-tup")){
	IsTuple = 1;

      }else if (!strcmp(argv[i], "-brem")){
	IsBrem = 1;

      }else if (!strcmp(argv[i], "-fname")){
	fname = argv[++i];

      }else if (!strcmp(argv[i], "-rname")){
	RandomFileName = argv[++i];

      }else if (!strcmp(argv[i], "-hsm")){
	IsSmear = 1;

      }else if (!strcmp(argv[i], "-ah")){
	IsAngleHist = 1;

      }else if (!strcmp(argv[i], "-hp")){
	IsHardPhoton = 1;

      }else if (!strcmp(argv[i], "-nhp")){
	IsHardPhoton = 0;

      }else if (!strcmp(argv[i], "-nfsr")){
	IsFSR = 0;

      }else if (!strcmp(argv[i], "-nvp")){
	IsNoVacPol = 1;

      }else if (!strcmp(argv[i], "-exclude")){
	for(int j = 0; j<MaxList; j++) InList[j] = true;
	while((i+1 < argc) && bool(strncmp(argv[i+1],"-",1))){
	  int j = atoi(argv[++i]);
	  if( j<MaxList ) InList[j] = false;
	}

      }else if (!strcmp(argv[i], "-include")){
	for(int j = 0; j<MaxList; j++) InList[j] = false;
	while((i+1 < argc) && bool(strncmp(argv[i+1],"-",1))){
	  int j = atoi(argv[++i]);
	  if( j<MaxList ) InList[j] = true;
	}

      }else if (!strcmp(argv[i], "-asm")){
	IsDTSmear = 1;
	int k = 0;
	while((i+1 < argc) && bool(strncmp(argv[i+1],"-",1))){
	  smear_par[k] = atof(argv[++i]);
	  k++;
	}
	if ( k != 6 && k != 9 ) throw logic_error("Number of parameters for smearing must be 6 or 9!");
	if ( k<9 ) {
	  smear_par[6] = 1.;
	  smear_par[7] = 0.;
	  smear_par[8] = 0.;
	}
      }else {
	cerr<<"Unknown option: "<<argv[i]<<endl;
      }
  }

  if( ( EBeam < 100 || EBeam > 25000 ) && !IsNoVacPol ){
    cerr<<"Invalid value of beam energy:"<<EBeam<<endl;
    exit(1);
  }

  gGlobal->Set_TotalError(te);
  
  gGlobal->Set_RelativeError(re);

  gGlobal->Set_Type(proc);

  gGlobal->Set_E(EBeam);
    
  gGlobal->Set_ThetaInt(ti);

  if(0>td)
    gGlobal->Set_ThetaMin(at-dt/2);
  else
    gGlobal->Set_ThetaMin(td);

  if(0>de){
    if(proc==0) 
      gGlobal->Set_dE_Abs(0.015*EBeam);
    else 
      gGlobal->Set_dE_Abs(0.0003*EBeam);
  }
  else
    gGlobal->Set_dE_Abs(de);

  if(0>nt0)
    gGlobal->Set_Theta0_Rel(1.5);
  else
    gGlobal->Set_Theta0_Rel(nt0);

  if(0<thm_min){
    gCut->ThetaRangeM(thm_min, thm_max);
    gGlobal->Set_ThetaMin((thm_min<M_PI-thm_max)?thm_min:M_PI-thm_max);
    cout<<"Miminal angle = "<<((thm_min<M_PI-thm_max)?thm_min:M_PI-thm_max)<<endl;
    //    gGlobal->Set_ThetaMin(1.1);
  }

  if(0<thp_min){
    gCut->ThetaRangeP(thp_min, thp_max);
  }
  gCut->CosPsi(cos(pc));

  gCut->DeltaTheta(dt);
  
  gCut->AverageTheta(at);

  gCut->DeltaPhi(dp);
  
  gCut->PAverage(am);
    
  gCut->PCross(cm);

  gCut->EMin(em);
  
  gConst->SetAlphaScale(al);

  if(IsVerbose) gConst->Print();

  gGlobal->Init();
  if(IsVerbose) gGlobal->Print();

  gCut->Init();
  if(IsVerbose) gCut->Print();

  if(NSim>0)
    cout<<"NSim = "<<NSim<<endl;

  if(fname.size()>0)
    cout<<"fname = "<<fname<<endl;

  cout<<"Cross-section statistical precision will be better than "
      <<gGlobal->Get_TotalError()<<" nb and "
      <<gGlobal->Get_RelativeError()*100<<"%"<<endl;

  if(IsBrem) 
    cout<<"Bremsstrahlung on beam pipe is switched on!"<<endl;

  if(IsSmear)
    cout<<"Angles will be smeared!"<<endl;

  if(IsTuple && NSim>0)
    cout<<"Ntuple will be sampled!"<<endl;

  if(!IsHardPhoton)
    cout<<"Hard photon on big angle is not included!"<<endl;
    
  if(IsNoVacPol)
    cout<<"Vacuum polarization is not included!"<<endl;

  if(!IsFSR)
    cout<<"Final state radiation is not included!"<<endl;

  cout<<flush;

  return proc;
}

void hist_sample(Int_t iproc){
  ostringstream ost;

  // Read angular resolution data 
  double theta_res[3],phi_res[3],d;
  if(IsSmear){
    ost<<"angular"<<Int_t(EBeam+0.5)<<".dat"<<ends;
    cout<<"Try to open file with angular resolution."<<endl<<flush;
    cout<<ost.str()<<endl<<flush;
    ifstream IN(ost.str().c_str());
    if(IN){
      for(int i=0;i<3;i++)IN>>theta_res[i]>>d;
      for(int i=0;i<3;i++)IN>>phi_res[i]>>d;
      IN.close();
    }else{
      cout<<"Unable to open file. Assume that detector is ideal."<<endl;
      IsSmear = 0;
    }
  }
  // Open file. If it does not exist it will be created.
  TFile *f = NULL;
  string file_name;
  if(fname == ""){
    ost.str(""); ost<<"";
    if(!IsTuple){
      ost<<proc_name[iproc]<<Int_t(EBeam+0.5)<<"hist_new_a2.root"<<ends;
    }else{
      ost<<proc_name[iproc]<<Int_t(EBeam+0.5)<<"tuple.root"<<ends;
    }
    file_name = ost.str();
  } else {
    file_name = fname;
  }
  cout<<"Try to open output file."<<endl<<flush;
  cout<<file_name<<endl<<flush;
  f = new TFile(file_name.c_str(),"RECREATE");
  cout<<"file opened."<<endl<<flush;

  Double_t width = 0.5;
  Double_t pmin = 90.;
  Double_t pmax = TMath::Sqrt(EBeam*EBeam-mass[iproc]*mass[iproc]);
  pmax *= 1. + 1.e-5;  // to make sure that all events will be inside histogram ranges
  Int_t nbins = int((pmax-pmin)/width);
  // to use this method bin width should be much less than momentum resolution 

  const Int_t ncth = 4, ndth = 5;
  TH2D *h[ncth][ndth];
  TTree *tree=NULL;
  static EVENT event;
  if(!IsTuple){
    // Create new histogram
    for(int i=0; i<ncth; i++){
      for(int j=0; j<ndth; j++){
	ost.str(""); ost<<proc_name[iproc]<<"_"<<i<<"_"<<j<<ends;
	string name = ost.str();

	ost.str(""); ost<<setw(3)<<setprecision(2)<<1.0+i*0.1;
	string hth = ost.str();
	
	ost.str(""); ost<<setw(4)<<setprecision(2)<<0.25-j*0.05;
	string dth = ost.str();
	
	ost.str(""); ost<<"Energy="<<Int_t(EBeam+0.5)<<" MeV, Theta_Cut="
			<<hth<<", Delta_Theta="<<dth<<ends;

	h[i][j] = new TH2D(name.c_str(), ost.str().c_str(),nbins,pmin,pmax,nbins,pmin,pmax);
	h[i][j]->SetDirectory(f);
      }
    }
  }else{
    // Create new tree
    ost.str("");ost<<proc_name[iproc]<<"tuple"<<ends;
    tree = new TTree(ost.str().c_str(),"Simulation");
    tree->Branch("event",&event,"Tptot[3]/F:Tth[3]/F:Tphi[3]/F:Tptott[3]/F:Ttht[3]/F:Tphit[3]/F:Vz:Type/I");
    tree->SetDirectory(f);
  }
  
  const Double_t alpha = 0.365073; // from 260 MeV point
  Double_t ar;// = TMath::Power(Ebeam/(Ebeam-pmin),alpha);
  const Double_t br    = 105.76; // from 260 MeV point
  Double_t r;

  Double_t p1,t1,f1,p2,t2,f2;
  Double_t bp1,bp2;
  Double_t smt1,smt2,smf1,smf2,th_a;
  Double_t prev, now, f_shift;
  UInt_t type;
  prev = timer->RealTime();
  for(int i=0; i<NSim; i++){

    gRad->MakeEvent(p1,t1,f1,p2,t2,f2,type);
    p1 *= EBeam;
    p2 *= EBeam;

    //simulation of bremsstrahlung photons parameterization from global simulation 
    if(iproc == 0 && IsBrem ){
      r   = gRandom->Rndm();
      ar  = TMath::Power(p1/(p1-pmin),alpha);
      bp1 = p1*(1.-TMath::Power(1./(ar*r-br*(r-1.)),1/alpha));
      
      r = gRandom->Rndm();
      ar  = TMath::Power(p2/(p2-pmin),alpha);
      bp2 = p2*(1.-TMath::Power(1./(ar*r-br*(r-1.)),1/alpha));

    }else{
      bp1 = p1;

      bp2 = p2;

    }

    if(IsSmear){
      //smearing of particle angles
      f_shift = 2*M_PI*gRandom->Rndm();
      
      f1 += f_shift;
      f2 += f_shift;
      
      if(gRandom->Rndm()<theta_res[0]) 
	smt1 = t1 + gRandom->Gaus(0.,theta_res[1]);
      else 
	smt1 = t1 + gRandom->Gaus(0.,theta_res[2]);
      
      if(gRandom->Rndm()<theta_res[0]) 
	smt2 = t2 + gRandom->Gaus(0.,theta_res[1]);
      else 
	smt2 = t2 + gRandom->Gaus(0.,theta_res[2]);
      
      if(gRandom->Rndm()<phi_res[0]) 
	smf1 = f1 + gRandom->Gaus(0.,phi_res[1]);
      else 
	smf1 = f1 + gRandom->Gaus(0.,phi_res[2]);
      
      if(gRandom->Rndm()<phi_res[0]) 
	smf2 = f2 + gRandom->Gaus(0.,phi_res[1]);
      else 
	smf2 = f2 + gRandom->Gaus(0.,phi_res[2]);
      
    }else{

      smt1 = t1;
      smt2 = t2;
      smf1 = f1;
      smf2 = f2;

    }

    if(smf1<0)      smf1 += 2*M_PI;
    if(smf1>2*M_PI) smf1 -= 2*M_PI;
    if(smf2<0)      smf2 += 2*M_PI;
    if(smf2>2*M_PI) smf2 -= 2*M_PI;
    if(f1<0)          f1 += 2*M_PI;
    if(f1>2*M_PI)     f1 -= 2*M_PI;
    if(f2<0)          f2 += 2*M_PI;
    if(f2>2*M_PI)     f2 -= 2*M_PI;

    if(IsTuple){
      event.Tptot[0] = bp1;
      event.Tptot[1] = bp2;
      event.Tth[0]   = smt1;
      event.Tth[1]   = smt2;
      event.Tphi[0]  = smf1;
      event.Tphi[1]  = smf2;
      event.Tptott[0]= p1;
      event.Tptott[1]= p2;
      event.Ttht[0]  = t1;
      event.Ttht[1]  = t2;
      event.Tphit[0] = f1;
      event.Tphit[1] = f2;
      event.Type     = type;

      double ttt[4*4];
      int npart;
      gRad->GetCrossPart()->GetEvent(ttt,npart);

      int j=2;
      if(npart>3)
	if(ttt[2*4+3]<ttt[3*4+3]) j = 3;

      event.Tptot[2] = 1000*ttt[j*4+3];
      event.Tth[2]   = acos(ttt[j*4+2]);
      event.Tphi[2]  = atan2(ttt[j*4+1],ttt[j*4+0]);

    }

    th_a=0.5*(smt1+M_PI-smt2);
    Double_t delta_th = TMath::Abs(smt1+smt2-M_PI);
    if(th_a > 1.0 && th_a < M_PI-1.0 &&
       delta_th<0.25 && 
       TMath::Abs(TMath::Abs(smf1-smf2)-M_PI)<0.15){
      if(!IsTuple){
	for(int i=0; i<ncth; i++)
	  for(int j=0; j<ndth; j++){
	    if(th_a > 1.+i*0.1 && th_a < M_PI - (1.+i*0.1) &&
	       delta_th<0.25-j*0.05 ) h[i][j]->Fill(bp1,bp2);
	  }
      }
      if(!(i%(NSim/100))){
	timer->Stop();
	now = timer->RealTime();
	if(now-prev>1){
	  cout<<"REALTIME = "<<now<<", "
	      <<"CPUTIME = "<<timer->CpuTime()<<", "
	      <<"EVENTNUM = "<<i<<", "
	      <<"p1 = "<<p1<<", "
	      <<"p2 = "<<p2
	      <<endl
	      <<flush;
	  prev = now;
	}
	timer->Start(kFALSE);
      }
      if(IsTuple) event.Vz = 1.;
    }else{
      if(IsTuple) event.Vz = 0.;
    }
    if(IsTuple) tree->Fill();
  }
  cout<<"Histogram has been sampled!"<<timer->RealTime()<<endl<<flush;

  if(!IsTuple){
    for(int i=0; i<ncth; i++)
      for(int j=0; j<ndth; j++)
	h[i][j]->Write();
  }else{
    tree->Write();
  }
  // Delete previous stored histograms
  f->Purge();
  cout<<"Histogram has been written!"<<timer->RealTime()<<endl<<flush;
  if(!IsTuple) 
    for(int i=0; i<ncth; i++)
      for(int j=0; j<ndth; j++)
	delete h[i][j];
  if(IsTuple) delete tree;
  delete f;
  cout<<"File has been closed!"<<timer->RealTime()<<endl<<flush;
}

void hist_sample_angle(Int_t iproc){
  ostringstream ost;

  // Open file. If it does not exist it will be created.
  TFile *f = NULL;
  string file_name;
  if(fname == ""){
    ost.str(""); ost<<"";
    ost<<proc_name[iproc]<<Int_t(EBeam+0.5)<<".root"<<ends;
    file_name = ost.str();
  } else {
    file_name = fname;
  }
  cout<<"Try to open output file."<<endl<<flush;
  cout<<file_name<<endl<<flush;
  f = new TFile(file_name.c_str(),"RECREATE");

  const double MAX_DPHI = 0.3, MAX_DTH = 0.5;
  const int ndphi = 6, ndth = 10;
  TH1D *hdth[ndphi];
  TH1D *hdphi[ndth];

  // Create new histogram
  for(int i=0; i<ndphi; i++){
    ost.str(""); ost<<proc_name[iproc]<<"_dth_"<<i;
    string name = ost.str();
    
    ost.str(""); ost<<setw(3)<<setprecision(2)<<MAX_DPHI - i*0.05;
    string dphi = ost.str();
    
    ost.str(""); ost<<"#Delta#theta, Energy = "<<Int_t(EBeam+0.5)<<" MeV, "
		    <<"#Delta#phi = "<<dphi;
    int nbins = 600;
    double dth_min = 0;
    double dth_max = MAX_DTH;
    hdth[i] = new TH1D(name.c_str(), ost.str().c_str(), nbins, dth_min, dth_max);
    hdth[i]->SetDirectory(f);
  }

  for(int i=0; i<ndth; i++){
    ost.str(""); ost<<proc_name[iproc]<<"_dphi_"<<i;
    string name = ost.str();
    
    ost.str(""); ost<<setw(3)<<setprecision(2)<<MAX_DTH - i*0.05;
    string dth = ost.str();
    
    ost.str(""); ost<<"#Delta#phi, Energy = "<<Int_t(EBeam+0.5)<<" MeV, "
		    <<"#Delta#theta = "<<dth;
    int nbins = 600;
    double dphi_min = 0;
    double dphi_max = MAX_DPHI;
    hdphi[i] = new TH1D(name.c_str(), ost.str().c_str(), nbins, dphi_min, dphi_max);
    hdphi[i]->SetDirectory(f);
  }

  Double_t p1,t1,f1,p2,t2,f2;
  UInt_t type;
  double prev = timer->RealTime(), now;
  for(int i=0; i<NSim; i++){

    gRad->MakeEvent(p1,t1,f1,p2,t2,f2,type);
    double a_delta_th  = fabs(t1 + t2 - M_PI);
    double a_delta_phi = fabs(fabs(f1 - f2) - M_PI);

    for(int j=0; j<ndphi; j++)
      if ( a_delta_phi < MAX_DPHI - j*0.05 ) hdth[j]->Fill(a_delta_th);
    
    for(int j=0; j<ndth; j++)
      if ( a_delta_th < MAX_DTH - j*0.05 ) hdphi[j]->Fill(a_delta_phi);
    
    if(!(i%(NSim/100))){
      timer->Stop();
      now = timer->RealTime();
      if(now-prev>1){
	cout<<"REALTIME = "<<now<<", "
	    <<"CPUTIME = "<<timer->CpuTime()<<", "
	    <<"EVENTNUM = "<<i<<", "
	    <<"p1 = "<<p1<<", "
	    <<"p2 = "<<p2
	    <<endl
	    <<flush;
	prev = now;
      }
      timer->Start(kFALSE);
    }
  }
  cout<<"Histogram has been sampled!"<<timer->RealTime()<<endl<<flush;

  for(int i=0; i<ndphi; i++)
    hdth[i]->Write();

  for(int i=0; i<ndth; i++)
    hdphi[i]->Write();

  // Delete previous stored histograms
  f->Purge();
  cout<<"Histogram has been written!"<<timer->RealTime()<<endl<<flush;
  
  for(int i=0; i<ndphi; i++)
    delete hdth[i];

  for(int i=0; i<ndth; i++)
    delete hdphi[i];

  delete f;
  cout<<"File has been closed!"<<timer->RealTime()<<endl<<flush;
}

void hist_sample_comp(){
  // Open file. If it does not exist it will be created.
  TFile *f = NULL;
  if(fname == "") return;
  cout<<"Try to open output file."<<endl<<flush;
  cout<<fname<<endl<<flush;
  f = new TFile(fname.c_str(),"RECREATE");

  double elemass = gConst->Me();
  double ebeam = gGlobal->Get_E();
  double emin = 0.8*ebeam;
  double thmin = gGlobal->Get_ThetaMin()*180/M_PI;
  double thmax = 180 - thmin;
  TH1D *an = new TH1D("an","Electron angle", 200, thmin - 1, thmax + 1);
  TH1D *en = new TH1D("en","Electron energy", 200, emin - 10, ebeam + 10);
  TH1D *ac = new TH1D("ac","Accolinearity", 200, -1, 11);
  TH1D *me = new TH1D("me","Missing energy", 200, -10, 0.4*ebeam);
  TH1D *ma = new TH1D("ma","Angle of missing energy",200,-1,181);

  Double_t prev, now;
  prev = timer->RealTime();
  for(int i=0; i<NSim; i++){
    Double_t p1,t1,f1,p2,t2,f2;
    UInt_t type;
    gRad->MakeEvent(p1,t1,f1,p2,t2,f2,type);
    p1 *= EBeam;
    p2 *= EBeam;

    an->Fill(t1*180/M_PI);
    double ele_en = sqrt(p1*p1 + elemass*elemass);
    double pos_en = sqrt(p2*p2 + elemass*elemass);
    if(ele_en>ebeam) ele_en = ebeam;
    if(pos_en>ebeam) pos_en = ebeam;
    en->Fill(ele_en-1e-12);
    ac->Fill(fabs(t1 + t2 - M_PI)*180/M_PI);
    double miss_en = 2*ebeam - ele_en - pos_en;
    if(miss_en>0.02*ebeam){
      me->Fill(miss_en);
      double cth0,sth0,cphi0,sphi0,cth1,sth1,cphi1,sphi1;
      sincos(t1, &sth0, &cth0);
      sincos(t2, &sth1, &cth1);
      sincos(f1,&sphi0,&cphi0);
      sincos(f2,&sphi1,&cphi1);
      double px = p1*sth0*cphi0 + p2*sth1*cphi1;
      double py = p1*sth0*sphi0 + p2*sth1*sphi1;
      double pz = p1*cth0 + p2*cth1;
      ma->Fill(acos(pz/sqrt(px*px+py*py+pz*pz))*180/M_PI);
    }

    if(!(i%(NSim/100))){
      timer->Stop();
      now = timer->RealTime();
      if(now-prev>1){
	cout<<"REALTIME = "<<now<<", "
	    <<"CPUTIME = "<<timer->CpuTime()<<", "
	    <<"EVENTNUM = "<<i<<", "
	    <<"p1 = "<<p1<<", "
	    <<"p2 = "<<p2
	    <<endl
	    <<flush;
	prev = now;
      }
      timer->Start(kFALSE);
    }
  }
  cout<<"Histogram has been sampled!"<<timer->RealTime()<<endl<<flush;
  f->Write();
  delete f;
  cout<<"File has been closed!"<<timer->RealTime()<<endl<<flush;
}

