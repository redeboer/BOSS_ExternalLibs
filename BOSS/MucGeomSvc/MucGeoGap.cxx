//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#include <iostream>
#include <vector>
#include "TGeoBBox.h"

#include "MucGeomSvc/MucGeoGap.h"
#include "MucGeomSvc/MucGeoStrip.h"
#include "MucGeomSvc/MucGeometron.h"
#include "MucGeomSvc/MucConstant.h"
#include "Identifier/MucID.h"

using namespace std;

MucGeoGap::MucGeoGap()
  : m_Part(-1),
    m_Seg(-1),
    m_Gap(-1),
    m_StripNum(-1),
    m_XSize(-1.0),
    m_YSize(-1.0),
    m_ZSize(-1.0)
{
  // Default constructor.
}

MucGeoGap::MucGeoGap(const int part,
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
		     const float dyFiducialToCenter)
  : m_Part(part),
    m_Seg(seg),
    m_Gap(gap),
    m_StripNum(stripNum),
    m_Orient(orient),
    m_XSize(xSize),
    m_YSize(ySize),
    m_ZSize(zSize),
    m_dzHighEdge(dzHighEdge),
    m_dzFarFrontGas(dzFarFrontGas),
    m_dzNearFrontGas(dzNearFrontGas),
    m_dzNearBackGas(dzNearBackGas),
    m_dzFarBackGas(dzFarBackGas),
    m_pMucGeoStrip(MucID::getStripNum(part, seg, gap))
{
  // Real constructor (construct strips externally).

  // v1Global, v2Global, v3Global are position vectors of the three survey targets in the
  // Bes global coordinate system.
  //cout << m_dzHighEdge << " " << m_dzFarFrontGas << " " << m_dzNearFrontGas << " " << dzNearBackGas << " " << dzFarBackGas << endl;
  Hep3Vector v1Global(xTarget1Global, yTarget1Global, zTarget1Global);
  Hep3Vector v2Global(xTarget2Global, yTarget2Global, zTarget2Global);
  Hep3Vector v3Global(xTarget3Global, yTarget3Global, zTarget3Global);

  Hep3Vector v2To1Global = v1Global - v2Global;
  Hep3Vector v3To1Global = v1Global - v3Global;

  // v1Gap is the position vector of target1 in the gap coordinate system. 
  Hep3Vector v1Gap(dxTarget1ToFiducial + dxFiducialToCenter,
		   dyTarget1ToFiducial + dyFiducialToCenter,
		   0.0);
 
  // Make rotaiton and translation matrix.
  Hep3Vector newX(1,0,0), newY(0,1,0), newZ(0,0,1);
  HepRotation rotateGap(newX, newY, newZ);

  if(part == 1) {
    newX = v2To1Global;
    newX.setMag(1.0);
    newY = v3To1Global;
    newY.setMag(1.0);
    newZ = newX.cross(newY);
  }
  if(part != 1) { 
    newY = -1.0*v2To1Global;
    newY.setMag(1.0);
    newZ = v3To1Global.cross(newY);
    newZ.setMag(1.0);
    newX = newY.cross(newZ);
  }

  HepRotation rotateGlobal(newX, newY, newZ);
  HepRotation rotateGapToGlobal(rotateGlobal * rotateGap);

  Hep3Vector  translateGapToGlobal(v1Global - rotateGapToGlobal * v1Gap);

  m_Rotation    = rotateGapToGlobal;
  m_Translation = translateGapToGlobal;
  m_Center      = m_Translation;

  //cout << " Rotation : " << endl << m_Rotation.xx() << "  " << m_Rotation.xy() << "  " << m_Rotation.xz() << endl << m_Rotation.yx() << "  " << m_Rotation.yy() << "  " << m_Rotation.yz() << endl << m_Rotation.zx() << "  " << m_Rotation.zy() << "  " << m_Rotation.zz() << endl;
  //cout << " Center   : " << m_Center   << endl;

  HepMatrix rM(3,3), rMT(3,3);
  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      rM[i][j] = m_Rotation(i,j);
    }
  }
  rMT = rM.T();

  Hep3Vector newXT(rMT(1,1),rMT(2,1),rMT(3,1));
  Hep3Vector newYT(rMT(1,2),rMT(2,2),rMT(3,2));
  Hep3Vector newZT(rMT(1,3),rMT(2,3),rMT(3,3));
  HepRotation rotateGlobalToGap(newXT,newYT,newZT);

  m_RotationT = rotateGlobalToGap;
}

