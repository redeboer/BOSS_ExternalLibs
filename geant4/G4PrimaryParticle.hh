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
// $Id: G4PrimaryParticle.hh,v 1.4 2006/09/28 14:29:43 kurasige Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//


#ifndef G4PrimaryParticle_h
#define G4PrimaryParticle_h 1

#include "globals.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class G4ParticleDefinition;
class G4VUserPrimaryParticleInformation;

// class description:
//
//  This is a class which represents a primary particle.
// This is completely deferent class from G4Track or G4DynamicParticle.
// This class is designed with taking into account the possibility of
// making this class persistent, i.e. kept with G4Event class object
// to ODBMS. Thus this class is almost free from any other Geant4 classes.
// The only exception is a pointer to G4ParticleDefinition but it can be
// rebuilt by the PDGcode.
//
//  Primary particles are stored in G4PrimaryVertex object with a form
// of linked list. Also, an object of this PrimaryParticle class can have
// one or more objects of this class as its daughters with a form of 
// linked list.
//  A parimary particle represented by this class object needs not to be
// a particle of type which Geant4 can simulate.
//  case a) mother particle is not a particle Geant4 can simulate
//   daughters associated to the mother will be examined.
//  case b) mother particle is a perticle Geant4 can simulate
//   daughters associated to the mother will be converted to G4Dynamic 
//   particle and be set to the mother G4Track. For this case, dauthers
//   are used as the "pre-fixed" decay channel.
//

class G4PrimaryParticle 
{
  public:
      inline void *operator new(size_t);
      inline void operator delete(void *aStackedTrack);

      G4PrimaryParticle();
      G4PrimaryParticle(G4int Pcode);
      G4PrimaryParticle(G4int Pcode,
                        G4double px,G4double py,G4double pz);
      G4PrimaryParticle(G4int Pcode,
                        G4double px,G4double py,G4double pz,G4double E);
      G4PrimaryParticle(G4ParticleDefinition* Gcode);
      G4PrimaryParticle(G4ParticleDefinition* Gcode,
                        G4double px,G4double py,G4double pz);
      G4PrimaryParticle(G4ParticleDefinition* Gcode,
                        G4double px,G4double py,G4double pz,G4double E);
      ~G4PrimaryParticle();

      const G4PrimaryParticle & operator=(const G4PrimaryParticle &right);
      G4int operator==(const G4PrimaryParticle &right) const;
      G4int operator!=(const G4PrimaryParticle &right) const;

  public: // with description
      void Print() const;
      // Print the properties of the particle.

  private:
      G4int PDGcode;
      G4ParticleDefinition * G4code;
      G4double Px;
      G4double Py;
      G4double Pz;
      
      G4PrimaryParticle * nextParticle;
      G4PrimaryParticle * daughterParticle;

      G4int trackID;  // This will be set if this particle is
                      // sent to G4EventManager and converted to
                      // G4Track. Otherwise = -1.

      G4double mass;  
      G4double charge;
      G4double polX;
      G4double polY;
      G4double polZ;
      G4double Weight0;
      G4double properTime;
      G4VUserPrimaryParticleInformation* userInfo;

  public: // with description
      // followings are get methods available.
      //   "trackID" will be set if this particle is sent to G4EventManager 
      //    and converted to G4Track. Otherwise = -1.
      //    The mass and charge in G4ParticleDefinition will be used in default.
      //   "SetMass" and "SetCharge" methods are used to set dynamical mass and charge 
      //   G4DynamicParticle."GetMass" and "GetCharge" methods will return 
      //   those in G4ParticleDefinition if users do not set dynamical ones. 

      G4double GetMass() const;
      G4double GetCharge() const;

      inline G4int GetPDGcode() const
      { return PDGcode; }
      inline G4ParticleDefinition * GetG4code() const
      { return G4code; }
      inline G4ThreeVector GetMomentum() const
      { return G4ThreeVector(Px,Py,Pz); }
      inline G4double GetPx() const
      { return Px; }
      inline G4double GetPy() const
      { return Py; }
      inline G4double GetPz() const
      { return Pz; }
      inline G4PrimaryParticle * GetNext() const
      { return nextParticle; }
      inline G4PrimaryParticle * GetDaughter() const
      { return daughterParticle; }
      inline G4int GetTrackID() const
      { return trackID; }
      inline G4ThreeVector GetPolarization() const
      { return G4ThreeVector(polX,polY,polZ); }
      inline G4double GetPolX() const { return polX; }
      inline G4double GetPolY() const { return polY; }
      inline G4double GetPolZ() const { return polZ; }
      inline G4double GetWeight() const { return Weight0; }
      inline void SetWeight(G4double w) { Weight0 = w; }
      inline void SetProperTime(G4double t) { properTime = t; }
      inline G4double GetProperTime() const { return properTime; }
      inline void SetUserInformation(G4VUserPrimaryParticleInformation* anInfo)
      { userInfo = anInfo; }
      inline G4VUserPrimaryParticleInformation* GetUserInformation() const
      { return userInfo; }

  public: // with description
      // Followings are available Set methods.
      void SetPDGcode(G4int Pcode);
      void SetG4code(G4ParticleDefinition * Gcode);
      inline void SetMomentum(G4double px, G4double py, G4double pz)
      { 
        Px = px;
        Py = py;
        Pz = pz; 
      }
      inline void Set4Momentum(G4double px, G4double py, G4double pz, G4double E)
      { 
        Px = px;
        Py = py;
        Pz = pz; 
        G4double mas2 = E*E - px*px - py*py - pz*pz;
        if(mas2>=0.)
        { mass = std::sqrt(mas2); }
        else
        { mass = -1.0; }
      }
      inline void SetNext(G4PrimaryParticle * np)
      { 
        if(nextParticle == 0)
        { nextParticle = np; }
        else
        { nextParticle->SetNext(np); }
      }
      inline void SetDaughter(G4PrimaryParticle * np)
      { 
        if(daughterParticle == 0)
        { daughterParticle = np; }
        else
        { daughterParticle->SetNext(np); }
      }
      inline void SetTrackID(G4int id)
      { trackID = id; }
      inline void SetMass(G4double mas)
      { mass = mas; }
      inline void SetCharge(G4double chg)
      { charge = chg; }
     inline void SetPolarization(G4double px,G4double py,G4double pz)
      {
        polX = px;
        polY = py;
        polZ = pz;
      }
};

#if defined G4PARTICLES_ALLOC_EXPORT
  extern G4DLLEXPORT G4Allocator<G4PrimaryParticle> aPrimaryParticleAllocator;
#else
  extern G4DLLIMPORT G4Allocator<G4PrimaryParticle> aPrimaryParticleAllocator;
#endif

inline void * G4PrimaryParticle::operator new(size_t)
{
  void * aPrimaryParticle;
  aPrimaryParticle = (void *) aPrimaryParticleAllocator.MallocSingle();
  return aPrimaryParticle;
}

inline void G4PrimaryParticle::operator delete(void * aPrimaryParticle)
{
  aPrimaryParticleAllocator.FreeSingle((G4PrimaryParticle *) aPrimaryParticle);
}


#endif

