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
// $Id: G4NeutronCoulombBarrier.hh,v 1.5 2009/03/04 11:05:02 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Dec 1999)

#ifndef G4NeutronCoulombBarrier_h
#define G4NeutronCoulombBarrier_h 1

#include "G4CoulombBarrier.hh"
#include "globals.hh"

class G4NeutronCoulombBarrier : public G4CoulombBarrier
{
public:
	G4NeutronCoulombBarrier() : G4CoulombBarrier(1,0) {}
	~G4NeutronCoulombBarrier() {}

private:
	G4NeutronCoulombBarrier(const G4NeutronCoulombBarrier & right);

	const G4NeutronCoulombBarrier & operator=(const G4NeutronCoulombBarrier & right);
	G4bool operator==(const G4NeutronCoulombBarrier & right) const;
	G4bool operator!=(const G4NeutronCoulombBarrier & right) const;
  
private:

	G4double BarrierPenetrationFactor(const G4double ) const
	{ return 1.0;}

};

#endif
