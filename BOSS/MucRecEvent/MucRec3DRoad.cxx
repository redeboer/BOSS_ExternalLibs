//$id$
//
//$log$

/*
 *    2003/12/25   Zhengyun You     Peking University
 * 
 *    2005/02/27   Zhengyun You      Peking University
 *                 transplanted to Gaudi framework
 */

#include <iostream>
#include <vector>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>

#include "Identifier/MucID.h"
#include "MucGeomSvc/MucGeomSvc.h"
#include "MucGeomSvc/MucConstant.h"
#include "MucRecEvent/MucRecHit.h"
#include "MucRecEvent/MucRec2DRoad.h"
#include "MucRecEvent/MucRec3DRoad.h"
#include "MucRecEvent/MucRecLineFit.h"

// Constructor.
MucRec3DRoad::MucRec3DRoad(MucRec2DRoad *road0, MucRec2DRoad *road1)
  : m_Road0(road0), m_Road1(road1),
    m_Index(-1), 
    m_Part(road0->GetPart()),
    m_Seg(road0->GetSeg()),
    m_LastGap(0),
    m_Group(-1)
{
  float x, y, z;
  road0->GetVertexPos(x, y, z);
  m_VertexPos.setX(x);
  m_VertexPos.setY(y);
  m_VertexPos.setZ(z);

  // If the part numbers don't match, something is seriously wrong!
  if ( (road1->GetPart() != m_Part) || (road1->GetSeg() != m_Seg) ) {
    cout << "MucRec3DRoad(ctor)-E1  mismatched 2D roads:"
	 << "  x part  = " << road0->GetPart() << " seg = " << road0->GetSeg() << endl
	 << "  y part  = " << road1->GetPart() << " seg = " << road1->GetSeg() << endl;
    m_Part  = -1;
    m_Seg   = -1;
  }
  
  // Compute the last-gap parameter; take the deeper of the 1D roads.
  if ( road1->GetLastGap() > road0->GetLastGap() ) {
    m_LastGap = road1->GetLastGap();
  } else {
    m_LastGap = road0->GetLastGap();
  }

  // Check that the two 1D roads have clusters in the same panels.
  // FIXME:  logic needs more thought!

  // Calculate the chi-square and number of degrees of freedom of the
  // 2-D trajectory (use the "simple" fits).
  // FIXME:  Can we calc. chi-square without brute force?

  m_DOF  = m_Road0->GetDegreesOfFreedom() + m_Road1->GetDegreesOfFreedom();
  m_Chi2 = 0.0;

  for (int gap = 0; gap < (int)MucID::getGapMax(); gap++) 
  {
    MucRecHit* hit;
    float dist, sigma;
    hit = m_Road0->GetHit(gap);
    if (hit) {
      if ( hit->Part() == 1 ) sigma  = hit->GetCenterSigma().y(); // ???
      else 	                  sigma  = hit->GetCenterSigma().y();
      
      dist   = m_Road0->GetHitDistance(hit) / sigma;
      m_Chi2 += dist * dist;
      //cout<<"in MucRec3DRoad dist1="<<dist<<" hitdis "<<dist*sigma<<" sigma="<<sigma<<" chi2="<<m_Chi2<<endl;
    }
    
    hit = m_Road1->GetHit(gap);
    if (hit) {
      if ( hit->Part() == 1 ) {
	    // sigma  = fabs(sqrt( (hit->GetCenterSigma().x())*(hit->GetCenterSigma().x())
      // 	       + (hit->GetCenterSigma().y())*(hit->GetCenterSigma().y()) ));
	      sigma  = hit->GetCenterSigma().x();
      }
      else {
	      sigma  = hit->GetCenterSigma().x();
      }
      dist   = m_Road1->GetHitDistance(hit) / sigma;
      m_Chi2 += dist * dist;
      // cout<<"in MucRec3DRoad dist2="<<dist<<" hitdis "<<dist*sigma<<" sigma="<<sigma<<" chi2="<<m_Chi2<<endl;
    }
  }
  
}

// Copy constructor.
MucRec3DRoad::MucRec3DRoad(const MucRec3DRoad& source)
  : m_VertexPos(source.m_VertexPos),
    m_VertexSigma(source.m_VertexSigma),
    m_Road0(source.m_Road0), m_Road1(source.m_Road1),
    m_Index(source.m_Index),
    m_Part(source.m_Part), m_Seg(source.m_Seg),
    m_LastGap(source.m_LastGap), 
    m_Chi2(source.m_Chi2), m_DOF(source.m_DOF),
    m_Group(source.m_Group)
{ }

