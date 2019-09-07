//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4IrregBox.hh,v 1.0 2003/09/30 21:26:00 ates Exp $
// GEANT4 tag $Name: geant4-05-02-patch-01 $
//
//
// --------------------------------------------------------------------
// GEANT 4 class header file
//
// G4IrregBox
//
// Class description:
//
//   A G4IrregBox is a general boxoid: No face is not necessarily
//   perpendicular to the any other face.  However 6 faces must exist
//   and they must form some sort of deformed box, trapiziod, or
//   whatever.
//
//   Note that only 1 parameter as an array of 8 points is required and
//   they are the points of the boxoid.  They must be ordered so that
//   the "bottom" 4 points are oriented roughly below the "top" four
//    ordered point.
//
//      pPnt[0] 1st top point: connected to Pnt2, Pnt4, and Pnt5
//      pPnt[1] 2nd top point: connected to Pnt1, Pnt3, and Pnt6
//      pPnt[2] 3rd top point: connected to Pnt2, Pnt4, and Pnt7
//      pPnt[3] 4th top point: connected to Pnt1, Pnt3, and Pnt8
//      pPnt[4] 1st bottom point: connected to Pnt6, Pnt8, and Pnt1
//      pPnt[5] 2nd bottom point: connected to Pnt5, Pnt7, and Pnt2
//      pPnt[6] 3rd bottom point: connected to Pnt6, Pnt8, and Pnt3
//      pPnt[7] 4th bottom point: connected to Pnt5, Pnt7, and Pnt4
//
//   Member Data:
//
//      G4ThreeVector fPnt[8]      "corners" of boxoid.
//                                 NOTE: Order is Important !!!
//      IrregBoxPlane fPlanes[8]   Plane equations of all faces.
//                                 NOTE: Order is Important !!!
//
// History:
//
// 30/09/2003 K.Ates: Created
// --------------------------------------------------------------------

#ifndef G4IrregBox_HH
#define G4IrregBox_HH

#include "G4CSGSolid.hh"

struct IrregBoxPlane
{
    G4double a, b, c, d;    // Normal unit vector (a, b, c) and offset (d)
    // => Ax + By + Cz + D = 0
};

enum IRREGBOXPLANE
{
    IBP_TOP = 0,    IBP_BOTTOM,
    IBP_FRONT,      IBP_BACK,
    IBP_LEFT,       IBP_RIGHT,
    IBP_UNDEF
};

class G4IrregBox : public G4CSGSolid
{

public:  // with description

    // Constructors

    G4IrregBox( const G4String& pName, const G4ThreeVector pPnt[8] );
    // Prepares plane equations and parameters from corner coordinates

    // Destructors
    virtual ~G4IrregBox(void);

    // Accessors

    IrregBoxPlane GetPlane( G4int n ) const
    {   return fPlanes[n];  }

    G4ThreeVector GetAllParameters( G4int n) const
    {   return fPnt[n];     }

    // Modifiers

    G4bool SetAllParameters( const G4ThreeVector pPnt[8] );

    // Methods for solid
/*
    void ComputeDimensions(       G4VPVParameterisation * p,
                            const G4int n,
                            const G4VPhysicalVolume * pRep );
*/
    G4bool CalculateExtent( const EAxis pAxis,
                            const G4VoxelLimits & pVoxelLimit,
                            const G4AffineTransform & pTransform,
                                  G4double & pMin, G4double & pMax ) const;

    EInside Inside( const G4ThreeVector & p ) const;

    G4ThreeVector SurfaceNormal( const G4ThreeVector & p ) const;

    G4double DistanceToIn(const G4ThreeVector & p, const G4ThreeVector & v) const;

    G4double DistanceToIn( const G4ThreeVector & p ) const;

    G4double DistanceToOut(const G4ThreeVector & p, const G4ThreeVector & v,
                           const G4bool calcNorm = false,
                                 G4bool * validNorm = 0,
                                 G4ThreeVector * n = 0) const;

    G4double DistanceToOut( const G4ThreeVector & p ) const;

    G4GeometryType GetEntityType(void) const;

    std::ostream& StreamInfo( std::ostream& os ) const;

    // Visualisation functions

    void           DescribeYourselfTo( G4VGraphicsScene & scene ) const;
    G4Polyhedron * CreatePolyhedron  (void) const;
    G4NURBS *      CreateNURBS       (void) const;

protected:  // with description

    G4bool MakePlanes(void);
    G4bool MakePlane( const G4ThreeVector & p1,
                      const G4ThreeVector & p2,
                      const G4ThreeVector & p3,
                      const G4ThreeVector & p4,
                            IrregBoxPlane & plane ) ;

    G4ThreeVectorList *
    CreateRotatedVertices( const G4AffineTransform & pTransform ) const;
    // Creates the List of transformed vertices in the format required
    // for G4CSGSolid:: ClipCrossSection and ClipBetweenSections

private:
    G4ThreeVector fPnt[8];
    IrregBoxPlane fPlanes[6];

};

#endif
