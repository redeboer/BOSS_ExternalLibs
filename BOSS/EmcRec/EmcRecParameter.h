//
//  Parameters for Emc Reconstruction
//  
//  No Parameter is allowed to be hard coded into code!
//  
//  Created by Zhe Wang, May 31, 2004
//
#ifndef EMC_REC_PARAMETER_A_H
#define EMC_REC_PARAMETER_A_H
#include <pthread.h>
#include <iostream>
#include <vector>

using namespace std;

class TGraph2DErrors;

class EmcRecParameter
{
  private:
    // Constructors and destructors 
    EmcRecParameter();
    ~EmcRecParameter();

  public:
    //static member functions
    static EmcRecParameter& GetInstance();
    static bool Exist();
    static void Kill();
    static void lock(){
      if(pthread_mutex_lock(&m_pthread_lock) != 0){
	std::cerr << "LOCK MUTEX_LOCK @ GZFSSTREAM" << std::endl;
      }
    };
    static void unlock(){
      if(pthread_mutex_unlock(&m_pthread_lock) != 0){
	std::cerr << "UNLOCK MUTEX_LOCK @ GZFSSTREAM" << std::endl;
      }
    };



  private:
    //static data members
    static EmcRecParameter* fpInstance;
    static pthread_mutex_t m_pthread_lock;


  public:
    //access to each parameter
    double ElectronicsNoiseLevel() const;
    double EThresholdSeed() const;
    double EThresholdCluster() const;
    double LogPosOffset() const;

    double TimeMin() const;
    double TimeMax() const;
    double MethodMode() const;
    double PosCorr() const;
    double DataMode() const;
    int ElecSaturation() const;

    double MoliereRadius() const;
    double LateralProfile() const;

    double ECorr(int n) const;
    double SigE(int n) const;
    double SigTheta(int n) const;
    double SigPhi(int n) const;

    double HitNb(int n) const;
    double ElecBias(int n) const;
    double SmCut(int n) const;

    double Peak(int n) const;

    double EastLogThetaPara(int n, int m) const;
    double WestLogThetaPara(int n, int m) const;

    double EastLogPhiPara(int n, int m) const;
    double WestLogPhiPara(int n, int m) const;

    double EastLogShMaxThetaPara(int n, int m) const;
    double WestLogShMaxThetaPara(int n, int m) const;

    double EastLogShMaxPhiPara(int n, int m) const;
    double WestLogShMaxPhiPara(int n, int m) const;



    double EastDataLogThetaPara(int n, int m) const;
    double WestDataLogThetaPara(int n, int m) const;


    double EastLinThetaPara(int n, int m) const;
    double WestLinThetaPara(int n, int m) const;

    double EastLinPhiPara(int n, int m) const;
    double WestLinPhiPara(int n, int m) const;

    double BarrPosDataCor(int ntheta, int nphi) const;
    double WestPosDataCor(int ntheta, int nphi) const;
    double EastPosDataCor(int ntheta, int nphi) const;

    double BarrPosMCCor(int ntheta, int nphi) const;
    double WestPosMCCor(int ntheta, int nphi) const;
    double EastPosMCCor(int ntheta, int nphi) const;

    double BarrLogThetaPara(int n, int m) const;
    double BarrLogPhiPara(int n, int m) const;

    double BarrLogShMaxThetaPara(int n, int m) const;
    double BarrLogShMaxPhiPara(int n, int m) const;
 
    double BarrLoglinThetaPara(int n, int m) const;
    double BarrLoglinPhiPara(int n, int m) const;


    double BarrLinThetaPara(int n, int m) const;
    double BarrLinPhiPara(int n, int m) const;
 
 
    double BarrShLinThetaPara(int n, int m) const;
    double BarrShLinPhiPara(int n, int m) const;

    double BarrDataLogThetaPara(int n, int m) const;

    inline bool DigiCalib() const { return digiCalib; }
    inline void SetDigiCalib(bool digi) { digiCalib=digi; }

    inline void SetTimeMin(double min) { fTimeMin=min; }
    inline void SetTimeMax(double max) { fTimeMax=max; }

    inline void SetMethodMode(double en) { fMethodMode=en; }
    inline void SetPosCorr(double en) { fPosCorr=en; }
    inline void SetDataMode(double en) { fDataMode=en; }    
 
    inline void SetElecSaturation(int IO){ fElecSaturation=IO;}



    inline void SetPeak(double e, int n) { peak[n]=e; }

    inline void SetEastLogThetaPara(double p,int n,int m) { eastLogThetaPara[n][m]=p; }
    inline void SetWestLogThetaPara(double p,int n,int m) { westLogThetaPara[n][m]=p; }

    inline void SetEastLogPhiPara(double p,int n,int m) { eastLogPhiPara[n][m]=p; }
    inline void SetWestLogPhiPara(double p,int n,int m) { westLogPhiPara[n][m]=p; }

    inline void SetEastLogShMaxThetaPara(double p,int n,int m) { eastLogShMaxThetaPara[n][m]=p; }
    inline void SetWestLogShMaxThetaPara(double p,int n,int m) { westLogShMaxThetaPara[n][m]=p; }

    inline void SetEastLogShMaxPhiPara(double p,int n,int m) { eastLogShMaxPhiPara[n][m]=p; }
    inline void SetWestLogShMaxPhiPara(double p,int n,int m) { westLogShMaxPhiPara[n][m]=p; }


    inline void SetEastDataLogThetaPara(double p,int n,int m) { eastDataLogThetaPara[n][m]=p; }
    inline void SetWestDataLogThetaPara(double p,int n,int m) { westDataLogThetaPara[n][m]=p; }


