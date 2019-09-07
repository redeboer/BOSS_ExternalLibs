//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofSD.hh


#ifndef BesTofSD_h
#define BesTofSD_h 1

#include "BesSensitiveDetector.hh"
#include "BesTofHit.hh"
#include <vector>
using namespace std;

class BesSensitiveManager;
class G4Step;
class G4HCofThisEvent;
class G4Event;
class G4ElectronIonPair;


class BesTofSD : public BesSensitiveDetector
{
  public:
    BesTofSD(G4String name);
    virtual ~BesTofSD();
    
    virtual void Initialize(G4HCofThisEvent* HCE);
    virtual G4bool ProcessHits(G4Step* aStep,G4TouchableHistory*);
    virtual void EndOfEvent(G4HCofThisEvent* HCE);

    void BeginOfTruthEvent(const G4Event*);
    void EndOfTruthEvent(const G4Event*);

    G4int SampleNumberOfIonsAlongStep(const G4Step* , G4ElectronIonPair *);

  private:
    BesTofHitsCollection* m_besTofCollection;
    BesTofHitsCollection* m_besTofList;
    G4int m_trackIndex;
    vector<G4int> m_trackIndexes;
    vector<int> m_scinIndex;
    G4ElectronIonPair* elIonPair;
    G4double m_event;
    double m_fanoFactor;
    double m_nionOff;
};


#endif

