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
// $Id: G4O15GEMChannel.hh,v 1.4 2009/09/15 12:54:16 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Sept. 2001)
//


#ifndef G4O15GEMChannel_h
#define G4O15GEMChannel_h 1

#include "G4GEMChannel.hh"
#include "G4O15GEMCoulombBarrier.hh"
#include "G4O15GEMProbability.hh"

class G4O15GEMChannel : public G4GEMChannel
{
public:
  // only available constructor
  G4O15GEMChannel() : G4GEMChannel(15,8,"O15",
				   &theEvaporationProbability,
				   &theCoulombBarrier)
  {
    theEvaporationProbability.SetCoulomBarrier(&theCoulombBarrier);
  }
  
  // destructor
  ~G4O15GEMChannel() {};
  
private:
  const G4O15GEMChannel & operator=(const G4O15GEMChannel & right);  
    
  G4O15GEMChannel(const G4O15GEMChannel & right);
  
public:
  G4bool operator==(const G4O15GEMChannel & right) const;
  G4bool operator!=(const G4O15GEMChannel & right) const;
    
private:
  
  G4O15GEMCoulombBarrier theCoulombBarrier;
	
  G4O15GEMProbability theEvaporationProbability;
  
};
#endif