/// ROOT constructor.
MucGeoGap::MucGeoGap(const int part,
		     const int seg,
		     const int gap,
		     const int orient,
		     const int stripNum,
		     const TGeoPhysicalNode *gapPhysicalNode,
		     const float ironThickness)
  : m_Part(part),
    m_Seg(seg),
    m_Gap(gap),
    m_StripNum(stripNum),
    m_Orient(orient),
    m_pMucGeoStrip(MucID::getStripNum(part, seg, gap)),
    m_IronThickness(ironThickness)

{
  TGeoBBox *gapBox = (TGeoBBox*)gapPhysicalNode->GetShape();

  m_XSize = gapBox->GetDX() * 2.0;
  m_YSize = gapBox->GetDY() * 2.0;
  m_ZSize = gapBox->GetDZ() * 2.0;

  m_dzHighEdge     = 20.0;
  m_dzFarFrontGas  = -4.5;
  m_dzNearFrontGas = -2.5;
  m_dzNearBackGas  =  2.5;
  m_dzFarBackGas   =  4.5;  

  double eRot[9], *pRot;
  pRot = eRot;
  pRot = gapPhysicalNode->GetMatrix()->GetRotationMatrix();

  //cout << "part " << part << " seg " << seg << " gap " << gap << endl; 
  //cout << pRot[0] << " " << pRot[1] << " " << pRot[2] << endl
  //     << pRot[3] << " " << pRot[4] << " " << pRot[5] << endl
  //     << pRot[6] << " " << pRot[7] << " " << pRot[8] << endl;
  Hep3Vector rotX(pRot[0], pRot[3], pRot[6]);
  Hep3Vector rotY(pRot[1], pRot[4], pRot[7]);
  Hep3Vector rotZ(pRot[2], pRot[5], pRot[8]);
  HepRotation rotateGapToGlobal(rotX, rotY, rotZ);
  m_Rotation = rotateGapToGlobal;

  Hep3Vector rotTX(pRot[0], pRot[1], pRot[2]);
  Hep3Vector rotTY(pRot[3], pRot[4], pRot[5]);
  Hep3Vector rotTZ(pRot[6], pRot[7], pRot[8]);
  HepRotation rotateGlobalToGap(rotTX, rotTY, rotTZ);
  m_RotationT = rotateGlobalToGap;
  
  double eTrans[3], *pTrans;
  pTrans = eTrans;
  pTrans = gapPhysicalNode->GetMatrix()->GetTranslation();
  
  Hep3Vector translateGapToGlobal(pTrans[0], pTrans[1], pTrans[2]);
  m_Translation = translateGapToGlobal;
  m_Center      = m_Translation;


  double eTrans_gap[3], *pTrans_gap;  //pTran is for box now!!!
  pTrans_gap = eTrans_gap;
  pTrans_gap = gapPhysicalNode->GetMatrix(2)->GetTranslation();
  //std::cout <<"gap position: "<<pTrans_gap[0]<< " " << pTrans_gap[1] << " " << pTrans_gap[2] << std::endl;

  Hep3Vector GapSurface1, GapSurface2;
  Hep3Vector GapCenter(pTrans_gap[0], pTrans_gap[1], pTrans_gap[2]);
  GapSurface1 = GapCenter - rotZ * 20; //Gap thickness/2= 20
  GapSurface2 = GapCenter + rotZ * 20; //Gap thickness/2= 20

  m_SurfaceInner = GapSurface1;
  m_SurfaceOuter = GapSurface2;
  if(GapSurface1.mag()>GapSurface2.mag())//keep Inner be the little one
    {
      m_SurfaceInner = GapSurface2;
      m_SurfaceOuter = GapSurface1;
    }

 //  std::cout <<"sur1 position: "<<GapSurface1[0]<<" "<<GapSurface1[1]<<" "<<GapSurface1[2]<<std::endl;
//   std::cout <<"sur2 position: "<<GapSurface2[0]<<" "<<GapSurface2[1]<<" "<<GapSurface2[2]<<std::endl; 
//   std::cout <<"box position: "<< pTrans[0] << " " << pTrans[1] << " " << pTrans[2] << std::endl;
}