// Destructor.
MucRec3DRoad::~MucRec3DRoad()
{
  // The 3D road objects do not "own" 2D objects, so don't delete them
  // here!
}

// Set the index for this road.
void
MucRec3DRoad::SetIndex(const int& index)
{
  if (index >= 0) m_Index = index;
}

// Set the group index for this road.
void
MucRec3DRoad::SetGroup(const int& Group)
{
  if (Group >= 0) m_Group = Group;
}

// Set the fit parameters : slope and intercept for XZ and YZ.
void 
MucRec3DRoad::SetSimpleFitParams(const float& slopeZX, 
				    const float& interceptZX,
				    const float& slopeZY, 
				    const float& interceptZY)
{
  m_SlopeZX     = slopeZX; 
  m_InterceptZX = interceptZX;
  m_SlopeZY     = slopeZY; 
  m_InterceptZY = interceptZY;
}


// the index for this road in the current event.
int
MucRec3DRoad::GetIndex() const
{
  return m_Index;
}

// the group index for this road in the current event.
int
MucRec3DRoad::GetGroup() const
{
  return m_Group;
}

// In which part was this road found?
int
MucRec3DRoad::GetPart() const
{
  return m_Part;
}

// In which seg was this road found?
int
MucRec3DRoad::GetSeg() const
{
  return m_Seg;
}

// Which gap is the last one with hits attached to this road?
int
MucRec3DRoad::GetLastGap() const
{
  return m_LastGap;
}

// How many gaps provide hits attached to this road?
int
MucRec3DRoad::GetNGapsWithHits() const
{
  int count = 0;
  for (int gap = 0; gap < (int)MucID::getGapMax(); gap++) {
    if ( (m_Road0->GetHitsPerGap(gap) > 0) || (m_Road1->GetHitsPerGap(gap) > 0) ) {
      count++;
    }
  }
  return count;
}

// How many hits in all does this road contain?
int
MucRec3DRoad::GetTotalHits() const
{
  return ( m_Road0->GetTotalHits() + m_Road1->GetTotalHits() );
}

// How many hits per gap does this road contain?
int
MucRec3DRoad::GetHitsPerGap(const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::HitsPerGap-E1  invalid gap = " << gap << endl;
    return 0;
  }

  return ( m_Road0->GetHitsPerGap(gap) + m_Road1->GetHitsPerGap(gap) );
}

// How many hits were attached in the gap with the most attached hits?
int
MucRec3DRoad::GetMaxHitsPerGap() const
{
  int nHit0 = m_Road0->GetMaxHitsPerGap();
  int nHit1 = m_Road1->GetMaxHitsPerGap();
  if( nHit0 > nHit1 ) {
    return nHit0;
  }
  else {
    return nHit1;
  }
}

// Does this road contain any hits in the given gap?
bool
MucRec3DRoad::HasHitInGap(const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::HasHitInGap-E2  invalid gap = " << gap << endl;
    return false;
  }
  
  return ( m_Road0->HasHitInGap(gap) || m_Road1->HasHitInGap(gap) );
}

// How many hits do two roads share?
int
MucRec3DRoad::GetNSharedHits(const MucRec3DRoad* road) const
{
  if (!road) {
    return 0;
  }

  int count = 0;
  count += m_Road0->GetNSharedHits( road->Get2DRoad(int(0)) );
  count += m_Road1->GetNSharedHits( road->Get2DRoad(int(1)) );

  return count;
}

// Difference between the last gap in the two 2-D roads.
int
MucRec3DRoad::GetLastGapDelta() const
{
  return abs( m_Road0->GetLastGap() - m_Road1->GetLastGap() );
}

// Difference between the number of hits in the two 2-D roads.
int
MucRec3DRoad::GetTotalHitsDelta() const
{
  return abs( m_Road0->GetTotalHits() - m_Road1->GetTotalHits() );
}

// How many degrees of freedom in the trajectory fit?
int
MucRec3DRoad::GetDegreesOfFreedom() const
{
  return m_DOF;
}

