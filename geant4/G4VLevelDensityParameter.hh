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
// $Id: G4VLevelDensityParameter.hh,v 1.6 2009/03/04 11:05:02 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Oct 1998) 
//



#ifndef G4VLevelDensityParameter_h
#define G4VLevelDensityParameter_h 1


#include "globals.hh"
#include "G4HadronicException.hh"

class G4VLevelDensityParameter 
{
public:
  G4VLevelDensityParameter();
  virtual ~G4VLevelDensityParameter();

private:  
  G4VLevelDensityParameter(const G4VLevelDensityParameter &right);

  const G4VLevelDensityParameter & operator=(const G4VLevelDensityParameter &right);
  G4bool operator==(const G4VLevelDensityParameter &right) const;
  G4bool operator!=(const G4VLevelDensityParameter &right) const;
  
public:
  virtual G4double LevelDensityParameter(const G4int A,const G4int Z,const G4double U) const = 0;

};


#endif
