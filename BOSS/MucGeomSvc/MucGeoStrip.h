//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_GEO_STRIP_H
#define MUC_GEO_STRIP_H

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Geometry/Plane3D.h>
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoPhysicalNode.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

#include "MucGeomSvc/MucGeoGap.h"

using namespace std;
using namespace CLHEP;

/**
 * Class MucGeoStrip describes a single strip in a gap of the muon chamber
 *
 * @author Zhengyun You \URL{mailto:youzy@pku.edu.cn}
 */

class MucGeoStrip
{
 public:

  /// Default constructor.
  MucGeoStrip( const int strip, MucGeoGap* const pGap);
  
  /// Desctructor.
  ~MucGeoStrip();
  
  /// Get the softID.
  //MucSoftID GetSoftID() const { return m_StripID; }
  /// Get Part No.
  int Part() { return m_pGap->Part(); }

  /// Get Seg No.
  int Seg()  { return m_pGap->Seg(); }

  /// Get Gap No.
  int Gap()  { return m_pGap->Gap(); }

  /// Get Strip No.
  int Strip() { return m_Strip; }
  
  /// Get the pointer to the gap that contains the strip.
  MucGeoGap *GetGap() const { return m_pGap; }

  /// Get the pointer to the adjacent strip on the -X or -Y side of this one.
  MucGeoStrip *GetLeftNeighbor() const { return m_pLeftNeighbor; }

  /// Get the pointer to the adjacent strip on the +X or +Y side of this one.
  MucGeoStrip *GetRightNeighbor() const { return m_pRightNeighbor; }

  /// Get position of low-X edge in the gap coordinate system.
  float GetXmin() const { return m_Xmin; }

  /// Get position of high-X edge in the gap coordinate system.
  float GetXmax() const { return m_Xmax; }

  /// Get position of low-Y edge in the gap coordinate system.
  float GetYmin() const { return m_Ymin; }

  /// Get position of high-Y edge in the gap coordinate system.
  float GetYmax() const { return m_Ymax; }

  /// Get position of low-Z edge in the gap coordinate system.
  float GetZmin() const { return m_Zmin; }

  /// Get position of high-Z edge in the gap coordinate system.
  float GetZmax() const { return m_Zmax; }

  /// Get center position of this strip (in the gap coordinate system).
  void GetCenterPos(float &x, float &y, float &z) const;
  
  /// Get uncertainty in the position of this strip (in the gap coordinate system).
  void GetCenterSigma(float& sx, float& sy, float& sz);
  
  /// Get the strip hit status.
  bool GetHitStatus() const { return m_HitStatus; }
  
  /// Set the edge, center and sigma of the strip (in the gap coordinate system).
  void SetStrip(const float x1, const float x2,
		const float y1, const float y2,
		const float xSize,
		const float ySize,
		const float zSize);
  
  /// Set strip from ROOT.
  void SetStrip(TGeoPhysicalNode *stripPhysicalNode);

  /// Set pointer to the adjacent strip on the -X or -Y side of this one.
  // (0 if no neighbor in this gap).
  void SetLeftNeighbor(MucGeoStrip* p);

  /// Set pointer to the adjacent strip on the +X or +Y side of this one.
  // (0 if no neighbor in this gap).
  void SetRightNeighbor(MucGeoStrip* p);

  /// Set the strip hit status.
  void SetHitStatus(const bool hit) { m_HitStatus = hit; }

  /// Is the point within the boundaries of this strip?
  bool IsInStrip(const float x, const float y, const float z) const;

  /// Does the line cross this strip?
  bool CrossGasChamber(const HepPoint3D linePoint, const Hep3Vector lineDir) const;

 private:

  float m_Xmin;   // Position of  low-X edge in the gap coordinate system.
  float m_Xmax;   // Position of high-X edge in the gap coordinate system.
  float m_Ymin;   // Position of  low-Y edge in the gap coordinate system.
  float m_Ymax;   // Position of high-Y edge in the gap coordinate system.
  float m_Zmin;   // Position of  low-Z edge in the gap coordinate system.
  float m_Zmax;   // Position of high-Z edge in the gap coordinate system.

  float m_dzFarFrontGas;  // Z distance from low  edge of front gas chamber to strip center. 
  float m_dzNearFrontGas; // Z distance from high edge of front gas chamber to strip center.
  float m_dzNearBackGas;  // Z distance from low  edge of back  gas chamber to strip center.
  float m_dzFarBackGas;   // Z distance from high edge of back  gas chamber to strip center.
  
  //MucSoftID    m_StripID;        // SoftID of this strip.
  int          m_Strip;        // Strip no of this strip in gap.
  MucGeoGap   *m_pGap;           // Pointer to the gap containing this strip.
  MucGeoStrip *m_pLeftNeighbor;  // Pointer to the left neighbor strip.
  MucGeoStrip *m_pRightNeighbor; // Pointer to the right neighbor strip.

  HepPoint3D m_CenterPos;    // Position of center (panel coordinate system)
  HepPoint3D m_CenterSigma;  // Uncertainty in center position (panel coords)
  bool       m_HitStatus;    // If = true, the strip get a hit.

  int   m_padID;           //which pad is expected to be intersected in. only useful for expected hit.

};

#endif    /* MUC_GEO_STRIP_H */
