//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: He Miao
//Created: Nov,12 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: BesEmcParameter.hh

#ifndef BesEmcParameter_h
#define BesEmcParameter_h 1

#include "globals.hh"

class BesEmcParameter
{
  public:

    BesEmcParameter();
    ~BesEmcParameter();

    //static member functions
    static BesEmcParameter& GetInstance();
    static bool Exist();
    static void Kill();

  private:
    //static data members
    static BesEmcParameter* fpInstance;
       
  public:

    void ReadData();

    G4long GetArraySize(){ return array_size; }
    G4double GetTau(){ return m_tau; }
    G4double GetHighRange(){ return m_highRange; }
    G4double GetMidRange(){ return m_midRange; }
    G4double GetLowRange(){ return m_lowRange; }
    G4double GetSampleTime(){ return m_sampleTime; }
    G4double GetPeakTime() { return m_peakTime; }
    G4double GetTimeOffset() { return m_timeOffset; }
    G4int GetADCbit(){ return m_bitNb; }
    G4double GetPhotonsPerMeV(){ return m_photonsPerMeV; }
    G4double GetNonuniformity(){ return m_nonuniformity; }

    G4double GetWorldRmin1(){ return WorldRmin1; }
    G4double GetWorldRmax1(){ return WorldRmax1; }
    G4double GetWorldRmin2(){ return WorldRmin2; }
    G4double GetWorldRmax2(){ return WorldRmax2; }
    G4double GetWorldDz(){ return WorldDz; }
    G4double GetWorldZPosition(){ return WorldZPosition; }
    G4double GetCrystalLength(){ return CrystalLength; }
    G4double GetCrystalLength1(){ return CrystalLength1; }

    G4int GetCryInOneLayer(G4int nb){ return cryNumInOneLayer[nb]; }
    G4int GetPentaInOneSector(G4int nb){ return pentaInOneSector[nb]; }

    G4double GetTyvekThickness(){ return fTyvekThickness; }
    G4double GetAlThickness(){ return fAlThickness; }
    G4double GetMylarThickness(){ return fMylarThickness; }
    
    G4double GetBSCRmin(){ return BSCRmin; }
    G4double GetBSCDz(){ return BSCDz; }
    G4double GetBSCRmin1(){ return BSCRmin1; }
    G4double GetBSCRmax1(){ return BSCRmax1; }
    G4double GetBSCRmin2(){ return BSCRmin2; }
    G4double GetBSCRmax2(){ return BSCRmax2; }
    G4double GetBSCDz1(){ return BSCDz1; }
    
    G4double GetBSCAngleRotat(){ return BSCAngleRotat; }
    G4int GetBSCNbPhi(){ return BSCNbPhi; }
    G4int GetBSCNbTheta(){ return BSCNbTheta; }
    
    G4double GetBSCYFront0(){ return BSCYFront0; }
    G4double GetBSCYFront(){ return BSCYFront; }
    G4double GetBSCYFront1(){ return BSCYFront1; }
    G4double GetBSCPosition0(){ return BSCPosition0; }
    G4double GetBSCPosition1(){ return BSCPosition1; }

    G4double            GetTaperRingDz(){ return TaperRingDz; }
    G4double            GetTaperRingThickness1(){ return TaperRingThickness1; }
    G4double            GetTaperRingThickness2(){ return TaperRingThickness2; }
    G4double            GetTaperRingThickness3(){ return TaperRingThickness3; }
    G4double            GetTaperRingTheta(){ return TaperRingTheta; }
    G4double            GetTaperRingInnerLength(){ return TaperRingInnerLength; }
    G4double            GetTaperRingOuterLength(){ return TaperRingOuterLength; }

    G4double            GetRearBoxLength(){ return rearBoxLength; }
    G4double            GetRearBoxDz(){ return rearBoxDz; }
    G4double            GetHangingPlateDz(){ return HangingPlateDz; }
    G4double            GetOCGirderAngle(){ return OCGirderAngle; }
    
    G4double            GetRearCasingThickness(){ return rearCasingThickness; }

