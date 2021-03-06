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
 // Hadronic Process: Neutron Inelastic Process
 // original by H.P. Wellisch
 // modified by J.L. Chuma, TRIUMF, 05-Nov-1996
 // Last modified: 03-Apr-1997
 
 // Note:  there is no .cc file
 
#ifndef G4NeutronInelasticProcess_h
#define G4NeutronInelasticProcess_h 1
 
// Class Description
// Process for Neutron Inelastic scattering; 
// to be used in your physics list in case you need this physics.
// Class Description - End

#include "G4HadronInelasticProcess.hh"
 

 class G4NeutronInelasticProcess : public G4HadronInelasticProcess
 {
 public:
    
    G4NeutronInelasticProcess(
     const G4String& processName = "NeutronInelastic" ) :
      G4HadronInelasticProcess( processName, G4Neutron::Neutron() )
    { }
    
    ~G4NeutronInelasticProcess()
    { }
 };
 
#endif
 
