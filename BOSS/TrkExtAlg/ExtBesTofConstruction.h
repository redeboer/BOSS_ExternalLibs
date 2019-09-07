#ifndef ExtBesTofConstruction_h
#define ExtBesTofConstruction_h 1

#include "TrkExtAlg/ExtBesSubdetector.h"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"


class ExtBesTofConstruction : public ExtBesSubdetector
{
	public:
		ExtBesTofConstruction();
		virtual ~ExtBesTofConstruction();
	
		void Construct(G4LogicalVolume* logicBes);
		void ConstructETF(G4LogicalVolume* logicBes);
	
	private:
		G4LogicalVolume*   logicalTof;
		G4VPhysicalVolume* physicalTof;


};

#endif
