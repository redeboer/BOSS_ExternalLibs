//
//  Parameters for Emc Reconstruction
//  
//  No Parameter is allowed to be hard coded into code!
//  
//  Created by Zhe Wang, May 31, 2004
//
#include "EmcRec/EmcRecParameter.h"
#include "TGraph2DErrors.h"
#include <fstream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

//Initialize static data member
EmcRecParameter* EmcRecParameter::fpInstance=0;
pthread_mutex_t EmcRecParameter::m_pthread_lock = PTHREAD_MUTEX_INITIALIZER;

// Constructors and destructors 
EmcRecParameter::EmcRecParameter()
{
  string paraPath = getenv("EMCRECROOT");
  if(paraPath==""){} //cout<<"EmcRec environment not set!";
  string paraPath1(paraPath);
  string paraPath2(paraPath);
  string paraPath3(paraPath);
  string paraPath4(paraPath);
  string paraPath3_1(paraPath);
  string paraPath4_1(paraPath);
  string paraPath5(paraPath);
  string paraPath6(paraPath);
  string paraPath7(paraPath);
  string paraPath8(paraPath);
  string paraPath9(paraPath);
  string paraPath10(paraPath);
  string paraPath11(paraPath);
  string paraPath12(paraPath);
  string paraPath11_1(paraPath);
  string paraPath12_1(paraPath);
  string paraPath13(paraPath);
  string paraPath14(paraPath);
  string paraPath15(paraPath);
  string paraPath16(paraPath);
  string paraPath17(paraPath);
  string paraPath18(paraPath);
  string paraPath19(paraPath);
  string paraPath18_1(paraPath);
  string paraPath19_1(paraPath);
  string paraPath20(paraPath);
  string paraPath21(paraPath);
  string paraPath22(paraPath);
  string paraPath23(paraPath);
  string paraPath24(paraPath);
  string paraPath25(paraPath);
  string paraPath26(paraPath);
  string paraPath27(paraPath);
  string paraPath28(paraPath);
  string paraPath29(paraPath);
  paraPath += "/share/EmcRecPara.dat";
  //cout<<paraPath<<endl;
  ifstream in;
  in.open(paraPath.c_str());
  assert(in);

  const int maxCharOfOneLine=255;
  char temp[maxCharOfOneLine];
  int inputNo=0;
  while(in.peek()!=EOF) {
    in.getline(temp,maxCharOfOneLine);
    if(temp[0]=='#') continue;
    inputNo++;
    switch(inputNo) {
      case 1:
        istringstream(temp)>>fElectronicsNoiseLevel>>fEThresholdSeed>>fEThresholdCluster>>fLogPosOffset;
        break;
      case 2:
        istringstream(temp)>>fMoliereRadius>>fLateralProfile;
        break;
      case 3:
        istringstream(temp)>>eCorr[0]>>eCorr[1]>>eCorr[2]>>eCorr[3];
        break;
      case 4:
        istringstream(temp)>>sigE[0]>>sigE[1]>>sigE[2]
          >>sigTheta[0]>>sigTheta[1]>>sigPhi[0]>>sigPhi[1];
        break;
      case 5:
        istringstream(temp)>>hitNb[0]>>hitNb[1]>>hitNb[2];
        break;
      case 6:
        istringstream(temp)>>elecBias[0]>>elecBias[1]>>elecBias[2]
          >>elecBias[3]>>elecBias[4];
        break;
      case 7:
        istringstream(temp)>>smCut[0]>>smCut[1]>>smCut[2]>>smCut[3];
        break;
      default:
        break;
    }
  }
  in.close();
  
  paraPath1 += "/share/Peak1.843_10.12calib.dat";
  ifstream in1;
  in1.open(paraPath1.c_str());
  assert(in1);
  int ntheta;
  double sigma,sigmaerr,peakerr;
  for(int i=0;i<56;i++) {
    in1>>ntheta>>sigma>>sigmaerr>>peak[i]>>peakerr;
  }
  in1.close();

  // Read energy correction parameters from PhotonCor/McCor
  paraPath2 += "/share/evset.txt";
  ifstream in2;
  in2.open(paraPath2.c_str());
  assert(in2);
  double energy,thetaid,peak1,peakerr1,res,reserr;

  dt = new TGraph2DErrors();
  dtErr = new TGraph2DErrors();
  for(int i=0;i<560;i++){
    in2>>energy;
    in2>>thetaid;
    in2>>peak1;
    in2>>peakerr1;
    in2>>res;
    in2>>reserr;
    dt->SetPoint(i,energy,thetaid,peak1);
    dt->SetPointError(i,0,0,peakerr1);
    dtErr->SetPoint(i,energy,thetaid,res);
    dtErr->SetPointError(i,0,0,reserr);

    if(i<28) e25min[int(thetaid)]=energy;
    if(i>=560-28) e25max[int(thetaid)]=energy;

  }
  in2.close();

//  Position correction parameters 
  paraPath3_1 += "/share/BarrLogShMaxThetaPara.dat";
  ifstream in3_1;
  in3_1.open(paraPath3_1.c_str());
  assert(in3_1);
  for(int i=0;i<132;i++){
    for(int j=0;j<5;j++){
  in3_1>>barrLogShMaxThetaPara[i][j];
  }
  }
  in3_1.close();
  
  paraPath4_1 += "/share/BarrLogShMaxPhiPara.dat";
  ifstream in4_1;
  in4_1.open(paraPath4_1.c_str());
  assert(in4_1);
  for(int i=0;i<132;i++){
    for(int j=0;j<5;j++){
  in4_1>>barrLogShMaxPhiPara[i][j];
  } 
  } 
  in4_1.close();

  paraPath11_1 += "/share/EastLogShMaxThetaPara.dat";
  ifstream in11_1;
  in11_1.open(paraPath11_1.c_str());
  assert(in11_1);
  for(int i=0;i<18;i++){
    for(int j=0;j<5;j++){
  in11_1>>eastLogShMaxThetaPara[i][j];
  }
  }
  in11_1.close();

  paraPath12_1 += "/share/WestLogShMaxThetaPara.dat";
  ifstream in12_1;
  in12_1.open(paraPath12_1.c_str());
  assert(in12_1);
  for(int i=0;i<18;i++){
    for(int j=0;j<5;j++){
  in12_1>>westLogShMaxThetaPara[i][j];
  }
  }
  in12_1.close();

  paraPath18_1 += "/share/EastLogShMaxPhiPara.dat";
  ifstream in18_1;
  in18_1.open(paraPath18_1.c_str());
  assert(in18_1);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in18_1>>eastLogShMaxPhiPara[i][j];
  }
  }
  in18_1.close();

  paraPath19_1 += "/share/WestLogShMaxPhiPara.dat";
  ifstream in19_1;
  in19_1.open(paraPath19_1.c_str());
  assert(in19_1);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in19_1>>westLogShMaxPhiPara[i][j];
  }
  }
  in19_1.close();