MucGeoGap& 
MucGeoGap::operator=(const MucGeoGap &orig)
{
  // Assignment operator.
  if (this != &orig) {             // Watch out for self-assignment!
    m_Part       = orig.m_Part;
    m_Seg        = orig.m_Seg;
    m_Gap        = orig.m_Gap;
    m_StripNum    = orig.m_StripNum;
    m_Orient      = orig.m_Orient;
    m_HitStatus   = orig.m_HitStatus;

    m_XSize      = orig.m_XSize;
    m_YSize      = orig.m_YSize;
    m_ZSize      = orig.m_ZSize;

    m_dzHighEdge     = orig.m_dzHighEdge;
    m_dzFarFrontGas  = orig.m_dzFarFrontGas;
    m_dzNearFrontGas = orig.m_dzNearFrontGas;
    m_dzNearBackGas  = orig.m_dzNearBackGas;
    m_dzFarBackGas   = orig.m_dzFarBackGas;


    m_Center      = orig.m_Center;
    m_Rotation    = orig.m_Rotation;
    m_RotationT   = orig.m_RotationT;
    m_Translation = orig.m_Translation;

    m_IronThickness = orig.m_IronThickness;

    m_pMucGeoStrip = orig.m_pMucGeoStrip;
  }
  return *this;
}

MucGeoGap::MucGeoGap(const MucGeoGap &orig)
  : m_Part(orig.m_Part),
    m_Seg(m_Seg),
    m_Gap(m_Gap),
    m_StripNum(orig.m_StripNum),
    m_Orient(orig.m_Orient),
    m_HitStatus(orig.m_HitStatus),
    m_XSize(orig.m_XSize),
    m_YSize(orig.m_YSize),
    m_ZSize(orig.m_ZSize),
    m_dzHighEdge(orig.m_dzHighEdge),
    m_dzFarFrontGas(orig.m_dzFarFrontGas),
    m_dzNearFrontGas(orig.m_dzNearFrontGas),
    m_dzNearBackGas(orig.m_dzNearBackGas),
    m_dzFarBackGas(orig.m_dzFarBackGas),
    m_Center(orig.m_Center),
    m_Rotation(orig.m_Rotation),
    m_RotationT(orig.m_RotationT),
    m_Translation(orig.m_Translation),
    m_IronThickness(orig.m_IronThickness)
{ 
  // Copy constructor.
  m_pMucGeoStrip = orig.m_pMucGeoStrip;
}
				 
MucGeoGap::~MucGeoGap()
{ 
  // Destructor.
  MucGeoStrip* pStrip = 0;
  while(m_pMucGeoStrip.size() > 0){
    pStrip = m_pMucGeoStrip.back();
    delete pStrip;
    m_pMucGeoStrip.pop_back();
  }
}

MucGeoStrip* 
MucGeoGap::GetStrip(const int strip) const
{
  // Point to a strip within this gap.
  if ( strip < 0 || strip >= int(m_pMucGeoStrip.size()) ) {
    std::cout << "Error: MucGeoGap::GetStrip() strip " << strip << " exceed strip range" << endl;
    return 0;
  }
  if ( !m_pMucGeoStrip[strip] ) {
    std::cout << "Error: MucGeoGap::GetStrip() strip " << strip << " not found" << endl;
    return 0;
  }
  
  return m_pMucGeoStrip[strip];
}

void
MucGeoGap::GetSize(float &xSize, float &ySize, float &zSize) const
{
  // Get size of this gap.
  xSize = m_XSize;
  ySize = m_YSize;
  zSize = m_ZSize;

  return;
}

void 
MucGeoGap::GetRotationMatrix(float &thetaX, float &phiX,
			     float &thetaY, float &phiY,
			     float &thetaZ, float &phiZ) const
{
  // Get the rotation angles (in degrees) of the gap in the global coordinate system.
  Hep3Vector x(m_Rotation.colX());
  Hep3Vector y(m_Rotation.colY());
  Hep3Vector z(m_Rotation.colZ());

  thetaX = kRad * x.theta();
  phiX   = kRad * x.phi();
  thetaY = kRad * y.theta();
  phiY   = kRad * y.phi();
  thetaZ = kRad * z.theta();
  phiZ   = kRad * z.phi();

  // cout << "Gap rotation matrix : " << endl
  //      << "X:  theta = " << thetaX << "  phi = " << phiX << endl
  //      << "Y:  theta = " << thetaY << "  phi = " << phiY << endl
  //      << "Z:  theta = " << thetaZ << "  phi = " << phiZ << endl;

  return;
}

