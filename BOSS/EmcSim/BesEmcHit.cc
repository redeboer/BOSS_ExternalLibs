//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesEmcHit.hh"

#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include <iomanip>

G4Allocator<BesEmcHit> BesEmcHitAllocator;
G4Allocator<BesEmcTruthHit> BesEmcTruthHitAllocator;

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcHit::BesEmcHit()
{
  TotalEdepCrystal = 0.; TotalTrackLengthCrystal = 0.;
  EdepCrystal = 0.; TrackLengthCrystal = 0.;
  EdepCasing = 0.;
  PositionCrystal= G4ThreeVector(0,0,0);
  TimeCrystal=0.;
  PartId=0;
  NumTheta=0;
  NumPhi=0;
  trackIndex=0;
  g4Index=0;
  momentum = G4ThreeVector(0,0,0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcHit::~BesEmcHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcHit::BesEmcHit(const BesEmcHit& right)
  : G4VHit()
{
  EdepCrystal = right.EdepCrystal;
  TrackLengthCrystal = right.TrackLengthCrystal;
  EdepCasing = right.EdepCasing;
  PositionCrystal= right.PositionCrystal;
  TimeCrystal = right.TimeCrystal;
  PartId = right.PartId;
  NumTheta = right.NumTheta;
  NumPhi = right.NumPhi;
  trackIndex = right.trackIndex;
  g4Index = right.g4Index;
  momentum = right.momentum;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const BesEmcHit& BesEmcHit::operator=(const BesEmcHit& right)
{
  EdepCrystal = right.EdepCrystal;
  TrackLengthCrystal = right.TrackLengthCrystal;
  EdepCasing = right.EdepCasing;
  PositionCrystal= right.PositionCrystal;
  TimeCrystal = right.TimeCrystal;
  PartId = right.PartId;
  NumTheta = right.NumTheta;
  NumPhi = right.NumPhi;
  trackIndex = right.trackIndex;
  g4Index = right.g4Index;
  momentum = right.momentum;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int BesEmcHit::operator==(const BesEmcHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcHit::Draw()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcHit::Print(G4int verboseLevel)
{
  if(verboseLevel>0)
    G4cout << "Hit in crystal:" <<NumTheta<<","<<NumPhi<<G4endl
	   <<"Energy deposited:"<<G4BestUnit(EdepCrystal,"Energy") <<G4endl;
  if(verboseLevel>1)
    G4cout <<"Hit time        :"<<G4BestUnit(TimeCrystal,"Time")<<G4endl
	   <<"    position    :"<<G4BestUnit(PositionCrystal,"Length")<<G4endl;
  if(verboseLevel>2)
    G4cout <<"Track length    :"<<G4BestUnit(TrackLengthCrystal,"Length")<<G4endl;

  
}

void BesEmcHit::Print()
{
  G4cout<<"time: "<<TimeCrystal<<" edep: "<<EdepCrystal<<G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcTruthHit::BesEmcTruthHit()
{
  m_trackIndex = -1;
  m_g4TrackId = -1;
  m_hitEmc = -1;
  m_PDGCode = 0;
  m_PDGCharge = 0;
  m_particleName = G4String();
  m_edep = 0;
  m_time = 0;
  m_momentum = G4ThreeVector(0,0,0);
  m_position = G4ThreeVector(0,0,0);
  m_hitMap.clear();
}

BesEmcTruthHit::~BesEmcTruthHit()
{}

BesEmcTruthHit::BesEmcTruthHit(const BesEmcTruthHit& right)
  : G4VHit()
{
  *this = right;
}

const BesEmcTruthHit& BesEmcTruthHit::operator=(const BesEmcTruthHit& right)
{
  m_hitMap.clear();
  std::map<Identifier,G4double>::const_iterator iHitMap;
  if(this!=&right) {
    for(iHitMap=right.Begin();
        iHitMap!=right.End();
        iHitMap++) {
      Insert(iHitMap->first,iHitMap->second);
    }
    
    m_identify = right.m_identify;
    m_trackIndex = right.m_trackIndex;
    m_g4TrackId = right.m_g4TrackId;
    m_hitEmc = right.m_hitEmc;
    m_PDGCode = right.m_PDGCode;
    m_PDGCharge = right.m_PDGCharge;
    m_particleName = right.m_particleName;
    m_edep = right.m_edep;
    m_time = right.m_time;
    m_momentum = right.m_momentum;
    m_position = right.m_position;
  }
  
  return *this;
}
  
void BesEmcTruthHit::Print()
{
  G4cout<<"Id: "<<m_identify<<"\tTrack Index: "<<m_trackIndex<<"\tG4 Track Id: "<<m_g4TrackId
    <<"\tHit Emc: "<<m_hitEmc<<"\tTotal Energy: "<<m_edep
    <<"\nPDGCode: "<<m_PDGCode<<"\tCharge: "<<m_PDGCharge<<"\tParticle Name: "<<m_particleName
    <<"\nGloble Time: "<<m_time<<"\tMomentum: "<<m_momentum.mag()<<"\tPosition: "<<m_position<<G4endl;

  std::map<Identifier,G4double>::iterator iHitMap;
  for(iHitMap=m_hitMap.begin();iHitMap!=m_hitMap.end();iHitMap++) {
    G4cout<<iHitMap->first<<"\t"<<iHitMap->second<<G4endl;
  }
}

std::map<Identifier,G4double>::const_iterator BesEmcTruthHit::Begin() const
{
  return m_hitMap.begin();
}

std::map<Identifier,G4double>::const_iterator BesEmcTruthHit::End() const
{
  return m_hitMap.end();
}

std::map<Identifier,G4double>::const_iterator BesEmcTruthHit::Find(Identifier id) const
{
  return m_hitMap.find(id);
}

G4double BesEmcTruthHit::GetEHit(Identifier id)
{
  return m_hitMap[id];
}

void BesEmcTruthHit::AddEHit(Identifier id,G4double energy)
{
  if(energy>0) m_hitMap[id]+=energy;
}

void BesEmcTruthHit::Insert(Identifier id,G4double energy)
{
  if(energy>0) m_hitMap[id]=energy;
}

G4int BesEmcTruthHit::Size() const
{
  return m_hitMap.size();
}
