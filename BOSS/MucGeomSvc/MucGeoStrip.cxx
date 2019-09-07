//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#include "TGeoBBox.h"

#include "MucGeomSvc/MucGeoStrip.h"
#include "MucGeomSvc/MucGeoGap.h"
#include "MucGeomSvc/MucGeometron.h"

using namespace std;

MucGeoStrip::MucGeoStrip( const int strip, MucGeoGap* const pGap)
  : m_Strip(strip),
    m_pGap(pGap),
    m_pLeftNeighbor(0L),
    m_pRightNeighbor(0L),
    m_CenterPos(0.0,0.0,0.0),
    m_CenterSigma(0.0,0.0,0.0)
{
  // Constructor (dummy strip position and sizes).
}

MucGeoStrip::~MucGeoStrip()
{
  // Destructor.
  m_pLeftNeighbor = 0L;
  m_pRightNeighbor = 0L;
  m_pGap = 0L;
}

void 
MucGeoStrip::GetCenterPos(float &x, float &y, float &z) const
{
  // Get center position of this strip (in the gap coordinate system).
  x = m_CenterPos.x();
  y = m_CenterPos.y();
  z = m_CenterPos.z();
  
  return;
}

void 
MucGeoStrip::GetCenterSigma(float &sx, float &sy, float &sz)
{
  // Get uncertainty in the position of this strip (in the gap coordinate system).
  sx = m_CenterSigma.x();
  sy = m_CenterSigma.y();
  sz = m_CenterSigma.z();
 
  return;
}

void 
MucGeoStrip::SetStrip(const float x1, const float x2,
		      const float y1, const float y2,
		      const float xSize,
		      const float ySize,
		      const float zSize)
{
  // Set the edge, center and sigma of the strip (in the gap coordinate system).
  double un = 1.0;  // Uncertainty.
  
  m_Xmin = x1;
  m_Xmax = x2;
  m_Ymin = y1;
  m_Ymax = y2;
  m_Zmin = -0.5*zSize;
  m_Zmax =  0.5*zSize;

  m_dzFarFrontGas  = GetGap()->GetdzFarFrontGas(); 
  m_dzNearFrontGas = GetGap()->GetdzNearFrontGas(); 
  m_dzNearBackGas  = GetGap()->GetdzNearBackGas(); 
  m_dzFarBackGas   = GetGap()->GetdzFarBackGas(); 

  m_CenterPos.setX(0.5*(m_Xmin + m_Xmax));
  m_CenterPos.setY(0.5*(m_Ymin + m_Ymax));
  m_CenterPos.setZ(0.0);

  m_CenterSigma.setX(un*xSize);
  m_CenterSigma.setY(un*ySize);
  m_CenterSigma.setZ(un*zSize);
}

void 
MucGeoStrip::SetStrip(TGeoPhysicalNode *stripPhysicalNode)
{
  // Set the edge, center and sigma of the strip (in the gap coordinate system).
  double un = 1.0;  // Uncertainty.
 
  TGeoBBox *stripBox = (TGeoBBox*)stripPhysicalNode->GetShape();
 
  double xSize = 2.0 * stripBox->GetDX();
  double ySize = 2.0 * stripBox->GetDY();
  double zSize = 2.0 * stripBox->GetDZ();
  
  m_dzFarFrontGas  = GetGap()->GetdzFarFrontGas(); 
  m_dzNearFrontGas = GetGap()->GetdzNearFrontGas(); 
  m_dzNearBackGas  = GetGap()->GetdzNearBackGas(); 
  m_dzFarBackGas   = GetGap()->GetdzFarBackGas(); 

  double eTrans[3], *pTrans;
  pTrans = eTrans;
  //int level = stripPhysicalNode->GetLevel();
  double eLocal[3], *pLocal;
  pLocal = eLocal;
  for (int i = 0; i < 3; i++) pLocal[i] = 0.0;
  stripPhysicalNode->GetNode()->LocalToMaster(pLocal, pTrans);
  
  m_CenterPos.setX( pTrans[0] );
  m_CenterPos.setY( pTrans[1] );
  m_CenterPos.setZ( pTrans[2] );

  m_Xmin = pTrans[0] - 0.5 * xSize;
  m_Xmax = pTrans[0] + 0.5 * xSize;
  m_Ymin = pTrans[1] - 0.5 * ySize;
  m_Ymax = pTrans[1] + 0.5 * ySize;
  m_Zmin = pTrans[2] - 0.5 * zSize;
  m_Zmax = pTrans[2] + 0.5 * zSize;

  m_CenterSigma.setX(un*xSize);
  m_CenterSigma.setY(un*ySize);
  m_CenterSigma.setZ(un*zSize);
  //cout << m_CenterPos << endl;
}

void 
MucGeoStrip::SetLeftNeighbor(MucGeoStrip* p)
{
  /// Set pointer to the adjacent strip on the -X or -Y side of this one.
  // (0 if no neighbor in this gap).
  if (p)
    m_pLeftNeighbor = p;
}

void 
MucGeoStrip::SetRightNeighbor(MucGeoStrip* p)
{
  /// Set pointer to the adjacent strip on the +X or +Y side of this one.
  // (0 if no neighbor in this gap).
  if (p)
    m_pRightNeighbor = p;
}

