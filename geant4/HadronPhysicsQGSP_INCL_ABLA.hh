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
// $Id: HadronPhysicsQGSP_INCL_ABLA.hh,v 1.1 2009/07/19 18:24:03 kaitanie Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:   HadronPhysicsQGSP_INCL_ABLA
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 23.11.2005 G.Folger: migration to non static particles
// 08.06.2006 V.Ivanchenko: remove stopping
// 25.04.2007 G.Folger: Add quasielastic option, use this by default
// 10.12.2007 G.Folger: Add projectilediffrative option for proton/neutron, off by default
//
//----------------------------------------------------------------------------
//
#ifndef HadronPhysicsQGSP_INCL_ABLA_h
#define HadronPhysicsQGSP_INCL_ABLA_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"
#include "G4MiscLHEPBuilder.hh"

#include "G4PiKBuilder.hh"
#include "G4LEPPiKBuilder.hh"
#include "G4QGSPPiKBuilder.hh"
#include "G4BertiniPiKBuilder.hh"
#include "G4InclAblaPiKBuilder.hh"

#include "G4ProtonBuilder.hh"
#include "G4LEPProtonBuilder.hh"
#include "G4QGSPProtonBuilder.hh"
#include "G4BertiniProtonBuilder.hh"
#include "G4InclAblaProtonBuilder.hh"

#include "G4NeutronBuilder.hh"
#include "G4LEPNeutronBuilder.hh"
#include "G4QGSPNeutronBuilder.hh"
#include "G4BertiniNeutronBuilder.hh"
#include "G4InclAblaNeutronBuilder.hh"

/**
 * Build hadronic physics using INCL/ABLA and QGSP models.
 *
 * @see G4InclAblaProtonBuilder
 * @see G4InclAblaNeutronBuilder
 * @see G4InclAblaPiKBuilder
 */
class HadronPhysicsQGSP_INCL_ABLA : public G4VPhysicsConstructor
{
  public: 
    HadronPhysicsQGSP_INCL_ABLA(const G4String& name ="hadron",G4bool quasiElastic=true);
    virtual ~HadronPhysicsQGSP_INCL_ABLA();

  public: 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

    void SetQuasiElastic(G4bool value) {QuasiElastic = value;}; 
    void SetProjectileDiffraction(G4bool value) {ProjectileDiffraction = value;}; 

  private:
    void CreateModels();
    G4NeutronBuilder * theNeutrons;
    G4LEPNeutronBuilder * theLEPNeutron;
    G4QGSPNeutronBuilder * theQGSPNeutron;
    G4BertiniNeutronBuilder * theBertiniNeutron;
    G4InclAblaNeutronBuilder * theInclAblaNeutron;
    
    G4PiKBuilder * thePiK;
    G4LEPPiKBuilder * theLEPPiK;
    G4QGSPPiKBuilder * theQGSPPiK;
    G4BertiniPiKBuilder * theBertiniPiK;
    G4InclAblaPiKBuilder * theInclAblaPiK;
    
    G4ProtonBuilder * thePro;
    G4LEPProtonBuilder * theLEPPro;
    G4QGSPProtonBuilder * theQGSPPro; 
    G4BertiniProtonBuilder * theBertiniPro;
    G4InclAblaProtonBuilder * theInclAblaPro;
    
    G4MiscLHEPBuilder * theMiscLHEP;
    
    G4bool QuasiElastic;
    G4bool ProjectileDiffraction;
};

#endif

