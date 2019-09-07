//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: for generators, TESTER and GENBES
//Author: Liuhm
//Created: June, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesPrimaryGeneratorAction_h
#define BesPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
//#include "TFile.h"
//#include "TNtuple.h"
#include "TH1F.h"

class G4ParticleGun;
class G4Event;
class G4ParticleTable; 
class BesPrimaryGeneratorMessenger;
class G4VPrimaryGenerator;

class BesPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

    BesPrimaryGeneratorAction();
    ~BesPrimaryGeneratorAction();

  public:

    void GeneratePrimaries(G4Event* anEvent);
// Getter and setters
    G4String GetGeneratorName(){return generatorName;}
    void SetGeneratorName(G4String genNew){generatorName=genNew;}
    G4String GetGenbesName(){return genbesName;}
    void SetGenbesName(G4String genbesNew){genbesName=genbesNew;} 
    void SetNParticle(G4int np){nParticle = np;}
    void SetParticleName(G4String pname){particleName = pname;}
    void SetMinCos(G4double min1){minCos = min1;}
    void SetMaxCos(G4double max1){maxCos = max1;}
    void SetPhiStart(G4double ps){phiStart =ps;}
    void SetPhiEnd(G4double pe){phiEnd = pe;}
    void SetMomentum(G4double pm){pMomentum = pm;}
    void SetDeltaP(G4double dp){deltaP = dp;}
    void SetPosX(G4double x){posX = x;}
    void SetPosY(G4double y){posY = y;}
    void SetPosZ(G4double z){posZ = z;}

  private:

    G4ParticleGun* particleGun;
    BesPrimaryGeneratorMessenger* messenger;
    G4VPrimaryGenerator* HEPEvt;
    G4String generatorName;
    G4String genbesName;
    G4bool isGenbes; 
    
    G4int nParticle;
    G4String particleName;
    G4double minCos;
    G4double maxCos;
    G4double phiStart;
    G4double phiEnd;
    G4double pMomentum;
    G4double deltaP;
    G4double posX;
    G4double posY;
    G4double posZ;
    //TFile* ftest;
    //TNtuple* tuple;
    //G4int counter;
    TH1F* h1;
    TH1F* h2;
    TH1F* h3;
}; 

#endif /*BesPrimaryGeneratorAction_h*/



