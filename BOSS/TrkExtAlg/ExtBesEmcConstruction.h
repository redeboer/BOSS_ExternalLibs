#ifndef ExtBesEmcConstruction_h
#define ExtBesEmcConstruction_h 1

#include "globals.hh"
#include "TrkExtAlg/ExtBesSubdetector.h"

//For Code construction------------
#include "ExtBesEmcGeometry.h"
#include "ExtBesEmcEndGeometry.h"
#include "G4VPVParameterisation.hh"

class G4Box;
class G4Tubs;
class G4Cons;
class G4Polyhedra;
class G4Trap;
class G4UnionSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UniformMagField;
class BesEmcDetectorMessenger;
class BesEmcSD;
class G4IrregBox;
class G4SubtractionSolid;
//--------------------------------

class ExtBesEmcConstruction : public ExtBesSubdetector
{
	public:
		ExtBesEmcConstruction();
		~ExtBesEmcConstruction();

		static ExtBesEmcConstruction* GetBesEmcConstruction() {return fBesEmcConstruction;};

		void Construct(G4LogicalVolume*);
		
		void ConstructSPFrame(G4LogicalVolume*, ExtBesEmcGeometry*);
		void ConstructEndGeometry(G4LogicalVolume*);
		G4int ComputeEndCopyNb(G4int);


		inline void SetVerboseLevel(G4int val){ verboseLevel=val;}

		void SetCrystalMaterial (G4String);

		void SetCasingMaterial (G4String);
		void SetCasingThickness(G4ThreeVector);

		void SetBSCRmin(G4double);
		void SetBSCNbPhi(G4int);
		void SetBSCNbTheta(G4int);
		void SetStartIDTheta(G4int);
		void SetBSCCrystalLength(G4double);
		void SetBSCYFront0(G4double);
		void SetBSCYFront(G4double);
		void SetBSCPosition0(G4double);
		void SetBSCPosition1(G4double);

		void SetMagField(G4double);

		void UpdateGeometry();

		void PrintEMCParameters();

  		G4int GetVerboseLevel() {return verboseLevel;};
  		G4double GetMagField()  {return fmagField;};

  		G4int GetStartIDTheta()          {return startID;};

  		G4Material* GetCrystalMaterial()  {return fCrystalMaterial;};
  		G4Material* GetCasingMaterial()       {return fCasingMaterial;};
  //G4ThreeVector GetCasingThickness()   {return G4ThreeVector(fTyvekThickness,fAlThickness,fMylarThickness);};

  		const G4VPhysicalVolume* GetEMC() {return physiEMC; }
  		const G4VPhysicalVolume* GetBSCPhi() {return physiBSCPhi;};
  		const G4VPhysicalVolume* GetBSCTheta() {return physiBSCTheta;};
  		const G4VPhysicalVolume* GetBSCCrystal(){return physiBSCCrystal;};
  		const G4VPVParameterisation* GetCrystalParam(){return crystalParam;};
	
	private:
		static ExtBesEmcConstruction* fBesEmcConstruction;

	private:
		void DefineMaterials();
		//		G4LogicalVolume* logicalEmc;
		//		G4VPhysicalVolume* physicalEmc;
		
//For code construction-----------------------
  G4int              verboseLevel;
  G4double           fmagField;
     
  G4Material*        fCrystalMaterial;

  G4int              phiNbCrystals;
  G4int              thetaNbCrystals;
  G4int              startID;

  G4Material*        fCasingMaterial;
  
  G4Material*        defaultMaterial;

  G4Tubs*             solidBSC;     //Barrel
  G4Tubs*             solidESC;     //Endcap
  G4UnionSolid*       solidEMC;
  G4LogicalVolume*    logicEMC;
  G4VPhysicalVolume*  physiEMC;
  
  G4Tubs*            solidBSCPhiTub;
  G4Cons*            solidConsPhi;
  G4SubtractionSolid* solidBSCPhi1;
  G4SubtractionSolid* solidBSCPhi;   //pointer to the solid Layer 
  G4LogicalVolume*   logicBSCPhi;    //pointer to the logical Layer
  G4VPhysicalVolume* physiBSCPhi;    //pointer to the physical Layer

  G4Trap*            solidBSCTheta; //pointer to the solid Absorber
  G4LogicalVolume*   logicBSCTheta; //pointer to the logical Absorber
  G4VPhysicalVolume* physiBSCTheta; //pointer to the physical Absorber
  
  G4Trap*            solidBSCCrystal; //pointer to the solid Absorber
  G4LogicalVolume*   logicBSCCrystal; //pointer to the logical Absorber
  G4VPhysicalVolume* physiBSCCrystal; //pointer to the physical Absorber

