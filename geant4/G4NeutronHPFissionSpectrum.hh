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
// $Id: G4NeutronHPFissionSpectrum.hh,v 1.11 2006/06/29 20:47:57 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
#ifndef G4NeutronHPFissionSpectrum_h
#define G4NeutronHPFissionSpectrum_h 1

#include "globals.hh"
#include "G4NeutronHPVector.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include <fstream>
#include "G4VNeutronHPEDis.hh"

// we will need a List of these .... one per term.

class G4NeutronHPFissionSpectrum : public G4VNeutronHPEDis
{
  public:
  G4NeutronHPFissionSpectrum()
  {
    expm1 = std::exp(-1.);
  }
  ~G4NeutronHPFissionSpectrum()
  {
  }
  
  inline void Init(std::ifstream & aDataFile)
  {
    theFractionalProb.Init(aDataFile, eV);
    theThetaDist.Init(aDataFile, eV);
  }
  
  inline G4double GetFractionalProbability(G4double anEnergy)
  {
    return theFractionalProb.GetY(anEnergy);
  }
  
  inline G4double Sample(G4double anEnergy) 
  {
    G4double theta = theThetaDist.GetY(anEnergy);
    // here we need to sample Maxwells distribution, if 
    // need be.
    G4double result, cut;
    G4double range =50*MeV;
    G4double max = Maxwell((theta*eV)/2., theta);
    G4double value;
    do
    {
      result = range*G4UniformRand();
      value = Maxwell(result, theta);
      cut = G4UniformRand();
    }
    while(cut > value/max);
    return result;
  }
  
  private:
 
  // this is the function to sample from. 
  inline G4double Maxwell(G4double anEnergy, G4double theta)
  {
    G4double result = std::sqrt(anEnergy/eV)*std::exp(-anEnergy/eV/theta);
    return result;
  }
  
  private:
  
  G4double expm1;
  
  G4NeutronHPVector theFractionalProb;
  
  G4NeutronHPVector theThetaDist;
  
};

#endif