//  Position correction parameters 
  paraPath3 += "/share/BarrLogThetaPara.dat";
  ifstream in3;
  in3.open(paraPath3.c_str());
  assert(in3);
  for(int i=0;i<66;i++){
    for(int j=0;j<5;j++){
  in3>>barrLogThetaPara[i][j];
  }
  }
  in3.close();
  
  paraPath4 += "/share/BarrLogPhiPara.dat";
  ifstream in4;
  in4.open(paraPath4.c_str());
  assert(in4);
  for(int i=0;i<66;i++){
    for(int j=0;j<5;j++){
  in4>>barrLogPhiPara[i][j];
  } 
  } 
  in4.close();

  paraPath5 += "/share/BarrLinThetaPara.dat";
  ifstream in5;
  in5.open(paraPath5.c_str());
  assert(in5);
  for(int i=0;i<66;i++){
    for(int j=0;j<5;j++){
  in5>>barrLinThetaPara[i][j];
  }
  }
  in5.close();

  paraPath6 += "/share/BarrLinPhiPara.dat";
  ifstream in6;
  in6.open(paraPath6.c_str());
  assert(in6);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in6>>barrLinPhiPara[i][j];
  }
  }
  in6.close();

  /*
  paraPath7 += "/share/BarrShLogThetaPara.dat";
  ifstream in7;
  in7.open(paraPath7.c_str());
  assert(in7);
  for(int i=0;i<66;i++){
    for(int j=0;j<5;j++){
  in7>>barrShLogThetaPara[i][j];
  }
  }
  in7.close();

  paraPath8 += "/share/BarrShLogPhiPara.dat";
  ifstream in8;
  in8.open(paraPath8.c_str());
  assert(in8);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in8>>barrShLogPhiPara[i][j];
  }
  }
  in8.close();
  */

  paraPath9 += "/share/BarrShLinThetaPara.dat";
  ifstream in9;
  in9.open(paraPath9.c_str());
  assert(in9);
  for(int i=0;i<66;i++){
    for(int j=0;j<5;j++){
  in9>>barrShLinThetaPara[i][j];
  }
  }
  in9.close();

  paraPath10 += "/share/BarrShLinPhiPara.dat";
  ifstream in10;
  in10.open(paraPath10.c_str());
  assert(in10);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in10>>barrShLinPhiPara[i][j];
  }
  }
  in10.close();


  paraPath11 += "/share/EastLogThetaPara.dat";
  ifstream in11;
  in11.open(paraPath11.c_str());
  assert(in11);
  for(int i=0;i<18;i++){
    for(int j=0;j<5;j++){
  in11>>eastLogThetaPara[i][j];
  }
  }
  in11.close();

  paraPath12 += "/share/WestLogThetaPara.dat";
  ifstream in12;
  in12.open(paraPath12.c_str());
  assert(in12);
  for(int i=0;i<18;i++){
    for(int j=0;j<5;j++){
  in12>>westLogThetaPara[i][j];
  }
  }
  in12.close();

  paraPath13 += "/share/EastLinThetaPara.dat";
  ifstream in13;
  in13.open(paraPath13.c_str());
  assert(in13);
  for(int i=0;i<6;i++){
    for(int j=0;j<5;j++){
  in13>>eastLinThetaPara[i][j];
  }
  }
  in13.close();

  paraPath14 += "/share/WestLinThetaPara.dat";
  ifstream in14;
  in14.open(paraPath14.c_str());
  assert(in14);
  for(int i=0;i<6;i++){
    for(int j=0;j<5;j++){
  in14>>westLinThetaPara[i][j];
  }
  }
  in14.close();

  paraPath15 += "/share/BarrDataLogThetaPara.dat";
  ifstream in15;
  in15.open(paraPath15.c_str());
  assert(in15);
  for(int i=0;i<22;i++){
    for(int j=0;j<5;j++){
  in15>>barrDataLogThetaPara[i][j];
  }
  }
  in15.close();

  paraPath16 += "/share/EastDataLogThetaPara.dat";
  ifstream in16;
  in16.open(paraPath16.c_str());
  assert(in16);
  for(int i=0;i<6;i++){
    for(int j=0;j<5;j++){
  in16>>eastDataLogThetaPara[i][j];
  }
  }
  in16.close();

  paraPath17 += "/share/WestDataLogThetaPara.dat";
  ifstream in17;
  in17.open(paraPath17.c_str());
  assert(in17);
  for(int i=0;i<6;i++){
    for(int j=0;j<5;j++){
  in17>>westDataLogThetaPara[i][j];
  }
  }
  in17.close();


  paraPath18 += "/share/EastLogPhiPara.dat";
  ifstream in18;
  in18.open(paraPath18.c_str());
  assert(in18);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in18>>eastLogPhiPara[i][j];
  }
  }
  in18.close();

  paraPath19 += "/share/WestLogPhiPara.dat";
  ifstream in19;
  in19.open(paraPath19.c_str());
  assert(in19);
  for(int i=0;i<3;i++){
    for(int j=0;j<5;j++){
  in19>>westLogPhiPara[i][j];
  }
  }
  in19.close();

  paraPath20 += "/share/EastLinPhiPara.dat";
  ifstream in20;
  in20.open(paraPath20.c_str());
  assert(in20);
  for(int i=0;i<1;i++){
    for(int j=0;j<5;j++){
  in20>>eastLinPhiPara[i][j];
  }
  }
  in20.close();

  paraPath21 += "/share/WestLinPhiPara.dat";
  ifstream in21;
  in21.open(paraPath21.c_str());
  assert(in21);
  for(int i=0;i<1;i++){
    for(int j=0;j<5;j++){
  in21>>westLinPhiPara[i][j];
  }
  }
  in21.close();

  paraPath22 += "/share/BarrLoglinThetaPara.dat";
  ifstream in22;
  in22.open(paraPath22.c_str());
  assert(in22);
  for(int i=0;i<22;i++){
    for(int j=0;j<5;j++){
  in22>>barrLoglinThetaPara[i][j];
  }
  }
  in22.close();

  paraPath23 += "/share/BarrLoglinPhiPara.dat";
  ifstream in23;
  in23.open(paraPath23.c_str());
  assert(in23);
  for(int i=0;i<1;i++){
    for(int j=0;j<5;j++){
  in23>>barrLoglinPhiPara[i][j];
  }
  }
  in23.close();


  int ith,iph;
  double dth,dtherr,sig,sigerr;

  paraPath24 += "/share/PosCorDataBarr.conf";
  ifstream in24;
  in24.open(paraPath24.c_str());
  assert(in24);

  for(int i=0;i<5280;i++){

    in24>>iph>>ith>>dth>>dtherr>>sig>>sigerr;
    barrPosDataCorPara[ith][iph]=dth;

  }
  in24.close();

  paraPath25 += "/share/PosCorDataWest.conf";
  ifstream in25;
  in25.open(paraPath25.c_str());
  assert(in25);
  for(int i=0;i<480;i++){

    in25>>ith>>iph>>dth>>dtherr>>sig>>sigerr;
    westPosDataCorPara[ith][iph]=dth;

  }
  in25.close();

  paraPath26 += "/share/PosCorDataEast.conf";
  ifstream in26;
  in26.open(paraPath26.c_str());
  assert(in26);
  for(int i=0;i<480;i++){

    in26>>ith>>iph>>dth>>dtherr>>sig>>sigerr;
    eastPosDataCorPara[ith][iph]=dth;

  }
  in26.close();

  ////////////////////MCposcor
  paraPath27 += "/share/PosCorMCBarr.conf";
  ifstream in27;
  in27.open(paraPath27.c_str());
  assert(in27);

  for(int i=0;i<5280;i++){

    in27>>iph>>ith>>dth>>dtherr>>sig>>sigerr;
    barrPosMCCorPara[ith][iph]=dth;

  }
  in27.close();

  paraPath28 += "/share/PosCorMCWest.conf";
  ifstream in28;
  in28.open(paraPath28.c_str());
  assert(in28);
  for(int i=0;i<480;i++){

    in28>>ith>>iph>>dth>>dtherr>>sig>>sigerr;
    westPosMCCorPara[ith][iph]=dth;

  }
  in28.close();

  paraPath29 += "/share/PosCorMCEast.conf";
  ifstream in29;
  in29.open(paraPath29.c_str());
  assert(in29);
  for(int i=0;i<480;i++){

    in29>>ith>>iph>>dth>>dtherr>>sig>>sigerr;
    eastPosMCCorPara[ith][iph]=dth;

  }
  in29.close();


}


