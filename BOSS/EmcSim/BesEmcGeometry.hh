//---------------------------------------------------------------------------//
//      BESIII Object_Oreiented Simulation and Reconstruction Tool           //
//---------------------------------------------------------------------------//
//Descpirtion: Geometry of EMC detector 
//Author: Fu Chengdong
//Created: Oct 23, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesEmcGeometry_h
#define BesEmcGeometry_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class BesEmcConstruction;
class G4VPhysicalVolume;
class EmcGdmlWriter;

class BesEmcGeometry
{
public:
  BesEmcGeometry();
  ~BesEmcGeometry();
  friend class BesEmcConstruction;
  friend class EmcGdmlWriter;
public:
  void ReadEMCParameters();// Read EMC parameters from database
  void ComputeEMCParameters();// Compute derived parameters of the calorimeter
  void ModifyForCasing();// Compute the sizes of the naked crystals 
                         // and the casing
  void PrintEMCParameters();// Print EMC parameters
public:
  
  inline void SetVerboseLevel(G4int val){ verboseLevel=val;}
  
  inline void SetBSCRmin(G4double val){ BSCRmin=val;};
  inline void SetBSCDz(G4double val){ BSCDz=val;};
  inline void SetBSCNbPhi(G4int val){BSCNbPhi=val;};
  inline void SetBSCNbTheta(G4int val){BSCNbTheta=val;};
  inline void SetBSCCrystalLength(G4double val){BSCCryLength=val;};
  inline void SetBSCCrystalLength1(G4double val){BSCCryLength1=val;};
  inline void SetBSCYFront0(G4double val){BSCYFront0=val;};
  inline void SetBSCYFront(G4double val){BSCYFront=val;};
  inline void SetBSCPosition0(G4double val){BSCPosition0=val;};
  inline void SetBSCPosition1(G4double val){BSCPosition1=val;};
  inline void SetBSCAngleRotat(G4double val){BSCAngleRotat=val;};

  void SetCasingThickness(G4ThreeVector);

public:
  G4int GetVerboseLevel() {return verboseLevel;};

  G4double GetBSCRmin()             {return BSCRmin;};
  G4double GetBSCDz()              {return BSCDz;};
  G4int GetBSCNbPhi()              {return BSCNbPhi;};
  G4int GetBSCNbTheta()            {return BSCNbTheta;};
  G4double GetBSCCryLength()       {return BSCCryLength;};
  G4double GetBSCCryLength1()       {return BSCCryLength1;};
  G4double GetBSCYFront()          {return BSCYFront;};
  G4double GetBSCYFront0()          {return BSCYFront0;};
  G4double GetBSCPositon0()          {return BSCPosition0;};
  G4double GetBSCPositon1()          {return BSCPosition1;};
  G4double GetBSCAngleRotat()          {return BSCAngleRotat;};
  
  G4ThreeVector GetCasingThickness()   {return G4ThreeVector(fTyvekThickness,fAlThickness,fMylarThickness);};

  G4double GetXPosition(G4int NbCrystal);
  G4double GetYPosition(G4int NbCrystal);
  G4double GetZPosition(G4int NbCrystal);
  G4double GetThetaPosition(G4int NbCrystal);
  G4double GetZHalfLength(G4int NbCrystal);
  G4double GetThetaAxis(G4int NbCrystal);
  G4double GetPhiAxis(G4int NbCrystal);
  G4double GetYHalfLength1(G4int NbCrystal);
  G4double GetXHalfLength1(G4int NbCrystal);
  G4double GetXHalfLength2(G4int NbCrystal);
  G4double GetTanAlpha1(G4int NbCrystal);
  G4double GetYHalfLength2(G4int NbCrystal);
  G4double GetXHalfLength3(G4int NbCrystal);
  G4double GetXHalfLength4(G4int NbCrystal);
  G4double GetTanAlpha2(G4int NbCrystal);
  G4VPhysicalVolume* GetPhysiBSCCrystal(G4int NbCrystal);