bool 
MucGeoStrip::IsInStrip(const float x,
		       const float y,
		       const float z) const
{
  // Is the point within the boundaries of this strip?
  return ( (x > m_Xmin) && (x < m_Xmax) 
	   && (y > m_Ymin) && (y < m_Ymax)
	   && (z >= m_dzFarFrontGas)
	   && (z <= m_dzFarBackGas) );
}


bool 
MucGeoStrip::CrossGasChamber(const HepPoint3D linePoint, 
			     const Hep3Vector lineDir) const
{
  // Does the line cross this strip?

  // We need to define the 6 planes then loop over to check for intersection.
  // All are defined in the gap coordinate.
  HepPlane3D stripSide[6];

  // Xmin.
  HepPoint3D p0Xmin(m_Xmin, m_Ymin, m_dzFarFrontGas);
  HepPoint3D p1Xmin(m_Xmin, m_Ymax, m_dzFarFrontGas);
  HepPoint3D p2Xmin(m_Xmin, m_Ymin, m_dzFarBackGas);
  HepPlane3D plane0(p0Xmin, p1Xmin, p2Xmin);
  stripSide[0] = plane0;

  // Xmax.
  HepPoint3D p0Xmax(m_Xmax, m_Ymin, m_dzFarFrontGas);
  HepPoint3D p1Xmax(m_Xmax, m_Ymax, m_dzFarFrontGas);
  HepPoint3D p2Xmax(m_Xmax, m_Ymin, m_dzFarBackGas);
  HepPlane3D plane1(p0Xmax, p1Xmax, p2Xmax);
  stripSide[1] = plane1;

  // Ymin.
  HepPoint3D p0Ymin(m_Xmin, m_Ymin, m_dzFarFrontGas);
  HepPoint3D p1Ymin(m_Xmax, m_Ymin, m_dzFarFrontGas);
  HepPoint3D p2Ymin(m_Xmin, m_Ymin, m_dzFarBackGas);
  HepPlane3D plane2(p0Ymin, p1Ymin, p2Ymin);
  stripSide[2] = plane2;

  // Ymax.
  HepPoint3D p0Ymax(m_Xmin, m_Ymax, m_dzFarFrontGas);
  HepPoint3D p1Ymax(m_Xmax, m_Ymax, m_dzFarFrontGas);
  HepPoint3D p2Ymax(m_Xmin, m_Ymax, m_dzFarBackGas);
  HepPlane3D plane3(p0Ymax, p1Ymax, p2Ymax);
  stripSide[3] = plane3;

  // Zmin.
  HepPoint3D p0Zmin(m_Xmin, m_Ymin, m_dzFarFrontGas);
  HepPoint3D p1Zmin(m_Xmax, m_Ymin, m_dzFarFrontGas);
  HepPoint3D p2Zmin(m_Xmin, m_Ymax, m_dzFarFrontGas);
  HepPlane3D plane4(p0Zmin, p1Zmin, p2Zmin);
  stripSide[4] = plane4;

  // Zmax.
  HepPoint3D p0Zmax(m_Xmin, m_Ymin, m_dzFarBackGas);
  HepPoint3D p1Zmax(m_Xmax, m_Ymin, m_dzFarBackGas);
  HepPoint3D p2Zmax(m_Xmin, m_Ymax, m_dzFarBackGas);
  HepPlane3D plane5(p0Zmax, p1Zmax, p2Zmax);
  stripSide[5] = plane5;

  //cout << m_Xmin << "  " << m_Xmax << endl
  //   << m_Ymin << "  " << m_Ymax << endl
  //   << m_dzFarFrontGas << "  " << m_dzFarBackGas << endl;
    
  //If particle intersects this strip there should only be two crossings.
  int numIntersections = 0;
  MucGeometron geometron;
  bool intersectFlag = 0;
  HepPoint3D gCross(0,0,0);

  for (int sideID = 0; sideID<6; sideID++)
  {
    intersectFlag = geometron.GetIntersectionLinePlane(linePoint, 
						       lineDir,
						       stripSide[sideID],
						       gCross);
    if( intersectFlag && 
  ( gCross.x() >= m_Xmin - 1) &&  // expend strip size, to avoid gap between two strip
  ( gCross.x() <= m_Xmax + 1) &&
  ( gCross.y() >= m_Ymin - 1) &&
  ( gCross.y() <= m_Ymax + 1) &&
  ( ( gCross.z() >= m_dzFarFrontGas && gCross.z() <= m_dzNearFrontGas ) ||
	  ( gCross.z() >= m_dzNearBackGas && gCross.z() <= m_dzFarBackGas) ) ) {
      numIntersections++;
      //cout << m_StripID.GetStrip() << gCross << endl;
    }
  }

  //if( numIntersections == 1 ) {
  //  cout<<"MucGeoStrip::IntersectStrip(): Warning only one intersection.  Should be two!" << endl;
  //}
  
  if( numIntersections > 2 ) {
//    cout<<"MucGeoStrip::CrossGasChamber(): Warning too many intersections " 
//	<< numIntersections 
//	<< endl;
  }

  if( numIntersections > 0 ) return true;
  
  return false;
}

