#ifndef ExtBesMucConstruction_h
#define ExtBesMucConstruction_h 1

#include "TrkExtAlg/ExtBesSubdetector.h"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"


class ExtBesMucConstruction : public ExtBesSubdetector
{
	public:
		ExtBesMucConstruction();
		virtual ~ExtBesMucConstruction();
		//construct MUC detector
		void Construct(G4LogicalVolume* logicBes);
		
		//your private data member (geometry, etc) here:
		private:
		G4VPhysicalVolume* physicalMuc;
		G4LogicalVolume*   logicalMuc;
};

#endif
