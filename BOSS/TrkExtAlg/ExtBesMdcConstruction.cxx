#include "TrkExtAlg/ExtBesMdcConstruction.h"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4UserLimits.hh"

#include "G4ios.hh"
#include "G4Geo/MdcG4Geo.h"

ExtBesMdcConstruction::ExtBesMdcConstruction() {}

ExtBesMdcConstruction::~ExtBesMdcConstruction() {}

void ExtBesMdcConstruction::Construct(G4LogicalVolume* logicBes)
{
	MdcG4Geo* aMdcG4Geo = new MdcG4Geo();
	G4LogicalVolume* logicalMdc = aMdcG4Geo->GetTopVolume();
	if(!logicalMdc){
           G4cout<<"ExtBesMdcConstruction::Construct(), logicalMdc not found"<<G4endl;
        }
	else{//construct Mdc
	   physicalMdc = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalMdc, "physicalMdc",logicBes, false, 0);}
}
