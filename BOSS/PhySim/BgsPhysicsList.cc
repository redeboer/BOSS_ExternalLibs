//-----------------------------------------------------------------------------
// File and Version Information:
// $Id: BgsPhysicsList.cc,v 1.3 2007/11/22 06:50:22 caogf Exp $
//
// Description:
//	BgsPhysicsList
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Torre Wenaus, David Williams, Marc Verderi, Bill Lockman, Dennis
//Wright
//
// Copyright Information:
//	Copyright (C) 2001         SLAC
//
// Created:
// Modification history:
//
//-----------------------------------------------------------------------------

#include "BaBar.hh"
#include <math.h>
#include "BgsPhysicsList.hh"
#include <iomanip>
//#include "Bogus/BgsPDGEncode.hh"
//#include "Bogus/BgsControl.hh"
//#include "Bogus/BgsLooperDeath.hh"
//#include "Bogus/BgsChargedStepDeath.hh"
//#include "Bogus/BgsPhysicsRegistrar.hh"
#include "BgsGenocide.hh"
#include "BgsGentleGenocide.hh"
//#include "Bogus/BgsLimitedTransportation.hh"

//#include "ErrLogger/ErrLog.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4VMultipleScattering.hh"
#include "G4MultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hIonisation.hh"
#include "G4Decay.hh"
#include "G4Material.hh"
#include "G4ProductionCuts.hh" 
#include "G4ProductionCutsTable.hh" 
#include "G4MaterialCutsCouple.hh"
//
// Gamma- and electro-nuclear models and processes
//
#include "G4GammaNuclearReaction.hh"
#include "G4ElectroNuclearReaction.hh"
#include "G4TheoFSGenerator.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4QGSModel.hh"
#include "G4GammaParticipants.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4PhotoNuclearProcess.hh"
#include "G4ElectronNuclearProcess.hh"
#include "G4PositronNuclearProcess.hh"
//
// Hadronic processes
//
#include "G4HadronElasticProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonMinusInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4LambdaInelasticProcess.hh"
#include "G4AntiLambdaInelasticProcess.hh"
#include "G4DeuteronInelasticProcess.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4AlphaInelasticProcess.hh"
#include "G4ShortLivedConstructor.hh"
//
// Hadronic interaction models
// Low energy (E < 20 GeV) part only
//
#include "G4LElastic.hh"
#include "G4LEAntiProtonInelastic.hh"
#include "G4LEAntiNeutronInelastic.hh"
#include "G4LEAntiLambdaInelastic.hh"
#include "G4LEDeuteronInelastic.hh"
#include "G4LETritonInelastic.hh"
#include "G4LEAlphaInelastic.hh"

#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPInelasticData.hh"
#include "G4NeutronHPFission.hh"
#include "G4NeutronHPFissionData.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPCaptureData.hh"
#include "G4LFission.hh"
#include "G4LCapture.hh"

#include "G4CascadeInterface.hh"
#include "CLHEP/Random/RanecuEngine.h"
//
// Pi+/pi- inelastic cross sections
//
#include "G4PiNuclearCrossSection.hh"
using std::ostream;

//BgsPhysicsList::BgsPhysicsList( BgsControl *theControl, 
//			        BgsPhysicsRegistrar *pr)
BgsPhysicsList::BgsPhysicsList() : G4VUserPhysicsList(),//bes
//    control(theControl),
//    first(true),
    first(false) //bes
//    physicsRegistrar(pr),
//    theLooperDeath(0)			// looperdeath process
{
  SetVerboseLevel(2);
  bertini_model = new G4CascadeInterface;
}


BgsPhysicsList::~BgsPhysicsList()
{
//  delete theLooperDeath;
}

