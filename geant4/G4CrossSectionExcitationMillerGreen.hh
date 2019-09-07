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
// $Id: G4CrossSectionExcitationMillerGreen.hh,v 1.3 2008/07/14 20:47:34 sincerti Exp $

#ifndef G4CROSSSECTIONEXCITATIONMILLERGREEN_HH
#define G4CROSSSECTIONEXCITATIONMILLERGREEN_HH 1
 
#include "G4CrossSectionExcitationMillerGreenPartial.hh"
#include "G4Track.hh"
#include "G4Proton.hh"
#include "G4CrossSectionExcitationEmfietzoglouPartial.hh"
#include "G4DNAGenericIonsManager.hh"
 
class G4CrossSectionExcitationMillerGreen
{
public:
  
  G4CrossSectionExcitationMillerGreen();
  
  virtual ~G4CrossSectionExcitationMillerGreen();
  
  G4double CrossSection(const G4Track&);
  
private:
   
  G4double lowEnergyLimitDefault;
  G4double highEnergyLimitDefault;

  std::map<G4String,G4double,std::less<G4String> > lowEnergyLimit;
  std::map<G4String,G4double,std::less<G4String> > highEnergyLimit;

  G4CrossSectionExcitationMillerGreenPartial partialCrossSection;
};

#endif
