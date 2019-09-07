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
// $Id: G4Sphere.hh,v 1.24 2009/03/31 07:51:49 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//
// --------------------------------------------------------------------
// GEANT 4 class header file
//
// G4Sphere
//
// Class description:
//
//   A G4Sphere is, in the general case, a section of a spherical shell,
//   between specified phi and theta angles
//
//   The phi and theta segments are described by a starting angle,
//   and the +ve delta angle for the shape.
//   If the delta angle is >=2*pi, or >=pi the shape is treated as
//   continuous in phi or theta respectively.
//
//   Theta must lie between 0-pi (incl).
//
//   Member Data:
//
//   fRmin  inner radius
//   fRmax  outer radius
//
//   fSPhi  starting angle of the segment in radians
//   fDPhi  delta angle of the segment in radians
//
//   fSTheta  starting angle of the segment in radians
//   fDTheta  delta angle of the segment in radians
//
//     
//   Note:
//      Internally fSPhi & fDPhi are adjusted so that fDPhi<=2PI,
//      and fDPhi+fSPhi<=2PI. This enables simpler comparisons to be
//      made with (say) Phi of a point.

// History:
// 28.3.94 P.Kent: old C++ code converted to tolerant geometry
// 17.9.96 V.Grichine: final modifications to commit
// --------------------------------------------------------------------

#ifndef G4Sphere_HH
#define G4Sphere_HH

#include "G4CSGSolid.hh"

class G4VisExtent;

class G4Sphere : public G4CSGSolid
{
  public:  // with description

    G4Sphere(const G4String& pName,
                   G4double pRmin, G4double pRmax,
                   G4double pSPhi, G4double pDPhi,
                   G4double pSTheta, G4double pDTheta);
      //
      // Constructs a sphere or sphere shell section
      // with the given name and dimensions
       
   ~G4Sphere();
      //
      // Destructor

    // Accessors
       
    inline G4double GetInnerRadius    () const;
    inline G4double GetOuterRadius    () const;
    inline G4double GetStartPhiAngle  () const;
    inline G4double GetDeltaPhiAngle  () const;
    inline G4double GetStartThetaAngle() const;
    inline G4double GetDeltaThetaAngle() const;

    // Modifiers

    inline void SetInnerRadius    (G4double newRMin);
    inline void SetOuterRadius    (G4double newRmax);
    inline void SetStartPhiAngle  (G4double newSphi, G4bool trig=true);
    inline void SetDeltaPhiAngle  (G4double newDphi);
    inline void SetStartThetaAngle(G4double newSTheta);
    inline void SetDeltaThetaAngle(G4double newDTheta);

    // Methods for solid

    inline G4double GetCubicVolume();
    G4double GetSurfaceArea();

    void ComputeDimensions(      G4VPVParameterisation* p,
                           const G4int n,
                           const G4VPhysicalVolume* pRep);

    G4bool CalculateExtent(const EAxis pAxis,
                           const G4VoxelLimits& pVoxelLimit,
                           const G4AffineTransform& pTransform,
                                 G4double& pmin, G4double& pmax) const;
         
    EInside Inside(const G4ThreeVector& p) const;

    G4ThreeVector SurfaceNormal( const G4ThreeVector& p) const;

    G4double DistanceToIn(const G4ThreeVector& p,
                          const G4ThreeVector& v) const;
    
    G4double DistanceToIn(const G4ThreeVector& p) const;
    
    G4double DistanceToOut(const G4ThreeVector& p,
                           const G4ThreeVector& v,
                           const G4bool calcNorm=G4bool(false),
                                 G4bool *validNorm=0,
                                 G4ThreeVector *n=0) const;
         
    G4double DistanceToOut(const G4ThreeVector& p) const;

    G4GeometryType GetEntityType() const;
 
    G4ThreeVector GetPointOnSurface() const;

    std::ostream& StreamInfo(std::ostream& os) const;

    // Visualisation functions

    G4VisExtent   GetExtent          () const;    
    void          DescribeYourselfTo(G4VGraphicsScene& scene) const;
    G4Polyhedron* CreatePolyhedron() const;
    G4NURBS*      CreateNURBS() const;
  
  public:  // without description
   
    G4Sphere(__void__&);
      //
      // Fake default constructor for usage restricted to direct object
      // persistency for clients requiring preallocation of memory for
      // persistifiable objects.

    // Old access functions

    inline G4double  GetRmin()   const;
    inline G4double  GetRmax()   const;
    inline G4double  GetSPhi()   const;
    inline G4double  GetDPhi()   const;
    inline G4double  GetSTheta() const;
    inline G4double  GetDTheta() const;
    inline G4double  GetInsideRadius() const;
    inline void SetInsideRadius(G4double newRmin);

  private:
 
    G4ThreeVectorList*
    CreateRotatedVertices(const G4AffineTransform& pTransform,
                                G4int& noPolygonVertices) const;
      //
      // Creates the List of transformed vertices in the format required
      // for G4VSolid:: ClipCrossSection and ClipBetweenSections

    inline void Initialize();
      //
      // Reset relevant values to zero

    inline void CheckThetaAngles(G4double sTheta, G4double dTheta);
    inline void CheckSPhiAngle(G4double sPhi);
    inline void CheckDPhiAngle(G4double dPhi);
    inline void CheckPhiAngles(G4double sPhi, G4double dPhi);
      //
      // Reset relevant flags and angle values

    inline void InitializePhiTrigonometry();
    inline void InitializeThetaTrigonometry();
      //
      // Recompute relevant trigonometric values and cache them

    G4ThreeVector ApproxSurfaceNormal( const G4ThreeVector& p) const;
      //
      // Algorithm for SurfaceNormal() following the original
      // specification for points not on the surface

  private:

    // Used by distanceToOut
    //
    enum ESide {kNull,kRMin,kRMax,kSPhi,kEPhi,kSTheta,kETheta};
  
    // used by normal
    //
    enum ENorm {kNRMin,kNRMax,kNSPhi,kNEPhi,kNSTheta,kNETheta};

    G4double fEpsilon, fRminTolerance, fRmaxTolerance, kAngTolerance;
      //
      // Radial and angular tolerances

    G4double fRmin, fRmax, fSPhi, fDPhi, fSTheta, fDTheta;
      //
      // Radial and angular dimensions

    G4double sinCPhi, cosCPhi, cosHDPhiOT, cosHDPhiIT,
             sinSPhi, cosSPhi, sinEPhi, cosEPhi, hDPhi, cPhi, ePhi;
      //
      // Cached trigonometric values for Phi angle

    G4double sinSTheta, cosSTheta, sinETheta, cosETheta,
             tanSTheta, tanSTheta2, tanETheta, tanETheta2, eTheta;
      //
      // Cached trigonometric values for Theta angle

    G4bool fFullPhiSphere, fFullThetaSphere, fFullSphere;
      //
      // Flags for identification of section, shell or full sphere
};

#include "G4Sphere.icc"

#endif