EmcRecParameter::~EmcRecParameter()
{
  //   cout<<"deconstructed"<<endl;
  delete dt;
  delete dtErr;
}

// static method
//Access to an instance
EmcRecParameter& EmcRecParameter::GetInstance()
{
  if(!Exist()) {
    fpInstance=new EmcRecParameter;
  }
  return *fpInstance;
}

bool EmcRecParameter::Exist()
{
  return fpInstance!=0;
}

void EmcRecParameter::Kill()
{
  if(Exist()) {
    delete fpInstance;
    fpInstance=0;
  }
}

//access to each parameter
double EmcRecParameter::ElectronicsNoiseLevel() const
{
  return fElectronicsNoiseLevel;
}

double EmcRecParameter::EThresholdSeed() const
{
  return fEThresholdSeed;
}

double EmcRecParameter::EThresholdCluster() const
{
  return fEThresholdCluster;
}

double EmcRecParameter::LogPosOffset() const
{
  return fLogPosOffset;
}

double EmcRecParameter::TimeMin() const
{
  return fTimeMin;
}

double EmcRecParameter::TimeMax() const
{
  return fTimeMax;
}


double EmcRecParameter::MethodMode() const
{
  return fMethodMode;
}

double EmcRecParameter::PosCorr() const
{ 
  return fPosCorr;
}

