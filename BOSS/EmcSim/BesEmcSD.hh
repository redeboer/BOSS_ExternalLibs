//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesEmcSD_h
#define BesEmcSD_h 1

#include "BesSensitiveDetector.hh"
#include "BesEmcHit.hh"
#include "globals.hh"
#include <vector>
using namespace std;

class BesEmcConstruction;
class BesEmcGeometry;
class BesSensitiveManager;
class G4HCofThisEvent;
class G4Step;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class BesEmcSD : public BesSensitiveDetector
{
  public:
  
      BesEmcSD(G4String, BesEmcConstruction*, BesEmcGeometry*);
     ~BesEmcSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*,G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);
      void clear();
      void DrawAll();
      void PrintAll();
      void BeginOfTruthEvent(const G4Event*);
      void EndOfTruthEvent(const G4Event*);
      void ComputeThetaPhi(G4int,G4int,G4int);
      G4int EndPhiNumberForGDML(G4int);
      G4int ComputeEndCopyNb(G4int);
      
  private:
  
      BesEmcHitsCollection*  CalCollection;      
      BesEmcTruthHitsCollection*  CalTruthList;
      BesEmcHitsCollection*  CalList;
      BesEmcConstruction*      Detector;
      BesEmcGeometry*          fBesEmcGeometry;
      G4int*                   HitID;
      G4int                    nHit;
      G4int                   m_trackIndex;
      G4int                   PartId;
      G4int                   CryNumberTheta;
      G4int                   CryNumberPhi;
};

#endif
