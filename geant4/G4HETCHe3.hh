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
// by V. Lara

#ifndef G4HETCHe3_h
#define G4HETCHe3_h 1

#include "G4HETCChargedFragment.hh"
#include "G4ReactionProduct.hh"
#include "G4He3.hh"

#include "G4He3CoulombBarrier.hh"


class G4HETCHe3 : public G4HETCChargedFragment
{
public:
  // default constructor
  G4HETCHe3():G4HETCChargedFragment(3,2,&theHe3CoulombBarrier,"He3") {}

  // copy constructor
  G4HETCHe3(const G4HETCHe3 &right): G4HETCChargedFragment(right) {}

  // destructor
  ~G4HETCHe3() {}

  // operators  
  const G4HETCHe3 & operator=(const G4HETCHe3 &right) 
  {
    if (&right != this) this->G4HETCChargedFragment::operator=(right);
    return *this;
  }
  
  G4bool operator==(const G4HETCHe3 &right) const
  { 
    return G4HETCChargedFragment::operator==(right);
  }

  
  G4bool operator!=(const G4HETCHe3 &right) const
  { 
    return G4HETCChargedFragment::operator!=(right);
  }


  G4ReactionProduct * GetReactionProduct() const
  {
    G4ReactionProduct * theReactionProduct =
      new G4ReactionProduct(G4He3::He3Definition());
    theReactionProduct->SetMomentum(GetMomentum().vect());
    theReactionProduct->SetTotalEnergy(GetMomentum().e());
#ifdef PRECOMPOUND_TEST
    theReactionProduct->SetCreatorModel("G4PrecompoundModel");
#endif
    return theReactionProduct;
  }   
    
private:
  virtual G4double GetAlpha()
  {
    G4double C = 0.0;
    G4double aZ = GetZ() + GetRestZ();
    if (aZ <= 30) 
      {
	C = 0.10;
      } 
    else if (aZ <= 50) 
      {
	C = 0.1 + -((aZ-50.)/20.)*0.02;
      } 
    else if (aZ < 70) 
      {
	C = 0.08 + -((aZ-70.)/20.)*0.02;
      }
    else 
      {
	C = 0.06;
      }
    return 1.0 + C*(4.0/3.0);
  }
  

  virtual G4double GetBeta()
  {
    return -GetCoulombBarrier();
  }
  
  //   virtual G4double FactorialFactor(const G4double N, const G4double P)
  //   {
  //     return 
  //       (N-3.0)*(P-2.0)*(
  // 		       (((N-2.0)*(P-1.0))/2.0) *(
  // 						 (((N-1.0)*P)/3.0) 
  // 						 )
  // 		       );
  //   }
  
  virtual G4double GetSpinFactor()
  {
    // 2s+1
    return 2.0;
  }    

  virtual G4double K(const G4Fragment & aFragment);

private:

  G4He3CoulombBarrier theHe3CoulombBarrier;

};

#endif