int
MucGeoGap::GuessStrip(const float x,
		      const float y,
		      const float ) const
{
  // Given a position (gap coordinate), find the approximate strip containing the position, as there are intervals between the strips.
  float x0, y0, z0;  // XYZ position of the strip NO min.
  float xn, yn, zn;  // XYZ position of the strip NO max.
  float dx;          // Approximate width of a strip and a interval.
  int n, iGuess;           

  n = m_StripNum - 1;

  GetStrip(0)->GetCenterPos(x0, y0, z0);
  GetStrip(n)->GetCenterPos(xn, yn, zn);

  //cout << n << endl;
  //cout << "x0 " << x0 << " xn " << xn << " x " << x << endl;
  //cout << "y0 " << y0 << " yn " << yn << " y " << y << endl;
  //cout << "orient " << m_Orient << endl;

  if(m_Orient == 0){
    dx = (yn-y0)/n;
    iGuess = int ((y-y0)/dx+0.5);
    return iGuess;
  } 
  else{
    dx = (xn-x0)/n;
    iGuess = int ((x-x0)/dx+0.5);
    return iGuess;
  }
}

HepPoint3D
MucGeoGap::ProjectToGap(const HepPoint3D gPoint,
			const Hep3Vector gDirection) const
{
  // Given a line by basepoint gPoint and direction gDirection,
  // find the intersection with the gap (in global coordinate).
  Hep3Vector gapVect  = m_Rotation.colZ();
  HepPoint3D gCross(0,0,0);

  HepPlane3D plane(gapVect, m_Center);
  MucGeometron geometron;
  //bool getPoint = 
  geometron.GetIntersectionLinePlane(gPoint, gDirection, plane, gCross);

  return gCross;
}

HepPoint3D
MucGeoGap::ProjectToGapWithSigma(const HepPoint3D gPoint,
    const Hep3Vector gDirection,
    const HepPoint3D gPointSigma,
    const Hep3Vector gDirectionSigma,
    HepPoint3D &gCross,
    HepPoint3D &gCrossSigma) const
{
  Hep3Vector gapVect  = m_Rotation.colZ();

  HepPlane3D plane(gapVect, m_Center);
  MucGeometron geometron;
  geometron.GetIntersectionLinePlaneWithSigma(gPoint, gDirection, gPointSigma, gDirectionSigma, plane, gCross, gCrossSigma);


  return gCross;
} 


void
MucGeoGap::ProjectToGapSurface(const HepPoint3D gPoint,
			       const Hep3Vector gDirection,
			       HepPoint3D& gCross1,
			       HepPoint3D& gCross2 ) const
{
  // Given a line by basepoint gPoint and direction gDirection,
  // find the intersection with the gap (in global coordinate).
  Hep3Vector gapVect  = m_Rotation.colZ();

  HepPlane3D planeInner(gapVect, m_SurfaceInner);
  HepPlane3D planeOuter(gapVect, m_SurfaceOuter);
  MucGeometron geometron;
  //bool getPoint = 

  geometron.GetIntersectionLinePlane(gPoint, gDirection, planeInner, gCross1);
  geometron.GetIntersectionLinePlane(gPoint, gDirection, planeOuter, gCross2);

}