double EmcRecParameter::DataMode() const
{ 
  return fDataMode;
} 
int EmcRecParameter::ElecSaturation() const
{
  return fElecSaturation;
}

double EmcRecParameter::MoliereRadius() const
{
  return fMoliereRadius;
}

double EmcRecParameter::LateralProfile() const
{
  return fLateralProfile;
}

double EmcRecParameter::ECorr(int n) const
{
  return eCorr[n];
}

double EmcRecParameter::SigE(int n) const
{
  return sigE[n];
}

double EmcRecParameter::SigTheta(int n) const
{
  return sigTheta[n];
}

double EmcRecParameter::SigPhi(int n) const
{
  return sigPhi[n];
}

double EmcRecParameter::HitNb(int n) const
{
  return hitNb[n];
}

double EmcRecParameter::ElecBias(int n) const
{
  return elecBias[n];
}

double EmcRecParameter::SmCut(int n) const
{
  return smCut[n];
}

double EmcRecParameter::Peak(int n) const
{
  return peak[n];
}

double EmcRecParameter::BarrLogThetaPara(int n , int m) const
{
  return barrLogThetaPara[n][m];
}



double EmcRecParameter::BarrLogPhiPara(int n ,int m) const
{
  return barrLogPhiPara[n][m];
}


double EmcRecParameter::BarrLoglinThetaPara(int n , int m) const
{
  return barrLoglinThetaPara[n][m];
}



