//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: passing parameters to Generator TESTER and GENBES 
//Author: Liuhm
//Created: Jun. 5, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesPrimaryGeneratorMessenger.hh"
#include "BesPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4ParticleTable.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ios.hh"

BesPrimaryGeneratorMessenger::BesPrimaryGeneratorMessenger(BesPrimaryGeneratorAction * mpga)
:myAction(mpga)
{
  G4UIparameter* parameter;
  G4bool omittable;
  particleTable = G4ParticleTable::GetParticleTable();  

  generatorDirectory = new G4UIdirectory("/generator/");
  generatorDirectory->SetGuidance("BESIII event gnerator");
  
  gennamCmd = new G4UIcmdWithAString("/generator/name",this);
  gennamCmd->SetGuidance("Select your event gerator: genbes or tester");
  gennamCmd->SetGuidance("The default generator is tester");
  gennamCmd->SetParameterName("name",true);
  gennamCmd->SetDefaultValue("tester");
  gennamCmd->SetCandidates("tester cosmic genbes");
   
  genbesCmd = new G4UIcmdWithAString("/generator/genbes",this);
  genbesCmd->SetGuidance("The BES data file");
  genbesCmd->SetGuidance("directory/filename");
  genbesCmd->SetParameterName("name",false);
  
  testerCmd = new G4UIcommand("/generator/tester",this);
  testerCmd->SetGuidance("The BES TESTER generator");
  testerCmd->SetGuidance("NP PID Cosmin Cosmax phimin phimax p delp");
  
  cosmicCmd = new G4UIcommand("/generator/cosmic",this);
  cosmicCmd->SetGuidance("The cosmic generator");
  cosmicCmd->SetGuidance("PID  x  y  z");

  parameter = new G4UIparameter("np",'i',omittable = true);
  parameter->SetGuidance("Number of particles in an event");
  parameter->SetParameterRange("np > 0");
  parameter->SetDefaultValue(1);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("pname",'s',omittable = true);
  parameter->SetGuidance("Particle name in Geant4");
  parameter->SetDefaultValue("pi-");
  G4String candidatelist;
  G4int nPtable = particleTable->entries();
  for(G4int i=0;i<nPtable;i++)
    {
      candidatelist += particleTable->GetParticleName(i);
      candidatelist += " ";
    }  
  parameter->SetParameterCandidates(candidatelist);
  testerCmd->SetParameter(parameter);
  
  parameter = new G4UIparameter("cosmin",'d',omittable = true);
  parameter->SetGuidance("Minimum cos(theta)");
  parameter->SetParameterRange("cosmin >= -1. && cosmin <= 1.");
  parameter->SetDefaultValue(-0.8);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("cosmax",'d',omittable = true);
  parameter->SetGuidance("Maximum cos(theta)");
  parameter->SetParameterRange("cosmax >= -1. && cosmax <= 1.");
  parameter->SetDefaultValue(0.8);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("phimin",'d',omittable = true);
  parameter->SetGuidance("Minimum azimuthal angle in degree");
  parameter->SetParameterRange("phimin >= 0. && phimin <= 360.");
  parameter->SetDefaultValue(0.);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("phimax",'d',omittable = true);
  parameter->SetGuidance("Maximum azimuthal angle in degree");
  parameter->SetParameterRange("phimax >= 0. && phimax <= 360.");
  parameter->SetDefaultValue(360.);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("pp",'d',omittable = true);
  parameter->SetGuidance("Particle Momentum in GeV");
  parameter->SetParameterRange("pp>=0.");
  parameter->SetDefaultValue(1.0);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("dp",'d',omittable = true);
  parameter->SetGuidance("Particle Momentum range in GeV");
  parameter->SetParameterRange("dp>=0.");
  parameter->SetDefaultValue(0.0);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("x",'d',omittable = true);
  parameter->SetGuidance("Particle x position");
  parameter->SetParameterRange("x >= -10000 && x <= 10000");
  parameter->SetDefaultValue(0.);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("y",'d',omittable = true);
  parameter->SetGuidance("Particle y position");
  parameter->SetParameterRange("y >= -10000 && y <= 10000");
  parameter->SetDefaultValue(0.);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("z",'d',omittable = true);
  parameter->SetGuidance("Particle z position");
  parameter->SetParameterRange("z >= -10000 && z <= 10000");
  parameter->SetDefaultValue(0.);
  testerCmd->SetParameter(parameter);

  parameter = new G4UIparameter("pname",'s',omittable = true);
  parameter->SetGuidance("Particle name in Geant4");
  parameter->SetDefaultValue("mu-");
  cosmicCmd->SetParameter(parameter);

  parameter = new G4UIparameter("x",'d',omittable = true);
  parameter->SetGuidance("Particle x position");
  parameter->SetParameterRange("x >= -10000 && x <= 10000");
  parameter->SetDefaultValue(0.);
  cosmicCmd->SetParameter(parameter);
 
  parameter = new G4UIparameter("y",'d',omittable = true);
  parameter->SetGuidance("Particle y position");
  parameter->SetParameterRange("y >= -10000 && y <= 10000");
  parameter->SetDefaultValue(0.);
  cosmicCmd->SetParameter(parameter);
  
  parameter = new G4UIparameter("z",'d',omittable = true);
  parameter->SetGuidance("Particle z position");
  parameter->SetParameterRange("z >= -10000 && z <= 10000");
  parameter->SetDefaultValue(0.);
  cosmicCmd->SetParameter(parameter);
 
}

BesPrimaryGeneratorMessenger::~BesPrimaryGeneratorMessenger()
{
  delete generatorDirectory;
  delete gennamCmd;
  delete genbesCmd;
  delete testerCmd;
  delete cosmicCmd;
}

void BesPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command == gennamCmd )
  { myAction->SetGeneratorName(newValue); }
  else if( command == genbesCmd )
  { myAction->SetGenbesName(newValue); }
  else if( command == testerCmd )
  { G4int np1;
    G4String pname1;
    G4double cosmin1,cosmax1,phimin1,phimax1,pp1,dp1,x,y,z;
    std::istringstream is ((char*)newValue.data());
    is >> np1 >> pname1 >> cosmin1 >> cosmax1
       >> phimin1 >> phimax1 >> pp1 >> dp1 >> x >> y >> z;

    myAction->SetNParticle(np1);
    myAction->SetParticleName(pname1); 
    myAction->SetMinCos(cosmin1);
    myAction->SetMaxCos(cosmax1);
    myAction->SetPhiStart(phimin1);
    myAction->SetPhiEnd(phimax1);
    myAction->SetMomentum(pp1);
    myAction->SetDeltaP(dp1);
    myAction->SetPosX(x);
    myAction->SetPosY(y);
    myAction->SetPosZ(z);
  }
  else if( command == cosmicCmd )
  {
    G4String pname;
    G4double x,y,z;
    std::istringstream is ((char*)newValue.data());
    is >> pname >> x >> y >> z;
    myAction->SetParticleName(pname);
    myAction->SetPosX(x);
    myAction->SetPosY(y);
    myAction->SetPosZ(z);
  }
}

G4String BesPrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String cv;
  if( command ==  gennamCmd )
  {cv = myAction->GetGeneratorName() ;}
  else if ( command == genbesCmd )
  { cv = myAction->GetGenbesName();}  
  return cv;
}















