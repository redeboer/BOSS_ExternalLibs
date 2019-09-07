#ifndef ExtBesMdcConstruction_H
#define ExtBesMdcConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;

#include "TrkExtAlg/ExtBesSubdetector.h"

class ExtBesMdcConstruction : public ExtBesSubdetector
{
	public:
		ExtBesMdcConstruction();
		~ExtBesMdcConstruction();
		
		void Construct(G4LogicalVolume*);

	private:
		G4VPhysicalVolume* physicalMdc;
		G4LogicalVolume*   logicalMdc;
};

#endif