    G4double            GetOrgGlassLengthX(){ return orgGlassLengthX; }
    G4double            GetOrgGlassLengthY(){ return orgGlassLengthY; }
    G4double            GetOrgGlassLengthZ(){ return orgGlassLengthZ; }

    G4double            GetPDLengthX(){ return PDLengthX; }
    G4double            GetPDLengthY(){ return PDLengthY; }
    G4double            GetPDLengthZ(){ return PDLengthZ; }

    G4double            GetAlPlateDz(){ return AlPlateDz; }
    G4double            GetPABoxDz(){ return PABoxDz; }
    G4double            GetPABoxThickness(){ return PABoxThickness; }

    G4double            GetCableDr(){ return cableDr; }
    G4double            GetWaterPipeDr(){ return waterPipeDr; }
    G4double            GetWaterPipeThickness(){ return waterPipeThickness; }

    G4double            GetSPBarThickness(){ return SPBarThickness; }
    G4double            GetSPBarThickness1(){ return SPBarThickness1; }
    G4double            GetSPBarwidth(){ return SPBarwidth; }

    G4double            GetEndRingDz(){ return EndRingDz; }
    G4double            GetEndRingDr(){ return EndRingDr; }
    G4double            GetEndRingRmin(){ return EndRingRmin; }

    G4double            GetLightOutput(G4int i){ return lightOutput[i]; }
    G4double            GetLightOutput(G4int, G4int, G4int);

  private:

    //digitise parameter
    G4long array_size;
    G4double m_tau;
    G4double m_highRange;
    G4double m_midRange;
    G4double m_lowRange;
    G4double m_sampleTime;
    G4double m_peakTime;
    G4double m_timeOffset;
    G4int m_bitNb;
    G4double m_photonsPerMeV;
    G4double m_nonuniformity;

    //endcap geometry parameter
    G4double WorldRmin1;
    G4double WorldRmax1;
    G4double WorldRmin2;
    G4double WorldRmax2;
    G4double WorldDz;
    G4double WorldZPosition;
    G4double CrystalLength;
    G4double CrystalLength1;

    G4int cryNumInOneLayer[6];
    G4int pentaInOneSector[5];

    //crystal casing parameter
    G4double fTyvekThickness;
    G4double fAlThickness;
    G4double fMylarThickness;
    
    //barrel geometry parameter
    G4double  BSCRmin;
    G4double  BSCDz;
    G4double  BSCRmin1;
    G4double  BSCRmax1;
    G4double  BSCRmin2;
    G4double  BSCRmax2;
    G4double  BSCDz1;

    G4double  BSCAngleRotat;
    G4int  BSCNbPhi;
    G4int  BSCNbTheta;

    G4double  BSCYFront0;
    G4double  BSCYFront;
    G4double  BSCYFront1;
    G4double  BSCPosition0;
    G4double  BSCPosition1;

    //barrel support frame parameter
    G4double            TaperRingDz;
    G4double            TaperRingThickness1;
    G4double            TaperRingThickness2;
    G4double            TaperRingThickness3;
    G4double            TaperRingTheta;
    G4double            TaperRingInnerLength;
    G4double            TaperRingOuterLength;

    G4double            rearBoxLength;
    G4double            rearBoxDz;
    G4double            HangingPlateDz;
    G4double            OCGirderAngle;
    
    G4double            rearCasingThickness;

    G4double            orgGlassLengthX;
    G4double            orgGlassLengthY;
    G4double            orgGlassLengthZ;

    G4double            PDLengthX;
    G4double            PDLengthY;
    G4double            PDLengthZ;

    G4double            AlPlateDz;
    G4double            PABoxDz;
    G4double            PABoxThickness;

    G4double            cableDr;
    G4double            waterPipeDr;
    G4double            waterPipeThickness;

    G4double            SPBarThickness;
    G4double            SPBarThickness1;
    G4double            SPBarwidth;

    G4double            EndRingDz;
    G4double            EndRingDr;
    G4double            EndRingRmin;

    //crystal light yield
    G4double            lightOutput[6240];
};

#endif