// Chi-square parameter (per degree of freedom) of the trajectory fit.
float
MucRec3DRoad::GetReducedChiSquare() const
{
  if (m_DOF < 0) {
    return -1.0;
  } 
  else {
    if (m_DOF == 0) {
      return 0.0;
    }
    else {
      return (m_Chi2/m_DOF);
    }
  }
}

// Get Z-X dimension slope.
float 
MucRec3DRoad::GetSlopeZX() const
{
  return m_SlopeZX;
}

// Get Z-X dimension intercept.
float 
MucRec3DRoad::GetInterceptZX() const
{
  return m_InterceptZX;
}

// Get Z-Y dimension slope.
float 
MucRec3DRoad::GetSlopeZY() const
{
  return m_SlopeZY;
}

// Get Z-Y dimension intercept.
float 
MucRec3DRoad::GetInterceptZY() const
{
  return m_InterceptZY;
}

// Get a pointer to the hit attached in a particular gap.
MucRecHit*
MucRec3DRoad::GetHit(const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::GetHit-E1  invalid gap = " << gap << endl;
    return 0;
  }
  
  if ( m_Road0->GetHit(gap) ) {
    return m_Road0->GetHit(gap);
  }
  else {
    if ( m_Road1->GetHit(gap) ) {
      return m_Road1->GetHit(gap);
    }
  }
  return 0;
}

int 
MucRec3DRoad::RefitNoCurrentGap(  const int &gap,
                         float &slopeZX,
                         float &interceptZX,
                         float &slopeZY,
                         float &interceptZY)
{
  float vx,  vy,  vk,  vr;
  float x0,  y0,  k0,  r0;
  float svx, svy, svk, svr;
  float sx0, sy0, sk0, sr0;
  int   xdof, ydof;
  float chi2x, chi2y;
  float spos0, spos1;

  // Determine the projection point of the "simple" fit to the desired gap.
  int status1, status2;

  // now these 2 2D road have been refit without current gap!
  if( m_Part == 1 ) {
    status1 = m_Road0->SimpleFitNoCurrentGap(gap, vr, r0, svr, sr0, chi2x, xdof);
    status2 = m_Road1->SimpleFitNoCurrentGap(gap, vk, k0, svk, sk0, chi2y, ydof);

    m_Road0->GetPosSigma(spos0);
    m_Road1->GetPosSigma(spos1);

    TransformPhiRToXY(vk,  vr,  k0,  r0,
          vx,  vy,  x0,  y0);

//    TransformPhiRToXY(svk, svr, sk0, sr0, svx, svy, sx0, sy0);
    TransformPhiRToXY(vk+svk, vr+svr, k0+sk0, r0+sr0, svx, svy, sx0, sy0);
    svx -= vx; svy -= vy; sx0 -= x0; sy0 -= y0;
  }
  else {
    status1 = m_Road0->SimpleFitNoCurrentGap(gap, vy, y0, svy, sy0, chi2y, ydof);
    status2 = m_Road1->SimpleFitNoCurrentGap(gap, vx, x0, svx, sx0, chi2x, xdof);
    m_Road0->GetPosSigma(spos0);
    m_Road1->GetPosSigma(spos1);
  }

  if(status1 == -1 || status2 == -1) return -1;
  slopeZX = vx; interceptZX = x0;
  slopeZY = vy; interceptZY = y0;
  
  return 0;
}

// Where does the trajectory of this road intersect a specific strip? ---for calib
vector<Identifier>
MucRec3DRoad::ProjectToStrip(const int& part, const int& gap, const HepPoint3D& gPoint, const Hep3Vector&gDirection)
{
  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid part = " << part << endl;
  } 

  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid gap = " << gap << endl;
  }
 
  return MucGeoGeneral::Instance()->FindIntersectStrips(part, gap, gPoint, gDirection);
} 

vector<Identifier>
MucRec3DRoad::ProjectToStrip(const int& part, const int& gap, const HepPoint3D& gPoint, const Hep3Vector&gDirection, vector<int>&padID, vector<float>&intersect_x, vector<float>&intersect_y, vector<float>&intersect_z)
{
  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid part = " << part << endl;
  }

  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid gap = " << gap << endl;
  }

  return MucGeoGeneral::Instance()->FindIntersectStrips(part, gap, gPoint, gDirection, padID, intersect_x, intersect_y, intersect_z);
}

