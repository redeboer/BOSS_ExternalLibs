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
// $Id: G4VImportanceAlgorithm.hh,v 1.8 2006/06/29 18:16:40 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// ----------------------------------------------------------------------
// Class G4VImportanceAlgorithm
//
// Class description:
//
// This is an interface used by importance sampling to get the 
// number of copies and weight a mother particle should be split 
// into when crossing a boundary of "importance cells". 
// The interface defines the input to be the ratio of the pre over
// the post importance and the weight of the mother track.
// It returns a struct containing the number of copies (including 
// the mother track) to be produced and the weight of each track.
// A user defined algorithm deriving from this interface may be used
// by the importance sampling.

// Author: Michael Dressel (Michael.Dressel@cern.ch)
// ----------------------------------------------------------------------
#ifndef G4VImportanceAlgorithm_hh
#define G4VImportanceAlgorithm_hh G4VImportanceAlgorithm_hh 

#include "G4Nsplit_Weight.hh"

class G4VImportanceAlgorithm
{

public:  // with description

  G4VImportanceAlgorithm();
  virtual ~G4VImportanceAlgorithm();
  virtual G4Nsplit_Weight Calculate(G4double ipre,
				    G4double ipost,
                                    G4double init_w) const = 0;
    // calculate the number of tracks and their weight according 
    // to the  pre and post importance value and the weight of
    // the mother track.
};

#endif