double EmcRecParameter::BarrLoglinPhiPara(int n ,int m) const
{
  return barrLoglinPhiPara[n][m];
}



double EmcRecParameter::BarrLinThetaPara(int n , int m) const
{
  return barrLinThetaPara[n][m];
}

double EmcRecParameter::BarrLinPhiPara(int n ,int m) const
{
  return barrLinPhiPara[n][m];
}

double EmcRecParameter::BarrLogShMaxThetaPara(int n , int m) const
{
  return barrLogShMaxThetaPara[n][m];
}

double EmcRecParameter::BarrLogShMaxPhiPara(int n ,int m) const
{
  return barrLogShMaxPhiPara[n][m];
}

double EmcRecParameter::BarrShLinThetaPara(int n , int m) const
{
  return barrShLinThetaPara[n][m];
}

double EmcRecParameter::BarrShLinPhiPara(int n ,int m) const
{
  return barrShLinPhiPara[n][m];
}


double EmcRecParameter::EastLogThetaPara(int n ,int m) const
{
  return eastLogThetaPara[n][m];
}

double EmcRecParameter::WestLogThetaPara(int n ,int m) const
{
  return westLogThetaPara[n][m];
}

double EmcRecParameter::EastLogPhiPara(int n ,int m) const
{
  return eastLogPhiPara[n][m];
}

double EmcRecParameter::WestLogPhiPara(int n ,int m) const
{
  return westLogPhiPara[n][m];
}


