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
// $Id: G4AntiSigmaMinus.hh,v 1.10 2006/06/29 19:15:16 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 
// ------------------------------------------------------------
//      GEANT 4 class header file
//
//      History: first implementation, based on object model of
//      4-th April 1996, G.Cosmo
// ****************************************************************
//  New implementation as a utility class  M.Asai, 26 July 2004
// ----------------------------------------------------------------

#ifndef G4AntiSigmaMinus_h
#define G4AntiSigmaMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                          AntiSigmaMinus                        ###
// ######################################################################

class G4AntiSigmaMinus : public G4ParticleDefinition
{
 private:
   static G4AntiSigmaMinus* theInstance;
   G4AntiSigmaMinus(){}
   ~G4AntiSigmaMinus(){}

 public:
   static G4AntiSigmaMinus* Definition();
   static G4AntiSigmaMinus* AntiSigmaMinusDefinition();
   static G4AntiSigmaMinus* AntiSigmaMinus();
};

#endif
