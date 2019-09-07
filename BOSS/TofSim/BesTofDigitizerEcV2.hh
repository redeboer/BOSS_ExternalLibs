//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Oct, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerEcV2.hh

#ifndef BesTofDigitizerEcV2_h
#define BesTofDigitizerEcV2_h 1

#include "GaudiKernel/NTuple.h"

#include "BesTofDigitizerV.hh"
#include "BesTofDigi.hh"
#include "ScintSingle.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
using namespace std;

class BesTofHit;
class BesRunAction;
class G4Svc;

    
const G4int m_profBinNEcV2 = 8000;
const G4int m_snpeBinNEcV2 = 2000;

class BesTofDigitizerEcV2 : public BesTofDigitizerV
{
  public:
    BesTofDigitizerEcV2();
    ~BesTofDigitizerEcV2();
    
    virtual void Digitize(ScintSingle*, BesTofDigitsCollection*);
    void ReadData(); 
    void TofPmtInit();
    void TofPmtAccum(BesTofHit* );
    void DirectPh(G4int, G4ThreeVector, G4double &, G4int&);
    G4double Scintillation(G4int);
    //G4double GenPhoton(G4int);
    G4double TransitTime();
    void AccuSignal(G4double, G4int);
    void TofPmtRspns(G4int);
    
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
    G4int m_nPhot[m_profBinNEcV2][2];
 
    G4Svc* m_G4Svc;
    G4double m_beamTime;
    


};

#endif

