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
#ifndef G4COLLISION_OUTPUT_HH
#define G4COLLISION_OUTPUT_HH

#include <iostream>

#include "G4InuclElementaryParticle.hh"
#include "G4InuclNuclei.hh"

#include <algorithm>
#include <vector>

class G4CollisionOutput {

public:

  G4CollisionOutput();

  G4CollisionOutput& operator=(const G4CollisionOutput& right);

  void reset() {
    nucleiFragments.resize(0);
    outgoingParticles.resize(0);
  
  };

  void addOutgoingParticle(const G4InuclElementaryParticle& particle) {
    outgoingParticles.push_back(particle);
  };

  void addOutgoingParticles(const std::vector<G4InuclElementaryParticle>& particles) {
    for(G4int i = 0; i < G4int(particles.size()); i++)
      outgoingParticles.push_back(particles[i]);
  };

  void addTargetFragment(const G4InuclNuclei& nuclei) {
    nucleiFragments.push_back(nuclei);
  };

  void addTargetFragments(const std::vector<G4InuclNuclei>& nuclea) {
    for(G4int i = 0; i < G4int(nuclea.size()); i++)
      nucleiFragments.push_back(nuclea[i]);
  };

  const std::vector<G4InuclElementaryParticle>& getOutgoingParticles() const {
    return outgoingParticles;
  };

  G4int numberOfNucleiFragments() const { 
    return nucleiFragments.size(); 
  };
 
  const std::vector<G4InuclNuclei>& getNucleiFragments() const {
    return nucleiFragments;
  };

  G4CascadeMomentum getTotalOutputMomentum() const {
    G4CascadeMomentum tot_mom;
    double eex_r = 0.0;
    G4int i(0);
    for(i = 0; i < G4int(outgoingParticles.size()); i++) {
      const G4CascadeMomentum& mom = outgoingParticles[i].getMomentum();
      for(G4int j = 0; j < 4; j++) tot_mom[j] += mom[j];
    };
    for(i = 0; i < G4int(nucleiFragments.size()); i++) {
      const G4CascadeMomentum& mom = nucleiFragments[i].getMomentum();
      for(G4int j = 0; j < 4; j++) tot_mom[j] += mom[j];
      eex_r += 0.001 * nucleiFragments[i].getExitationEnergy();
    };
    tot_mom[0] += eex_r;
    return tot_mom;
  };

  void printCollisionOutput() const {
    G4cout << " Output: " << G4endl  
	   << " Outgoing Particles: " << outgoingParticles.size() << G4endl;
    G4int i(0);
    for(i = 0; i < G4int(outgoingParticles.size()); i++) {
      outgoingParticles[i].printParticle(); 
    };
    G4cout << " Nuclei fragments: " << nucleiFragments.size() << G4endl;      
    for(i = 0; i < G4int(nucleiFragments.size()); i++) {
      nucleiFragments[i].printParticle(); 
    };
  };

  void trivialise(G4InuclParticle* bullet, 
		  G4InuclParticle* target) {
    if(G4InuclNuclei* nuclei_target = dynamic_cast<G4InuclNuclei*>(target)) {     
      nucleiFragments.push_back(*nuclei_target);
    }
    else {
      G4InuclElementaryParticle* particle =
	dynamic_cast<G4InuclElementaryParticle*>(target);
      outgoingParticles.push_back(*particle);
    }; 
    if(G4InuclNuclei* nuclei_bullet = dynamic_cast<G4InuclNuclei*>(bullet)) {     
      nucleiFragments.push_back(*nuclei_bullet);
    }
    else {
      G4InuclElementaryParticle* particle =
	dynamic_cast<G4InuclElementaryParticle*>(bullet);
      outgoingParticles.push_back(*particle);
    }; 
  };

  void setOnShell(G4InuclParticle* bullet, 
		  G4InuclParticle* target);

  void setRemainingExitationEnergy() { 
    eex_rest = 0.0;
    for(G4int i = 0; i < G4int(nucleiFragments.size()); i++) 
      eex_rest += 0.001 * nucleiFragments[i].getExitationEnergy();
  };

  double getRemainingExitationEnergy() const { 
    return eex_rest; 
  };

  G4bool acceptable() const { 
    return on_shell; 
  };

private: 

G4int verboseLevel;
  std::vector<G4InuclElementaryParticle> outgoingParticles;

  std::vector<G4InuclNuclei> nucleiFragments;

  G4double eex_rest;

  std::pair<std::pair<G4int, G4int>, G4int> selectPairToTune(G4double de) const; 

  G4bool on_shell;

};        

#endif // G4COLLISION_OUTPUT_HH 

