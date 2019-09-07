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
// $Id: G4ParticleChange.hh,v 1.13 2007/03/11 07:19:06 kurasige Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 
// ------------------------------------------------------------
//	GEANT 4 class header file 
//
// 
// ------------------------------------------------------------
//   Implemented for the new scheme                 23 Mar. 1998  H.Kurahige
//
// Class Description
//  This class is a concrete class for ParticleChange which
//  has all functionality in old scheme.
//-
//  This class contains the results after invocation of a physics process.
//  This includes final states of parent particle (momentum, energy,
//  etc) and secondary particles generated by the interaction.
//  The tracking assumes that all the values of energy and
//  momentum are in global reference system, therefore all the
//  needed Lorentz transformations must have been already Done
//  when filling the data-members of this class.
//-
//  ---------------------------------------------------------------
//  IMPORTANT NOTE: Although the name of the class and methods are
//   "Change", what it stores (and returns in get) are the "FINAL" 
//   values of the Position, Momentum, etc.
// 
//  ------------------------------------------------------------
//  modify AddSecondary methods for "GoodForTracking" flag 
//                                                 8 June 1998 H.Kurashige
//   Add Track weight                              12 Nov. 1998  H.Kurashige
//   Add Get/SetMomentumDirectionChange             6 Feb. 1999 H.Kurashige 
//   Add Get/SetDynamicMass                        5 Oct. 1999 H.Kurashige
//   Add Get/SetDynamicCharge                       5 Oct. 1999 H.Kurashige
//   Rename SetXXX methods to ProposeXXX   DynamicCharge  Oct. 2005 H.Kurashige
//   Add get/ProposeMagneticMoment                  Mar 2007 H.Kurashige
//   Fix treatment of weight                        Mar 2007 H.Kurashige
// -------------------------------------------------------------
 
#ifndef G4ParticleChange_h
#define G4ParticleChange_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ThreeVector.hh"
#include "G4ThreeVector.hh"
class G4DynamicParticle;
#include "G4VParticleChange.hh"

class G4ParticleChange: public G4VParticleChange
{ 
  public:
    // default constructor
    G4ParticleChange();
//    G4ParticleChange(G4bool useEB);

    // destructor
    virtual ~G4ParticleChange();

  protected:
    // hide copy constructor and assignment operaor as protected
    G4ParticleChange(const G4ParticleChange &right);
    G4ParticleChange & operator=(const G4ParticleChange &right);

  public:
    // equal/unequal operator
    G4bool operator==(const G4ParticleChange &right) const;
    G4bool operator!=(const G4ParticleChange &right) const;

  public: // with description
    // ----------------------------------------------------
    // --- the following methods are for updating G4Step -----   
    // Return the pointer to the G4Step after updating the Step information
    // by using final state information of the track given by a physics
    // process    
    virtual G4Step* UpdateStepForAlongStep(G4Step* Step);
    // A physics process gives the final state of the particle 
    // relative to the initial state at the beginning of the Step,
    // i.e., based on information of G4Track (or equivalently 
    // the PreStepPoint)
    // In this method, the differences (delta) between these two states 
    // are calculated, and are accumulated in PostStepPoint.
    // Take note that the return type of GetMomentumChange is a
    // pointer to G4ParticleMometum. Also it is a normalized 
    // momentum vector.
    virtual G4Step* UpdateStepForAtRest(G4Step* Step);
    virtual G4Step* UpdateStepForPostStep(G4Step* Step);
    // A physics process gives the final state of the particle 
    // based on information of G4Track (or equivalently the PreStepPoint)
 
    virtual void Initialize(const G4Track&);
    // Initialize all propoerties by using G4Track information

  protected: // with description
    G4Step* UpdateStepInfo(G4Step* Step);
    //  Update the G4Step specific attributes 
    //  (i.e. SteppingControl, LocalEnergyDeposit, and TrueStepLength)

  public:     // with description
    
    // ----------------------------------------------------
    //--- methods to keep information of the final state--
    //  IMPORTANT NOTE: 
    //  These ProposeXXX methods stores (and returns in GetXXX methods) 
    //   the "FINAL" values of the Position, Momentum, etc.

    const G4ThreeVector* GetMomentumDirection() const;
    void ProposeMomentumDirection(G4double Px, G4double Py, G4double Pz);
    void ProposeMomentumDirection(const G4ThreeVector& Pfinal);
    // Get/Propose the MomentumDirection vector: it is the final momentum direction.