// Where does the trajectory of this road intersect a specific gap?
void
MucRec3DRoad::ProjectWithSigma(const int& gap, float& x, float& y, float& z, float& sigmaX, float& sigmaY, float& sigmaZ)
{
  x = 0.0; sigmaX = 0.0;
  y = 0.0; sigmaY = 0.0;
  z = 0.0; sigmaZ = 0.0;

  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid gap = " << gap << endl;
    return;
  }

  float vx,  vy,  vk,  vr;
  float x0,  y0,  k0,  r0;
  float svx, svy, svk, svr;
  float sx0, sy0, sk0, sr0;
  int   xdof, ydof;
  float chi2x, chi2y;
  float spos0, spos1;

  // Determine the projection point of the "simple" fit to the desired gap.
  if( m_Part == 1 ) {
    m_Road0->GetSimpleFitParams(vr, r0, svr, sr0, chi2x, xdof);
    m_Road1->GetSimpleFitParams(vk, k0, svk, sk0, chi2y, ydof);
    m_Road0->GetPosSigma(spos0);
    m_Road1->GetPosSigma(spos1);
    
    TransformPhiRToXY(vk,  vr,  k0,  r0, vx,  vy,  x0,  y0);
    //TransformPhiRToXY(svk, svr, sk0, sr0, svx, svy, sx0, sy0);
    TransformPhiRToXY(vk+svk, vr+svr, k0+sk0, r0+sr0, svx, svy, sx0, sy0);
    svx -= vx; svy -= vy; sx0 -= x0; sy0 -= y0;
  }
  else {
    m_Road0->GetSimpleFitParams(vy, y0, svy, sy0, chi2y, ydof);
    m_Road1->GetSimpleFitParams(vx, x0, svx, sx0, chi2x, xdof);
    m_Road0->GetPosSigma(spos0);
    m_Road1->GetPosSigma(spos1);
        
  }

/*    
  cout << " vr " << vr << " vk " << vk << endl;
  cout << " r0 " << r0 << " k0 " << k0 << endl;
  cout << " vx " << vx << " vy " << vy << endl;
  cout << " x0 " << x0 << " y0 " << y0 << endl;
*/  

  MucGeoGeneral::Instance()->FindIntersection(m_Part, m_Seg, gap,
					      vx,  vy,  1.0, x0,  y0,  0.0,
					      svx, svy, 0.0, sx0, sy0, 0.0,
					      x, y, z,
					      sigmaX, sigmaY, sigmaZ);
/*
  if(fabs(vr)>10000)  ///////////////////if fabs(vr) too big, r0 will be intersection in x coordinate!!!
    MucGeoGeneral::Instance()->FindIntersection(m_Part, m_Seg, gap,
        vx,  vy,  1.0, x0,  y0,  r0,
        svx, svy, 0.0, sx0, sy0, 0.0,
        x, y, z,
        sigmaX, sigmaY, sigmaZ);
*/

  sigmaX = spos1;
  sigmaY = spos0;
  sigmaZ = 0.0;

  float a, b, c;
  float sa, sb, sc; int whichhalf;
  //cout<<"in MucRec3DRoad xyz form linefit "<<x<<" "<<y<<" "<<z<<endl;

  if( m_Part == 1 ) {
    m_Road1->GetSimpleFitParams(a, b, c, whichhalf, sa, sb, sc, chi2x, ydof);
    //cout<<"in MucRec3DRoad "<<vy<<" "<<y0<<" "<<a<<" "<<b<<" "<<c<<endl;
  }

  return;
}

