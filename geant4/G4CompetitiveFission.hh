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
// $Id: G4CompetitiveFission.hh,v 1.3 2006/06/29 20:13:19 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Oct 1998)

#ifndef G4CompetitiveFission_h
#define G4CompetitiveFission_h 1

#include "G4VEvaporationChannel.hh"
#include "G4Fragment.hh"
#include "G4VFissionBarrier.hh"
#include "G4FissionBarrier.hh"
#include "G4VEmissionProbability.hh"
#include "G4FissionProbability.hh"
#include "G4VLevelDensityParameter.hh"
#include "G4FissionLevelDensityParameter.hh"
#include "G4FissionParameters.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "Randomize.hh"

//#define debug

class G4CompetitiveFission : public G4VEvaporationChannel
{
public:
  
  G4CompetitiveFission();
  virtual ~G4CompetitiveFission();

private:
  G4CompetitiveFission(const G4CompetitiveFission &right);

  const G4CompetitiveFission & operator=(const G4CompetitiveFission &right);
public:
  G4bool operator==(const G4CompetitiveFission &right) const;
  G4bool operator!=(const G4CompetitiveFission &right) const;

public:
  G4FragmentVector * BreakUp(const G4Fragment &theNucleus);

  void Initialize(const G4Fragment & fragment);

  inline void SetFissionBarrier(G4VFissionBarrier * aBarrier)
  {
    if (MyOwnFissionBarrier) delete theFissionBarrierPtr;
    theFissionBarrierPtr = aBarrier;
    MyOwnFissionBarrier = false;
  }

  inline void SetEmissionStrategy(G4VEmissionProbability * aFissionProb)
  {
    if (MyOwnFissionProbability) delete theFissionProbabilityPtr;
    theFissionProbabilityPtr = aFissionProb;
    MyOwnFissionProbability = false;
  }


  inline void SetLevelDensityParameter(G4VLevelDensityParameter * aLevelDensity)
  { 
    if (MyOwnLevelDensity) delete theLevelDensityPtr;
    theLevelDensityPtr = aLevelDensity;
    MyOwnLevelDensity = false;
  }


  inline G4double GetFissionBarrier(void) const { return FissionBarrier; }

  inline G4double GetEmissionProbability(void) const { return FissionProbability; }

  inline G4double GetLevelDensityParameter(void) const { return LevelDensityParameter; }

  inline G4double GetMaximalKineticEnergy(void) const { return MaximalKineticEnergy; }
private:

  // Maximal Kinetic Energy that can be carried by fragment
  G4double MaximalKineticEnergy;


  // For Fission barrier
  G4VFissionBarrier * theFissionBarrierPtr;
  G4double FissionBarrier;
  G4bool MyOwnFissionBarrier;

  // For Fission probability emission
  G4VEmissionProbability * theFissionProbabilityPtr;
  G4double FissionProbability;
  G4bool MyOwnFissionProbability;


  // For Level Density calculation
  G4bool MyOwnLevelDensity;
  G4VLevelDensityParameter * theLevelDensityPtr;
  G4double LevelDensityParameter;




  //  --------------------


  // Sample AtomicNumber of Fission products
  G4int FissionAtomicNumber(const G4int A, const G4FissionParameters & theParam);
  G4double MassDistribution(const G4double x, const G4double A, const G4FissionParameters & theParam);


  // Sample Charge of fission products
  G4int FissionCharge(const G4double A, const G4double Z, const G4double Af);


  // Sample Kinetic energy of fission products
  G4double FissionKineticEnergy(const G4double A, const G4double Z,
				const G4double Af1, const G4double Zf1,
				const G4double Af2, const G4double Zf2,
				const G4double U, const G4double Tmax,
				const G4FissionParameters & theParam);
    


  G4double Ratio(const G4double A,const G4double A11,const G4double B1,const G4double A00);
  G4double SymmetricRatio(const G4double A,const G4double A11);
  G4double AsymmetricRatio(const G4double A,const G4double A11);



  G4ThreeVector IsotropicVector(const G4double Magnitude = 1.0);


#ifdef debug
  void CheckConservation(const G4Fragment & theInitialState,
			 G4FragmentVector * Result) const;
#endif


};



#endif