    const G4ThreeVector* GetPolarization() const;
    void  ProposePolarization(G4double Px, G4double Py, G4double Pz);
    void  ProposePolarization(const G4ThreeVector& finalPoralization);
    // Get/Propose the final Polarization vector.

    G4double GetEnergy() const;
    void ProposeEnergy(G4double finalEnergy);
    // Get/Propose the final kinetic energy of the current particle.

    G4double GetProperTime() const;
    void ProposeProperTime(G4double finalProperTime);
    //  Get/Propose th final eProperTime 

    const G4ThreeVector* GetPosition() const;
    void ProposePosition(G4double x, G4double y, G4double z);
    void ProposePosition(const G4ThreeVector& finalPosition);
    //  Get/Propose the final position of the current particle.

    G4double GetGlobalTime() const;
    void ProposeGlobalTime(G4double finalGlobalTime);
    //  Get/Propose the final GlobalTime
 
    G4double GetMass() const;
    void ProposeMass(G4double finalMass);
    //   Get/Propose the final dynamical Mass in G4DynamicParticle

    G4double GetCharge() const;
    void ProposeCharge(G4double finalCharge);
    //   Get/Propose the final dynamical Charge in G4DynamicParticle

    G4double GetMagneticMoment() const;
    void ProposeMagneticMoment(G4double finalMagneticMoment);
    //   Get/Propose the final MagneticMoment in G4DynamicParticle
  
    G4double GetWeight() const;
    void ProposeWeight(G4double finalWeight);
    //   Get/Propose the final Weight of the parent particle

    //  -- Utility functions --
    G4ThreeVector GetGlobalPosition(const G4ThreeVector& displacement) const;
    //  Convert the position displacement to the global position.

    G4double GetGlobalTime(G4double timeDelay) const;
    //  Convert the time delay to the global time.

    G4ThreeVector CalcMomentum(G4double           energy,
			       G4ThreeVector direction,
			       G4double           mass      ) const;
    //  Calculate momentum by using Energy, Momentum Direction, and Mass 
    // ----------------------------------------------------


    // ----------------------------------------------------
    // --- methods for adding secondaries
    void AddSecondary(G4Track* aSecondary);
    //  Add a secondary particle to theListOfSecondaries.

    void AddSecondary(G4DynamicParticle* aSecondary,
		      G4bool             IsGoodForTracking = false );
    //  Add a secondary particle to theListOfSecondaries.
    //  position and time are same as thePositionChange and theTimeChange 

    void AddSecondary(G4DynamicParticle* aSecondary, 
		      G4ThreeVector      position,
		      G4bool             IsGoodForTracking = false   );
    //  Add a secondary particle to theListOfSecondaries.
    //  global time are same as theTimeChange and theTimeChange 

    void AddSecondary(G4DynamicParticle* aSecondary, 
		      G4double           time,
		      G4bool             IsGoodForTracking = false );
    //  Add a secondary particle to theListOfSecondaries.
    //  position and are same as thePositionChange  
    // ----------------------------------------------------

  public:
    virtual void DumpInfo() const;

  protected:
    G4ThreeVector theMomentumDirectionChange;
    //  It is the vector containing the final momentum direction
    //  after the invoked process. The application of the change
    //  of the momentum direction of the particle is not Done here.
    //  The responsibility to apply the change is up the entity
    //  which invoked the process.

    G4ThreeVector thePolarizationChange;
    //  The changed (final) polarization of a given track
    
    G4double theEnergyChange;
    //  The final kinetic energy of the current track
    
    G4ThreeVector thePositionChange;
    //  The changed (final) position of a given track
    
    G4double theTimeChange;
    //  The changed (final) global time of a given track
    
    G4double theProperTimeChange;
    //  The changed (final) proper time of a given track
    
    // Obsolete   Mar 2007
    // G4double theWeightChange;
    //  The Changed (final) weight of a given track

    G4double theMassChange;
    //  The Changed (final) mass of a given track

    G4double theChargeChange;
    //  The Changed (final) charge of a given track
 
    G4double theMagneticMomentChange;
    //  The Changed (final) MagneticMoment of a given track
  
    const G4Track* theCurrentTrack;
    
  public:
    // for Debug 
    virtual G4bool CheckIt(const G4Track&);
};

#include "G4ParticleChange.icc"

#endif
















