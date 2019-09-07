//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerV.hh

#ifndef BesTofDigitizerV_h
#define BesTofDigitizerV_h 1

#include "BesTofDigi.hh"
#include "BesTofHit.hh"
#include "G4DigiManager.hh"
#include "ScintSingle.hh"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "TofSimSvc/ITofSimSvc.h"
#include "TofQElecSvc/ITofQElecSvc.h"

#include "GaudiKernel/NTuple.h"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"

class G4Svc;

class BesTofDigitizerV
{
  public:
    BesTofDigitizerV();
    ~BesTofDigitizerV();
    void Initialize(); 
    virtual void Digitize(ScintSingle*, BesTofDigitsCollection*) {;}
    
  protected:
    BesTofDigitsCollection* m_besTofDigitsCollection;
    BesTofHitsCollection* m_THC;
    
    ITofCaliSvc* m_tofCaliSvc;
    ITofSimSvc* m_tofSimSvc;
    ITofQElecSvc* m_tofQElecSvc;

    G4double m_ADC[2];
    G4double m_TDC[2];
    G4int m_trackIndex;
    G4double m_globalTime;

    static bool  m_booked;

    static NTuple::Tuple* m_tupleTof1;
    static NTuple::Item<double> m_partId;
    static NTuple::Item<double> m_scinNb;
    static NTuple::Item<double> m_edep;
    static NTuple::Item<double> m_nHits; 
    static NTuple::Item<double> m_time1st0;
    static NTuple::Item<double> m_time1st1;
    static NTuple::Item<double> m_timelast0;
    static NTuple::Item<double> m_timelast1;
    static NTuple::Item<double> m_totalPhot0;
    static NTuple::Item<double> m_totalPhot1;
    static NTuple::Item<double> m_NphAllSteps;
    static NTuple::Item<double> m_max0;
    static NTuple::Item<double> m_max1;
    static NTuple::Item<double> m_tdc0;
    static NTuple::Item<double> m_adc0;
    static NTuple::Item<double> m_tdc1;
    static NTuple::Item<double> m_adc1;
    
    static NTuple::Tuple* m_tupleTof2;
    static NTuple::Item<double> m_eTotal;
    static NTuple::Item<double> m_nDigi;
    static NTuple::Item<double> m_partIdMPV;
    static NTuple::Item<double> m_scinNbMPV;
    static NTuple::Item<double> m_edepMPV;
    static NTuple::Item<double> m_nDigiOut;

    static NTuple::Tuple* m_tupleTof3;
    static NTuple::Item<double> m_forb;
    static NTuple::Item<double> m_timeFlight;
    static NTuple::Item<double> m_ddT;
    static NTuple::Item<double> m_scinSwim;
    static NTuple::Item<double> m_scinTime;
    static NTuple::Item<double> m_transitTime;
    static NTuple::Item<double> m_endTime;

    static NTuple::Item<double> m_edepHit;


  //static NTuple::Tuple* m_tupleTof4;//Write  out the basic information for the Mrpc Digitizer
  //static NTuple::Item<double> m_partIdmrpc;
  //static NTuple::Item<double> m_stripidentifier;
  //static NTuple::Item<double> m_trackindex;
  //static NTuple::Item<double> m_signal_pc;
  //static NTuple::Item<double> m_time_threshold;
  //static NTuple::Item<double> m_time_1sthit;
  //static NTuple::Item<double> m_time_1;
  //static NTuple::Item<double> m_time_2;
  //static NTuple::Item<double> m_firedstrip;
  //static NTuple::Item<double> m_numberions;

  private:
    G4Svc* m_G4Svc;


};
#endif

