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
// $Id: G4Poisson.hh,v 1.9 2006/06/29 19:00:44 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 
// ------------------------------------------------------------
//      GEANT 4 class header file 
// ------------------------------------------------------------
// Class description:
//
// G4Poisson is the C++ implementation of the CERNLIB GPOISS algorithm
// for the generation of Poisson distributed random numbers. It has been
// adapted to invoke HepRandom from CLHEP for the primary engine generators.
// GPOISS is recognized to be a faster algorithm, providing however a less
// accurate output, than the algorithm adopted in CLHEP.

// ------------------------------------------------------------
#ifndef G4POISSON_HH
#define G4POISSON_HH

#include "globals.hh"
#include "Randomize.hh"

inline G4long G4Poisson(G4double mean)
{
  G4long number = 0;
  const G4int border = 16;
  G4double limit = 2e9;

  if(mean <= border) {
    G4double position = CLHEP::RandFlat::shoot();
    G4double poissonValue = std::exp(-mean);
    G4double poissonSum = poissonValue;

    while(poissonSum <= position) {
      number++ ;
      poissonValue *= mean/number;
      poissonSum += poissonValue;
    }
    return number;
  } // the case of mean <= 16

  G4double value, t, y;
  t = std::sqrt(-2*std::log(CLHEP::RandFlat::shoot()));
  y = twopi*CLHEP::RandFlat::shoot();
  t *= std::cos(y);
  value = mean + t*std::sqrt(mean) + 0.5;
  if(value <= 0) {return 0;}
  if(value >= limit) { return G4long(limit);}
  return G4long(value);
}

#endif  /* G4POISSON_HH */
