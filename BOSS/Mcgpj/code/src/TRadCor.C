#include <iostream>
#include "TRadCor.h"
#include "TRandom.h"
#include "TGlobal.h"
#include "TStopwatch.h"
#include "TH1.h"

#ifdef HIST
extern TH1 *gH[], *gH2[];
#endif

TRadCor::TRadCor(TVCrossPart *m){
  fNEvents = 10000;
  fMatrix  = m;
  fNPart   = fMatrix->GetNPart();
  fMax     = new double[fNPart];
  fMin     = new double[fNPart];
  fSigma   = new double[fNPart];
  fCross   = new double[fNPart];
  fCross2  = new double[fNPart];
  fECross  = new double[fNPart];
  fECross2 = new double[fNPart];
  fSM      = new double[fNPart];
  fSM2     = new double[fNPart];
  fPart    = new double[fNPart];
  fNSim    = new unsigned int[fNPart];
  fN       = new unsigned int[fNPart];
  fWN      = new unsigned int[fNPart];
  fInList  = new bool[fNPart];
  for(unsigned int i=0; i<fNPart; i++) fInList[i] = true;

  fNew     = true;
  fIsSmear = false;
  Init();
}

TRadCor::TRadCor(){
  fMatrix  = 0;
  fWN      = 0;
  fN       = 0;
  fNSim    = 0;
  fPart    = 0;
  fSM2     = 0;
  fSM      = 0;
  fECross2 = 0;
  fECross  = 0;
  fCross2  = 0;
  fCross   = 0;
  fSigma   = 0;
  fMin     = 0;
  fMax     = 0;
  fNPart   = 0;
  fInList  = 0;
}

void TRadCor::Init(){
  fMatrix->Init();
  for(unsigned int i=0; i<fNPart; i++){
    fMax[i]     =-1e20;
    fMin[i]     = 1e20;
    fSigma[i]   = 1e20;
    fNSim[i]    = 0;
    fN[i]       = 0;
    fWN[i]      = 0;
    fSM[i]      = 0;
    fSM2[i]     = 0;
    fPart[i]    = 0;
    fCross[i]   = 0;
    fCross2[i]  = 0;
    fECross[i]  = 0;
    fECross2[i] = 0;
  }
  
  fTheta_Aver_0  = gCut->AverageThetaMin() - 0.5*gGlobal->Get_de()/(gGlobal->Get_E()*cos(gCut->AverageThetaMin()));
  fTheta_Aver_0  = (fTheta_Aver_0<gGlobal->Get_ThetaMin()) ? gGlobal->Get_ThetaMin() : fTheta_Aver_0;
}

TRadCor::~TRadCor(){
  delete [] fMax;
  delete [] fMin;
  delete [] fSigma;
  delete [] fCross;
  delete [] fCross2;
  delete [] fECross;
  delete [] fECross2;
  delete [] fSM;
  delete [] fSM2;
  delete [] fPart;
  delete [] fNSim;
  delete [] fN;
  delete [] fWN;
  delete [] fInList;
}

void TRadCor::MakeMaximum(const unsigned int &ipart){
  SetSpecialCase(ipart);
  for(unsigned int i=0;i<fNEvents;i++){
    double m = fMatrix->GetValue(ipart);
    fSM[ipart]  += m;
    fSM2[ipart] += m*m;
    fWN[ipart]++;
    if ( m != 0  &&  m > fMax[ipart] ) fMax[ipart] = m; 
    if ( m != 0  &&  m < fMin[ipart] ) fMin[ipart] = m; 
#ifdef HIST
    double p0, t0, ph0, p1, t1, ph;
    fMatrix->GetEvent(p0, t0, ph0, p1, t1, ph);
    gH[ipart]->Fill(fabs(t0+t1-M_PI), m);
#endif
  }
  fCross2[ipart]  = gGlobal->Get_Norm()*fSM[ipart]/fWN[ipart];
  fECross2[ipart] = gGlobal->Get_Norm()*sqrt((fSM2[ipart]-1.*fSM[ipart]*fSM[ipart]/fWN[ipart]))/fWN[ipart];
}

void TRadCor::MakeIntegral(const unsigned int &ipart){
  SetSpecialCase(ipart);
  for(unsigned int i=0;i<fNEvents;i++){
    double m = fMatrix->GetValue(ipart);
      
    fSM[ipart] += m;
    fSM2[ipart] += m*m;
    fWN[ipart]++;
    fN[ipart]++;
#ifdef HIST
    double p0, t0, ph0, p1, t1, ph;
    fMatrix->GetEvent(p0, t0, ph0, p1, t1, ph);
    gH[ipart]->Fill(fabs(t0+t1-M_PI), m);
#endif
    if ( m != 0  &&  m > fMax[ipart]*gRandom->Rndm() ) {
      fNSim[ipart]++;
#ifdef HIST
      gH2[ipart]->Fill(fabs(t0+t1-M_PI));
#endif
      //      fMatrix->GetEvent()->CosPrint();
    }
  }
  fCross[ipart]  = gGlobal->Get_Norm()*fNSim[ipart]*fMax[ipart]/fN[ipart];
  double r = (1.0*fNSim[ipart])/fN[ipart];
  fECross[ipart] = gGlobal->Get_Norm()*fMax[ipart]*sqrt(r*(1.-r)/fN[ipart]);

  fCross2[ipart]  = gGlobal->Get_Norm()*fSM[ipart]/fWN[ipart];
  fECross2[ipart] = gGlobal->Get_Norm()*sqrt((fSM2[ipart]-1.*fSM[ipart]*fSM[ipart]/fWN[ipart]))/fWN[ipart];
}

