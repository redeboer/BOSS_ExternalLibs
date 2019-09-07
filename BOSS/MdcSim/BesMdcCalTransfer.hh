//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: Oct.26, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesMdcCalTransfer_h
#define BesMdcCalTransfer_h 1

#include "BesMdcHit.hh"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

class BesMdcCalTransfer
{
  public:
    BesMdcCalTransfer(void);
    ~BesMdcCalTransfer(void);

  public:
  void SetHitPointer(BesMdcHit* hit);

  void GetSigma(double& sigma1, double& sigma2,double& f);

  double D2T(double driftDNew);

  double GetT0(void);

  double GetTimeWalk(void);

  double GetEff(void);
  private:
  G4int layerId, cellId, posFlag; 
  G4double driftD, edep, theta, enterAngle, z; 

  IMdcCalibFunSvc* mdcCalFunSvc;
};
#endif

