//
//File: ExtBesDetectorConstruction.h
//Author: L.L.Wang
//Description: Constuction of BESIII detector using package GdmlToG4.
//History: 2005.8.5 created by L.L.Wang
//


#ifndef ExtBesDetectorConstruction_H
#define ExtBesDetectorConstruction_H

#include "G4String.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

#include "TrkExtAlg/ExtBesMdcConstruction.h"
#include "TrkExtAlg/ExtBesTofConstruction.h"
#include "TrkExtAlg/ExtBesEmcConstruction.h"
#include "TrkExtAlg/ExtBesMucConstruction.h"

#include "TrkExtAlg/ExtBesMagneticField.h"
//#include "ReadBoostRoot.hh"

class ExtBesDetectorConstruction : public G4VUserDetectorConstruction
{
	public:
		ExtBesDetectorConstruction(const bool bFieldOn,int tofversion);
		~ExtBesDetectorConstruction();
		G4VPhysicalVolume* Construct();


	private:
		G4VPhysicalVolume* fWorld;
		ExtBesMagneticField*  besField;
		ExtBesMdcConstruction Mdc;
		ExtBesTofConstruction Tof;
		

		ExtBesEmcConstruction Emc;
		ExtBesMucConstruction Muc;
		int m_detVer;
};

#endif
