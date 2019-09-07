//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created:  Oct.26, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesMdcDigi.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<BesMdcDigi> BesMdcDigiAllocator;

BesMdcDigi::BesMdcDigi()
{
  trackID=0;
  layerNo=0;
  cellNo=0;
  edep=0;
  driftT=0;
}

BesMdcDigi::~BesMdcDigi() {}

BesMdcDigi::BesMdcDigi(const BesMdcDigi& right)
:G4VDigi()
{
  trackID   = right.trackID;
  layerNo   = right.layerNo;
  cellNo    = right.cellNo;
  edep      = right.edep;
  driftT    = right.driftT;
}


const BesMdcDigi& BesMdcDigi::operator=(const BesMdcDigi& right)
{
  trackID   = right.trackID;
  layerNo   = right.layerNo;
  cellNo    = right.cellNo;
  edep      = right.edep;
  driftT    = right.driftT;
  return *this;
}


int BesMdcDigi::operator==(const BesMdcDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

/*
void BesMdcDigi::Draw()
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
*/

void BesMdcDigi::Print()
{
  G4cout << "  trackID: " << trackID << "  layerNo: " << layerNo
         << "  cellNo:  " << cellNo
         << "  energy deposit: " << G4BestUnit(edep,"Energy")
         << "  driftT: " << G4BestUnit(driftT,"Time")<<G4endl;
}


