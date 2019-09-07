//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Using gdml in G4
//Author: Liuhm 
//Created: May 25, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesDetectorConstruction_H
#define BesDetectorConstruction_H 1

#include "G4String.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include "BesMdcConstruction.hh"
#include "BesTofConstruction.hh"
#include "BesEmcConstruction.hh"
#include "BesMucConstruction.hh"
#include "BesSCM.hh"
#include "BesPip.hh"

#include "BesMagneticField.hh"
#include "SAXProcessor.hh"
#include "ProcessingConfigurator.hh"

class BesDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    BesDetectorConstruction();
    ~BesDetectorConstruction();

  public:
         G4VPhysicalVolume* Construct();
         G4LogicalVolume*   FindLogicalVolume( const G4String& vn);
         void SetPipeSCM(G4int value) {m_pipeSCM = value;}
	 void SetVisAttributes(G4String volumeName);
  private:
   //SAXProcessor sxp;
   //ProcessingConfigurator config;
   BesMagneticField* besField; 
   BesMdcConstruction* mdc;
   BesTofConstruction* tof;
   BesEmcConstruction* emc;
   BesMucConstruction* muc;
   BesSCM* m_SCM;
   BesPip* m_Pip;

   G4int m_pipeSCM;
   G4LogicalVolume* lWorld;
   G4VPhysicalVolume* fWorld;
};

#endif





