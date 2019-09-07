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
// $Id: G4LPhysicsFreeVector.hh,v 1.11 2008/09/22 08:26:33 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 
// ------------------------------------------------------------------
//
// Class G4LPhysicsFreeVector -- header file
//
// Class description:
//
// Derived from base class G4PhysicsVector
// This is a free vector for Low Energy Physics cross section data.
// The class name includes an "L" to distinguish it from other groups
// who may wish to implement a free vector in a different way.
// A subdivision method is used to find the energy|momentum bin.

// F.W. Jones, TRIUMF, 04-JUN-96
// 11-NOV-00 H.Kurashige: use STL vector for dataVector and binVector
// 02-APR-08 A.Bagulya: use GetValue() from base class
// ------------------------------------------------------------------

#ifndef G4LPhysicsFreeVector_h
#define G4LPhysicsFreeVector_h 1

#include "G4PhysicsVector.hh"

class G4LPhysicsFreeVector : public G4PhysicsVector  
{

public: // with description

   G4LPhysicsFreeVector();

   G4LPhysicsFreeVector(size_t nbin, G4double binmin, G4double binmax);

   ~G4LPhysicsFreeVector();

   G4LPhysicsFreeVector(const G4LPhysicsFreeVector&);
   G4LPhysicsFreeVector& operator=(const G4LPhysicsFreeVector&);
     // Copy constructor and assignment operator.

   void PutValues(size_t binNumber, G4double binValue, G4double dataValue);
     // G4PhysicsVector has PutValue() but it is inconvenient.
     // Want to simultaneously fill the bin and data vectors.

   void SetVerboseLevel(G4int value);

   G4int GetVerboseLevel(G4int);

   G4double GetLastEnergy();

   size_t GetLastBin();

   void DumpValues();

private:

   G4int verboseLevel;

   size_t FindBinLocation(G4double theEnergy) const;
     // Pure virtual in G4PhysicsVector
};

#include "G4LPhysicsFreeVector.icc"

#endif
