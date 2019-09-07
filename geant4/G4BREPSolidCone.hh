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
// $Id: G4BREPSolidCone.hh,v 1.11 2006/06/29 18:37:51 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// ----------------------------------------------------------------------
// Class G4BREPSolidCone
//
// Class description:
//
//  Definition of a generic BREP cone.
//
//  G4BREPSolidCone(const G4String& name,
//                  const G4ThreeVector& origin,
//                  const G4ThreeVector& axis,
//                  const G4ThreeVector& direction,
//                        G4double length,
//                        G4double radius,
//                        G4double large_radius)

// Authors: J.Sulkimo, P.Urban.
// Revisions by: L.Broglia, G.Cosmo.
// ----------------------------------------------------------------------
#ifndef __G4BREPSolidCone
#define __G4BREPSolidCone

#include "G4BREPSolid.hh"

class G4BREPSolidCone : public G4BREPSolid
{
 public:  // with description

  G4BREPSolidCone(const G4String& name, const G4ThreeVector& origin,
                  const G4ThreeVector& axis, const G4ThreeVector& direction,		   
                  G4double length, G4double radius, G4double large_radius);
    // Constructor

  ~G4BREPSolidCone();
    // Empty destructor.

  void Initialize();
    // Computes the bounding box for solids and surfaces.
    // Converts concave planes to convex.

  EInside Inside(register const G4ThreeVector& Pt) const;
    // Determines if the point Pt is inside, outside or on the surface
    // of the solid.

  G4ThreeVector SurfaceNormal(const G4ThreeVector& p) const;
    // Returns the outwards pointing unit normal of the shape for the
    // surface closest to the point at offset p.

  G4double DistanceToIn(const G4ThreeVector& p) const;
    // Calculate the distance to the nearest surface of a shape from an
    // outside point p. The distance can be an underestimate.

  G4double DistanceToIn(register const G4ThreeVector& p, 
			register const G4ThreeVector& v) const;
    // Returns the distance along the normalised vector v to the shape,
    // from the point at offset p. If there is no intersection, returns
    // kInfinity. The first intersection resulting from `leaving' a
    // surface/volume is discarded. Hence, it is tolerant of points on
    // the surface of the shape.

  G4double DistanceToOut(register const G4ThreeVector& p, 
			 register const G4ThreeVector& v, 
			 const G4bool calcNorm=false, 
			 G4bool *validNorm=0, G4ThreeVector *n=0) const;
    // Returns the distance along the normalised vector v to the shape,
    // from a point at an offset p inside or on the surface of the shape.

  G4double DistanceToOut(const G4ThreeVector& p) const;
    // Calculates the distance to the nearest surface of a shape from an
    // inside point. The distance can be an underestimate.

  virtual std::ostream& StreamInfo(std::ostream& os) const;
    // Streams solid contents to output stream.

 public:  // without description

  G4BREPSolidCone(__void__&);
    // Fake default constructor for usage restricted to direct object
    // persistency for clients requiring preallocation of memory for
    // persistifiable objects.

 private:

  G4BREPSolidCone(const G4BREPSolidCone&);
  G4BREPSolidCone& operator=(const G4BREPSolidCone&);
    // Private copy constructor and assignment operator.
  
 private:
     
  struct G4BREPConeParams
  {
    G4ThreeVector origin;
    G4ThreeVector axis;
    G4ThreeVector direction;
    G4double      length;
    G4double      radius;
    G4double      large_radius;
  } constructorParams;

};

#endif
