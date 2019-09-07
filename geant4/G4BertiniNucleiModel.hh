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
#ifndef G4BERTININUCLEI_MODEL_HH
#define G4BERTININUCLEI_MODEL_HH

#ifndef G4INUCL_ELEMENTARY_PARTICLE_HH
#include "G4InuclElementaryParticle.hh"
#endif

#include "G4CascadParticle.hh"
#include "G4InuclSpecialFunctions.hh"
#include "G4CascadSpecialFunctions.hh"
#include "G4ElementaryParticleCollider.hh"

#include <vector>

class G4InuclNuclei;

using namespace G4InuclSpecialFunctions;
using namespace G4CascadSpecialFunctions;

typedef std::pair<G4InuclElementaryParticle, G4double> partner;
typedef std::vector<partner> partners;

class G4BertiniNucleiModel {

  /*! \class G4BertiniNucleiModel
   *  \brief Implements HETC nuclei model in Geant4
   *  \author Aatos Heikkinen and
   *  \author Original HETC authors
   *  \version 0.0
   *  \date 25.11.2002
   *  \bug 
   *  \warning Wery preliminary
   */

public:

  G4BertiniNucleiModel(); 
     
  G4BertiniNucleiModel(G4InuclNuclei* nuclei) {
    generateModel(nuclei->getA(), nuclei->getZ());
  };

  void generateModel(G4double a, 
		     G4double z);   /// create nuclei model 

  /// @param a is atom mumber (number of protons and neutron in nuclei)
  /// @param z in number of protons in nuclei
  /// @see 
  /// @return void

  void reset() {
    neutronNumberCurrent = neutronNumber;
    protonNumberCurrent = protonNumber;
  };

  void printModel() const; 

  G4double getDensity(G4int ip, 
		      G4int izone) const {

    return nucleon_densities[ip - 1][izone];
  };

  G4double getFermiMomentum(G4int ip, 
			    G4int izone) const {

    return fermi_momenta[ip - 1][izone];
  };

  G4double getFermiKinetic(G4int ip, 
			   G4int izone) const {
    G4double ekin = 0.0;

    if (ip < 3 && izone < number_of_zones) {
      G4double pf = fermi_momenta[ip - 1][izone]; 
      G4double mass = ip == 1 ? 0.93827 : 0.93957;
      ekin = std::sqrt(pf * pf + mass * mass) - mass;
    };  

    return ekin;
  };

  G4double getPotential(G4int ip, 
			G4int izone) const {
    G4int ip0 = ip < 3 ? ip - 1 : 2;

    return izone < number_of_zones ? zone_potentials[ip0][izone] : 0.0;
  };

  std::vector<G4CascadParticle> 
  generateParticleFate(G4CascadParticle& cparticle,
		       G4ElementaryParticleCollider* theElementaryParticleCollider); 

  G4double getNumberOfNeutrons() const { 

    return neutronNumberCurrent; 
  };

  G4double getNumberOfProtons() const { 

    return protonNumberCurrent; 
  };

  G4bool empty() const { /*!< See if there is no nucleon. */
  /// @return TRUE if no current nucleons.

    return neutronNumberCurrent < 1.0 && protonNumberCurrent < 1.0; 
  };

  G4bool stillInside(const G4CascadParticle& cparticle) { /*!< Check if the cascade particle is still inside the nuclei. */
  /// @return TRUE if no current nucleons.
    return cparticle.getCurrentZone() < number_of_zones;
  };

  G4CascadParticle initializeCascad(G4InuclElementaryParticle* particle);

  std::pair<std::vector<G4CascadParticle>, std::vector<G4InuclElementaryParticle> > initializeCascad(G4InuclNuclei* bullet, G4InuclNuclei* target);

  std::pair<G4int, G4int> getTypesOfNucleonsInvolved() const {

    return std::pair<G4int, G4int>(current_nucl1, current_nucl2);
  };

  G4bool worthToPropagate(const G4CascadParticle& cparticle) const; /*!< Check if cascade particle will continue. */
    
private: 
  G4int verboseLevel;  /*!< Each cascade class has a verbosity level of its own. */
  G4bool passFermi(const std::vector<G4InuclElementaryParticle>& particles, 
		   G4int zone);

  void boundaryTransition(G4CascadParticle& cparticle);

  G4InuclElementaryParticle generateNucleon(G4int type, 
					    G4int zone) const;

  G4InuclElementaryParticle generateQuasiDeutron(G4int type1, 
						 G4int type2,
						 G4int zone) const;

  partners generateInteractionPartners(G4CascadParticle& cparticle) const;

  G4double volNumInt(G4double r1, 
		     G4double r2, 
		     G4double cu, 
		     G4double d1) const; /// :::

  G4double volNumInt1(G4double r1, 
		      G4double r2, 
		      G4double cu2) const;  /// :::

  G4double getRatio(G4int ip) const;

  std::vector<std::vector<G4double> > nucleon_densities;
  std::vector<std::vector<G4double> > zone_potentials;
  std::vector<std::vector<G4double> > fermi_momenta;
  std::vector<G4double> zone_radii;
  std::vector<G4double> binding_energies;
  G4double nuclei_radius;
  G4int number_of_zones; /*!< Usually = 3, but number of zones is free parameter. */
  G4double A; 
  G4double Z;
  G4double neutronNumber;
  G4double protonNumber;
  G4double neutronNumberCurrent;
  G4double protonNumberCurrent;
  G4int current_nucl1;
  G4int current_nucl2;
};        

#endif 
