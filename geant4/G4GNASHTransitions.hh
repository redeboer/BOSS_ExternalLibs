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
#ifndef G4GNASHTransitions_hh
#define G4GNASHTransitions_hh 1

#include "G4VPreCompoundTransitions.hh"
#include "G4Fragment.hh"

class G4GNASHTransitions : public G4VPreCompoundTransitions
{
public:
  G4GNASHTransitions() {}
  virtual ~G4GNASHTransitions() {}

private:

  G4GNASHTransitions(const G4GNASHTransitions &) : G4VPreCompoundTransitions() {}

  const G4GNASHTransitions& operator=(const G4GNASHTransitions &right);

  G4bool operator==(const G4GNASHTransitions &right) const;

  G4bool operator!=(const G4GNASHTransitions &right) const;

public:

  virtual G4double CalculateProbability(const G4Fragment & aFragment);
  
  virtual G4Fragment PerformTransition(const G4Fragment & aFragment);

//JMQ 03/01/08
 public:
// inline G4double GetTransitionProb1() const
G4double GetTransitionProb1() 
{return 0;}
// inline G4double GetTransitionProb2() const
G4double GetTransitionProb2() 
{return 0;}
// inline G4double GetTransitionProb3() const
G4double GetTransitionProb3() 
{return 0;}
//


};

#endif
