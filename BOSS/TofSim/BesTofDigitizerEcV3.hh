//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: liuy
//Created: Oct, 2008
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerEcV3.hh

#ifndef BesTofDigitizerEcV3_h
#define BesTofDigitizerEcV3_h 1

#include "GaudiKernel/NTuple.h"

#include "BesTofDigitizerV.hh"
#include "BesTofDigi.hh"
#include "ScintSingle.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"

using namespace std;

class BesTofHit;
class BesRunAction;
class G4Svc;

const G4int m_profBinNEcV3 = 30000;
const G4int m_snpeBinNEcV3 = 3000;

const int num1 = 400; // bin number in histogram
const int nR = 43;  // r bin number in geometry
const int nPhi = 6; // phi bin number
const int nZ = 6;   // z bin number

class BesTofDigitizerEcV3 : public BesTofDigitizerV
{
  public:
    BesTofDigitizerEcV3();
    ~BesTofDigitizerEcV3();
    
    virtual void Digitize(ScintSingle*, BesTofDigitsCollection*);
    void ReadData();
    void ReadEffTree();
    void TofPmtInit();
    void TofPmtAccum(BesTofHit* );
    void DirectPh(G4int, G4int, G4int, G4double&);
    G4double Scintillation(G4int);
    G4double TransitTime();
    void AccuSignal(G4double, G4int);
//    void TofPmtRspns(G4int);
    void TofPmtRspns(G4int, G4int);
    G4double BirksLaw(BesTofHit* hit);
    
  private:

    G4double m_ecR1;
    
    G4double m_tau1Ec;
    G4double m_tau2Ec;
    G4double m_tau3Ec;
    G4double m_tauRatioEc;
    G4double m_refIndexEc;
    G4double m_phNConstEc;
    G4double m_Cpe2pmtEc;
    G4double m_rAngleEc;
    G4double m_QEEc;
    G4double m_CEEc;
    G4double m_peCorFacEc;
    G4double m_attenEc;
    
    G4double m_ttsMeanEc;
    G4double m_ttsSigmaEc;
    G4double m_PMTgainEc;
    G4double m_CeEc;
    G4double m_riseTimeEc;
    G4double m_LLthreshEc;
    G4double m_HLthreshEc;
    G4double m_preGainEc;
    G4double m_noiseSigmaEc; 
    
    G4double m_t1st[2];
    G4double m_tLast[2];
    G4double m_timeBinSize;
    G4int m_totalPhot[2];
    G4int m_nPhot[m_profBinNEcV3][2];
 
    G4Svc* m_G4Svc;
    G4double m_beamTime;
    RealizationSvc* m_RealizationSvc;

    float propTime[50][10][10][num1],prob[50][10][10][num1],eff[50][10][10];

};

#endif

