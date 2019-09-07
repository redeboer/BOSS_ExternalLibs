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
// by JPW, working, but to be cleaned up. @@@@
// D. Wright, 23-Dec-2006 Added isotope dependence
// G.Folger, 25-Nov-2009: extend to 100TeV, using a constant above 20GeV
//

#ifndef G4NeutronInelasticCrossSection_h
#define G4NeutronInelasticCrossSection_h

#include "globals.hh"
#include "G4Neutron.hh"

// Class Description
// Cross-sections for neutron nuclear scattering from 14 MeV up to 20 GeV, getting the
// low energy threshold behaviour right.
// H.P. Wellisch (TRIUMF), M. Laidlaw (British Columbia U.). 1996. 
// Class Description - End

#include "G4VCrossSectionDataSet.hh"

class G4NeutronInelasticCrossSection : public G4VCrossSectionDataSet
{
   public:
   
   virtual
   G4bool IsApplicable(const G4DynamicParticle* aPart, const G4Element* aEle)
   {
     G4bool result = false;
     if(( aPart->GetDefinition()==G4Neutron::Neutron()) &&
        ( aPart->GetKineticEnergy()<100*TeV) &&
          aPart->GetKineticEnergy()>19.9*MeV) result = true;
     if(aEle->GetZ()<2) result = false;
     return result;
   }

   G4bool IsZAApplicable(const G4DynamicParticle* aPart,
                         G4double ZZ, G4double /*AA*/)
   {
     G4bool result = false;
     if(( aPart->GetDefinition() == G4Neutron::Neutron()) &&
        ( aPart->GetKineticEnergy() < 100*TeV) &&
          aPart->GetKineticEnergy() > 19.9*MeV) result = true;
     if(ZZ < 2) result = false;
     return result;
   }
 

   virtual
   G4double GetCrossSection(const G4DynamicParticle*, 
                            const G4Element*, G4double aTemperature);

   G4double GetIsoZACrossSection(const G4DynamicParticle* aParticle,
                                 G4double ZZ, G4double AA,
                                 G4double /*aTemperature*/)
   {
     return GetCrossSection(aParticle->GetKineticEnergy(), AA, ZZ);
   }
  

   G4double GetCrossSection(G4double anEnergy, G4double anA, G4double aZ);

   virtual
   void BuildPhysicsTable(const G4ParticleDefinition&)
   {}

   virtual
   void DumpPhysicsTable(const G4ParticleDefinition&) 
   {G4cout << "G4NeutronInelasticCrossSection: uses formula"<<G4endl;}

};

#endif
