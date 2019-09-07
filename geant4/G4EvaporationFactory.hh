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
// $Id: G4EvaporationFactory.hh,v 1.3 2006/06/29 20:09:55 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara


#ifndef G4EvaporationFactory_hh
#define G4EvaporationFactory_hh


#include "G4VEvaporationFactory.hh"

class G4EvaporationFactory : public G4VEvaporationFactory
{
public:
  G4EvaporationFactory() {};
  virtual ~G4EvaporationFactory() {};

private:
  G4EvaporationFactory(const G4EvaporationFactory & ) : G4VEvaporationFactory() {};
  const G4EvaporationFactory & operator=(const G4EvaporationFactory & val);
  G4bool operator==(const G4EvaporationFactory & val) const;
  G4bool operator!=(const G4EvaporationFactory & val) const;

private:
  std::vector<G4VEvaporationChannel*> * CreateChannel();


};
#endif