  G4UniformMagField* magField;      //pointer to the magnetic field
  
  BesEmcDetectorMessenger* detectorMessenger;  //pointer to the Messenger
  BesEmcSD*          besEMCSD;  //pointer to the sensitive detector
  ExtBesEmcGeometry     *besEMCGeometry;
  ExtBesEmcEndGeometry  *emcEnd;
  G4VPVParameterisation*  crystalParam;


//Endcap geometry
  G4Cons*            solidEnd;      //pointer to the endcap
  G4LogicalVolume*   logicEnd;
  G4VPhysicalVolume* physiEnd;

  G4Cons*            solidEndPhi;
  G4LogicalVolume*   logicEndPhi;
  G4VPhysicalVolume* physiEndPhi;

  G4IrregBox*        solidEndCasing;
  G4LogicalVolume*   logicEndCasing;
  G4VPhysicalVolume* physiEndCasing;

  G4IrregBox*        solidEndCrystal;
  G4LogicalVolume*   logicEndCrystal;
  G4VPhysicalVolume* physiEndCrystal;

//other substance on the rear face of crystal
private:

  G4Box*              solidRear;          //virtual volume
  G4LogicalVolume*    logicRear;
  G4VPhysicalVolume*  physiRear;

  G4Box*              solidCasingBox;     //casing
  G4Box*              solidAirHole;
  G4SubtractionSolid* solidRearCasing;
  G4LogicalVolume*    logicRearCasing;
  G4VPhysicalVolume*  physiRearCasing;

  G4Box*              solidOrgGlass;      //organic glass
  G4LogicalVolume*    logicOrgGlass;
  G4VPhysicalVolume*  physiOrgGlass;

  G4Box*              solidPD;            //photodiode
  G4LogicalVolume*    logicPD;
  G4VPhysicalVolume*  physiPD;

  G4Box*              solidAlBox;         //Al plate
  G4SubtractionSolid* solidAlPlate;
  G4LogicalVolume*    logicAlPlate;
  G4VPhysicalVolume*  physiAlPlate;

  G4Box*              solidPreAmpBox;     //preamplifier box
  G4LogicalVolume*    logicPreAmpBox;
  G4VPhysicalVolume*  physiPreAmpBox;

  G4Box*              solidAirInPABox;    //air in preamplifier box
  G4LogicalVolume*    logicAirInPABox;
  G4VPhysicalVolume*  physiAirInPABox;

  G4Box*              solidHangingPlate;  //stainless steel for hanging the crysta
  G4LogicalVolume*    logicHangingPlate;
  G4VPhysicalVolume*  physiHangingPlate;

  G4Cons*             solidOCGirder;      //opening-cut girder
  G4LogicalVolume*    logicOCGirder;
  G4VPhysicalVolume*  physiOCGirder;

  G4Tubs*             solidCable;         //cable
  G4LogicalVolume*    logicCable;
  G4VPhysicalVolume*  physiCable;

  G4Tubs*             solidWaterPipe;     //water pipe
  G4LogicalVolume*    logicWaterPipe;
  G4VPhysicalVolume*  physiWaterPipe;

//barrel support frame
private:
  G4Tubs*             solidSupportBar;      //support bar
  G4LogicalVolume*    logicSupportBar;
  G4VPhysicalVolume*  physiSupportBar;
  
  G4Tubs*             solidSupportBar1;
  G4LogicalVolume*    logicSupportBar1;
  G4VPhysicalVolume*  physiSupportBar1;
  
  G4Tubs*             solidEndRing;       //end ring
  G4LogicalVolume*    logicEndRing;
  G4VPhysicalVolume*  physiEndRing;

  G4Tubs*             solidGear;
  G4LogicalVolume*    logicGear;
  G4VPhysicalVolume*  physiGear;

  G4Tubs*             solidTaperRing1;     //taper ring
  G4LogicalVolume*    logicTaperRing1;
  G4VPhysicalVolume*  physiTaperRing1;

  G4Cons*             solidTaperRing2;
  G4LogicalVolume*    logicTaperRing2;
  G4VPhysicalVolume*  physiTaperRing2;

  G4Cons*             solidTaperRing3;
  G4LogicalVolume*    logicTaperRing3;
  G4VPhysicalVolume*  physiTaperRing3;

private:
  G4Material*         rearCasingMaterial;
  G4Material*         organicGlass;
  G4Material*         stainlessSteel;
  G4Material*         cable;
  G4Material*         waterPipe;

//----------------------------------------------------------------


};

#endif
