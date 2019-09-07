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
// $Id: G4EvaporationInuclCollider.hh,v 1.2 2008/06/29 23:56:03 dennis Exp $
#ifndef G4EVAPORATIONINUCL_COLLIDER_HH
#define G4EVAPORATIONINUCL_COLLIDER_HH
 
#include "G4Collider.hh"

#include "G4EquilibriumEvaporator.hh"
#include "G4Fissioner.hh"
#include "G4BigBanger.hh"

#include "G4ElementaryParticleCollider.hh"
#include "G4InteractionCase.hh"
#include "G4InuclNuclei.hh"
#include "G4InuclSpecialFunctions.hh"
#include "G4Analyser.hh"

using namespace G4InuclSpecialFunctions;

class G4EvaporationInuclCollider {

public:

  G4EvaporationInuclCollider();

  G4EvaporationInuclCollider(G4EquilibriumEvaporator* eqevaporator, G4Fissioner* fissioner, G4BigBanger* bigbanger) {

    setEquilibriumEvaporator(eqevaporator, fissioner, bigbanger);
  };

  void setEquilibriumEvaporator(G4EquilibriumEvaporator* eqevaporator, G4Fissioner* fissioner, G4BigBanger* bigbanger) {
    theEquilibriumEvaporator = eqevaporator;
    theEquilibriumEvaporator->setFissioner(fissioner);   
    theEquilibriumEvaporator->setBigBanger(bigbanger);   
  };

  void setBigBanger(G4BigBanger* bigbanger) {

    theBigBanger = bigbanger;   
  };
 
  G4CollisionOutput collide(G4InuclParticle* bullet, G4InuclParticle* target);
  
private: 

  G4int verboseLevel;

  G4bool inelasticInteractionPossible(G4InuclParticle* bullet,
				      G4InuclParticle* target, 
				      G4double ekin) const;

  G4InteractionCase bulletTargetSetter(G4InuclParticle* bullet,
				       G4InuclParticle* target) const; 

  G4bool explosion(G4InuclNuclei* target) const; 
       
  G4EquilibriumEvaporator* theEquilibriumEvaporator;
  G4BigBanger* theBigBanger;

};        

#endif // G4EVAPORATIONINUCL_COLLIDER_HH 