void 
BgsPhysicsList::ConstructParticle()
{

  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program.

  ConstructBosons();
  ConstructLeptons();
  ConstructMesons();
  ConstructBaryons();
  ConstructIons();
/*
  if (first) {
    first = false;

    //
    // Check to make sure our particles have valid PDG encodings
    //
    BgsPDGEncode encode(false);

    G4ParticleTable *table = G4ParticleTable::GetParticleTable();

    G4int nProb = 0;
    G4int n = table->entries();
    while(n--) {
      G4ParticleDefinition *part = table->GetParticle(n);
      if (encode.pdt(part) == 0) {
        nProb++;
	std::cerr << "The Geant4 particle \""
                      << part->GetParticleName()
                      << "\" is not recognized by BgsPDGEncode" << std::endl;
      }
    }
  
    if(nProb > 0) std::cerr << "One or more PDG encoding errors" <<
std::endl;
  }
*/
  // Add short lived particles for high energy models, 
  // but don't check PDG codes - they are not propagated in Bogus anyway
   
  G4ShortLivedConstructor shortLived;
  shortLived.ConstructParticle();
}

void 
BgsPhysicsList::ConstructBosons()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  // gamma
  G4Gamma::GammaDefinition();

  // optical photon
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void 
BgsPhysicsList::ConstructLeptons()
{
  // leptons
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  G4TauPlus::TauPlusDefinition();
  G4TauMinus::TauMinusDefinition();

  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
  G4NeutrinoTau::NeutrinoTauDefinition();
  G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();
}

void 
BgsPhysicsList::ConstructMesons()
{
  //  mesons
  G4PionPlus     ::PionPlusDefinition();
  G4PionMinus    ::PionMinusDefinition();
  G4PionZero     ::PionZeroDefinition();
  G4Eta          ::EtaDefinition();
  G4EtaPrime     ::EtaPrimeDefinition();
  //  G4RhoZero      ::RhoZeroDefinition();
  G4KaonPlus     ::KaonPlusDefinition();
  G4KaonMinus    ::KaonMinusDefinition();
  G4KaonZero     ::KaonZeroDefinition();
  G4AntiKaonZero ::AntiKaonZeroDefinition();
  G4KaonZeroLong ::KaonZeroLongDefinition();
  G4KaonZeroShort::KaonZeroShortDefinition();
}

void 
BgsPhysicsList::ConstructBaryons()
{
  //  baryons
  G4Proton       ::ProtonDefinition();
  G4AntiProton   ::AntiProtonDefinition();
  G4Neutron      ::NeutronDefinition();
  G4AntiNeutron  ::AntiNeutronDefinition();
  G4Lambda        ::LambdaDefinition();
  G4AntiLambda    ::AntiLambdaDefinition();
  G4SigmaPlus     ::SigmaPlusDefinition();
  G4SigmaZero     ::SigmaZeroDefinition();
  G4SigmaMinus    ::SigmaMinusDefinition();
  G4AntiSigmaPlus ::AntiSigmaPlusDefinition();
  G4AntiSigmaZero ::AntiSigmaZeroDefinition();
  G4AntiSigmaMinus::AntiSigmaMinusDefinition();
  G4XiZero        ::XiZeroDefinition();
  G4XiMinus       ::XiMinusDefinition();
  G4AntiXiZero    ::AntiXiZeroDefinition();
  G4AntiXiMinus   ::AntiXiMinusDefinition();
  G4OmegaMinus    ::OmegaMinusDefinition();
  G4AntiOmegaMinus::AntiOmegaMinusDefinition();
  G4Deuteron      ::DeuteronDefinition();
  G4Triton        ::TritonDefinition();
  G4He3           ::He3Definition();
  G4Alpha         ::AlphaDefinition();
}

void
BgsPhysicsList::ConstructIons()
{  
  G4GenericIon::GenericIonDefinition();
}

