//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Rewrite G4ShortLivedConstructor.hh, remove quark part
//Author: Liuhm
//Created: 2003/5/19
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesShortLivedConstructor_h
#define BesShortLivedConstructor_h 1

#include "globals.hh"
#include "G4ios.hh"

class BesShortLivedConstructor
{
  //This class is a utility class for construction 
  //short lived particles

  public:
    BesShortLivedConstructor();
    ~BesShortLivedConstructor();
  
  public:
    void ConstructParticle();
 
  protected:
    void ConstructResonances();
    void ConstructBaryons();
    void ConstructMesons();

  private:
    static G4bool isConstructed;
    // flag for checking whether resonces exist or not
};

#endif











