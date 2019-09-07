//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Muon Chamber as a concrete class
//             it's better to define an envelope then position it in BES
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created: Nov, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesMucConstruction_h
#define BesMucConstruction_h 1

#include "BesSubdetector.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

class G4LogicalVolume;

class BesMucConstruction : public BesSubdetector
{  
public:
  
  BesMucConstruction();
  virtual ~BesMucConstruction();

  //construct MUC detector
  void Construct(G4LogicalVolume* logicBes);
  G4VPhysicalVolume* GetPhysicalMuc(){return physicalMuc;};

   
  //your private data member (geometry, etc) here:
private:
  G4VPhysicalVolume* physicalMuc;
  G4LogicalVolume*   logicalMuc;
  
};
#endif

   
     





