void 
BgsPhysicsList::ConstructProcess()
{
  //if (control->UseBgsTran()) {
/*  if(0) {
    AddBgsTransportation(control->GetMaxTrackingStepSize(),
			 control->GetMaxVacStepSize(), 
			 control->GetVacName());
    std::cout << "+^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^+ " << 
	    std::endl;
    std::cout << "+v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v+ " <<
	    std::endl;
    std::cout << " +---                                                  ---+ " <<
	    std::endl;
    std::cout << " +---                 BgsTransportation                ---+ " <<
      std::endl;
    std::cout << " +---                      USED !!                     ---+ " <<
      std::endl;
    std::cout << " +---                                                  ---+ " <<
      std::endl;
    std::cout << "+^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^+ " <<
      std::endl;
    std::cout << "+v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v+ " <<
      std::endl;
    
  }
  else {
  */
    AddTransportation();
    std::cout << "+^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^+ " <<
      std::endl;
    std::cout << "+v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v+ " <<
      std::endl;
    std::cout << " +---                                                  ---+ " <<
      std::endl;
    std::cout << " +---                 G4Transportation                 ---+ " <<
      std::endl;
    std::cout << " +---                      USED !!                     ---+ " <<
      std::endl;
    std::cout << " +---                                                  ---+ " <<
      std::endl;
    std::cout << "+^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^+ " <<
      std::endl;
    std::cout << "+v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v+ " <<
      std::endl;
//  }
  //  AddParameterisation();
  ConstructEM();
  ConstructLeptHad();
  ConstructHad();
  ConstructGeneral();
  ConstructNeutrinoGenocide();
  ConstructIonFix();
  ConstructNeutronFix();
}
/*
void  
BgsPhysicsList::AddBgsTransportation( G4double maxTrackStepSize, G4double
maxVacStepSize, const G4String& vacName )
{
  static const bool beParanoid = false;
  BgsTransportation *theTransportationProcess =
    new BgsLimitedTransportation(maxTrackStepSize*mm, maxVacStepSize*mm,
vacName, beParanoid );

  // loop over all particles in G4ParticleTable
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (!particle->IsShortLived()) {
      // Add transportation process for all particles other than
"shortlived"
      if ( pmanager == 0) {
	// Error !! no process manager
	ErrMsg(fatal) << "G4VUserPhysicsList::AddTransportation : no process
manager" << endmsg;
      } else {
	// add transportation with ordering = ( -1, "first", "first" )
        pmanager ->AddProcess(theTransportationProcess);
        pmanager ->SetProcessOrderingToFirst(theTransportationProcess, 
                                             idxAlongStep);
        pmanager ->SetProcessOrderingToFirst(theTransportationProcess, 
                                             idxPostStep);
        physicsRegistrar->Register( theTransportationProcess, pmanager,
                                    "transport" );
      }
    } else {
      // shortlived particle case
    }
  }
}
*/
// **************************** EM Processes********************************