// Where does the trajectory of this road intersect a specific gap?
void
MucRec3DRoad::ProjectNoCurrentGap(const int& gap, float& x, float& y, float& z, float& sigmaX, float& sigmaY, float& sigmaZ,
				  float& quad_x1, float& quad_y1, float& quad_z1, float& quad_x2, float& quad_y2, float& quad_z2)
{

  x = 0.0; sigmaX = 0.0;
  y = 0.0; sigmaY = 0.0;
  z = 0.0; sigmaZ = 0.0;

  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid gap = " << gap << endl;
    return;
  }

  float vx,  vy,  vk,  vr;
  float x0,  y0,  k0,  r0;
  float svx, svy, svk, svr;
  float sx0, sy0, sk0, sr0;
  int   xdof, ydof;
  float chi2x, chi2y;
  float spos0, spos1;

  // Determine the projection point of the "simple" fit to the desired gap.
  
  // now these 2 2D road have been refit without current gap!
  if( m_Part == 1 ) {
    m_Road0->SimpleFitNoCurrentGap(gap, vr, r0, svr, sr0, chi2x, xdof);
    m_Road1->SimpleFitNoCurrentGap(gap, vk, k0, svk, sk0, chi2y, ydof);

    m_Road0->GetPosSigma(spos0);
    m_Road1->GetPosSigma(spos1);
    
    TransformPhiRToXY(vk,  vr,  k0,  r0, vx,  vy,  x0,  y0);
    
    //TransformPhiRToXY(svk, svr, sk0, sr0, svx, svy, sx0, sy0);
    TransformPhiRToXY(vk+svk, vr+svr, k0+sk0, r0+sr0, svx, svy, sx0, sy0);
    svx -= vx; svy -= vy; sx0 -= x0; sy0 -= y0;
  } 
  else {
    m_Road0->SimpleFitNoCurrentGap(gap, vy, y0, svy, sy0, chi2y, ydof);
    m_Road1->SimpleFitNoCurrentGap(gap, vx, x0, svx, sx0, chi2x, xdof);
    m_Road0->GetPosSigma(spos0);
    m_Road1->GetPosSigma(spos1);
        
  } 
  /*  
  cout << " vr " << vr << " vk " << vk << endl;
  cout << " r0 " << r0 << " k0 " << k0 << endl;
  cout << " vx " << vx << " vy " << vy << endl;
  cout << " x0 " << x0 << " y0 " << y0 << endl;
  */

  MucGeoGeneral::Instance()->FindIntersection(m_Part, m_Seg, gap,
                vx,  vy,  1.0, x0,  y0,  0.0,
                svx, svy, 0.0, sx0, sy0, 0.0,
                x, y, z,  
                sigmaX, sigmaY, sigmaZ); 
  if(fabs(vr)>10000)  ///////////////////if fabs(vr) too big, r0 will be intersection in x coordinate!!!
    MucGeoGeneral::Instance()->FindIntersection(m_Part, m_Seg, gap,
        vx,  vy,  1.0, x0,  y0,  r0,
        svx, svy, 0.0, sx0, sy0, 0.0,
        x, y, z,  
        sigmaX, sigmaY, sigmaZ); 
        
  sigmaX = spos1;
  sigmaY = spos0;
  sigmaZ = 0.0;

  float a, b, c;
  float sa, sb, sc; int whichhalf;

  //calc orient now
  int orient = 0;
  if(m_Part == 1 && gap%2 == 0) orient = 1;
  if(m_Part != 1 && gap%2 == 1) orient = 1;

  if(orient == 0) m_Road0->GetSimpleFitParams(a, b, c, whichhalf, sa, sb, sc, chi2x, ydof);
  else            m_Road1->GetSimpleFitParams(a, b, c, whichhalf, sa, sb, sc, chi2x, ydof);

  //cout<<"orient = "<<orient<<"  abc: "<<a<<" "<<b<<" "<<c<<endl;
  quad_x1 = -9999; quad_y1 = -9999; quad_z1 = -9999; quad_x2 = -9999; quad_y2 = -9999; quad_z2 = -9999; 

  if(orient == 0 && m_Road0->GetQuadFitOk()  ||  orient == 1 && m_Road1->GetQuadFitOk() )
  MucGeoGeneral::Instance()->FindIntersectionQuadLocal(m_Part, m_Seg, gap,
	a, b, c, whichhalf, quad_x1, quad_y1, quad_z1, quad_x2, quad_y2, quad_z2, sigmaX, sigmaY, sigmaZ);

  return;
}

void
MucRec3DRoad::Project(const int &gap, const float vx, const float x0,const float vy,const float y0 , float &x, float &y, float &z)
{
  float svx=0, svy=0, sx0=0, sy0=0;
  float sigmaX,sigmaY,sigmaZ;
  MucGeoGeneral::Instance()->FindIntersection(m_Part, m_Seg, gap,
                vx,  vy,  1.0, x0,  y0,  0.0,
                svx, svy, 0.0, sx0, sy0, 0.0,
                x, y, z,
                sigmaX, sigmaY, sigmaZ);
}

