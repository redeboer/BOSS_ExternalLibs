//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Hemiao
//Created: Sep, 2004
//Comment:
//---------------------------------------------------------------------------//
// $Id:BesEmcDigitizer.hh

#ifndef BesEmcDigitizer_h
#define BesEmcDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "BesEmcDigi.hh"
#include "BesEmcHit.hh"
#include "globals.hh"
#include <vector>
#include "GaudiKernel/NTuple.h"
using namespace std;

class BesEmcHit;
class G4Svc;
class IEmcCalibConstSvc;

class CrystalSingle
{
  friend class BesEmcDigitizer;

  public:
  
    CrystalSingle()
    {
      hitIndexes = new vector<G4int>;
    }
    CrystalSingle(const CrystalSingle& right)
    {
      partId = right.partId;
      nTheta = right.nTheta;
      nPhi = right.nPhi;
      edep = right.edep;
      hitIndexes =right.hitIndexes;
    }
    const CrystalSingle& operator=(const CrystalSingle& right)
    {
      partId = right.partId;
      nTheta = right.nTheta;
      nPhi = right.nPhi;
      edep = right.edep;
      hitIndexes =right.hitIndexes;
      return *this;
    }
    ~CrystalSingle()
    {
      delete hitIndexes;
    }

    G4int GetPartId() {return partId;}
    G4int GetNTheta() {return nTheta;}
    G4int GetNPhi() {return nPhi;}
    G4double GetEdep() {return edep;}
    vector<G4int>* GetHitIndexes() {return hitIndexes;}

    void SetPartId(G4int id) {partId = id;}
    void SetNTheta(G4int theta) {nTheta = theta;}
    void SetNPhi(G4int phi) {nPhi = phi;}
    void SetEdep(G4double e) {edep = e;}
    void AddEdep(G4double e) {edep += e;}        

  private:

    G4int partId;
    G4int nTheta;
    G4int nPhi;
    G4double edep;
    vector<G4int>* hitIndexes;
};

class BesEmcDigitizer : public G4VDigitizerModule
{
public:
  BesEmcDigitizer(G4String modName);
  ~BesEmcDigitizer();

public:
  virtual void Digitize();
  virtual void GroupHits(BesEmcHitsCollection*);
  // Add noise to 5x5 matrix around the seed
  virtual void AddNoise5x5(G4double coherentNoise);
  // Add noise to all crystals
  virtual void AddNoiseAll(G4double coherentNoise);
  
private:          //memory size needed optimizing
  void Initialize();
//  void Smear();

private:
  BesEmcDigitsCollection* m_besEmcDigitsCollection;
  vector<CrystalSingle*>* m_crystalGroup;
  G4double m_energy;

private:
  G4Svc* m_G4Svc;
  IEmcCalibConstSvc *m_emcCalibConstSvc;
  NTuple::Tuple* m_tupleEmc1;   //digi level
  NTuple::Item<long> m_partId;
  NTuple::Item<long> m_nTheta;
  NTuple::Item<long> m_nPhi;
  NTuple::Item<double> m_eDep;
  NTuple::Item<long> m_nHits;
  NTuple::Item<double> m_adc;
  NTuple::Item<long> m_tdc;
  
  NTuple::Tuple* m_tupleEmc2;   //event level
  NTuple::Item<double> m_eTot;
  NTuple::Item<long> m_nDigi;

};
#endif