void 
BgsPhysicsList::ConstructEM()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "gamma") {
      // gamma
      // Construct processes for gamma

      pmanager->AddDiscreteProcess( new G4PhotoElectricEffect());
      pmanager->AddDiscreteProcess( new G4ComptonScattering());
      pmanager->AddDiscreteProcess( new G4GammaConversion());

    } else if (particleName == "e-") {
      //electron
      // Construct processes for electron
      //change for geant4.9.0.p01
      G4MultipleScattering* ms = new G4MultipleScattering();
      //ms->MscStepLimitation(false,0.02);
      ms->SetRangeFactor(0.02);

      //      ms->SetLateralDisplasmentFlag(false);
      
      G4eIonisation *ionizationProcess = new G4eIonisation();
      //      ionizationProcess->SetLinearLossLimit(1.0);
      
      pmanager->AddProcess( ms,                      -1, 1,1);
      pmanager->AddProcess( ionizationProcess,       -1, 2,2);
      pmanager->AddProcess( new G4eBremsstrahlung(), -1,-1,3);

    } else if (particleName == "e+") {
      //positron
      // Construct processes for positron

      //change for geant4.9.p01
      G4MultipleScattering* ms = new G4MultipleScattering();
      //ms->MscStepLimitation(false,0.02);
      ms->SetRangeFactor(0.02);

      //      ms->SetLateralDisplasmentFlag(false);
      
      G4eIonisation *ionizationProcess = new G4eIonisation();
      //      ionizationProcess->SetLinearLossLimit(1.0);
      
      pmanager->AddProcess( ms,                        -1, 1,1);
      pmanager->AddProcess( ionizationProcess,         -1, 2,2);
      pmanager->AddProcess( new G4eBremsstrahlung(),   -1,-1,3);
      pmanager->AddProcess( new G4eplusAnnihilation(),  0,-1,4);
  
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
      //muon  
      // Construct processes for muon+

      //change for geant4.9.0.p01
      G4MultipleScattering* ms = new G4MultipleScattering();
      //ms->MscStepLimitation(false,0.02);
      ms->SetRangeFactor(0.02);

     //      ms->SetLateralDisplasmentFlag(false);
      
      G4MuIonisation *ionizationProcess = new G4MuIonisation();
      //      ionizationProcess->SetLinearLossLimit(1.0);
      
      pmanager->AddProcess( ms,                       -1, 1,1);
      pmanager->AddProcess( ionizationProcess,        -1, 2,2);
      pmanager->AddProcess( new G4MuBremsstrahlung(), -1,-1,3);
      pmanager->AddProcess( new G4MuPairProduction(), -1,-1,4);
     
    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      // all others charged particles except geantino
      G4int AP=1;
      if (particle->GetParticleName() == "GenericIon") {
        //ostream& o = ErrMsg(warning);
	      std::cerr <<"*********************************************************************" <<std::endl;
	      std::cerr << "*** Disabling G4MultipleScattering process for particle " <<particle->GetParticleName() << std::endl;
	std::cerr <<"*********************************************************************" <<std::endl;
      } else {
  //change for Geant4.9.0.p01
	G4MultipleScattering* ms = new G4MultipleScattering();
  //ms->MscStepLimitation(false,0.02);
  ms->SetRangeFactor(0.02);

	//       	ms->SetLateralDisplasmentFlag(false);
      	pmanager->AddProcess( ms, -1,AP,AP);
      	AP++;
      }
      G4hIonisation *ionizationProcess = new G4hIonisation();
      //      ionizationProcess->SetLinearLossLimit(1.0); 
      
      pmanager->AddProcess( ionizationProcess, -1,AP,AP);
    }
  }
}

/*
void BgsPhysicsList::AddProcess( G4VProcess *process, 
        			 G4int ordAtRestDoIt,
        			 G4int ordAlongStepDoIt,
        			 G4int ordPostStepDoIt,
			         G4ProcessManager *manager,
				 const char *category,
		                 GVertex::Cause cause   )
{
  manager->AddProcess( process, ordAtRestDoIt, ordAlongStepDoIt, 
                     ordPostStepDoIt );
  physicsRegistrar->Register( process, manager, category, cause );
}


void BgsPhysicsList::AddDiscreteProcess( G4VProcess *process, 
			        	 G4ProcessManager *manager,
					 const char *category,
		                	 GVertex::Cause cause   )
{
  manager->AddDiscreteProcess( process );
  physicsRegistrar->Register( process, manager, category, cause );
}

				 
void BgsPhysicsList::AddElasticHadronProcess(G4HadronElasticProcess
*process, 
                                             G4LElastic *model,
                                             G4ProcessManager *manager, 
                                             const char *category,
                                             GVertex::Cause cause )
{
  process->RegisterMe(model);   // Register interaction model with process
  manager->AddDiscreteProcess(process);
  physicsRegistrar->Register(process,manager,category,cause);
}
*/


void 
BgsPhysicsList::SetStatusEmProcess()
{
}

// ************************** Parameterisation******************************
/*
void 
BgsPhysicsList::AddParameterisation()
{
  G4FastSimulationManagerProcess* theFastSimulationManagerProcess = 
    new G4FastSimulationManagerProcess();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    pmanager->AddDiscreteProcess( theFastSimulationManagerProcess );
  }
}
*/
// ************************** Generic Processes******************************

void 
BgsPhysicsList::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess( theDecayProcess );
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
//      physicsRegistrar->
//	Register( theDecayProcess, pmanager, "dcay", GVertex::decay );
    }
  }