    inline void SetEastLinThetaPara(double p,int n,int m) { eastLinThetaPara[n][m]=p; }
    inline void SetWestLinThetaPara(double p,int n,int m) { westLinThetaPara[n][m]=p; }

    inline void SetEastLinPhiPara(double p,int n,int m) { eastLinPhiPara[n][m]=p; }
    inline void SetWestLinPhiPara(double p,int n,int m) { westLinPhiPara[n][m]=p; }


    inline void SetBarrLogThetaPara(double p,int n,int m) {barrLogThetaPara[n][m]=p; }
    inline void SetBarrLogPhiPara(double p,int n,int m) { barrLogPhiPara[n][m]=p; }
 
    inline void SetBarrLoglinThetaPara(double p,int n,int m) {barrLoglinThetaPara[n][m]=p; }
    inline void SetBarrLoglinPhiPara(double p,int n,int m) { barrLoglinPhiPara[n][m]=p; }

    inline void SetBarrLinThetaPara(double p,int n,int m) { barrLinThetaPara[n][m]=p; }
    inline void SetBarrLinPhiPara(double p,int n,int m) { barrLinPhiPara[n][m]=p; }
    
    inline void SetBarrLogShMaxThetaPara(double p,int n,int m) { barrLogShMaxThetaPara[n][m]=p; }
    inline void SetBarrLogShMaxPhiPara(double p,int n,int m) { barrLogShMaxPhiPara[n][m]=p; }
 
    inline void SetBarrShLinThetaPara(double p,int n,int m) { barrShLinThetaPara[n][m]=p; }
    inline void SetBarrShLinPhiPara(double p,int n,int m) { barrShLinPhiPara[n][m]=p; }

    inline void SetBarrPosDataCor(double p, int nphi,int ntheta) {barrPosDataCorPara[ntheta][nphi]=p; }
    inline void SetWestPosDataCor(double p, int nphi,int ntheta) {westPosDataCorPara[ntheta][nphi]=p; }
    inline void SetEastPosDataCor(double p, int nphi,int ntheta) {eastPosDataCorPara[ntheta][nphi]=p; }

    inline void SetBarrPosMCCor(double p, int nphi,int ntheta) {barrPosMCCorPara[ntheta][nphi]=p; }
    inline void SetWestPosMCCor(double p, int nphi,int ntheta) {westPosMCCorPara[ntheta][nphi]=p; }
    inline void SetEastPosMCCor(double p, int nphi,int ntheta) {eastPosMCCorPara[ntheta][nphi]=p; }

    inline void SetBarrDataLogThetaPara(double p,int n,int m) {barrDataLogThetaPara[n][m]=p; }

    inline std::string PositionMode1() const { return positionMode1; }
    inline std::string PositionMode2() const { return positionMode2; }
    void SetPositionMode(std::vector<std::string>& mode);

    double ECorrMC(double eg, double theid) const;
    double ErrMC(double eg, double theid) const;
    double E25min(int n) const;
    double E25max(int n) const;

    double e25min[28];
    double e25max[28];
  private:
    //each parameter
    // ElectronicsNoiseLevel
    double fElectronicsNoiseLevel;
    // Energy threshold for seed search
    double fEThresholdSeed;
    // Energy threshold for cluster search
    double fEThresholdCluster;
    // LogPosOffset
    double fLogPosOffset;

    // Time window left
    double fTimeMin;
    // Time window right
    double fTimeMax;
    //Position correction method
    double fMethodMode;

   //MC reconstruction for electronics saturation
   // 0: using Bhabha calibration constants
   // 1: using Emaxdata from data
   int fElecSaturation;

    //Position correction or not
    double fPosCorr;
    double fDataMode;
    // Moliere radius
    double fMoliereRadius;
    // Lateral profile
    double fLateralProfile;

    // Energy correction
    double eCorr[4];
    // Energy error
    double sigE[3];
    // Theta error
    double sigTheta[2];
    // Phi error
    double sigPhi[2];

    // Hit number
    double hitNb[3];
    // Correction of electronics bias
    double elecBias[5];
    // Cluster splitting cut with second moment
    double smCut[4];

    // Digi calibration
    bool digiCalib;

    // Shower energy correction VS theta
    double peak[56];

    double barrLogThetaPara[66][5];
    double barrLogPhiPara[66][5];

    double eastLogThetaPara[18][5];
    double westLogThetaPara[18][5];

    double eastLogPhiPara[3][5];
    double westLogPhiPara[3][5];

    double barrLogShMaxThetaPara[132][5];
    double barrLogShMaxPhiPara[132][5];

    double eastLogShMaxThetaPara[18][5];
    double westLogShMaxThetaPara[18][5];

    double eastLogShMaxPhiPara[3][5];
    double westLogShMaxPhiPara[3][5];

    double eastDataLogThetaPara[6][5];
    double westDataLogThetaPara[6][5];


    double eastLinThetaPara[6][5];
    double westLinThetaPara[6][5];

    double eastLinPhiPara[1][5];
    double westLinPhiPara[1][5];

    double barrLoglinThetaPara[22][5];
    double barrLoglinPhiPara[1][5];
 
    double barrLinThetaPara[66][5];
    double barrLinPhiPara[3][5];
 
    double barrShLinThetaPara[66][5];
    double barrShLinPhiPara[3][5];

    double barrDataLogThetaPara[22][5];

    double barrPosDataCorPara[44][120];
    double westPosDataCorPara[6][100];
    double eastPosDataCorPara[6][100];

    double barrPosMCCorPara[44][120];
    double westPosMCCorPara[6][100];
    double eastPosMCCorPara[6][100];

    std::string positionMode1;
    std::string positionMode2;

    // Shower energy correction
    TGraph2DErrors *dt;
    // Energy error
    TGraph2DErrors *dtErr;
};

#endif // EMC_REC_PARAMETER_A_H

