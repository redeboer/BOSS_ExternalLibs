//
//File: ExtBesDetectorConstruction.cxx
//Author: L.L.Wang
//Description: Constuction of BESIII detector using package GdmlToG4.
//History: 2005.8.5 created by L.L.Wang
//


#include "TrkExtAlg/ExtBesDetectorConstruction.h"

#include "G4Colour.hh"
#include "globals.hh"
#include <cstdlib>
#include <fstream>
#include "G4String.hh"

#include "GDMLProcessor.hh"
#include "GDMLExpressionEvaluator.hh"

// Added here just to help resolve properly dependencies
#include "G4BooleanSolid.hh"
#include "G4Geo/BesG4Geo.h"


ExtBesDetectorConstruction::ExtBesDetectorConstruction(const bool bFieldOn, int detVer)
{
	if(bFieldOn) besField = new ExtBesMagneticField();
	fWorld=0;
	m_detVer = detVer;
}


ExtBesDetectorConstruction::~ExtBesDetectorConstruction()
{
	if(besField)
	delete besField;
}


G4VPhysicalVolume* ExtBesDetectorConstruction::Construct()
{
	BesG4Geo* aBesG4Geo = new BesG4Geo();

	fWorld=(G4VPhysicalVolume *)GDMLProcessor::GetInstance()->GetWorldVolume();  
	if( fWorld == 0 ) {
	          G4Exception("World volume not set properly check your setup selection criteria or GDML input!");
        }

	delete aBesG4Geo;
			    
	G4LogicalVolume* logicalBes = const_cast<G4LogicalVolume*>( GDMLProcessor::GetInstance()->GetLogicalVolume("logicalWorld" ) );
	Mdc.Construct(logicalBes);

	
	if( m_detVer==1 || m_detVer==2 ) { 
	  std::cout << "ExBesDetectorConstruction:     You are using the Sintillator Tof Detector!" << std::endl;
	  Tof.Construct(logicalBes);
	}
	else if( m_detVer==3 ) {
	  std::cout << "ExBesDetectorConstruction:     You are using the upgraded (MRPC) Tof Detector!" << std::endl;
	  Tof.ConstructETF(logicalBes);
	}
	else {
	  std::cout << "ExBesDetectorConstruction:     Detector Version is not well defined! " << m_detVer << " \n  " << std::endl;
	  std::cout << "                               Use in your jobOption script " << std::endl;
	  std::cout << "                               TrkExtAlg.Tof = 1; for the upgraded Tof (MRPC Endcaps, double sided readout) " << std::endl;
	  std::cout << "                               TrkExtAlg.Tof = 0; for the Sintillator Tof. " << std::endl;
	}

	
        Emc.Construct(logicalBes);
        Muc.Construct(logicalBes);
	
	return fWorld;
}
