//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University 
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_GEO_GAP_H
#define MUC_GEO_GAP_H

#include <iostream>
#include <vector>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Geometry/Plane3D.h>
#include <CLHEP/Vector/Rotation.h>
#include <CLHEP/Matrix/Matrix.h>
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoPhysicalNode.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

using namespace std;
using namespace CLHEP;

/**
 * Class MucGeoGap describes a single gap in the muon chamber
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 */

class MucGeoStrip;

class MucGeoGap
{
 public:

  /// Default constructor.
  MucGeoGap();

  /// Real constructor (construct strips externally).
  MucGeoGap(const int part,
	    const int seg,
	    const int gap,
	    const int orient,
	    const int stripNum,
	    const float xSize,
	    const float ySize,
	    const float zSize,
	    const float xTarget1Global,
	    const float yTarget1Global,
	    const float zTarget1Global,
	    const float xTarget2Global,
	    const float yTarget2Global,
	    const float zTarget2Global,
	    const float xTarget3Global,
	    const float yTarget3Global,
	    const float zTarget3Global,
	    const float dzHighEdge,
	    const float dzFarFrontGas,
	    const float dzNearFrontGas,
	    const float dzNearBackGas,
	    const float dzFarBackGas,
	    const float dxTarget1ToFiducial,
	    const float dyTarget1ToFiducial,
	    const float dxFiducialToCenter,
	    const float dyFiducialToCenter);

  /// ROOT constructor.
  MucGeoGap(const int part,
	    const int seg,
	    const int gap,
	    const int orient,
	    const int stripNum,
	    const TGeoPhysicalNode *gapPhyscialNode,
	    const float ironThickness);
 
  /// Assignment constructor.
  MucGeoGap& operator=(const MucGeoGap& orig);

  /// Copy constructor.
  MucGeoGap(const MucGeoGap& orig);

  /// Desctructor.
  ~MucGeoGap();

  /// Get part identifier (0,2 for cap, 1 for barrel).
  int Part() const { return m_Part; }

  /// Get seg identifier (0-7).
  int Seg() const { return m_Seg; }
  
  /// Get gap identifier (0-8).
  int Gap() const { return m_Gap; }

  /// Get orientation of strips in this gap
  /// ( 0 for strips parrel to x axis, 1 for strips parrel to y axis. in gap coordinate).
  int Orient() const { return m_Orient; }

  /// Get SoftID.
  // MucSoftID GetSoftID() const { return m_GapID; }

  /// Get number of strips in this gap.
  int GetStripNum() const { return m_StripNum; }

  float GetIronThickness() const { return m_IronThickness; }

  /// Get whether this gap is hit.
  bool GetHitStatus() const { return m_HitStatus; }

  /// Get m_dzFarFrontGas;
  float GetdzFarFrontGas()  const { return m_dzFarFrontGas; }  

  /// Get m_dzNearFrontGas;
  float GetdzNearFrontGas() const { return m_dzNearFrontGas; }  

  /// Get m_dzFarFrontGas;
  float GetdzFarBackGas()   const { return m_dzFarBackGas; }  

  /// Get m_dzNearBackGas;
  float GetdzNearBackGas()  const { return m_dzNearBackGas; }  

  /// Point to a strip within this gap.
  MucGeoStrip *GetStrip(const int strip) const;

  /// Get gap center position in global coordinate.
  HepPoint3D GetCenter() const { return m_Center; }

  /// Get size of this gap.
  void GetSize(float &xSize, float &ySize, float &zSize) const;
 
  /// Get the rotation angles (in degrees) of the gap in global coordinate.
  void GetRotationMatrix(float &thetaX, float &phiX,
			 float &thetaY, float &phiY,
			 float &thetaZ, float &phiZ) const;
  
  /// Given a position (gap coordinate), guess the approximate strip containing the position,
  /// as there are intervals between the strips.
  int GuessStrip(const float x, const float y,const float z) const;
  
  /// Given a line, find the intersection with the gap in the global.
  HepPoint3D ProjectToGap(const HepPoint3D gPoint, 
			  const Hep3Vector gVect) const;

  HepPoint3D ProjectToGapWithSigma(const HepPoint3D gPoint,
              const Hep3Vector gVect,
              const HepPoint3D gPointSigma,
              const Hep3Vector gVectSigma,
              HepPoint3D &gCross,
              HepPoint3D &gCrossSigma) const;

