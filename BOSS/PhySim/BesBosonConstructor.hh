//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Rewrite G4BesBosonConstructor.hh, remove optical photon
//Author: Liuhm 
//Created: 2003/5/19
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesBosonConstructor_h
#define BesBosonConstructor_h 1

#include "globals.hh"
#include "G4ios.hh"

class BesBosonConstructor
{
  //This class is a utility class for constructBoson 
  //short lived particles

  public:
    BesBosonConstructor();
    ~BesBosonConstructor();
  
  public:
    void ConstructParticle();
};

#endif