HepPoint3D
MucGeoGap::ProjectToGapQuadLocal(const int part,      //liangyt 2009.3.12 
			const int orient,
                        const float a,    //y = ax*x + b*x +c;
                        const float b,
                        const float c,
                        const int whichhalf,
                        HepPoint3D& gCross1,
                        HepPoint3D& gCross2) const
{
  Hep3Vector gapVect, center_local;
  if(part == 1 && orient == 1)  {gapVect  = m_Rotation.colZ(); center_local = m_Center;}
  else if(part == 1 && orient == 0)  {//in this orientation, the fitting is done in local coordinate.
     gapVect.setX(0); gapVect.setY(1); gapVect.setZ(0);
     center_local.setX(0); center_local.setY(m_Center.mag()); center_local.setZ(0);
  }
  else  {//in this orientation, the fitting is done in local coordinate. 
     gapVect.setX(1); gapVect.setY(0); gapVect.setZ(0);
     center_local.setX(m_Center.z()); center_local.setY(0); center_local.setZ(0);	
  }

  //cout<<"in mucgeogap: part = "<<part<<" o: "<<orient<<" gapvect: "<<gapVect<<"  center = "<<m_Center<<endl;
  //cout<<"in mucgeogap: center local = "<<center_local<<endl;

  HepPlane3D plane(gapVect, center_local);

  MucGeometron geometron;
  geometron.GetIntersectionQuadPlaneLocal(part,orient, a, b, c, plane, gCross1, gCross2);


}


HepPoint3D
MucGeoGap::ProjectToGap(const HepPoint3D gPoint,      //liangyt 2007.4.9 
			const float vy,
			const float y0,
			const float a,    //y = ax*x + b*x +c;
			const float b,
			const float c,
			const int whichhalf,
			HepPoint3D& gCross1,
			HepPoint3D& gCross2) const
{
  // Given a line by basepoint gPoint and direction gDirection,
  // find the intersection with the gap (in global coordinate).
  Hep3Vector gapVect  = m_Rotation.colZ();

  HepPlane3D plane(gapVect, m_Center);
  MucGeometron geometron;
  //bool getPoint = 
  geometron.GetIntersectionQuadPlane(gPoint, vy, y0, a, b, c, plane, gCross1, gCross2);

  HepPoint3D localCross = TransformToGap(gCross1);

  bool found = false;
  bool found1 = false;
  bool found2 = false;
  HepPoint3D temp;
  int good = 0;

  if ( IsInGap(localCross.x(),
	       localCross.y(),
	       localCross.z()) ) {
    good = 1;
    found1 = true;
  }

  localCross = TransformToGap(gCross2);
  if ( IsInGap(localCross.x(),
	       localCross.y(),
	       localCross.z()) ) {
    good = 2;
    found2 = true;
  }
  if(found1&&found2) {
    float center = b/(-2*a);
    if(whichhalf==2){
      if(gCross1.x()>center) good = 1;
      if(gCross2.x()>center) good = 2;
    }
    if(whichhalf==1){
      if(gCross1.x()<center) good = 1;
      if(gCross2.x()<center) good = 2;
    }
  }
  if(good == 2) {temp = gCross1; gCross1 = gCross2; gCross2 = temp;}  //keep gCross1 be the good one!
  //cout<<"detail: "<<whichhalf<<" "<<gCross1<<" & "<<gCross2<<" "<<a<<" "<<b<<" "<<c<<endl;

  return gCross1;
}


void
MucGeoGap::ProjectToGapSurface(const HepPoint3D gPoint,      //liangyt 2007.4.9 
			       const float vy,
			       const float y0,
			       const float a,    //y = ax*x + b*x +c;
			       const float b,
			       const float c,
			       const int whichhalf,
			       HepPoint3D& gCross1,
			       HepPoint3D& gCross2) const
{
  // Given a line by basepoint gPoint and direction gDirection,
  // find the intersection with the gap (in global coordinate).

  HepPoint3D cross1, cross2, cross3, cross4;
  
  Hep3Vector gapVect  = m_Rotation.colZ();

  HepPlane3D planeInner(gapVect, m_SurfaceInner);
  HepPlane3D planeOuter(gapVect, m_SurfaceOuter);

  MucGeometron geometron;
  //bool getPoint = 
  geometron.GetIntersectionQuadPlane(gPoint, vy, y0, a, b, c, planeInner, cross1, cross2);
  geometron.GetIntersectionQuadPlane(gPoint, vy, y0, a, b, c, planeOuter, cross3, cross4);

  gCross1 = CompareIntersection(whichhalf, cross1, cross2, a, b, c);
  gCross2 = CompareIntersection(whichhalf, cross3, cross4, a, b, c);

}

