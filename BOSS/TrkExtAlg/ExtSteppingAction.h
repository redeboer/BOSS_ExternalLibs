//
//File: ExtSteppingAction.hh
//data: 2005.3.16
//Author: L.L.Wang
//
//Description: 
//


#ifndef EXTSTEPPINGACTION
#define EXTSTEPPINGACTION 1

#include "G4UserSteppingAction.hh"
#include "G4Material.hh"

#include "TrkExtAlg/Ext_xp_err.h"
#include "CLHEP/Matrix/SymMatrix.h" 
#include "ExtEvent/RecExtTrack.h"
#include "MucRawEvent/MucDigi.h"
#include "McTruth/MucMcHit.h"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
using namespace CLHEP;

class ExtSteppingAction: public G4UserSteppingAction
{
 public:
   // Constructor and destructors
   ExtSteppingAction();
   ~ExtSteppingAction();

   void Reset();
   void MucReset();
   void UserSteppingAction(const G4Step* currentStep);
   
   void SetInitialPath(double aPath) {initialPath = aPath;};
   void SetInitialTof(double aTof) {initialTof = aTof;};
   void SetBetaInMDC(double aBeta) {myBetaInMDC = aBeta;};
   void SetXpErrPointer(Ext_xp_err* xpErr) {extXpErr = xpErr;};

   void SetMsgFlag(bool aMsgFalg) {msgFlag = aMsgFalg;};
   void SetMucKalFlag(bool aMucKalFlag){myUseMucKalFlag=aMucKalFlag;};
   void SetMucWindow(int aMucWindow){myMucWindow=aMucWindow;};
   void SetExtTrackPointer(RecExtTrack *aExtTrack) {myExtTrack = aExtTrack;}; 
//   inline void SetMsFlag(bool value){msFlag=value};
   void CalculateEmcEndThetaPhi(int npart, int sector, int nb, int &ntheta, int &nphi);
   int CalculateEmcEndPhiNb(int num);
   int CalculateEmcEndCopyNb(int num);
   
   void Set_which_tof_version(int version){m_which_tof_version=version;}
   int Get_which_tof_version(void){return m_which_tof_version;}

   void InfmodMuc(Hep3Vector &pos,Hep3Vector &mom,HepSymMatrix &err);
   Hep3Vector GetGapID(G4String vol);
   bool TrackStop(){return m_trackstop;}
   void SetMucDigiColPointer(MucDigiCol* rawdigicol){ m_mucdigicol = rawdigicol;}
private:
   //Calculate chicc
   void CalculateChicc(G4Material* currentMaterial);
   
   double chicc;//a const about the materimal
   double initialPath;//particle path in MDC
   double initialTof;//particle TOF in MDC
   double myBetaInMDC;//particle velocity/C in MDC
   
   double myPathIntoCrystal;
   double myPathOutCrystal;
   double myPathInCrystal;
   
   double myPathIntoTof1;
   double myPathOutTof1;
   vector<double> myPathInTof1;

   double myPathIntoTof2;
   double myPathOutTof2;
   vector<double> myPathInTof2;
   int myMucWindow;
//   bool msFlag;  //Caculation of multiple scattering error  switch flag
   
   Ext_xp_err* extXpErr;

//units mm,MeV --> cm,GeV
   HepSymMatrix myOutputSM;
   HepSymMatrix & myOutputSymMatrix(const HepSymMatrix &);
   
    MucDigiCol* m_mucdigicol;
    bool myUseMucKalFlag;
    Hep3Vector RememberID;
    bool m_trackstop;
    int myMucnfit_;
    double myMucchisq_;
    double myMucdepth_;
    int myMucbrLastLay_;
    int myMucecLastLay_;
    int myMucnhits_;
    HepSymMatrix m_err_mod;
    Hep3Vector m_pos_mod;
    Hep3Vector m_mom_mod;

    Hep3Vector RemPositon;
    Hep3Vector RemMomentum;
    HepSymMatrix RemXpErr;
    int RemStep; 
    double RemDist;
    double RemDepth;
    Hep3Vector RemID;
    G4String RemVol;
   
//Ext data
   RecExtTrack *myExtTrack;

   bool msgFlag;//Message on/off
   bool myTofFlag;//when enter "physicalTof",this flag will be ture.
   bool myTof1Flag;//If we get Tof1 data,the flag will be true.
   bool myTof2Flag;
   bool myInTof1;
   bool myOutTof1;
   bool myInTof2;
   bool myOutTof2;
   bool myPhyEmcFlag;//when enter "BSC",this flag will be ture.
   bool myEmcFlag;
   bool myEmcPathFlag;
   bool myMucFlag;

   double myTof1R;
   double myTof1Z;
   double myTof2R;

   double myEmcR1;
   double myEmcR2;
   double myEmcZ;

   double myMucR;
   double myMucZ;

   int m_which_tof_version;   
}; 
  
#endif  
