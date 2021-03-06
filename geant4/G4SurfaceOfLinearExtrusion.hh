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
// $Id: G4SurfaceOfLinearExtrusion.hh,v 1.5 2006/06/29 18:40:53 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// ----------------------------------------------------------------------
// Class G4SurfaceOfLinearExtrusion
//
// Class description:
// 
// Definition of a surface of linear extrusion.
// (not implemented yet).

// ----------------------------------------------------------------------
#ifndef included_G4SurfaceOfLinearExtrusion
#define included_G4SurfaceOfLinearExtrusion

#include "G4Surface.hh"

class G4SurfaceOfLinearExtrusion : public G4Surface
{

public:  // with description

  G4SurfaceOfLinearExtrusion();
  virtual ~G4SurfaceOfLinearExtrusion();
    // Constructor & destructor.

private:

  G4SurfaceOfLinearExtrusion(const G4SurfaceOfLinearExtrusion &);
  G4SurfaceOfLinearExtrusion& operator=(const G4SurfaceOfLinearExtrusion &);
    // Private copy constructor and assignement operator.

};

#endif
