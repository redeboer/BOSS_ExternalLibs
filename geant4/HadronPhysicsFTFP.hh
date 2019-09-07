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
// $Id: HadronPhysicsFTFP.hh,v 1.3 2008/06/19 08:08:22 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:   
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 23.11.2005 G.Folger: migration to non static particles
// 08.06.2006 V.Ivanchenko: remove stopping
// 19.06.2008 G.Folger: change default for QE to NOT use Chips QE
//
//----------------------------------------------------------------------------
//
#ifndef HadronPhysicsFTFP_h
#define HadronPhysicsFTFP_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"
#include "G4MiscLHEPBuilder.hh"

#include "G4PiKBuilder.hh"
#include "G4LEPPiKBuilder.hh"
#include "G4FTFPPiKBuilder.hh"

#include "G4ProtonBuilder.hh"
#include "G4LEPProtonBuilder.hh"
#include "G4FTFPProtonBuilder.hh"

#include "G4NeutronBuilder.hh"
#include "G4LEPNeutronBuilder.hh"
#include "G4FTFPNeutronBuilder.hh"

class HadronPhysicsFTFP : public G4VPhysicsConstructor
{
  public: 
    HadronPhysicsFTFP(const G4String& name ="hadron",G4bool quasiElastic=false);
    virtual ~HadronPhysicsFTFP();

  public: 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  private:
    void CreateModels();
    G4NeutronBuilder * theNeutrons;
    G4LEPNeutronBuilder * theLEPNeutron;
    G4FTFPNeutronBuilder * theFTFPNeutron;
    
    G4PiKBuilder * thePiK;
    G4LEPPiKBuilder * theLEPPiK;
    G4FTFPPiKBuilder * theFTFPPiK;
    
    G4ProtonBuilder * thePro;
    G4LEPProtonBuilder * theLEPPro;
    G4FTFPProtonBuilder * theFTFPPro;    
    
    G4MiscLHEPBuilder * theMiscLHEP;
    
    G4bool QuasiElastic;
};

#endif

