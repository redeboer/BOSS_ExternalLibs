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
// $Id: G4Hyperbola.hh,v 1.10 2006/06/29 18:39:36 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// ----------------------------------------------------------------------
// Class G4Hyperbola
//
// Class description:
// 
// Definition of a generic hyperbola.

// Authors: J.Sulkimo, P.Urban.
// Revisions by: L.Broglia, G.Cosmo.
// ----------------------------------------------------------------------
#ifndef __HYPERBOLICCURVE_H
#define __HYPERBOLICCURVE_H 

#include "G4Conic.hh"

class G4Hyperbola : public G4Conic
{

public:  // with description

  G4Hyperbola();
  virtual ~G4Hyperbola();
    // Constructor & destructor.

  G4Hyperbola(const G4Hyperbola& right);
  G4Hyperbola& operator=(const G4Hyperbola& right);
    // Copy constructor and assignment operator.

  G4Curve* Project(const G4Transform3D& tr=
                   HepGeom::Transform3D::Identity);
    // Transforms and projects the curve.

  G4bool Tangent(G4CurvePoint& cp, G4Vector3D& v);
    // Returns tangent to curve at a given point, if existing.
    // The tangent is computed from the 3D point representation.

  inline G4double  GetPMax() const;
  inline G4Point3D GetPoint(G4double param) const;
  inline G4double  GetPPoint(const G4Point3D& p) const;
    // Accessors methods.

  inline G4double GetSemiAxis() const;
  inline G4double GetSemiImagAxis() const;
  inline void Init(G4Axis2Placement3D position0,
	           G4double semiAxis0, G4double semiImagAxis0);
    // Get/Set for the geometric data.

public:  // without description

  inline G4int IntersectRay2D(const G4Ray& ray);

  //void IntersectRay2D(const G4Ray& ray, G4CurveRayIntersection& is);
  //G4Hyperbola(G4Point3d, G4Point3d, G4Point3d, 
  //            G4Point3d, G4double, G4double );
  //G4Point3d EvaluateByParameterValue(G4double u);
  //G4Point3d GetBoundMax();
  //G4Point3d GetBoundMin();    

protected:

  void InitBounded();

private:

  G4int Inside(const G4Point3D&, const G4Ray&);

private:

  G4Point3D Focus1;
  G4Point3D Focus2;    
  G4Point3D ProjFocus1;
  G4Point3D ProjFocus2; 

  // geometric data
  G4double semiAxis;
  G4double semiImagAxis;

  G4double ratioAxisImagAxis;

  G4Transform3D toUnitHyperbola;

  G4double forTangent;
    // R_1^2/R_2^2
};

#include "G4Hyperbola.icc"

#endif
