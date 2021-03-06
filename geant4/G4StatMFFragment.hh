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
// $Id: G4StatMFFragment.hh,v 1.3 2006/06/29 20:24:07 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara

#ifndef G4StatMFFragment_h
#define G4StatMFFragment_h 1

#include "G4StatMFParameters.hh"
#include "G4ThreeVector.hh"
#include "G4Fragment.hh"

class G4StatMFFragment {

public:
    // Constructor
    G4StatMFFragment(const G4int anA, const G4int aZ) :
	theA(anA),theZ(aZ),
	_position(0.0,0.0,0.0),
	_momentum(0.0,0.0,0.0)
	{}


    // Destructor
    virtual ~G4StatMFFragment() {};


private:
    // Default constructor
    G4StatMFFragment(){};
	
    // Copy constructor
    G4StatMFFragment(const G4StatMFFragment & right);

    // operators
    G4StatMFFragment & operator=(const G4StatMFFragment & right);
public:
    G4bool operator==(const G4StatMFFragment & right) const;
    G4bool operator!=(const G4StatMFFragment & right) const;
	
public:

    G4double GetCoulombEnergy(void) const;
	
    G4double GetEnergy(const G4double T) const;
	
    G4double GetInvLevelDensity(void) const;

    G4double GetA(void) const {return theA;}
	
    G4double GetZ(void) const {return theZ;}
	
    void SetPosition(const G4ThreeVector aPosition) {_position = aPosition;}
	
    G4ThreeVector GetPosition(void) {return _position;}
	
    void SetMomentum(const G4ThreeVector aMomentum) {_momentum = aMomentum;}

    G4ThreeVector GetMomentum(void) {return _momentum;}

    G4Fragment * GetFragment(const G4double T);
	
    G4double GetNuclearMass(void)
	{return G4ParticleTable::GetParticleTable()->GetIonTable()
	                       ->GetIonMass(static_cast<G4int>(theZ), static_cast<G4int>(theA));}
	

private:

    G4double CalcExcitationEnergy(const G4double T);

private:

    G4double theA;
	
    G4double theZ;
	
    G4ThreeVector _position;
	
    G4ThreeVector _momentum;
};

#endif

