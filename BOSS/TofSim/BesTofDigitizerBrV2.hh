//---------------------------------------------------------------------------///      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Oct, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerBrV2.hh

#ifndef BesTofDigitizerBrV2_h
#define BesTofDigitizerBrV2_h 1

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

const G4int m_profBinN = 30000;
const G4int m_snpeBinN = 3000;

class BesTofDigitizerBrV2 : public BesTofDigitizerV
{
  public:
    BesTofDigitizerBrV2();
    ~BesTofDigitizerBrV2();
    
    virtual void Digitize(ScintSingle*, BesTofDigitsCollection*);
    void ReadData(); 
    void TofPmtInit();
    void TofPmtAccum(BesTofHit*, G4int);
    void DirectPh(G4int, G4ThreeVector, G4double &, G4int&);
    G4double Scintillation(G4int);
    //G4double GenPhoton(G4int);
    G4double TransitTime();
    void AccuSignal(G4double, G4int);
    void TofPmtRspns(G4int);
    G4double Reflectivity(G4double n1,G4double n2,G4double theta);
    G4double Reflectivity(G4double n1,G4double n2,G4double n3,G4double theta);
    G4double BirksLaw(BesTofHit* hit);
    
  private:

    G4double m_scinLength;
    
    G4double m_tau1;
    G4double m_tau2;
    G4double m_tau3;
    G4double m_tauRatio;
    G4double m_refIndex;
    G4double m_phNConst;
    G4double m_Cpe2pmt;
    G4double m_rAngle;
    G4double m_QE;
    G4double m_CE;
    G4double m_peCorFac;
    
    G4double m_ttsMean;
    G4double m_ttsSigma;
    G4double m_Ce;
    //G4double m_BrWPMTgain[176];
    //G4double m_BrWRiseTime[176];
    //G4double m_BrEPMTgain[176];
    //G4double m_BrERiseTime[176];
    G4double m_LLthresh;
    G4double m_HLthresh;
    G4double m_preGain;
    G4double m_noiseSigma; 
    
    G4double m_t1st[2];
    G4double m_tLast[2];
    G4double m_timeBinSize;
    G4int m_totalPhot[2];
    G4int m_nPhot[m_profBinN][2];
 
    //BesRunAction* m_runAction;
    G4Svc* m_G4Svc;
    G4double m_beamTime;
    RealizationSvc* m_RealizationSvc;

/*
    NTuple::Tuple* m_tupleTof1;
    NTuple::Item<double> m_partId;
    NTuple::Item<double> m_scinNb;
    NTuple::Item<double> m_edep;
    NTuple::Item<double> m_nHits; 
    NTuple::Item<double> m_time1st0;
    NTuple::Item<double> m_time1st1;
    NTuple::Item<double> m_timelast0;
    NTuple::Item<double> m_timelast1;
    NTuple::Item<double> m_totalPhot0;
    NTuple::Item<double> m_totalPhot1;
    NTuple::Item<double> m_NphAllSteps;
    NTuple::Item<double> m_max0;
    NTuple::Item<double> m_max1;
    NTuple::Item<double> m_tdc0;
    NTuple::Item<double> m_adc0;
    NTuple::Item<double> m_tdc1;
    NTuple::Item<double> m_adc1;
    NTuple::Item<double> nReachPMTf;
    NTuple::Item<double> nEnterPMTf;
    NTuple::Item<double> nEnterf;
    NTuple::Item<double> nRefPMTf;
    NTuple::Item<double> nReachAirBoxf;
    NTuple::Item<double> nRefAirBoxf;

    
    NTuple::Tuple* m_tupleTof2;
    NTuple::Item<double> m_eTotal;
    NTuple::Item<double> m_nDigi;
    NTuple::Item<double> m_partIdMPV;
    NTuple::Item<double> m_scinNbMPV;
    NTuple::Item<double> m_edepMPV;
    NTuple::Item<double> m_nDigiOut;

    NTuple::Tuple* m_tupleTof3;
    NTuple::Item<double> m_forb;
    NTuple::Item<double> m_timeFlight;
    NTuple::Item<double> m_ddT;
    NTuple::Item<double> m_scinSwim;
    NTuple::Item<double> m_scinTime;
    NTuple::Item<double> m_transitTime;
    NTuple::Item<double> m_endTime;
*/

};

#endif

