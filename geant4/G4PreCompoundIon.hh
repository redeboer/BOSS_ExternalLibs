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
//J. M. Quesada (August 2008).  
//Based  on previous work by V. Lara
//

#ifndef G4PreCompoundIon_h
#define G4PreCompoundIon_h 1

#include "G4PreCompoundFragment.hh"

class G4PreCompoundIon : public G4PreCompoundFragment
{
protected:
  // default constructor
  G4PreCompoundIon() {}

public:

  // copy constructor
  G4PreCompoundIon(const G4PreCompoundIon &right): 
    G4PreCompoundFragment(right) {}
    
  // constructor  
  G4PreCompoundIon(const G4double anA, 
		   const G4double aZ, 
		   G4VCoulombBarrier* aCoulombBarrier,
		   const G4String & aName): 
    G4PreCompoundFragment(anA,aZ,aCoulombBarrier,aName) {}
    
  virtual ~G4PreCompoundIon() {}
    
  // operators  
  const G4PreCompoundIon & 
  operator=(const G4PreCompoundIon &right) 
  {
    if (&right != this) this->G4PreCompoundFragment::operator=(right);
    return *this;
  }
    
  G4bool operator==(const G4PreCompoundIon &right) const 
  { 
    return G4PreCompoundFragment::operator==(right);
  }
    
  G4bool operator!=(const G4PreCompoundIon &right) const 
  { 
    return G4PreCompoundFragment::operator!=(right);
  }
    
  virtual G4double ProbabilityDistributionFunction(const G4double eKin, 
                                                   const G4Fragment& aFragment);

  private:

  G4bool IsItPossible(const G4Fragment& aFragment) ;
  
  protected:

  virtual G4double CrossSection(const G4double ekin)=0; 

  virtual G4double GetRj(const G4int NumberParticles, const G4int NumberCharged) = 0; 

  virtual G4double FactorialFactor(const G4double N, const G4double P) = 0;

  virtual G4double CoalescenceFactor(const G4double A) = 0; 

   };

#endif