double TRadCor::Norm(const unsigned int &ipart){
  return 1;
}

void TRadCor::MakeAllMaximums(){
  for(unsigned int i=0; i<fNPart; i++){
    SetSpecialCase(i);
    MakeMaximum(i);
  }
}

void TRadCor::MakeAllIntegrals(){
  for(unsigned int i=0; i<fNPart; i++){
    SetSpecialCase(i);
    MakeIntegral(i);
  }
}

void TRadCor::MakePart(){
  fPart[0] = fCross2[0];
  for(unsigned int i=1; i<fNPart; i++)
    fPart[i] = fPart[i-1] + fCross2[i];

  for(unsigned int i=0; i<fNPart; i++){
    fPart[i]/= fPart[fNPart-1];
  }
}

unsigned int TRadCor::MakeEvent(){
  double r = gRandom->Rndm();
  unsigned int ipart = 0;
  while(r>fPart[ipart]) ipart++;
  SetSpecialCase(ipart);
  while (true){
    double m = fMatrix->GetValue(ipart);
    if ( m != 0  &&  m>fMax[ipart]*gRandom->Rndm() ) break;
  }
  return ipart;
}

void TRadCor::MakeCrossSection(){
  using namespace std;
  bool *done = new bool[fNPart];
  TStopwatch *timer = new TStopwatch();
  timer->Start(kFALSE);
  //  timer->Stop();
  //  cout<<"REALTIME = "<<timer->RealTime()<<", CPUTIME = "<<timer->CpuTime()<<endl<<flush;
  //  timer->Start(kFALSE);

  for(unsigned int j=0;j<fNPart;j++) done[j] = false;
  int n = 0;
  for(unsigned int j=0;j<fNPart;j++) if(fInList[j]) n++;
  double te  = gGlobal->Get_TotalError()/sqrt(n/2.);
  double tre = gGlobal->Get_RelativeError()*sqrt(2.);
  bool alldone = false;
  while(!alldone){
    alldone = true;
    for(unsigned int j=0;j<fNPart; j++){
      if(!fInList[j])continue;
      unsigned int i = j;
      if(!done[j]){
	MakeMaximum(i);
	double as = fabs(GetCross2(i));
	double es = fabs(GetECross2(i));
	if((es < te && es/as < tre) || ( es < 0.2*te )){
	  cout<<"Part = "<<i<<", Min = "<<GetMin(i)<<", Max = "<<GetMax(i)<<", "
	      <<"N = "<<GetWN(i)<<", "
	      <<"sw = "<<GetCross2(i)<<" +- "<<es<<" nb"
	      <<endl;
	  done[j] = true;
	} else {
	  alldone = false;
	}
      }
    }
  }
  
  timer->Stop();
  cout<<"REALTIME = "<<timer->RealTime()<<", CPUTIME = "<<timer->CpuTime()<<endl<<flush;
  timer->Start(kFALSE);
  for(unsigned int j=0;j<fNPart;j++) done[j] = false;
  
  double su = 0, sw = 0, esu=0, esw=0;
  alldone = false;
  while(!alldone){
    alldone = true;
    for(unsigned int j=0;j<fNPart;j++){
      if(!fInList[j])continue;
      unsigned int i = j;
      if(!done[j]){
	MakeIntegral(i);
	double as = fabs(GetCross2(i));
	double es = fabs(GetECross2(i));
	if ( (es < te/sqrt(2.) && es/as < tre/sqrt(2.) && (GetNSim(i) > 4000 || GetMax(i)<=0) ) ||
	     (es < 0.2*te/sqrt(2.)  && GetNSim(i) > 400  ) || 
	     (es/as < 1e-5 && GetECross(i)/GetCross(i) < 1e-4)
	     ){
	  cout<<"Part = "<<i<<", "
	      <<"N = "<<GetNSim(i)<<", "
	      <<"s = "<<GetCross(i)<<" +- "<<GetECross(i)<<" nb"<<", "
	      <<"N = "<<GetWN(i)<<", "
	      <<"sw = "<<GetCross2(i)<<" +- "<<es<<" nb"
	      <<endl;
	  su  += GetCross(i);
	  esu += GetECross(i)*GetECross(i);
	  sw  += GetCross2(i);
	  esw += es*es;
	  done[j] = true;
	} else {
	  alldone = false;
	}
      }
    }
  }
  esu = sqrt(esu);
  esw = sqrt(esw);
  cout<<"CrossSection = "<<su<<" +- "<<esu<<" nb, "
      <<"relative error is "<<esu/su*100<<"%"<<endl;
  cout<<"CrossSection2 = "<<sw<<" +- "<<esw<<" nb, "
      <<"relative error is "<<esw/sw*100<<"%"<<endl;
  
  timer->Stop();
  cout<<"REALTIME = "<<timer->RealTime()<<", CPUTIME = "<<timer->CpuTime()<<endl<<flush;
  timer->Start(kFALSE);

  MakePart();

  delete [] done;
  delete timer;
}
