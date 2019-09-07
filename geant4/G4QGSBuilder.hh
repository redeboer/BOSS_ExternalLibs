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
// $Id: G4QGSBuilder.hh,v 1.1.2.1 2010/04/01 08:54:38 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:  G4QGSBuilder
//
// Author: 28 June 2009 V.Ivanchenko
//
// Modified:
//
//----------------------------------------------------------------------------
//
#ifndef G4QGSBuilder_h
#define G4QGSBuilder_h 1

#include "globals.hh"
#include "G4VHadronModelBuilder.hh"
#include "G4QGSModel.hh"
#include "G4QGSParticipants.hh"

class G4ExcitedStringDecay;
class G4QuasiElasticChannel;
class G4PreCompoundModel;
class G4ProjectileDiffractiveChannel;

class G4QGSBuilder : public G4VHadronModelBuilder
{
public: 

  G4QGSBuilder(const G4String& name ="",
	       G4PreCompoundModel* p = 0,
	       G4bool quasiElastic=true,
	       G4bool projectileDiffraction=false);

  virtual ~G4QGSBuilder();

protected:

  virtual G4HadronicInteraction* BuildModel();

private:

  // copy constructor and hide assignment operator
  G4QGSBuilder(G4QGSBuilder &);
  G4QGSBuilder & operator=(const G4QGSBuilder &right);

  G4QGSModel< G4QGSParticipants > * theQGStringModel;

  G4ExcitedStringDecay*     theQGStringDecay;
  G4QuasiElasticChannel*    theQuasiElastic;

  G4ProjectileDiffractiveChannel* theProjectileDiffraction;
  G4PreCompoundModel* thePreCompound;

  G4bool quasielFlag;
  G4bool diffFlag;

};

#endif

