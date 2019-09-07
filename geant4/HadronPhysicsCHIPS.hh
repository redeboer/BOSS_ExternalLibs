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
// $Id: HadronPhysicsCHIPS.hh,v 1.2 2009/11/20 10:24:28 mkossov Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:   HadronPhysicsCHIPS
//
// Author: 2009  M. Kosov
//
// Modified:
//
//----------------------------------------------------------------------------

#ifndef HadronPhysicsCHIPS_h
#define HadronPhysicsCHIPS_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"
#include "G4QInelasticCHIPSBuilder.hh"

class HadronPhysicsCHIPS : public G4VPhysicsConstructor
{
  public: 
    HadronPhysicsCHIPS(const G4String& name ="CHIPS hadronic");
    virtual ~HadronPhysicsCHIPS();

  public: 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  private:
    G4QInelasticCHIPSBuilder* theInelasticCHIPS;    
};

#endif