double EmcRecParameter::EastLogShMaxThetaPara(int n ,int m) const
{
  return eastLogShMaxThetaPara[n][m];
}

double EmcRecParameter::WestLogShMaxThetaPara(int n ,int m) const
{
  return westLogShMaxThetaPara[n][m];
}

double EmcRecParameter::EastLogShMaxPhiPara(int n ,int m) const
{
  return eastLogShMaxPhiPara[n][m];
}

double EmcRecParameter::WestLogShMaxPhiPara(int n ,int m) const
{
  return westLogShMaxPhiPara[n][m];
}


double EmcRecParameter::EastLinPhiPara(int n ,int m) const
{
  return eastLinPhiPara[n][m];
}

double EmcRecParameter::WestLinPhiPara(int n ,int m) const
{
  return westLinPhiPara[n][m];
}


double EmcRecParameter::EastLinThetaPara(int n ,int m) const
{
  return eastLinThetaPara[n][m];
}

double EmcRecParameter::WestLinThetaPara(int n ,int m) const
{
  return westLinThetaPara[n][m];
}

double EmcRecParameter::BarrDataLogThetaPara(int n , int m) const
{
  return barrDataLogThetaPara[n][m];
}

double EmcRecParameter::BarrPosDataCor(int ntheta, int nphi) const
{
  return barrPosDataCorPara[ntheta][nphi];
}

double EmcRecParameter::WestPosDataCor(int ntheta, int nphi) const
{
  return westPosDataCorPara[ntheta][nphi];
}

double EmcRecParameter::EastPosDataCor(int ntheta, int nphi) const
{
  return eastPosDataCorPara[ntheta][nphi];
}

double EmcRecParameter::BarrPosMCCor(int ntheta, int nphi) const
{
  return barrPosMCCorPara[ntheta][nphi];
}

double EmcRecParameter::WestPosMCCor(int ntheta, int nphi) const
{
  return westPosMCCorPara[ntheta][nphi];
}

double EmcRecParameter::EastPosMCCor(int ntheta, int nphi) const
{
  return eastPosMCCorPara[ntheta][nphi];
}



double EmcRecParameter::EastDataLogThetaPara(int n ,int m) const
{
  return eastDataLogThetaPara[n][m];
} 

double EmcRecParameter::WestDataLogThetaPara(int n ,int m) const
{
  return westDataLogThetaPara[n][m];
} 




void EmcRecParameter::SetPositionMode(std::vector<std::string>& mode)
{
  if(mode.size()==2) {
    positionMode1=mode[0];
    positionMode2=mode[1];
  }
}

//The following function is copied from PhotonCor/McCor
double EmcRecParameter::ECorrMC(double eg, double theid) const
{
  double Energy5x5=eg;
  if(eg<E25min(int(theid))) eg=E25min(int(theid));
  if(eg>E25max(int(theid))) eg=E25max(int(theid))-0.001;
  if(theid<=0)theid=0.001;
  if(theid>=27)theid=26.999;
  Float_t einter = eg + 0.00001;
  Float_t tinter = theid+0.0001;
  double ecor=dt->Interpolate(einter,tinter);
  if(!(ecor))return Energy5x5;
  if(ecor<0.5)return Energy5x5;
  double EnergyCor=Energy5x5/ecor;
  return EnergyCor;
}

// Get energy error
double EmcRecParameter::ErrMC(double eg, double theid) const
{

  if(eg<E25min(int(theid))) eg=E25min(int(theid));
  if(eg>E25max(int(theid))) eg=E25max(int(theid))-0.001;
  if(theid<=0)theid=0.001;
  if(theid>=27)theid=26.999;
  Float_t einter = eg + 0.00001;
  Float_t tinter = theid+0.0001;
  double err=dtErr->Interpolate(einter,tinter);
  return err;
}

double EmcRecParameter::E25min(int n) const
{
  return e25min[n];
}
double EmcRecParameter::E25max(int n) const
{
  return e25max[n];
}