HepPoint3D
MucGeoGap::CompareIntersection(const int whichhalf, const HepPoint3D gCross1,
			       const HepPoint3D gCross2, const float a, 
			       const float b, const float c )const
{
  bool found = false;
  bool found1 = false;
  bool found2 = false;
  int good = 0;

  HepPoint3D localCross = TransformToGap(gCross1);
  if ( IsInGap(localCross.x(),
	       localCross.y(),
	       0.0) ) {
    good = 1;
    found1 = true;
  }

  localCross = TransformToGap(gCross2);
  if ( IsInGap(localCross.x(),
	       localCross.y(),
	       0.0) ) {
    good = 2;
    found2 = true;
  }
  if(found1&&found2) {
    float center = b/(-2*a);
    if(whichhalf==2){
      if(gCross1.x()>center) good = 1;
      if(gCross2.x()>center) good = 2;
    }
    if(whichhalf==1){
      if(gCross1.x()<center) good = 1;
      if(gCross2.x()<center) good = 2;
    }
  }

  if(good == 1) return gCross1;
  else if(good == 2) return gCross2;
  else {
    HepPoint3D Empty(-9999,-9999,-9999);
    cout<<"in MucGeoGap:: both intersection position are bad!!!"<<endl; return Empty;
  }

}


Hep3Vector 
MucGeoGap::RotateToGlobal(const Hep3Vector pVect) const
{
  // Rotate a vector from gap coordinate to Bes global coordinate.
  return m_Rotation * pVect;
}

Hep3Vector
MucGeoGap:: RotateToGap(const Hep3Vector gVect) const 
{
  // Rotate a vector from Bes global coordinate to gap coordinate.
  return m_RotationT * gVect;
}

HepPoint3D 
MucGeoGap::TransformToGlobal(const HepPoint3D pPoint) const
{
  // Transform a point from gap coordinate to Bes global coordinate.
  return m_Rotation * pPoint + m_Translation;
}

HepPoint3D
MucGeoGap::TransformToGap(const HepPoint3D gPoint) const
{
  // Transform a point from Bes global coordinate to gap coordinate.
  return m_RotationT * (gPoint - m_Translation);
}

bool 
MucGeoGap::IsInGap(const float x,
		   const float y,
		   const float z) const
{
  // Check if the point (given in gap coordinate) is within the gap boundary.
  return ( ( x > -0.5*m_XSize - kGapEdge ) && ( x < 0.5*m_XSize + kGapEdge ) &&
	   ( y > -0.5*m_YSize - kGapEdge ) && ( y < 0.5*m_YSize + kGapEdge ) &&
	   ( z > ( m_dzHighEdge - m_ZSize ) ) && ( z < m_dzHighEdge ) );
}

MucGeoStrip* 
MucGeoGap::AddStrip(const int strip)
{ 
  // Add a strip to the gap.
  MucGeoStrip *pStrip;
  MucGeoStrip *neighbor;

  if( strip >= int(m_pMucGeoStrip.size()) ) {
    cout << " MucGeoGap::AddStrip  strip number "
	 << strip << " outside of expected range " 
	 << m_pMucGeoStrip.size() 
	 << endl;
    return 0;
  }

  if( (strip + 1) > m_StripNum ) m_StripNum = strip + 1;

  if(m_pMucGeoStrip[strip]) {
    // The strip object has already been created; don't create a new one.
    return m_pMucGeoStrip[strip];
  }

  pStrip = new MucGeoStrip(strip,  this);
  
  m_pMucGeoStrip[strip] = pStrip;
  pStrip->SetLeftNeighbor(0L);
  pStrip->SetRightNeighbor(0L);
  
  // Make this strip and the previous one neighbors.
  if ( strip > 0 ) {
    neighbor = m_pMucGeoStrip[strip-1];
    if(neighbor) {
      neighbor->SetRightNeighbor(pStrip);
      pStrip->SetLeftNeighbor(neighbor);
    }
  }
  
  return pStrip;
}

ostream&
operator << (ostream& s, const MucGeoGap& gap)
{
  //float x0, y0, z0;
  float dx, dy, dz;
  HepPoint3D center;
  center = gap.GetCenter();
  gap.GetSize(dx,dy,dz);

  s << " Identifier : " << gap.Part() << " " << gap.Seg() << " " << gap.Gap() << endl;
  s << " Strip number " << gap.GetStripNum() << endl;
  s << " Center position ("  << center.x() << "," << center.y() << "," << center.z() << ")" << endl;
  s << " Size (" << dx << "," << dy << "," << dz << ")" << endl;

  return s;
}