/*
  if (control->GetLooperCut()>0) {

    // Set special process to kill loopers
    theLooperDeath = new BgsLooperDeath( control->GetLooperCut()*MeV );
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      if (theLooperDeath->IsApplicable(*particle)) { 
	G4ProcessManager* pmanager = particle->GetProcessManager();
	pmanager->AddProcess(theLooperDeath, -1, -1, 5);
        physicsRegistrar->
	  Register( theLooperDeath, pmanager, "loop", GVertex::looperDeath
);
      }
    }
    ErrMsg(warning) << "Loopers with pt < " << control->GetLooperCut() 
                    << " MeV will be killed" << endmsg;
  } 

  if (control->GetMaxNumberOfSteps()>0) {

    //
    // Set special process to kill runaway particles
    // Only needed if dE/dx is turned off!
    //
    // Do not abuse!
    //
    theStepDeath = new BgsChargedStepDeath( control->GetMaxNumberOfSteps()
);
    
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      if (theStepDeath->IsApplicable(*particle)) { 
	G4ProcessManager* pmanager = particle->GetProcessManager();
	pmanager->AddProcess(theStepDeath, -1, -1, 5);
        physicsRegistrar->
	  Register( theStepDeath, pmanager, "maxStep", GVertex::runAway );
      }
    }
    ErrMsg(warning) 
      << "\n  Charged particles will be killed if they take more than " 
      << control->GetMaxNumberOfSteps() << " steps.\n"
      << "  If you do not understand this message, you should be very
concerned.\n" 
      << "  If this message appears in production, you should be very
upset." << endmsg;
  }
 */ 
}

// ************************** Hadronic Processes******************************

void 
BgsPhysicsList::ConstructLeptHad()
{
  //
  // Gamma-nuclear process
  //

  // low energy part
  G4GammaNuclearReaction* lowEGammaModel = new G4GammaNuclearReaction();
  lowEGammaModel->SetMaxEnergy(3.5*GeV);
  G4PhotoNuclearProcess* thePhotoNuclearProcess = new
G4PhotoNuclearProcess();
  thePhotoNuclearProcess->RegisterMe(lowEGammaModel);

  // bias the cross section
  //
/*  double thePhotoNuclearBias = control->GetPhotoNuclearBias();
  if (thePhotoNuclearBias != 1.) {
    thePhotoNuclearProcess->BiasCrossSectionByFactor(thePhotoNuclearBias);

  // print out a warning if biasing
  //
    ErrMsg(warning) << "*** Biasing the photo-nuclear process by factor "
		    << thePhotoNuclearBias << endmsg; 
  }
*/
  // high energy part
  G4TheoFSGenerator* highEGammaModel = new G4TheoFSGenerator();
  G4GeneratorPrecompoundInterface* preComModel =
                         new G4GeneratorPrecompoundInterface();
  highEGammaModel->SetTransport(preComModel);

  G4QGSModel<G4GammaParticipants>* theStringModel =
                         new G4QGSModel<G4GammaParticipants>;
  G4QGSMFragmentation* fragModel = new G4QGSMFragmentation();
  G4ExcitedStringDecay* stringDecay =
                            new G4ExcitedStringDecay(fragModel);
  theStringModel->SetFragmentationModel(stringDecay);

  highEGammaModel->SetHighEnergyGenerator(theStringModel);
  highEGammaModel->SetMinEnergy(3.*GeV);
  highEGammaModel->SetMaxEnergy(20.*GeV);

  thePhotoNuclearProcess->RegisterMe(highEGammaModel);

  G4ProcessManager* gamMan = G4Gamma::Gamma()->GetProcessManager();

  gamMan->AddDiscreteProcess(thePhotoNuclearProcess);
  //
  // Electron-nuclear process
  //
  G4ElectroNuclearReaction* theElectronReaction =
                                   new G4ElectroNuclearReaction();
  G4ElectronNuclearProcess* theElectronNuclearProcess =
                                   new G4ElectronNuclearProcess();
  theElectronNuclearProcess->RegisterMe(theElectronReaction);

  G4ProcessManager* electronMan =
G4Electron::Electron()->GetProcessManager();

  electronMan->AddProcess(theElectronNuclearProcess, -1, -1, 4);

  // bias the cross section
  //
/*
  G4double theElectroNuclearBias = control->GetElectroNuclearBias();
  if (theElectroNuclearBias != 1.) {

theElectronNuclearProcess->BiasCrossSectionByFactor(theElectroNuclearBias);

  // print out a warning if biasing
  //
    ErrMsg(warning) << "*** Biasing the electron-nuclear process by factor "
		    << theElectroNuclearBias << endmsg; 
  }
*/
  //
  // Positron-nuclear process
  //
  G4PositronNuclearProcess* thePositronNuclearProcess =
                                   new G4PositronNuclearProcess();
  thePositronNuclearProcess->RegisterMe(theElectronReaction);

  G4ProcessManager* positronMan =
G4Positron::Positron()->GetProcessManager();
  positronMan->AddProcess(thePositronNuclearProcess, -1, -1, 5);

  // bias the cross section
  //
/*
  if (theElectroNuclearBias != 1.) { 

thePositronNuclearProcess->BiasCrossSectionByFactor(theElectroNuclearBias);
    ErrMsg(warning) << "*** Biasing the positron-nuclear process by factor "
		    << theElectroNuclearBias << endmsg; 
  }
  */
}

