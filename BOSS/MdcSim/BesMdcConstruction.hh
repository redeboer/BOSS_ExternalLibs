//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Main part for construct MDC geometry 
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: Used in  "BesDetectorConstruction"
//---------------------------------------------------------------------------/

#ifndef BesMdcConstruction_H
#define BesMdcConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;

//#include "G4VUserDetectorConstruction.hh"
#include "BesSubdetector.hh"
#include "BesMdcGeoParameter.hh"
#include "BesMdcSD.hh"
#include "G4VisAttributes.hh"

class BesMdcConstruction : public BesSubdetector
{
public:

  BesMdcConstruction();
  ~BesMdcConstruction();
  
  void Construct(G4LogicalVolume*);
  
private:
  void TubeConstruct(G4LogicalVolume*);
  void StereoLayerConstruct(G4LogicalVolume*, G4int);
  void AxialLayerConstruct(G4LogicalVolume*, G4int);
  void AxialCellConstruct(G4LogicalVolume*, G4int, G4int);
private:
  
  // Logical volumes
  G4LogicalVolume* mdc_log;
  
  // Physical volumes
  G4VPhysicalVolume* mdc_phys;
  
  BesMdcGeoParameter* mdc;
  BesMdcSD* aTrackerSD;

  G4double outR, innerR, length;
  G4double startAngle, spanAngle;
  G4double posX, posY, posZ;
  G4double signalWireR,fieldWireR;

  G4RotationMatrix* Rot;

  G4VisAttributes* visAtt;
};

#endif
