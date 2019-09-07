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
// $Id: G4FConicalSurface.hh,v 1.13 2006/06/29 18:39:21 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// ----------------------------------------------------------------------
// Class G4FConicalSurface
//
// Class description:
// 
// Definition of a generic conical surface.
/*
       Position.axis|
                    |
      --         ---|---   small_radius  
    l  |        /   |   \
    e  |       /    |    \
    n  |      /     |     \
    g  |     /      |      \
    t  |    /       |       \
    h  |   /        |        \
      --   ---------|---------  large_radius
                 Position
*/

// The code for G4FConicalSurface has been derived from the original
// implementation in the "Gismo" package.
//
// Author:  Alan Breakstone
// Adaptation: J.Sulkimo, P.Urban.
// Revisions by: L.Broglia, G.Cosmo.
// ----------------------------------------------------------------------
#ifndef __FCONIC_H
#define __FCONIC_H

#include "G4PointRat.hh"
#include "G4Axis2Placement3D.hh"
#include "G4Surface.hh"

class G4FConicalSurface : public G4Surface
{

 public:  // with description
  
  G4FConicalSurface();
  virtual ~G4FConicalSurface();
    // Default constructor and destructor.

  G4FConicalSurface( const G4Point3D& o, const G4Vector3D& a,
		     G4double l, G4double sr, G4double lr );
    // o : origin of the G4FConicalSurface.
    // a : axis of the G4FConicalSurface.
    // l : length of the G4FConicalSurface.
    // sl: small radius of the G4FConicalSurface.
    // lr: large radius of the G4FConicalSurface.

  virtual G4Vector3D SurfaceNormal( const G4Point3D& p ) const;	
    // Returns the normal to the surface on point p.

  G4int Inside( const G4Vector3D& x ) const;
    // Returns 0 if point x is outside G4ConicalSurface, 1 if Inside.

  inline G4String GetEntityType() const;
    // Returns the type identifier.

  virtual const char* Name() const;
    // Returns the class type name.

  virtual void PrintOn( std::ostream& os = G4cout ) const;
    // Printing function.

  G4int operator==( const G4FConicalSurface& c ) const;
    // Equality operator.

  G4int Intersect( const G4Ray& ry );
    // Counts the number of intersections of a bounded conical surface by a ray.
    // At first, calculates the intersections with the semi-infinite 
    // conical surface; then, it counts the intersections within the
    // finite conical surface boundaries, and sets the "distance" to the 
    // closest distance from the start point to the nearest intersection.
    // If the point is on the surface it returns or the intersection with
    // the opposite surface or kInfinity.
    // If no intersection is found, it sets distance = kInfinity and returns 0.

  void CalcBBox();
    // Computes the bounding-box.

  virtual G4double HowNear( const G4Vector3D& x ) const;
    // Computes the shortest distance from the point x to the G4FConicalSurface.
    // The distance will always be positive.
    // This function works only with Cone axis equal (0,0,1) or (0,0,-1),
    // it projects the surface and the point on the x,z plane and computes
    // the distance in analytical way.

  virtual G4int WithinBoundary( const G4Vector3D& x ) const;
    // Returns 1 if the point x is within the boundary, returns 0 otherwise.

  virtual G4double Scale() const;
    // Returns the size of a G4FConicalSurface.
    // Used for Scale-invariant tests of surface thickness.
    // If the small radius is zero, returns the large radius.

  virtual G4double Area() const;
    // Calculates the area of a G4FConicalSurface.

  virtual void resize( G4double l, G4double sr, G4double lr );
    // Changes the radii and length of the G4FConicalSurface.
    //	- l  (input) argument: the new length
    //	- sr (input) argument: the new small radius
    //	- lr (input) argument: the new large radius


  inline G4double GetLength()      const;
  inline G4double GetSmallRadius() const;
  inline G4double GetLargeRadius() const;
  inline G4double GetTan_Angle()   const;
    // Accessors to dimensions of the G4FConicalSurface.

protected:

  G4double length;
    // length of G4FConicalSurface

  G4double small_radius;
    // small radius of G4FConicalSurface, can be zero
  G4double large_radius;
    // large radius of G4FConicalSurface, must be greater than the small
    // radius. Note that the angle of the G4ConicalSurface is calculated
    // from these three quantities.
	
  G4Axis2Placement3D Position;
  G4double tan_angle;

private:

  G4FConicalSurface(const G4FConicalSurface&);
  G4FConicalSurface& operator=(const G4FConicalSurface&);
    // Private copy constructor and assignment operator.

};

#include "G4FConicalSurface.icc"

#endif
