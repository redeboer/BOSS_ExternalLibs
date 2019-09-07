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
// $Id: G4VNeutronHPEnergyAngular.hh,v 1.12 2008/07/21 23:26:29 tkoi Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 080718 Add ClearHistories() method by T. Koi
//
#ifndef G4VNeutronHPEnergyAngular_h
#define G4VNeutronHPEnergyAngular_h 1

#include "G4ios.hh"
#include <fstream>
#include "globals.hh"
#include "G4ReactionProduct.hh"

class G4VNeutronHPEnergyAngular
{
  public:
  
  G4VNeutronHPEnergyAngular()
  {
    theTarget = 0;
    theNeutron = 0;
    theQValue=0;
  }
  virtual ~G4VNeutronHPEnergyAngular(){}
  
  public:
  
  virtual void Init(std::ifstream & aDataFile) = 0;
  virtual G4ReactionProduct * Sample(G4double anEnergy, 
                                     G4double massCode, 
                                     G4double mass) = 0;
  virtual G4double MeanEnergyOfThisInteraction() = 0; // returns value cashed in sample
  
  void SetNeutron(G4ReactionProduct * aNeutron) 
  { 
    theNeutron = aNeutron; 
    if(theTarget!=0) theCMS = *theNeutron+*theTarget;
  }
  
  void SetTarget(G4ReactionProduct * aTarget)
  { 
    theTarget = aTarget; 
  }
  
  G4ReactionProduct * GetTarget() { return theTarget; }
  
  G4ReactionProduct * GetNeutron() { return theNeutron; }
  
  G4ReactionProduct * GetCMS() { return &theCMS; }

  inline void SetQValue(G4double aValue) { theQValue = aValue; }
  
  protected:
  
  inline G4double GetQValue() { return theQValue; }
  
  private:
  
  G4double theQValue;
    
  G4ReactionProduct * theTarget;
  G4ReactionProduct * theNeutron;
  G4ReactionProduct theCMS;
    
   public:
      virtual void ClearHistories(){;};
};
#endif
