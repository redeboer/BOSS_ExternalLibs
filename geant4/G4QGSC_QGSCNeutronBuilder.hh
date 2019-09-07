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
#ifndef G4QGSC_QGSCNeutronBuilder_h
#define G4QGSC_QGSCNeutronBuilder_h 1

#include "globals.hh"

#include "G4HadronElasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4VNeutronBuilder.hh"

#include "G4NeutronInelasticCrossSection.hh"
#include "G4TheoFSGenerator.hh"
#include "G4QStringChipsParticleLevelInterface.hh"
#include "G4QGSModel.hh"
#include "G4QGSParticipants.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4QuasiElasticChannel.hh"

class G4QGSC_QGSCNeutronBuilder : public G4VNeutronBuilder
{
  public: 
    G4QGSC_QGSCNeutronBuilder(G4bool quasiElastic=false);
    virtual ~G4QGSC_QGSCNeutronBuilder();

  public: 
    virtual void Build(G4HadronElasticProcess * ){}
    virtual void Build(G4HadronFissionProcess * ){}
    virtual void Build(G4HadronCaptureProcess * ){}
    virtual void Build(G4NeutronInelasticProcess * aP);
    
    void SetMinEnergy(G4double aM) {theMin = aM;}

  private:
    G4QGSModel< G4QGSParticipants > * theStringModel;
    G4QStringChipsParticleLevelInterface * theCascade;
    G4TheoFSGenerator * theModel;
    G4ExcitedStringDecay * theStringDecay;
    G4QuasiElasticChannel * theQuasiElastic;

    G4NeutronInelasticCrossSection theXSec;
    G4double theMin;

};

// 2009 by M. Kosov

#endif