void 
BgsPhysicsList::ConstructHad()
{
  // One process handles hadronic elastic processes for all hadrons.
  // However hadronic inelastic processes are unique to each hadron.

  // For pi+ and pi- only, substitute pi-Nuclear cross sections 
  G4PiNuclearCrossSection* piNucCS = new G4PiNuclearCrossSection();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    // *******
    // * pi+ *
    // *******
    if (particleName == "pi+") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process
    
      G4PionPlusInelasticProcess *inel_process = new
G4PionPlusInelasticProcess();
      inel_process->AddDataSet(piNucCS);
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * pi- *
    // *******
    } else if (particleName == "pi-") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4PionMinusInelasticProcess *inel_process = new
G4PionMinusInelasticProcess();
      inel_process->AddDataSet(piNucCS);
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * K+  *
    // *******
    } else if (particleName == "kaon+") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4KaonPlusInelasticProcess* inel_process = new
G4KaonPlusInelasticProcess();
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * K-  *
    // *******
    } else if (particleName == "kaon-") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4KaonMinusInelasticProcess* inel_process = new
G4KaonMinusInelasticProcess();
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * K0L *
    // *******
    } else if (particleName == "kaon0L") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4KaonZeroLInelasticProcess* inel_process = new
G4KaonZeroLInelasticProcess();
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * K0S *
    // *******
    } else if (particleName == "kaon0S") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4KaonZeroSInelasticProcess* inel_process = 
      new G4KaonZeroSInelasticProcess();
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * p   *
    // *******
    } else if (particleName == "proton") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);
  
      // Inelastic process

      G4ProtonInelasticProcess *inel_process = new
