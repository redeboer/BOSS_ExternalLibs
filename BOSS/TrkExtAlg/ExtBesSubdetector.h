#ifndef ExtBesSubdetector_H
#define ExtBesSubdetector_H 1


#include "GDMLProcessor.hh"
#include "G4LogicalVolume.hh"

class ExtBesSubdetector
{
	public:
		ExtBesSubdetector(){};
		virtual ~ExtBesSubdetector(){};

		//construct your detector and position it in BES world volume
		//You MUST derive your concrete class and implement it
		virtual void Construct(G4LogicalVolume* bes) = 0;
		inline G4LogicalVolume *FindLogicalVolume( const G4String& vn);

};

inline G4LogicalVolume* ExtBesSubdetector::FindLogicalVolume( const G4String& vn )
{
	return const_cast<G4LogicalVolume*>( GDMLProcessor::GetInstance()->GetLogicalVolume(vn) );
}

#endif
