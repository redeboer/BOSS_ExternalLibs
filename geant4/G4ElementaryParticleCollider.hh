//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
#ifndef G4ELEMENTARY_PARTICLE_COLLIDER_HH
#define G4ELEMENTARY_PARTICLE_COLLIDER_HH

#include "G4Collider.hh"
#include "G4InuclElementaryParticle.hh"
#include "G4InuclSpecialFunctions.hh"
#include "G4CascadSpecialFunctions.hh"
#include "G4LorentzConvertor.hh"
#include "G4NucleonSampler.hh"
#include "G4PionSampler.hh"

 
using namespace G4InuclSpecialFunctions;
using namespace G4CascadSpecialFunctions;


class G4ElementaryParticleCollider {

public:

  G4ElementaryParticleCollider();

  G4CollisionOutput collide(G4InuclParticle* bullet,
			    G4InuclParticle* target);

private:

  G4int verboseLevel;

  void initializeArrays();

  G4int generateMultiplicity(G4int is, G4double ekin) const;

  void collide(G4InuclElementaryParticle* bullet,
	       G4InuclElementaryParticle* target,
	       G4CollisionOutput& output);

      
  std::vector<G4InuclElementaryParticle> 
  generateSCMfinalState(G4double ekin, G4double etot_scm, G4double pscm,	     
		        G4InuclElementaryParticle* particle1,
			G4InuclElementaryParticle* particle2, 
			G4LorentzConvertor* toSCM) const; 


  std::vector<G4double>
  generateMomModules(const std::vector<G4int>& kinds, G4int mult,
		     G4int is, G4double ekin, G4double etot_cm) const; 


  G4CascadeMomentum
  particleSCMmomentumFor2to2(G4int is, G4int kw, G4double ekin,
			     G4double pscm) const; 


  G4int getElasticCase(G4int is, G4int kw, G4double ekin) const;


  std::vector<G4int>
  generateStrangeChannelPartTypes(G4int is, G4int mult, 
				  G4double ekin) const;


  G4double
  getMomModuleFor2toMany(G4int is, G4int mult, G4int knd, 
			 G4double ekin) const; 


  G4bool satisfyTriangle(const std::vector<G4double>& modules) const; 
	
  G4CascadeMomentum
  particleSCMmomentumFor2to3(G4int is, G4int knd, G4double ekin, 
			     G4double pmod) const; 


  std::pair<G4double, G4double> 
  adjustIntervalForElastic(G4double ekin, G4double ak, G4double ae,
                           G4int k, G4int l, const std::vector<G4double>& ssv, 
			   G4double st) const;
 
  std::vector<G4InuclElementaryParticle> 
  generateSCMpionAbsorption(G4double etot_scm,
			    G4InuclElementaryParticle* particle1,
			    G4InuclElementaryParticle* particle2) const; 

  G4NucleonSampler nucSampler;
  G4PionSampler piSampler;

  // Parameter arrays

  G4double rmn[14][10][2];    
  G4double ang[4][4][13];
  G4double abn[4][4][4];

};

#endif // G4ELEMENTARY_PARTICLE_COLLIDER_HH


