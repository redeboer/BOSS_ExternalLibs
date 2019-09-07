#include "TrkExtAlg/ExtBesTofConstruction.h"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4Geo/TofG4Geo.h"
#include "G4Geo/MRPCG4Geo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"


ExtBesTofConstruction::ExtBesTofConstruction() 

{
}

ExtBesTofConstruction::~ExtBesTofConstruction() {}

void ExtBesTofConstruction::Construct(G4LogicalVolume* logicBes)
{

	TofG4Geo* aTofG4Geo = new TofG4Geo();
	logicalTof = aTofG4Geo->GetTopVolume();
	if(!logicalTof)  G4cout<<"ExtBesTofConstruction::Construct(), logicalTof not found"<<G4endl;
	physicalTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTof,"physicalTof",logicBes,false,0);
}


void ExtBesTofConstruction::ConstructETF(G4LogicalVolume* logicBes) {

  MRPCG4Geo* aTofG4Geo = new MRPCG4Geo();
  logicalTof = aTofG4Geo->GetTopVolume();
  if(!logicalTof)  G4cout<<"ExtBesTofConstruction::Construct(), logicalTof not found"<<G4endl;
  physicalTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTof,"physicalTof",logicBes,false,0);
}
