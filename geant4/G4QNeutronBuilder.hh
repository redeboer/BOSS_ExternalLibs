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
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4QNeutronBuilder
//
// Author: 2009 M. Kosov
//
// Modified:
//
//----------------------------------------------------------------------------
// Short description: for possible use in QGSC_QGSC (not needed for QGSC_CHIPS)
//-----------------------------------------------------------------------------
//
#ifndef G4QNeutronBuilder_h
#define G4QNeutronBuilder_h 1

#include "globals.hh"

#include "G4NeutronInelasticProcess.hh"
#include "G4QInelastic.hh"
#include "G4QDiscProcessMixer.hh"
#include "G4VNeutronBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

#include <vector>

class G4QNeutronBuilder
{
public: 
  G4QNeutronBuilder();
  virtual ~G4QNeutronBuilder();

public: 
  void Build();
  void RegisterMe(G4VNeutronBuilder * aB) {theModelCollections.push_back(aB);}

private:
  G4QDiscProcessMixer*             theProcessMixer;
  std::vector<G4VNeutronBuilder *> theModelCollections;
  G4NeutronInelasticProcess*       theNeutronInelastic;
  G4QInelastic*                    theCHIPSInelastic;    
  
  G4bool wasActivated;

};

// 2009 by M. Kosov

#endif

