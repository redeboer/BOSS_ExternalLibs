//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Re-write G4MesonConstructor.hh for BES, remeove Bottom mesons
//Author: Liuhm 
//Created: 2003/05/19
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesMesonConstructor_h
#define BesesonConstructor_h 1

#include "globals.hh"
#include "G4ios.hh"

class BesMesonConstructor
{
  //This class is a utility class for construction 
  //short lived particles

  public:
    BesMesonConstructor();
    ~BesMesonConstructor();
  
  public:
    void ConstructParticle();

  protected:
    void ConstructLightMesons();
    void ConstructCharmMesons();
};

#endif