G4ProtonInelasticProcess();
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *********
    // * p-bar *
    // *********
    } else if (particleName == "anti_proton") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4AntiProtonInelasticProcess *inel_process = 
         new G4AntiProtonInelasticProcess();
      G4LEAntiProtonInelastic *inel_model = new G4LEAntiProtonInelastic();
      inel_process->RegisterMe(inel_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // *******
    // * n   *
    // *******
    } else if (particleName == "neutron") {

      //if (control->UseHPNeutrons()) {
      if(1){
        G4cout << "High precision neutron models chosen" << G4endl;

        putenv("G4NEUTRONHPDATA=/afs/ihep.ac.cn/bes3/offline/sw/packages/geant4/4.9.0/slc4_ia32_gcc346/geant4.9.0.p01/data/G4NDL3.11/");

        // Elastic process
        G4HadronElasticProcess* el_process = new G4HadronElasticProcess();

        // High precision model and data below 20 MeV
        G4NeutronHPElastic* hpel_model = new G4NeutronHPElastic();
        G4NeutronHPElasticData* el_data = new G4NeutronHPElasticData();
        el_process->AddDataSet(el_data);
        el_process->RegisterMe(hpel_model);

        // LEP model above 20 MeV
        G4LElastic* el_model = new G4LElastic();
        el_model->SetMinEnergy(19.9*MeV);
        el_process->RegisterMe(el_model);
      
        pmanager->AddDiscreteProcess(el_process);
        //physicsRegistrar->Register(el_process,pmanager,"hade",
        //                         GVertex::hadronElastic);

        // Inelastic process
        G4NeutronInelasticProcess* inel_process = 
                                       new G4NeutronInelasticProcess();

        // High precision model and data below 20 MeV
        G4NeutronHPInelastic* hpinel_model = new G4NeutronHPInelastic();
        G4NeutronHPInelasticData* hpinel_data = new
G4NeutronHPInelasticData();
        inel_process->AddDataSet(hpinel_data);
        inel_process->RegisterMe(hpinel_model);

        // Bertini model above 20 MeV
        G4CascadeInterface* neutron_bertini = new G4CascadeInterface;
        neutron_bertini->SetMinEnergy(19.9*MeV);
        inel_process->RegisterMe(neutron_bertini);

        pmanager->AddDiscreteProcess(inel_process);
        //physicsRegistrar->Register(inel_process,pmanager,"hadi",
        //                         GVertex::hadronInelastic);

        // Capture process
        G4HadronCaptureProcess* cap_process = new G4HadronCaptureProcess();

        // High precision model and data below 20 MeV
        G4NeutronHPCapture* hpcap_model = new G4NeutronHPCapture();
        G4NeutronHPCaptureData* hpcap_data = new G4NeutronHPCaptureData();
        cap_process->AddDataSet(hpcap_data);
        cap_process->RegisterMe(hpcap_model);

        // LEP model above 20 MeV - default cross sections are used here
	// hence no need to explicitly invoke AddDataSet method
        G4LCapture* cap_model = new G4LCapture();
        cap_model->SetMinEnergy(19.9*MeV);
        cap_process->RegisterMe(cap_model);

        pmanager->AddDiscreteProcess(cap_process);
	// Note: need to update GVertex to include hadronCapture
//       physicsRegistrar->Register(cap_process,pmanager,"hadi",
//                                 GVertex::hadronInelastic);
      
        // Fission process
        G4HadronFissionProcess* fis_process = new G4HadronFissionProcess();

        // High precision model and data below 20 MeV
        G4NeutronHPFission* hpfis_model = new G4NeutronHPFission();
        G4NeutronHPFissionData* hpfis_data = new G4NeutronHPFissionData();
        fis_process->AddDataSet(hpfis_data);
        fis_process->RegisterMe(hpfis_model);

        // LEP model above 20 MeV - default cross sections are used here
	// hence no need to explicitly invoke AddDataSet method
        G4LFission* fis_model = new G4LFission();
        fis_model->SetMinEnergy(19.9*MeV);
        fis_process->RegisterMe(fis_model);

        pmanager->AddDiscreteProcess(fis_process);
	// Note: need to update GVertex to include hadronFission
//        physicsRegistrar->Register(fis_process,pmanager,"hadi",
//                                 GVertex::hadronInelastic);
      
      } else {

        // Elastic process
	G4HadronElasticProcess *process = new G4HadronElasticProcess();
	G4LElastic *model =  new G4LElastic();
	process->RegisterMe(model);
	pmanager->AddDiscreteProcess(process);

        // Inelastic process
        G4NeutronInelasticProcess *inel_process = 
                                     new G4NeutronInelasticProcess();
        inel_process->RegisterMe(bertini_model);
        pmanager->AddDiscreteProcess(inel_process);
//        physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                                 GVertex::hadronInelastic);
      }
    // *********
    // * n-bar *
    // *********
    } else if (particleName == "anti_neutron") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4AntiNeutronInelasticProcess *inel_process = 
         new G4AntiNeutronInelasticProcess();
      G4LEAntiNeutronInelastic *inel_model = new G4LEAntiNeutronInelastic();
      inel_process->RegisterMe(inel_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // **********
    // * lambda *
    // **********
    } else if (particleName == "lambda") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4LambdaInelasticProcess* inel_process = new
