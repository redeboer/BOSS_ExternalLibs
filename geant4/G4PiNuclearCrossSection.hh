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
#ifndef G4PiNuclearCrossSection_h
#define G4PiNuclearCrossSection_h

#include "G4VCrossSectionDataSet.hh"

#include "globals.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4PiData.hh"
#include "G4HadTmpUtil.hh"

class G4PiNuclearCrossSection : public G4VCrossSectionDataSet
{
  public:
  
  G4PiNuclearCrossSection();
  virtual ~G4PiNuclearCrossSection();

  G4bool IsApplicable(const G4DynamicParticle* aParticle, const G4Element* anElement)
  {
    G4bool result = false;
    if(aParticle->GetDefinition() == G4PionMinus::PionMinus()) result=true;
    if(aParticle->GetDefinition() == G4PionPlus::PionPlus())   result=true;
    if(G4lrint(anElement->GetZ()) == 1) result = false;
    if(aParticle->GetKineticEnergy() > 99.9*TeV) result=false;
    return result;
  }

  G4bool IsZAApplicable(const G4DynamicParticle* particle,
                        G4double ZZ, G4double /*AA*/)
  {
    G4bool result = false;
    if(particle->GetDefinition() == G4PionMinus::PionMinus()) result=true;
    if(particle->GetDefinition() == G4PionPlus::PionPlus())   result=true;
    if(G4lrint(ZZ) == 1) result = false;
    if(particle->GetKineticEnergy() > 99.9*TeV) result=false;
    return result;
  }

  G4double GetCrossSection(const G4DynamicParticle* particle, 
                           const G4Element* element,
                           G4double temperature)
  {
    return GetIsoZACrossSection(particle, element->GetZ(), 
                                element->GetN(), temperature);
  }

  G4double GetIsoZACrossSection(const G4DynamicParticle* aParticle,
                                G4double ZZ, G4double AA,
                                G4double /*aTemperature*/);

  G4double GetTotalXsc()  { return fTotalXsc;   };
  G4double GetElasticXsc(){ return fElasticXsc; };


  void BuildPhysicsTable(const G4ParticleDefinition&) {}
  void DumpPhysicsTable(const G4ParticleDefinition&) {}
  
  private:
  G4double Interpolate(G4int Z1, G4int Z2, G4int Z, G4double x1, G4double x2);

// add Hydrogen from PDG group.

static const G4double e1[38];
static const G4double he_t[38];
static const G4double he_in[38];
static const G4double be_m_t[38];
static const G4double be_m_in[38];
static const G4double be_p_t[24];
static const G4double be_p_in[24];
static const G4double e2[39];
static const G4double c_m_t[39];
static const G4double c_m_in[39];
static const G4double c_p_t[24];
static const G4double c_p_in[24];
static const G4double n_m_t[39];
static const G4double n_m_in[39];
static const G4double n_p_t[27];
static const G4double n_p_in[27];
static const G4double e3[31];
static const G4double o_m_t[31];
static const G4double o_m_in[31];
static const G4double o_p_t[20];
static const G4double o_p_in[20];
static const G4double na_m_t[31];
static const G4double na_m_in[31];
static const G4double na_p_t[22];
static const G4double na_p_in[22];
static const G4double e3_1[31];
static const G4double al_m_t[31];
static const G4double al_m_in[31];
static const G4double al_p_t[21];
static const G4double al_p_in[21];
static const G4double ca_m_t[31];
static const G4double ca_m_in[31];
static const G4double ca_p_t[23];
static const G4double ca_p_in[23];

static const G4double e4[32];
static const G4double fe_m_t[32];
static const G4double fe_m_in[32];
static const G4double fe_p_t[25];
static const G4double fe_p_in[25];
static const G4double cu_m_t[32];
static const G4double cu_m_in[32];
static const G4double cu_p_t[25];
static const G4double cu_p_in[25];
static const G4double e5[34];
static const G4double mo_m_t[34];
static const G4double mo_m_in[34];
static const G4double mo_p_t[27];
static const G4double mo_p_in[27];
static const G4double cd_m_t[34];
static const G4double cd_m_in[34];
static const G4double cd_p_t[28];
static const G4double cd_p_in[28];
static const G4double e6[35];
static const G4double sn_m_t[35];
static const G4double sn_m_in[35];
static const G4double sn_p_t[29];
static const G4double sn_p_in[29];
static const G4double w_m_t[35];
static const G4double w_m_in[35];
static const G4double w_p_t[30];
static const G4double w_p_in[30];
static const G4double e7[35];
static const G4double pb_m_t[35];
static const G4double pb_m_in[35];
static const G4double pb_p_t[30];
static const G4double pb_p_in[30];
static const G4double u_m_t[35];
static const G4double u_m_in[35];
static const G4double u_p_t[30];
static const G4double u_p_in[30];

std::vector<G4int> theZ;
std::vector<G4PiData *> thePipData;
std::vector<G4PiData *> thePimData;

 // cross sections

  G4double fTotalXsc;
  G4double fElasticXsc;


};

#endif
