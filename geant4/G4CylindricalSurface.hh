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
// $Id: G4CylindricalSurface.hh,v 1.10 2006/06/29 18:39:12 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// ----------------------------------------------------------------------
// Class G4CylindricalSurface
//
// Class Description:
//   
// Definition of a generic cylindrical surface.

// The code for G4CylindricalSurface has been derived from the original
// implementation in the "Gismo" package.
//
// Author: A.Breakstone
// Adaptation: J.Sulkimo, P.Urban.
// Revisions by: L.Broglia, G.Cosmo.
// ----------------------------------------------------------------------
#ifndef __G4CYLINDERSURFACE_H
#define __G4CYLINDERSURFACE_H

#include "G4Surface.hh"

class G4CylindricalSurface : public G4Surface
{

 public:  // with description

  G4CylindricalSurface();
    // Default constructor.

  G4CylindricalSurface( const G4Vector3D& o, const G4Vector3D& a, G4double r ); 
    // Normal constructor:
    // - first argument is the origin of the G4CylindricalSurface
    // - second argument is the axis of the G4CylindricalSurface
    // - third argument is the radius of the G4CylindricalSurface.

  virtual ~G4CylindricalSurface();
    // Destructor.

  inline G4int operator==( const G4CylindricalSurface& c ) const;
    // Equality operator.

  inline G4String GetEntityType() const;
    // Returns the shape identifier.

  virtual const char* NameOf() const;
    // Returns the class name.

  virtual void PrintOn( std::ostream& os = G4cout ) const;
    // Printing function, streaming surface's attributes.

  virtual G4double HowNear( const G4Vector3D& x ) const;
    // Returns the distance from a point to a G4CylindricalSurface.
    // The point x is the (input) argument.
    // The distance is positive if the point is Inside, negative otherwise.

  virtual G4Vector3D Normal( const G4Vector3D& p ) const;
    // Returns the Normal unit vector to a G4CylindricalSurface at a point p
    // on (or nearly on) the G4CylindricalSurface.

  virtual G4Vector3D SurfaceNormal( const G4Point3D& p ) const;	
    // Returns the Normal unit vector to the G4CylindricalSurface at a point 
    // p on (or nearly on) the G4CylindricalSurface.

  virtual G4int Inside( const G4Vector3D& x ) const;
    // Returns 1 if the point x is Inside the G4CylindricalSurface,
    // returns 0 otherwise.
    // Outside means that the distance to the G4CylindricalSurface would 
    // be negative.
    // Uses the HowNear() function to calculate this distance.

  virtual G4int WithinBoundary( const G4Vector3D& x ) const;
    // Function overwritten by finite-sized derived classes which returns
    // 1 if the point x is within the boundary, 0 otherwise.
    // Since a G4CylindricalSurface is infinite in extent, the function will
    // just check if the point is on the G4CylindricalSurface (to the surface
    // precision).

  virtual G4double Scale() const;
    // Function overwritten by finite-sized derived classes which returns
    // the radius, unless it is zero, in which case it returns the smallest
    // non-zero dimension. 
    // Used for Scale-invariant tests of surface thickness.

  G4int Intersect(const G4Ray& ry);
    // Returns the distance along a Ray (straight line with G4Vector3D) to
    // leave or enter a G4CylindricalSurface.
    // If the G4Vector3D of the Ray is opposite to that of the Normal to
    // the G4CylindricalSurface at the intersection point, it will not leave 
    // the G4CylindricalSurface.
    // Similarly, if the G4Vector3D of the Ray is along that of the Normal 
    // to the G4CylindricalSurface at the intersection point, it will not enter
    // the G4CylindricalSurface.
    // This method is called by all finite shapes sub-classed to 
    // G4CylindricalSurface.
    // A negative result means no intersection.
    // If no valid intersection point is found, the distance and intersection
    // point are set to large numbers.

  inline G4Vector3D GetAxis() const;
  inline G4double GetRadius() const;
    // Return the axis and radius of the G4CylindricalSurface.

  void SetRadius( G4double r );
    // Changes the radius of the G4CylindricalSurface.
    // Requires radius to be non-negative.

 public:  // without description

/*
  virtual G4double distanceAlongRay( G4int which_way, const G4Ray* ry,
                                     G4Vector3D& p ) const;
    // Returns the distance along a Ray to enter or leave a
    // G4CylindricalSurface. Arguments:
    // - first (input) argument is +1 to leave or -1 to enter
    // - second (input) argument is a pointer to the Ray
    // - third (output) argument returns the intersection point.

  virtual G4double distanceAlongHelix( G4int which_way, const Helix* hx,
                                       G4Vector3D& p ) const;
    // Returns the distance along a Helix to enter or leave a
    // G4CylindricalSurface. Arguments:
    // - first (input) argument is +1 to leave or -1 to enter
    // - second (input) argument is a pointer to the Helix
    // - third (output) argument returns the intersection point.

  virtual void rotate( G4double alpha, G4double beta, 
                       G4double gamma, G4ThreeMat& m, G4int inverse ); 
    // Rotates the G4CylindricalSurface (the angles are assumed to be given
    // in radians). Arguments:
    // - first about global x-axis by angle alpha,
    // - second about global y-axis by angle beta,
    // - third about global z-axis by angle gamma
    // - fourth (output) argument gives the calculated rotation matrix
    // - fifth (input) argument is an integer flag which if non-zero
    //   reverses the order of the rotations

  virtual void rotate( G4double alpha, G4double beta, 
                       G4double gamma, G4int inverse ); 
    // Rotates the G4CylindricalSurface (the angles are assumed to be given
    // in radians). Arguments: 
    // - first about global x-axis by angle alpha,
    // - second about global y-axis by angle beta,
    // - third about global z-axis by angle gamma
    // - fourth (input) argument is an integer flag which if non-zero
    //   reverses the order of the rotations
*/


 protected:          // make available to derived classes

  G4Vector3D axis;
    // Direction of axis of G4CylindricalSurface (unit vector).

  G4double radius;
    // Radius of G4CylindricalSurface.


 private:

  G4CylindricalSurface(const G4CylindricalSurface&);
  G4CylindricalSurface& operator=(const G4CylindricalSurface&);
    // Private copy constructor and assignment operator.

/*
  virtual G4double gropeAlongHelix( const Helix* hx ) const;
    // Private function to use a crude technique to find the intersection
    // of a Helix with a G4CylindricalSurface. It returns the turning angle
    // along the Helix at which the intersection occurs or -1.0 if no
    // intersection point is found.  The argument to the call is the pointer
    // to the Helix.
*/

};

#include "G4CylindricalSurface.icc"

#endif