G4LambdaInelasticProcess();
      inel_process->RegisterMe(bertini_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // **************
    // * lambda-bar *
    // **************
    } else if (particleName == "anti_lambda") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4AntiLambdaInelasticProcess *inel_process = 
         new G4AntiLambdaInelasticProcess();
      G4LEAntiLambdaInelastic *inel_model = new G4LEAntiLambdaInelastic();
      inel_process->RegisterMe(inel_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // **************
    // * deuteron   *
    // **************
    } else if (particleName == "deuteron") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4DeuteronInelasticProcess *inel_process = 
         new G4DeuteronInelasticProcess();
      G4LEDeuteronInelastic *inel_model = new G4LEDeuteronInelastic();
      inel_process->RegisterMe(inel_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // **************
    // * triton     *
    // **************
    } else if (particleName == "triton") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4TritonInelasticProcess *inel_process = 
         new G4TritonInelasticProcess();
      G4LETritonInelastic *inel_model = new G4LETritonInelastic();
      inel_process->RegisterMe(inel_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    // **************
    // * alpha      *
    // **************
    } else if (particleName == "alpha") {

      // Elastic process
      G4HadronElasticProcess *process = new G4HadronElasticProcess();
      G4LElastic *model =  new G4LElastic();
      process->RegisterMe(model);
      pmanager->AddDiscreteProcess(process);

      // Inelastic process

      G4AlphaInelasticProcess *inel_process = new G4AlphaInelasticProcess();
      G4LEAlphaInelastic *inel_model = new G4LEAlphaInelastic();
      inel_process->RegisterMe(inel_model);
      pmanager->AddDiscreteProcess(inel_process);
//      physicsRegistrar->Register(inel_process,pmanager,"hadi",
//                               GVertex::hadronInelastic);
    }
  }  // while 
}


//
// ConstructNeutrinoGenocide
//
void BgsPhysicsList::ConstructNeutrinoGenocide()
{
  BgsGenocide *genocide = new BgsGenocide();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "nu_e"   ||
        particleName == "nu_mu"  ||
	particleName == "nu_tau" ||
        particleName == "anti_nu_e"   ||
        particleName == "anti_nu_mu"  ||
	particleName == "anti_nu_tau" ||

	// temporary fix for K0, K0bar until CHIPS photonuclear model isfixed

	particleName == "kaon0"  ||
	particleName == "anti_kaon0"    ) {
	
	pmanager->AddProcess(genocide, -1, -1, 5);
//        physicsRegistrar->Register( genocide, pmanager, "neutrinoDeath", 
//                                  GVertex::neutrino );
    }
  }
}


//
// ConstructIonFix
//
void BgsPhysicsList::ConstructIonFix()
{
  BgsGenocide *genocide = new
//BgsGentleGenocide(control->GetIonEnergyCut()*MeV,
//                                              60);
  BgsGentleGenocide(0.01*MeV,
                                              60);		  

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if ( particleName == "triton" ||
         particleName == "alpha"  ||
	 particleName == "proton" ||
	 particleName == "deuteron"  ) {
	
	pmanager->AddProcess(genocide, -1, -1, 5);
//        physicsRegistrar->Register( genocide, pmanager, "ionFix", 
//                                  GVertex::minimumEnergy );
    }
  }
}


//
// ConstructNeutronFix
//
void BgsPhysicsList::ConstructNeutronFix()
{
  BgsGenocide *genocide = new
BgsGentleGenocide(0.01*MeV,0);

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if ( particleName == "neutron"  ) {
	
	pmanager->AddProcess(genocide, -1, -1, 1);
//        physicsRegistrar->Register( genocide, pmanager, "neutronFix", 
//                                  GVertex::minimumEnergy );
    }
  }
}


// ****************************** Cuts***************************************


void BgsPhysicsList::SetCuts()
{
  // Set default cuts, all volumes 

  SetDefaultCutValue(0.7*mm);
  SetCutsWithDefault();

  // Enable print out of cuts after tables are built
  // This is now done in BgsRunAction
  //
  //   	if (verboseLevel > 1) DumpCutValuesTable(); 
}
