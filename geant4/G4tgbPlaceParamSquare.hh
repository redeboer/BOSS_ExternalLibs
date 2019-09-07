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
// $Id: G4tgbPlaceParamSquare.hh,v 1.1 2008/10/23 14:43:43 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//
// class G4tgbPlaceParamSquare
//
// Class description:
//
// Class to represent simple Cartesian parameterisations along a square.

// History:
// - Created.                                 P.Arce, CIEMAT (November 2007)
// -------------------------------------------------------------------------

#ifndef G4tgbPlaceParamSquare_H
#define G4tgbPlaceParamSquare_H 1

#include "globals.hh"
#include "G4tgbPlaceParameterisation.hh"
#include "G4ThreeVector.hh"

class G4VPhysicalVolume;

class G4tgbPlaceParamSquare : public G4tgbPlaceParameterisation
{ 
  public:  // with description

    G4tgbPlaceParamSquare( G4tgrPlaceParameterisation* );
   ~G4tgbPlaceParamSquare();

    void ComputeTransformation(const G4int copyNo,
                               G4VPhysicalVolume *physVol) const;

  private:

    G4int theNCopies1;
    G4int theNCopies2;
    G4ThreeVector theDirection1;
    G4ThreeVector theDirection2;
    G4double theStep1;
    G4double theStep2;
    G4double theOffset1;
    G4double theOffset2;
};

#endif
