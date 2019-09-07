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
// $Id: G4HepMCParticle.cpp,v 1.2 2011/02/15 01:05:25 dengzy Exp $
// GEANT4 tag $Name: G4Svc-00-01-52 $
//
//

#include "G4Svc/G4HepMCParticle.h"

G4Allocator<G4HepMCParticle> aHEPEvtParticleAllocator;

G4HepMCParticle::G4HepMCParticle()
{;}

G4HepMCParticle::G4HepMCParticle(G4PrimaryParticle* pp,G4int isthep, G4int barcode)
:theParticle(pp),ISTHEP(isthep), barcodeEndVtx(barcode)
{;}

G4HepMCParticle::~G4HepMCParticle()
{;}

const G4HepMCParticle & 
G4HepMCParticle::operator=(const G4HepMCParticle &)
{ return *this; }

G4int G4HepMCParticle::operator==(const G4HepMCParticle &right) const
{ return (this==&right); }
G4int G4HepMCParticle::operator!=(const G4HepMCParticle &right) const
{ return (this!=&right); }




