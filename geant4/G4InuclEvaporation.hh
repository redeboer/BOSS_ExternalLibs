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
// $Id: G4InuclEvaporation.hh,v 1.5 2007/05/24 23:27:01 miheikki Exp $
// Defines an interface to evaporation models of Bertini cascase (BERT)
// based on INUCL code.
//
#ifndef G4INUCLEVAPORATION_h
#define G4INUCLEVAPORATION_h 1

#include "globals.hh"
#include "G4VEvaporation.hh"
#include "G4Fragment.hh"

//#define DEBUG

class G4InuclEvaporation : public G4VEvaporation {
public:
  G4InuclEvaporation();
  ~G4InuclEvaporation();

private:
  G4InuclEvaporation(const G4InuclEvaporation &right);

  const G4InuclEvaporation & operator=(const G4InuclEvaporation &right);
  G4bool operator==(const G4InuclEvaporation &right) const;
  G4bool operator!=(const G4InuclEvaporation &right) const;

  void fillResult( std::vector< G4DynamicParticle * > secondaryParticleVector,
		   G4FragmentVector * aResult );
public:

  G4FragmentVector * BreakItUp(const G4Fragment &theNucleus);
       
  void setVerboseLevel( const G4int verbose );

private:
  G4int verboseLevel;

#ifdef DEBUG

#endif

};

#endif
