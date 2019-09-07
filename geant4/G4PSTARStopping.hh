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
// $Id: G4PSTARStopping.hh,v 1.5 2008/11/13 12:04:37 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $

#ifndef G4PSTARStopping_h
#define G4PSTARStopping_h 1

//---------------------------------------------------------------------------
//
// ClassName:   G4PSTARStopping
//
// Description: Data on stopping power
//
// Author:      Anton Ivantchenko 18.04.2006
//
// Organisation:        QinetiQ Ltd, UK
// Customer:            ESA/ESTEC, NOORDWIJK
// Contract:            CSMAN-5288
//
// Modifications:
// 
//----------------------------------------------------------------------------
//
// Class Description:
//
// Data on Stopping Powers from the NIST Data Base  
// http://physics.nist.gov/PhysRefData/STAR/Text/PSTAR.html
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "globals.hh"

class G4Material;

class G4PSTARStopping 
{ 
public: 

  G4PSTARStopping();

  ~G4PSTARStopping();

  G4int GetIndex(const G4Material*);

  G4double GetElectronicDEDX(G4int idx, G4double energy);
  G4double GetElectronicDEDX(const G4Material*, G4double energy);

private:

  void Initialise();

  // hide assignment operator
  G4PSTARStopping & operator=(const  G4PSTARStopping &right);
  G4PSTARStopping(const  G4PSTARStopping&);

  const G4Material* currentMaterial;
  G4int index, matIndex;
  G4String name[74];
  G4double currentE, res;
  G4double e[74][60], kinE[60]; 
};

inline G4double G4PSTARStopping::GetElectronicDEDX(const G4Material* mat, 
						   G4double energy)
{
  return GetElectronicDEDX(GetIndex(mat), energy);
}

#endif
