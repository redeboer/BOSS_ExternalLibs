//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Mimic the BES generator, TESTER
//Author: Liuhm
//Created: June, 2003
//Modified: Liuhm, 7 April, 2004, re-randomize phi distribution
//Comment: Use Geant4 ParticleGun
//---------------------------------------------------------------------------//

#include "BesPrimaryGeneratorAction.hh"
#include "BesPrimaryGeneratorMessenger.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleMomentum.hh"
#include "Randomize.hh"
#include "G4HEPEvtInterface.hh"
#include "TFile.h"
#include "TH1F.h"

BesPrimaryGeneratorAction::BesPrimaryGeneratorAction()
{
  nParticle = 1;
  particleName = "pi-";
  minCos = -0.8;
  maxCos = 0.8;
  phiStart = 0.;
  phiEnd = 360.;
  pMomentum = 1.; 
  deltaP = 0.;
  posX=0;
  posY=0;
  posZ=0;

  particleGun = new G4ParticleGun(1);
  isGenbes = true;
  HEPEvt = 0;
  generatorName = "tester";  
  //TESTER parameters passed by this messenger  
  messenger = new BesPrimaryGeneratorMessenger(this);
  
  if(generatorName == "cosmic")
  {
    std::string path = getenv("GENSIMROOT");
    G4cout<<"path: "<<path<<G4endl;

    path += "/root/";
    std::string pFile = path + "ppdc.root";
    std::string thetaFile = path + "theta.root";
    std::string phiFile = path +"phi.root";
  
    TFile* f1 = new TFile(pFile.c_str());
    h1 = (TH1F*)f1->Get("htemp");
  
    TFile* f2 = new TFile(thetaFile.c_str());
    h2 = (TH1F*)f2->Get("htemp");
  
    TFile* f3 = new TFile(phiFile.c_str());
    h3 = (TH1F*)f3->Get("htemp");
  
    //ftest = new TFile("ftest.root","recreate");
    //tuple = new TNtuple("test","test","p:theta:phi"); 
    //counter = 0;
  }
}

BesPrimaryGeneratorAction::~BesPrimaryGeneratorAction()
{
  delete particleGun;
  if(messenger) delete messenger; 
  if(generatorName=="genbes")delete HEPEvt; 
}

void BesPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if(generatorName=="tester")
  {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle
      = particleTable->FindParticle(particleName);
    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticlePosition(G4ThreeVector(posX,posY,posZ));
        
    //set TESTER parameters
    for(G4int i=0; i<nParticle; i++)
    {   
      G4double pMag = pMomentum;
      //randomize p
      if(deltaP>0.)pMag = pMomentum - deltaP*(1.0 - 2.0*G4UniformRand());
      pMag = pMag*GeV;
      //randomize cos(theta)
      G4double costheta = minCos +(maxCos - minCos)*G4UniformRand();
      //randomize phi
      G4double phi = phiStart+(phiEnd-phiStart)*G4UniformRand();
      phi = phi*degree;
      G4double sintheta = sqrt(1.-costheta*costheta);
      //computer 3-vector momentum
      G4ParticleMomentum aMomentum; 
      aMomentum[0] = pMag*sintheta*cos(phi);
      aMomentum[1] = pMag*sintheta*sin(phi);
      aMomentum[2] = pMag*costheta;  
      //use ParticleGun to generate event 
      particleGun->SetParticleMomentum(aMomentum);
      particleGun->GeneratePrimaryVertex(anEvent);
    }
  }
  else if(generatorName=="cosmic")
  { 
    G4cout<<"generatorName: "<<generatorName<<G4endl;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle
      = particleTable->FindParticle(particleName);
    G4cout<<"particleName: "<<particleName<<G4endl;

    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticlePosition(G4ThreeVector(posX,posY,posZ));
    
    /*std::string path = getenv("GENSIMROOT");
    G4cout<<"path: "<<path<<G4endl;

    path += "/root/";
    std::string pFile = path + "ppdc.root";
    std::string thetaFile = path + "theta.root";
    std::string phiFile = path +"phi.root";*/

    //TFile* f1 = new TFile(pFile.c_str());
    //H1F* h1 = (TH1F*)f1->Get("htemp");
    G4double pMag = h1->GetRandom()*GeV;
    G4cout<<"pMag: "<<pMag<<G4endl;
    //f1->Close();

    //TFile* f2 = new TFile(thetaFile.c_str());
    //TH1F* h2 = (TH1F*)f2->Get("htemp");
    //randomize cos(theta)
    G4double theta =(Double_t)h2->GetRandom();
    G4cout<<"theta: "<<theta<<G4endl;
    G4double costheta = cos(theta);
    //f2->Close();

    //TFile* f3 = new TFile(phiFile.c_str());
    //TH1F* h3 = (TH1F*)f3->Get("htemp");
    //randomize phi
    G4double phi = (Double_t)h3->GetRandom();
    G4cout<<"phi: "<<phi<<G4endl;
    //f3->Close();

    //f1->Close();
    //f2->Close();
    //f3->Close();
    //ftest->ReOpen("update");
    //tuple->Fill(pMag,theta,phi);
    //counter++;
    //if(counter==2000)
    //  tuple->Write();
 
    G4double sintheta = sqrt(1.-costheta*costheta);
    //computer 3-vector momentum
    G4ParticleMomentum aMomentum; 
    aMomentum[0] = pMag*sintheta*cos(phi);
    aMomentum[1] = pMag*sintheta*sin(phi);
    aMomentum[2] = pMag*costheta;  
    //use ParticleGun to generate event 
    particleGun->SetParticleMomentum(aMomentum);
    particleGun->GeneratePrimaryVertex(anEvent);
  }

  else if(generatorName=="genbes")
  {
    G4cout<<"genbes called"<<G4endl;
    if(isGenbes)
    {
      isGenbes=false;
      HEPEvt=new G4HEPEvtInterface(genbesName);
    }
    HEPEvt->GeneratePrimaryVertex(anEvent);
  } 
}

