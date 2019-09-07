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
//
// $Id: G4ParticleChangeForGamma.hh,v 1.9 2008/01/11 19:57:12 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//
// ------------------------------------------------------------
//	GEANT 4 class header file
//
//
// ------------------------------------------------------------
// 15 April 2005 V.Ivanchenko for gamma EM processes
//
// Modified:
// 30.05.05 : add   UpdateStepForAtRest (V.Ivanchenko)
// 04.12.05 : apply UpdateStepForPostStep in any case (mma) 
// 26.08.06 : Add->Set polarization; 
//            add const method to access track; 
//            add weight modification (V.Ivanchenko) 
//
// ------------------------------------------------------------
//
//  Class Description
//  This class is a concrete class for ParticleChange for gamma processes
//
#ifndef G4ParticleChangeForGamma_h
#define G4ParticleChangeForGamma_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VParticleChange.hh"

class G4DynamicParticle;

class G4ParticleChangeForGamma: public G4VParticleChange
{
public:
  // default constructor
  G4ParticleChangeForGamma();

  // destructor
  virtual ~G4ParticleChangeForGamma();

  // with description
  // ----------------------------------------------------
  // --- the following methods are for updating G4Step -----

  G4Step* UpdateStepForAtRest(G4Step* pStep);
  G4Step* UpdateStepForPostStep(G4Step* Step);
  // A physics process gives the final state of the particle
  // based on information of G4Track

  void InitializeForPostStep(const G4Track&);
  //Initialize all propoerties by using G4Track information

  void AddSecondary(G4DynamicParticle* aParticle);
  // Add next secondary

  G4double GetProposedKineticEnergy() const;
  void SetProposedKineticEnergy(G4double proposedKinEnergy);
  // Get/Set the final kinetic energy of the current particle.

  const G4ThreeVector& GetProposedMomentumDirection() const;
  void ProposeMomentumDirection(G4double Px, G4double Py, G4double Pz);
  void ProposeMomentumDirection(const G4ThreeVector& Pfinal);
  // Get/Propose the MomentumDirection vector: it is the final momentum direction.

  const G4ThreeVector& GetProposedPolarization() const;
  void ProposePolarization(const G4ThreeVector& dir);
  void ProposePolarization(G4double Px, G4double Py, G4double Pz);

  const G4Track* GetCurrentTrack() const;

  virtual void DumpInfo() const;

  // for Debug
  virtual G4bool CheckIt(const G4Track&);

protected:
  // hide copy constructor and assignment operaor as protected
  G4ParticleChangeForGamma(const G4ParticleChangeForGamma &right);
  G4ParticleChangeForGamma & operator=(const G4ParticleChangeForGamma &right);

private:

  const G4Track* currentTrack;
  // The pointer to G4Track

  G4double proposedKinEnergy;
  //  The final kinetic energy of the current particle.

  G4ThreeVector proposedMomentumDirection;
  //  The final momentum direction of the current particle.

  G4ThreeVector proposedPolarization;
  //  The final polarization of the current particle.
};

// ------------------------------------------------------------

inline G4double G4ParticleChangeForGamma::GetProposedKineticEnergy() const
{
  return proposedKinEnergy;
}

inline void G4ParticleChangeForGamma::SetProposedKineticEnergy(G4double energy)
{
  proposedKinEnergy = energy;
}

inline
 const G4ThreeVector& G4ParticleChangeForGamma::GetProposedMomentumDirection() const
{
  return proposedMomentumDirection;
}

inline
 void G4ParticleChangeForGamma::ProposeMomentumDirection(const G4ThreeVector& dir)
{
  proposedMomentumDirection = dir;
}

inline
 void G4ParticleChangeForGamma::ProposeMomentumDirection(G4double Px, G4double Py, G4double Pz)
{
  proposedMomentumDirection.setX(Px);
  proposedMomentumDirection.setY(Py);
  proposedMomentumDirection.setZ(Pz);
}

inline const G4Track* G4ParticleChangeForGamma::GetCurrentTrack() const
{
  return currentTrack;
}

inline
 const G4ThreeVector& G4ParticleChangeForGamma::GetProposedPolarization() const
{
  return proposedPolarization;
}

inline
 void G4ParticleChangeForGamma::ProposePolarization(const G4ThreeVector& dir)
{
  proposedPolarization = dir;
}

inline
 void G4ParticleChangeForGamma::ProposePolarization(G4double Px, G4double Py, G4double Pz)
{
  proposedPolarization.setX(Px);
  proposedPolarization.setY(Py);
  proposedPolarization.setZ(Pz);
}

inline void G4ParticleChangeForGamma::InitializeForPostStep(const G4Track& track)
{
  theStatusChange = track.GetTrackStatus();
  theLocalEnergyDeposit = 0.0;
  theNonIonizingEnergyDeposit = 0.0;
  InitializeSecondaries(track);
  theParentWeight = track.GetWeight();
  proposedKinEnergy = track.GetKineticEnergy();
  proposedMomentumDirection = track.GetMomentumDirection();
  proposedPolarization = track.GetPolarization();
  currentTrack = &track;
}

//----------------------------------------------------------------
// method for updating G4Step
//

inline G4Step* G4ParticleChangeForGamma::UpdateStepForAtRest(G4Step* pStep)
{
  pStep->AddTotalEnergyDeposit( theLocalEnergyDeposit );
  pStep->SetStepLength( 0.0 );
  if (!fSetParentWeightByProcess)
    pStep->GetPostStepPoint()->SetWeight( theParentWeight );
  return pStep;
}

inline G4Step* G4ParticleChangeForGamma::UpdateStepForPostStep(G4Step* pStep)
{
  G4StepPoint* pPostStepPoint = pStep->GetPostStepPoint();
  pPostStepPoint->SetKineticEnergy( proposedKinEnergy );
  pPostStepPoint->SetMomentumDirection( proposedMomentumDirection );
  pPostStepPoint->SetPolarization( proposedPolarization );

  // update weight
  // this feature is commented out, it should be overwritten in case
  // if energy loss processes will use biasing
  if (!fSetParentWeightByProcess)
    pPostStepPoint->SetWeight( theParentWeight );
  
  pStep->AddTotalEnergyDeposit( theLocalEnergyDeposit );
  pStep->AddNonIonizingEnergyDeposit( theNonIonizingEnergyDeposit );
  return pStep;
}

inline void G4ParticleChangeForGamma::AddSecondary(G4DynamicParticle* aParticle)
{
  //  create track
  G4Track* aTrack = new G4Track(aParticle, currentTrack->GetGlobalTime(),
                                           currentTrack->GetPosition());

  //   Touchable handle is copied to keep the pointer
  aTrack->SetTouchableHandle(currentTrack->GetTouchableHandle());

  //  add a secondary
  G4VParticleChange::AddSecondary(aTrack);
}

#endif

