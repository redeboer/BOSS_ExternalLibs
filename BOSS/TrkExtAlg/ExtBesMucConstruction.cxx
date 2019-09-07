#include "TrkExtAlg/ExtBesMucConstruction.h"
#include "G4Geo/MucG4Geo.h"

ExtBesMucConstruction::ExtBesMucConstruction() {}

ExtBesMucConstruction::~ExtBesMucConstruction() {}

void ExtBesMucConstruction::Construct(G4LogicalVolume* logicBes)
{
	MucG4Geo *aMucG4Geo = new MucG4Geo();
        logicalMuc = aMucG4Geo->GetTopVolume();
	logicalMuc  = FindLogicalVolume("logicalMuc");
	if(logicalMuc)	physicalMuc = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalMuc,"PhysicalMuc",logicBes,false,0);
	else G4cout<<"ExtBesMucConstruction::Construct() 1, logicalMuc not found!"<<G4endl;

}
	
