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
#ifndef G4InclAblaNeutronBuilder_h
#define G4InclAblaNeutronBuilder_h 1

#include "globals.hh"

#include "G4HadronElasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4VNeutronBuilder.hh"

#include "G4InclAblaCascadeInterface.hh"   
#include "G4NeutronInelasticCrossSection.hh"

/**
 * Builder for neutron processes using the INCL/ABLA intra-nuclear
 * cascade and de-excitation models.
 *
 * By default the INCL/ABLA models are used for projectile energies 0 - 3 GeV.
 *
 * The builder uses INCL cascade model with build-in ABLA
 * de-excitation. This is implemented in interface
 * G4InclAblaCascadeInterface.
 *
 * @see G4InclAblaCascadeInterface
 * @see G4InclAblaProtonBuilder
 * @see G4InclAblaPiKBuilder
 */
class G4InclAblaNeutronBuilder : public G4VNeutronBuilder
{
  public: 
    G4InclAblaNeutronBuilder();
    virtual ~G4InclAblaNeutronBuilder();

  public: 
    virtual void Build(G4HadronElasticProcess * aP);
    virtual void Build(G4HadronFissionProcess * aP);
    virtual void Build(G4HadronCaptureProcess * aP);
    virtual void Build(G4NeutronInelasticProcess * aP);
    
    void SetMinEnergy(G4double aM) {theMin = aM;}
    void SetMaxEnergy(G4double aM) {theMax = aM;}

  private:
    G4InclAblaCascadeInterface * theModel;    
    G4NeutronInelasticCrossSection theXSec;
    G4double theMin;
    G4double theMax;

};

// 2002 by J.P. Wellisch

#endif

