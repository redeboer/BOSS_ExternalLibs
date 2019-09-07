#ifndef ExtBesEmcEndGeometry_h
#define ExtBesEmcEndGeometry_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
class ExtBesEmcConstruction;

class ExtBesEmcEndGeometry
{
  public:
    ExtBesEmcEndGeometry();
    ~ExtBesEmcEndGeometry();
    friend class ExtBesEmcConstruction;

  public:
    void ReadParameters();
    void ComputeParameters();
    void Exchange(G4int cry1, G4int cry2);
    void ReflectX();
    void ModifyForCasing(G4ThreeVector pos[8], G4int CryNb);
    G4ThreeVector ComputeDimAndPos(const G4int, const G4int, const G4int);

  public:
    inline G4int GetCryNumInOneLayer(G4int num){ return cryNumInOneLayer[num]; }

  private:
    G4double WorldRmin1;
    G4double WorldRmax1;
    G4double WorldRmin2;
    G4double WorldRmax2;
    G4double WorldDz;
    G4double WorldZPosition;

    G4double fTyvekThickness;
    G4double fAlThickness;
    G4double fMylarThickness;
    G4double totalThickness;
    G4double CrystalLength;
                    
  private:
    G4double param[35][24];
    G4double penta[5][6];
    G4int cryNumInOneLayer[6];
    G4int pentaInOneSector[5];
    G4ThreeVector fPnt[35][8];        //8 point of one crystal
    G4ThreeVector fPnt1[35][8];
    G4ThreeVector cryPoint[8];
};

#endif
