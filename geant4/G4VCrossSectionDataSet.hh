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
// $Id: G4VCrossSectionDataSet.hh,v 1.13 2009/01/24 11:54:47 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:    G4VCrossSectionDataSet
//
// Author  F.W. Jones, TRIUMF, 20-JAN-97
//
// Modifications:
// 23.01.2009 V.Ivanchenko move constructor and destructor to source
//
 
//
// Class Description
// This is a base class for hadronic cross section data sets.  Users may 
// derive specialized cross section classes and register them with the
// appropriate process, or use provided data sets.
// Class Description - End

#ifndef G4VCrossSectionDataSet_h
#define G4VCrossSectionDataSet_h 1

#include "G4DynamicParticle.hh"
#include "G4Element.hh"


class G4VCrossSectionDataSet
{
public: //with description

  G4VCrossSectionDataSet();

  virtual ~G4VCrossSectionDataSet();

  // The following methods need to be implemented for each new data set.
  virtual
  G4bool IsApplicable(const G4DynamicParticle*, const G4Element*) = 0;

  virtual
  G4bool IsZAApplicable(const G4DynamicParticle*, G4double /*Z*/, G4double /*A*/);

  virtual
  G4double GetCrossSection(const G4DynamicParticle*, 
			   const G4Element*, 
			   G4double aTemperature = 0.) = 0;

  virtual
  G4double GetIsoCrossSection(const G4DynamicParticle*, const G4Isotope*, 
			      G4double aTemperature = 0.);

  virtual
  G4double GetIsoZACrossSection(const G4DynamicParticle*, G4double /*Z*/,
				G4double /*A*/, G4double aTemperature = 0.);

  virtual
  void BuildPhysicsTable(const G4ParticleDefinition&) = 0;

  virtual
  void DumpPhysicsTable(const G4ParticleDefinition&) = 0;

public: // Without Description

  inline void SetVerboseLevel(G4int value)
  {
    verboseLevel = value;
  }

  inline G4int GetVerboseLevel()
  {
    return verboseLevel;
  }

protected:

  G4int verboseLevel;
};

#endif