  /// Given a line, find the intersection with two surface of the gap in the global.
  void ProjectToGapSurface(const HepPoint3D gPoint, 
				 const Hep3Vector gVect,
				 HepPoint3D& cross1,
				 HepPoint3D& cross2) const;
  
  HepPoint3D ProjectToGapQuadLocal(const int part,
                          const int orient,   const float a,
                          const float b,  const float c, const int whichhalf,
                          HepPoint3D& cross1,
                          HepPoint3D& cross2) const;



  /// Given parabola, and a line, find the intersection with the gap in the global.
  HepPoint3D ProjectToGap(const HepPoint3D gPoint, 
			  const float vy,const float y0, const float a,
			  const float b,  const float c, const int whichhalf,
			  HepPoint3D& cross1,
			  HepPoint3D& cross2) const;

  /// Given parabola, and a line, find the intersection with two surface of the gap in the global.
  void ProjectToGapSurface(const HepPoint3D gPoint, 
			   const float vy,const float y0, const float a,
			   const float b,  const float c, const int whichhalf,
			   HepPoint3D& cross1,
			   HepPoint3D& cross2) const;
  

  HepPoint3D CompareIntersection(const int whichhalf, const HepPoint3D gCross1, const HepPoint3D gCross2,
				 const float a, const float b, const float c )const;

  
  /// Rotate a vector from gap coordinate to global coordinate.
  Hep3Vector RotateToGlobal(const Hep3Vector pVect) const; 

  /// Rotate a vector from global coordinate to gap coordinate.
  Hep3Vector RotateToGap(const Hep3Vector gVect) const;
  
  /// Transform a point from gap coordinate to global coordinate.
  HepPoint3D TransformToGlobal(const HepPoint3D pPoint) const; 
  
  /// Transform a point from global coordinate to gap coordinate.
  HepPoint3D TransformToGap(const HepPoint3D gPoint) const;
  
  /// Check if the point (given in gap coordinate) is within the gap boundary.
  bool IsInGap(const float x, const float y,const float z) const;

  /// Add a strip to the gap.
  MucGeoStrip *AddStrip(const int strip);

  /// Set SoftID of the gap.
  //void SetID(const int part, const int seg, const int gap)
  //{ m_GapID.SetID(part, seg, gap); }

  /// Tell whether or not this gap registered a signal in this event.
  void SetHitStatus(const bool hit) { m_HitStatus = hit; }
  
 private:
   
  int   m_Part;             // Part No of this gap.
  int   m_Seg;              // Segment No of this gap.
  int   m_Gap;              // Gap No of this gap.
  int   m_StripNum;         // Number of strips in this gap.
  int   m_Orient;           // Orientation of this gap.
  bool  m_HitStatus;        // Whether this gap is hit.

  float m_XSize;            // X size of this gap.
  float m_YSize;            // Y size of this gap.
  float m_ZSize;            // Z size of this gap.
  float m_dzHighEdge;       // Z distance from higer Z edge to strip center.
  
  float m_dzFarFrontGas;    // Z distance from low  edge of front gas chamber to strip center. 
  float m_dzNearFrontGas;   // Z distance from high edge of front gas chamber to strip center.
  float m_dzNearBackGas;    // Z distance from low  edge of back  gas chamber to strip center.
  float m_dzFarBackGas;     // Z distance from high edge of back  gas chamber to strip center.

  float m_IronThickness;    // Thickness of iron before this gap 
  
  HepPoint3D  m_Center;     // Position of gap center in global coordinate.
  HepRotation m_Rotation;   // Rotaion matrix from global to gap coordinate.
  HepRotation m_RotationT;  // Rotaion matrix from gap to global coordinate.
  Hep3Vector  m_Translation;// Transform matrix from global to gap coordinate.

  HepPoint3D  m_SurfaceInner;     // Position of the inner surface center in this gap in global coordinate.
  HepPoint3D  m_SurfaceOuter;     // Position of the outer surface center in this gap in global coordinate.

  TGeoPhysicalNode *m_PhyNode;

  vector<MucGeoStrip*> m_pMucGeoStrip; 
};
 
ostream& operator << (ostream& s, const MucGeoGap& gap);

#endif    /* MUC_GEO_GAP_H */

