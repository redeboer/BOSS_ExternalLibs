#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>

#include "TFile.h"
#include "TRandom3.h"

#include "TRadCor.h"
#include "TEPCrossPart.h"
#include "TMuCrossPart.h"
#include "TPiCrossPart.h"
#include "TKnCrossPart.h"
#include "TRadGlobal.h"
#include "TKinemCut.h"

TRadCor *gRad;
TRadGlobal *gGlobal;
TKinemCut *gCut;
TConstants *gConst;
int pid[2];
extern "C" {
  void init_prime_gen_(const int&, const double&, 
		      const double&, const double&);
  void makeevent_(double*, int*, int&);
  void genfinish_(void);
}
using namespace std;
string RandomFileName;
void init_prime_gen_(const int& proc, const double& EBeam, 
		    const double& te, const double& c){
  cout<<proc<<" "<<EBeam<<" "<<te<<" "<<c<<endl<<flush;

  gConst  = new TConstants();
  gGlobal = new TRadGlobal();
  gCut    = new TKinemCut();

  int NRad         = 10000;
  int IsHardPhoton = 1;
  int IsNoVacPol   = 0;
  int IsFSR        = 1;
  RandomFileName = ".random_stat.root";
  double pc  = 0.;
  double de  = -1.;
  double nt0 = -1.;
  double dt  = 0.5;
  double dp  = 0.3;
  double at  = c;
  double td  = c - dt/2;
  double am  = 90.;
  double cm  = 90.;
  double em  = 50.;
  double ti  = 0.473;
  double al  = 1.;
  double thm = -1.;
  double thp = -1.;
  double re  = 0.05;

  if((proc%10)==3){
    td = 0.025;
    dt = 10;
    dp = 10;
    cm = 0;
    am = 0;
  }
  
  const int MaxList = 20;
  bool InList[MaxList];
  for(int j = 0; j<MaxList; j++) InList[j] = true;

  if( ( EBeam < 100 || EBeam > 2500 ) && !IsNoVacPol ){
    cerr<<"Invalid value of beam energy:"<<EBeam<<endl;
    exit(1);
  }

  gGlobal->Set_TotalError(te);
  
  gGlobal->Set_RelativeError(re);

  gGlobal->Set_Type((proc%10));

  gGlobal->Set_E(EBeam);
    
  gGlobal->Set_ThetaInt(ti);

  if(0>td)
    gGlobal->Set_ThetaMin(at-dt/2);
  else
    gGlobal->Set_ThetaMin(td);

  if(0>de){
    if((proc%10)==0) 
      gGlobal->Set_dE_Abs(0.015*EBeam);
    else 
      gGlobal->Set_dE_Abs(0.0003*EBeam);
  }
  else
    gGlobal->Set_dE_Abs(de);

  if(0>nt0){
    if(proc>10) 
      gGlobal->Set_Theta0_Rel(0.0);
    else
      gGlobal->Set_Theta0_Rel(1.5);
  }else
    gGlobal->Set_Theta0_Rel(nt0);

  if(0<thm){
    gCut->ThetaMinM(thm);
    gGlobal->Set_ThetaMin(thm);
  }

  if(0<thp)
    gCut->ThetaMinP(thp);

  gCut->CosPsi(cos(pc));

  gCut->DeltaTheta(dt);
  
  gCut->AverageTheta(at);

  gCut->DeltaPhi(dp);
  
  gCut->PAverage(am);
    
  gCut->PCross(cm);

  gCut->EMin(em);
  
  gConst->SetAlphaScale(al);

  gConst->Print();

  try{
  gGlobal->Init();
  }catch(std::logic_error lge){
	    cout<<lge.what()<<endl;
	      exit(-1);
	      }
  gGlobal->Print();

  gCut->Init();
  gCut->Print();

  cout<<"Cross-section statistical precision will be better than "
      <<gGlobal->Get_TotalError()<<" nb and "
      <<gGlobal->Get_RelativeError()*100<<"%"<<endl;

  if(!IsHardPhoton)
    cout<<"Hard photon on big angle is not included!"<<endl;
    
  if(IsNoVacPol)
    cout<<"Vacuum polarization is not included!"<<endl;

  if(!IsFSR)
    cout<<"Final state radiation is not included!"<<endl;

  if(proc>10)
    cout<<"Alpha order generation only!"<<endl;

  cout<<flush;

  if(gRandom) delete gRandom;
  gRandom = new TRandom3();
  struct stat stbuf;
  if( stat(RandomFileName.c_str(),&stbuf) == 0 ){
    cout<<"Reading random seed from file \""<<RandomFileName<<"\"."<<endl;
    gRandom->ReadRandom(RandomFileName.c_str());
  }

  TVCrossPart *MatrixElements;
  if(proc==0){
    MatrixElements = new TEPCrossPart();
    InList[18] = false;
  }
  else if(proc==1)
    MatrixElements = new TMuCrossPart();
  else if(proc==2)
    MatrixElements = new TPiCrossPart();
  else if(proc==3)
    MatrixElements = new TKnCrossPart();
  else if(proc==10){
    MatrixElements = new TEPCrossPart();
    for(int j = 0; j<MaxList; j++) InList[j] = false;
    InList[16] = true;
    InList[17] = true;
    InList[18] = true;
  }
  else
    return;

  if(IsNoVacPol)
    MatrixElements->SetZeroVP();

  if(!IsFSR)
    MatrixElements->SetNoFSR();

  gRad = new TRadCor(MatrixElements);
  gRad->SetNEvents(NRad);
  gRad->SetPartList(InList);
  gRad->Init();

  MatrixElements->SetHardPhoton(IsHardPhoton);
  gRad->MakeCrossSection();
  if((proc%10)==2)((TPiCrossPart*)MatrixElements)->GetFormFactor()->Print();

  if((proc%10)==0){
    pid[0] = 3; pid[1] = 2;
  }
  if((proc%10)==1){
    pid[0] = 6; pid[1] = 5;
  }
  if((proc%10)==2){
    pid[0] = 9; pid[1] = 8;
  }
  if((proc%10)==3){
    pid[0] = 10; pid[1] = 16;
  }
  if((proc%10)==4){
    pid[0] = 12; pid[1] = 11;
  }
}

void makeevent_(double *mom, int *part, int &n){
  gRad->MakeEvent(mom, n);
  part[0] = pid[0];
  part[1] = pid[1];
  for(int i=2;i<n;i++){
    part[i] = 1;
  }
}

void genfinish_(void){
  struct stat stbuf;
  if( stat(RandomFileName.c_str(),&stbuf) == 0 ){
    remove(RandomFileName.c_str());
  }
  cout<<"Writing random seed to file \""<<RandomFileName<<"\"."<<endl;
  gRandom->WriteRandom(RandomFileName.c_str());
  delete gRandom;
}
