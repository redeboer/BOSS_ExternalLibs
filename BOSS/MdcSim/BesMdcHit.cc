//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Track type hit class for BESIII MDC
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#include "BesMdcHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<BesMdcHit> BesMdcHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesMdcHit::BesMdcHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesMdcHit::~BesMdcHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesMdcHit::BesMdcHit(const BesMdcHit& right)
  : G4VHit()
{
  trackID   = right.trackID;
  layerNo   = right.layerNo;
  cellNo    = right.cellNo;
  edep      = right.edep;
  pos       = right.pos;
  driftD    = right.driftD;
  driftT    = right.driftT;
  globalT   = right.globalT;
  theta     = right.theta;
  enterAngle= right.enterAngle;
  posFlag   = right.posFlag;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const BesMdcHit& BesMdcHit::operator=(const BesMdcHit& right)
{
  trackID   = right.trackID;
  layerNo   = right.layerNo;
  cellNo    = right.cellNo;
  edep      = right.edep;
  pos       = right.pos;
  driftD    = right.driftD;
  driftT    = right.driftT;
  globalT   = right.globalT;
  theta     = right.theta;
  enterAngle= right.enterAngle;
  posFlag   = right.posFlag;  
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int BesMdcHit::operator==(const BesMdcHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesMdcHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesMdcHit::Print()
{
  G4cout << "  trackID: " << trackID 
         << "  layerNo: " << layerNo
         << "  cellNo:  " << cellNo
         << "  energy deposit: " << G4BestUnit(edep,"Energy")
	 << "  position: " << G4BestUnit(pos,"Length") 
         << "  driftD: " << G4BestUnit(driftD,"Length")
         << "  driftT: " << G4BestUnit(driftT,"Time")
	 << "  globalT: " << G4BestUnit(globalT,"Time")
	 << "  theta: " << G4BestUnit(theta,"Angle")
	 << "  enterAngle: " << G4BestUnit(enterAngle,"Angle")
	 << "  posFlag: " << posFlag
	 <<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