// Where does the trajectory of this road intersect two surface of a specific gap?
// a line or a parabola!
void
MucRec3DRoad::Project(const int& gap, float& x1, float& y1, float& z1, float& x2, float& y2, float& z2) //x1: inner; x2: outer
{
  float sigmaX1, sigmaY1, sigmaZ1;
  float sigmaX2, sigmaY2, sigmaZ2;

  x1 = 0.0; sigmaX1 = 0.0;
  y1 = 0.0; sigmaY1 = 0.0;
  z1 = 0.0; sigmaZ1 = 0.0;
  x2 = 0.0; sigmaX2 = 0.0;
  y2 = 0.0; sigmaY2 = 0.0;
  z2 = 0.0; sigmaZ2 = 0.0;

  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec3DRoad::Project-E1  invalid gap = " << gap << endl;
    return;
  }

  float vx,  vy,  vk,  vr;
  float x0,  y0,  k0,  r0;
  float svx, svy, svk, svr;
  float sx0, sy0, sk0, sr0;
  int   xdof, ydof;
  float chi2x, chi2y;

  // Determine the projection point of the "simple" fit to the desired gap.

  if( m_Part == 1 ) {
    m_Road0->GetSimpleFitParams(vr, r0, svr, sr0, chi2x, xdof);
    m_Road1->GetSimpleFitParams(vk, k0, svk, sk0, chi2y, ydof);
    TransformPhiRToXY(vk,  vr,  k0,  r0, 
		      vx,  vy,  x0,  y0);

    //cout<<"in MucRec3DRoad after vx,vy"<<endl;
    TransformPhiRToXY(svk, svr, sk0, sr0,
		      svx, svy, sx0, sy0);
  }
  else {
    m_Road0->GetSimpleFitParams(vy, y0, svy, sy0, chi2y, ydof);
    m_Road1->GetSimpleFitParams(vx, x0, svx, sx0, chi2x, xdof);
  }
  /*  
  cout << " vr " << vr << " vk " << vk << endl;
  cout << " r0 " << r0 << " k0 " << k0 << endl;
  cout << " vx " << vx << " vy " << vy << endl;
  cout << " x0 " << x0 << " y0 " << y0 << endl;
  */

  MucGeoGeneral::Instance()->FindIntersectionSurface(m_Part, m_Seg, gap,
						     vx,  vy,  1.0, x0,  y0,  0.0,
						     svx, svy, 0.0, sx0, sy0, 0.0,
						     x1, y1, z1, x2, y2, z2,
						     sigmaX1, sigmaY1, sigmaZ1,
						     sigmaX2, sigmaY2, sigmaZ2);

  float a, b, c;
  float sa, sb, sc; int whichhalf;
  //cout<<"in MucRec3DRoad xyz form linefit "<<x1<<" "<<y1<<" "<<z1<<" "<<x2<<" "<<y2<<" "<<z2<<endl;

  //if this trajectory is parabola!
  int projectwithquad = 0;
  if( m_Part == 1 && projectwithquad) {
    m_Road1->GetSimpleFitParams(a, b, c, whichhalf, sa, sb, sc, chi2x, ydof);

    //cout<<"in MucRec3DRoad "<<vy<<" "<<y0<<" "<<a<<" "<<b<<" "<<c<<endl;
    if(m_Road1->GetQuadFitOk()){
      MucGeoGeneral::Instance()->FindIntersectionSurface(m_Part, m_Seg, gap,
							 vy,  x0,  y0,  0.0,
							 a, b, c,  //y = a*x*x + b*x +c
							 whichhalf,
							 svx, svy, 0.0, sx0, sy0, 0.0,
							 x1, y1, z1, x2, y2, z2,
							 sigmaX1, sigmaY1, sigmaZ1);
    }

  }

  return;
}

// get a pointer to the 2D road in the 3D road
MucRec2DRoad* 
MucRec3DRoad::Get2DRoad(const int& orient) const
{
  if (orient == 0) {
    return m_Road0;
  }
  else {
    return m_Road1;
  }
}

// Get indices of all hits in the road.
vector<Identifier>
MucRec3DRoad::GetHitsID() const
  
