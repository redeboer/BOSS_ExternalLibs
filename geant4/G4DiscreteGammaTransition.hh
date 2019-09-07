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
//
// -------------------------------------------------------------------
//      GEANT 4 class file 
//
//      CERN, Geneva, Switzerland
//
//      File name:     G4DiscreteGammaTransition
//
//      Author:        Maria Grazia Pia   (pia@genova.infn.it)
// 
//      Creation date: 23 October 1998
//
//      Modifications: 
//
//        8 March 2002,  Fan Lei (flei@space.qinetiq.com)
//              added 
//              1) SetRDM () to switch on/off IC process
//              2) GetOrbitNumber () to return the CE oribit number
//              3) GetBondEnergy() for the converted e-
//              4) IsaGamma() to separate CE from gamma
//
//        21 Nov. 2001, Fan Lei (flei@space.qinetiq.com)
//              i) added G4int _nucleusZ initialise it through the constructor
//              ii) modified SelectGamma() to allow the generation of conversion electrons      
//              iii) added #include G4AtomicShells.hh
//      
//        15 April 1999, Alessandro Brunengo (Alessandro.Brunengo@ge.infn.it)
//              Added creation time evaluation for products of evaporation
//      
// -------------------------------------------------------------------

#ifndef G4DiscreteGammaTransition_hh
#define G4DiscreteGammaTransition_hh

#include "globals.hh"
#include "G4VGammaTransition.hh"
#include "G4NuclearLevel.hh"

class G4DiscreteGammaTransition : public G4VGammaTransition
{
public:

  // Constructor
  G4DiscreteGammaTransition(const G4NuclearLevel& level);
  G4DiscreteGammaTransition(const G4NuclearLevel& level, G4int Z);

  // Destructor
  ~G4DiscreteGammaTransition();

  // Functions

public:

  virtual void SetEnergyFrom(const G4double energy);
  virtual G4double GetGammaEnergy();
  virtual G4double GetGammaCreationTime();
  virtual void SelectGamma();

  void SetICM(G4bool ic) { _icm = ic;};
  G4double GetBondEnergy () {return _bondE;};
  G4int GetOrbitNumber () {return _orbitE;};
  G4bool IsAGamma() {return _aGamma;};
 
private:
  
  G4int _nucleusZ;
  G4int _orbitE;
  G4double _bondE;
  G4bool _aGamma;
  G4bool _icm;

  G4double _gammaEnergy;
  G4NuclearLevel _level;     
  G4double _excitation;
  G4double _gammaCreationTime;
};


#endif

