//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------
//Description: Sensitive detector definition for axial layers of MDC 
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: Used in "BesMdc"
//---------------------------------------------------------------------------//

#ifndef BesMdcSD_h
#define BesMdcSD_h 1
class TFile;
class TH1F;
class G4Svc;

//#include "G4VSensitiveDetector.hh"
#include "BesSensitiveDetector.hh"
#include "BesMdcHit.hh"
#include "BesMdcGeoParameter.hh"
#include "BesMdcCalTransfer.hh"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "CalibDataSvc/CalibDataSvc.h"
#include "DedxCurSvc/DedxCurSvc.h"
#include "GaudiKernel/NTuple.h"
#include "TF1.h"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class BesSensitiveManager;
class G4Svc;

class BesMdcSD : public BesSensitiveDetector
{
public:
  BesMdcSD(G4String);
  ~BesMdcSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  void BeginOfTruthEvent(const G4Event*);
  void EndOfTruthEvent(const G4Event*);

  G4double Distance(G4int, G4int,G4ThreeVector,G4ThreeVector,G4ThreeVector&,G4double&);
 
  void dedxFuncInti(void);

private:
  G4int hitPointer[43][288],truthPointer[43][288];
  BesMdcHitsCollection* hitsCollection;
  BesMdcHitsCollection* truthCollection;
  BesMdcGeoParameter* mdcGeoPointer;
  BesMdcCalTransfer* mdcCalPointer;
  MdcGeomSvc* mdcGeomSvc;
  G4Svc* m_G4Svc;
  TF1 *dEdE_mylanfunc;
  
  ///dedx sim ---------------------------
  CalibDataSvc* m_calibDataSvc;
  IDedxCurSvc* m_pDedxCurSvc;
  TH1F* m_dedx_hists;
  G4int m_version;
  G4int m_numDedxHists;
  G4int m_numBg;
  G4int m_numTheta;
  std::vector<G4double> m_bgRange;
  G4int GetBetagammaIndex(G4double bg);
  G4int GetAngleIndex(G4double);
  G4int GetChargeIndex(G4int);
  G4double GetValDedxCurve(G4double bg, G4double charge);
  G4double dedxSample(G4double betagamma, G4double length, G4double theta);
  
  //dedx ntuple
  NTuple::Tuple* m_tupleMdc;
  NTuple::Item<double> m_betaGamma;
  NTuple::Item<double> m_fitval;
  NTuple::Item<double> m_random;
  NTuple::Item<double> m_dedx;
  NTuple::Item<double> m_de;
  //NTuple::Item<double> m_length;
  NTuple::Item<double> m_charge;
  NTuple::Item<double> m_costheta;
  ///------------------------------------
};

#endif

