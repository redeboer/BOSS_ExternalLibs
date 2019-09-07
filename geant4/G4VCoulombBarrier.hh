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
// $Id: G4VCoulombBarrier.hh,v 1.5 2009/03/04 11:05:02 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Nov 1999)

#ifndef G4VCoulombBarrier_h
#define G4VCoulombBarrier_h 1

#include "globals.hh"


class G4VCoulombBarrier
{
public:
  G4VCoulombBarrier(const G4int anA, const G4int aZ);
  virtual ~G4VCoulombBarrier();

protected:
  G4VCoulombBarrier();

private:
  G4VCoulombBarrier(const G4VCoulombBarrier & right);

  const G4VCoulombBarrier & operator=(const G4VCoulombBarrier & right);
  G4bool operator==(const G4VCoulombBarrier & right) const;
  G4bool operator!=(const G4VCoulombBarrier & right) const;
  
public:
  virtual G4double GetCoulombBarrier(const G4int ARes, const G4int ZRes, 
				     const G4double U) const = 0;
					
  G4int GetA(void) const {return theA;}
  G4int GetZ(void) const {return theZ;}
					
private: 
	
  G4int theA;
  G4int theZ;

};

#endif