  G4double GetRearBoxLength(G4int NbCrystal);
  G4double GetRearBoxDz(G4int NbCrystal);
  G4double GetRearBoxPosX(G4int NbCrystal);
  G4double GetRearBoxPosY(G4int NbCrystal);
  G4double GetRearBoxPosZ(G4int NbCrystal);
  G4double GetRearBoxPosTheta(G4int NbCrystal);

private:
  G4int              verboseLevel;

  G4double           BSCCryLength;
  G4double           BSCCryLength1;
  G4double           BSCYFront0;
  G4double           BSCYFront;
  G4double           BSCYFront1;
  G4double           BSCPosition0;
  G4double           BSCPosition1;
  
  G4int              BSCNbPhi;
  G4int              BSCNbTheta;

  G4double           BSCPhiRmin;
  G4double           BSCPhiRmax;
  G4double           BSCPhiDz;
  G4double           BSCPhiSphi;
  G4double           BSCPhiDphi;
  G4double           BSCAngleRotat;
  G4double           BSCRmin;
  G4double           BSCRmax;
  G4double           BSCDz;

  G4double           BSCRmin1;
  G4double           BSCRmax1;
  G4double           BSCRmin2;
  G4double           BSCRmax2;
  G4double           BSCDz1;
  
  G4double           zHalfLength[150];
  G4double           thetaAxis[150];
  G4double           phiAxis[150];
  G4double           yHalfLength1[150];
  G4double           xHalfLength1[150];
  G4double           xHalfLength2[150];
  G4double           tanAlpha1[150];
  G4double           yHalfLength2[150];
  G4double           xHalfLength3[150];
  G4double           xHalfLength4[150];
  G4double           tanAlpha2[150];
  G4double           thetaPosition[150];
  G4double           xPosition[150];
  G4double           yPosition[150];
  G4double           zPosition[150];

  G4VPhysicalVolume* physiBSCCrystal[50];

  G4double           fTyvekThickness;
  G4double           fAlThickness;
  G4double           fMylarThickness;

  /////////////////////////////////////////////////////////////
  //other substance on the rear face of crystal
  G4double            rearBoxLength;
  G4double            rearBoxDz;
  G4double            rearBoxPosX[150];
  G4double            rearBoxPosY[150];
  G4double            rearBoxPosZ[150];

  //tyvek casing
  G4double            rearCasingThickness;

  //organic glass
  G4double            orgGlassLengthX;
  G4double            orgGlassLengthY;
  G4double            orgGlassLengthZ;

  //photodiode
  G4double            PDLengthX;
  G4double            PDLengthY;
  G4double            PDLengthZ;

  //Al plate
  G4double            AlPlateDz;

  //preamplifier box
  G4double            PABoxDz;
  G4double            PABoxThickness;

  //stainless steel plate for hanging the crystal
  G4double            HangingPlateDz;

  G4double            OCGirderRmin1[22];
  G4double            OCGirderRmin2[22];
  G4double            OCGirderDz[22];
  G4double            OCGirderAngle; 
  G4double            OCGirderPosZ[22];

  //cable
  G4double            cableLength[22];
  G4double            cableDr;
  G4double            cablePosX[22];
  G4double            cablePosY[22];
  G4double            cablePosZ[22];

  //water pipe
  G4double            waterPipeDr;
  G4double            waterPipeThickness;

  /////////////////////////////////////////////////////////////
  //barrel support frame
  G4double            SPBarThickness;
  G4double            SPBarThickness1;
  G4double            SPBarwidth;
  G4double            SPBarDphi;
  G4double            RFBarThickness;
  G4int               RFBarHoleNb;
  G4double            RFBarHoleDphi;
  G4double            RFBarHoleDz;

  //end ring
  G4double            EndRingDz;
  G4double            EndRingDr;
  G4double            EndRingRmin;

  //taper ring
  G4double            TaperRingThickness1;
  G4double            TaperRingThickness2;
  G4double            TaperRingThickness3;
  G4double            TaperRingTheta;
  G4double            TaperRingRmin1;
  G4double            TaperRingRmin2;
  G4double            TaperRingDr;
  G4double            TaperRingDz;
  G4double            TaperRingInnerLength;
  G4double            TaperRingOuterLength;
  G4double            TaperRingOuterLength1;

};
#endif


