//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Rewrite G4BesBosonConstructor.hh, remove optical photon
//Author: Liuhm 
//Created: 2003/5/19
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesBosonConstructor.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
// Bosons
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4Gamma.hh"

BesBosonConstructor::BesBosonConstructor()
{
}

BesBosonConstructor::~BesBosonConstructor()
{
}


void BesBosonConstructor::ConstructParticle()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  // gamma
  G4Gamma::GammaDefinition();
}










