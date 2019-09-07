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
// $Id: G4HepMCParticle.h,v 1.1 2007/09/03 06:45:32 dengzy Exp $
// GEANT4 tag $Name: G4Svc-00-01-52 $
//
//


#ifndef G4HepMCParticle_h
#define G4HepMCParticle_h 1

#include "globals.hh"
#include "G4Allocator.hh"
#include "G4PrimaryParticle.hh"

// class desccription:
//
//  This class is exclusively used by G4HepMCInterface.

class G4HepMCParticle 
{
  public:
      inline void *operator new(size_t);
      inline void operator delete(void *aStackedTrack);

      G4HepMCParticle();
      G4HepMCParticle(G4PrimaryParticle* pp,G4int isthep, G4int barcode);
      ~G4HepMCParticle();

      const G4HepMCParticle & operator=(const G4HepMCParticle &right);
      G4int operator==(const G4HepMCParticle &right) const;
      G4int operator!=(const G4HepMCParticle &right) const;

  private:
      G4PrimaryParticle * theParticle;
      G4int ISTHEP; // Status code of the entry
                    // Set to be 0 after generating links of
                    // G4PrimaryParticle object
      G4int barcodeEndVtx;
  public:
      inline G4PrimaryParticle * GetTheParticle()
      { return theParticle; }
      inline void Done()
      { ISTHEP *= -1; }
      inline G4int GetISTHEP()
      { return ISTHEP; }
      inline G4int GetBarcodeEndVtx()
      { return barcodeEndVtx;}
};

#if defined G4EVENT_ALLOC_EXPORT
  extern G4DLLEXPORT G4Allocator<G4HepMCParticle> aHEPEvtParticleAllocator;
#else
  extern G4DLLIMPORT G4Allocator<G4HepMCParticle> aHEPEvtParticleAllocator;
#endif

inline void * G4HepMCParticle::operator new(size_t)
{
  void * aHEPEvtParticle;
  aHEPEvtParticle = (void *) aHEPEvtParticleAllocator.MallocSingle();
  return aHEPEvtParticle;
}

inline void G4HepMCParticle::operator delete(void * aHEPEvtParticle)
{
  aHEPEvtParticleAllocator.FreeSingle((G4HepMCParticle *) aHEPEvtParticle);
}


#endif