{
  vector<Identifier> idCon;
  vector<Identifier>::iterator hit;
  vector<Identifier> hitRoad0 = m_Road0->GetHitsID();
  vector<Identifier> hitRoad1 = m_Road1->GetHitsID();

  // List will be ordered by orientation.

  // Road0 first ...
  for ( hit = hitRoad0.begin(); hit != hitRoad0.end(); hit++) {
    int index = *hit;
    idCon.push_back(*hit);
    //cout << " MucRec3DRoad::HitIndices Road0 = " << index << endl;
  }
  
  // ... then Road1.
  for ( hit = hitRoad1.begin(); hit != hitRoad1.end();hit++) {
    int index = *hit;
    idCon.push_back(*hit);
    //cout << " MucRec3DRoad::HitIndices Road1 = " << index << endl;
  }
  
  return idCon;
}

// Transform the Phi, ZR cord. to ZX, ZY cord.
void
MucRec3DRoad::TransformPhiRToXY(const float& vk, const float& vr,
				   const float& k0, const float& r0,
				   float& vx, float& vy, 
				   float& x0, float& y0) const
{
  vx = 0.0; vy = 0.0;
  x0 = 0.0; y0 = 0.0;

  // cout << vk << "  " << vr << "  " << endl; 

  //float pi  = 3.1415926536;
  float phi = 0.25*kPi*m_Seg; 

  // From  y0 = vk * x0 + k0;
  //       y0 - r0*sin(phi)
  //      ------------------  = tan(phi + 0.5*kPi); 
  //       x0 - r0*cos(phi)

  if ( cos(phi) + vk*sin(phi) == 0.0 ) {
    cout << " track parallel to gap, some error occurs! " << endl;
    // m_Seg = 1, and vk = -1;
    //            2,           0;
    //            3,           1;
    //            5,          -1;
    //            6,           0;
    //            7,           1;
  }
  else {
    /*
    vx = vr / ( cos(phi) + vk*sin(phi) );
    vy = vk * vx;
    x0 = (r0 - k0)  / ( cos(phi) + vk );
    y0 = vk * x0 + k0;
    */

    float atan_vk = atan(vk);
    if(atan_vk<0) atan_vk += kPi;

    //float deltaPhi = fabs(atan(vk)) - int( fabs(atan(vk))/(0.25*kPi) )*0.25*kPi;
    //if (deltaPhi > 0.125*kPi) deltaPhi = 0.25*kPi - delta

    float deltaPhi =  atan_vk - (m_Seg%4)*(0.25*kPi);

    //vx = vr*cos(deltaPhi)*GetVxSign(vk)/sqrt(1.0+vk*vk);  //change to vr/cos()... liangyt 2007.4.10
    //I think it should be vr/cos...

    vx = (vr/fabs(cos(deltaPhi)))*GetVxSign(vk)/sqrt(1.0+vk*vk);
    vy = vk*vx;
    x0 = (r0 - k0*sin(phi)) / (vk*sin(phi) + cos(phi));
    y0 = vk * x0 + k0;

    float safe_vr = vr;

    if(fabs(vr)>10000){
      if(vr>0) safe_vr = 10000;
      else     safe_vr = -10000;
      vx = (safe_vr/fabs(cos(deltaPhi)))*GetVxSign(vk)/sqrt(1.0+vk*vk);
      vy = vk*vx;
      y0 = -vy*r0;
      x0 = (y0 - k0)/vk;
    }

  }
}

float
MucRec3DRoad::GetVxSign(const float vk) const
{  
  float segmentPhiMin = 0.25*kPi*(m_Seg-2);
  float segmentPhiMax = 0.25*kPi*(m_Seg+2);
  if (m_Seg > 4) { 
    segmentPhiMin -= 2.0*kPi;
    segmentPhiMax -= 2.0*kPi;
  }
  
  // vk = tan(theta); theta = atan(vk); -90<theta<90
  float theta = atan(vk);
  if (theta >= segmentPhiMin && theta <= segmentPhiMax) return 1.0;
  else return -1.0;
}

// Print Hits Infomation.
void
MucRec3DRoad::PrintHitsInfo()
{
  m_Road0->PrintHitsInfo();
  m_Road1->PrintHitsInfo();

  cout << "Intersection with each gap : " << endl;
  for (int iGap = 0; iGap <= m_LastGap; iGap++) {
    float x, y, z, sigmaX, sigmaY, sigmaZ;
    ProjectWithSigma(iGap, x, y, z, sigmaX, sigmaY, sigmaZ);
    cout << "   gap " << iGap 
	 << " (" << x 
	 << ", " << y
	 << ", " << z
	 << ")"
	 << endl; 
  }
}
