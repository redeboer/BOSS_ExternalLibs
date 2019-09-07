//$id$
//
//$log$

/*
 *    2004/03/08   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

using namespace std;

#include <iostream>
#include <vector>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>

#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucTrackParameter.h"
#include "MucRecEvent/MucRec2DRoad.h"
#include "MucRecEvent/MucRec3DRoad.h"
#include "MucGeomSvc/MucConstant.h"

// Constructor.
RecMucTrack::RecMucTrack()
  : //m_trackId(-1),    //--------------->
  m_ExtTrackID(-1),
  m_MdcPos(0.0, 0.0, 0.0),
  m_MdcMomentum(0.0, 0.0, 0.0),
  m_MucPos(0.0, 0.0, 0.0),
  m_MucPosSigma(0.0, 0.0, 0.0),
  m_MucMomentum(0.0, 0.0, 0.0),
  m_CurrentPos(0.0, 0.0, 0.0),
  m_CurrentDir(0.0, 0.0, 0.0),
  m_CurrentInsct(0.0, 0.0, 0.0),
  m_Good3DLine(0),
  m_pHits(0),
  m_pExpectedHits(0),
  m_Intersections(0),
  m_Directions(0)
{ 
  // initialize m_IntersectionInner/Outer.
  for(int igap = 0; igap < 9; igap++){
    m_IntersectionInner[igap].set(-9999,-9999,-9999);
    m_IntersectionOuter[igap].set(-9999,-9999,-9999);    
  }
  m_id = 0;
  m_status = -1;
  m_type = -1;

  m_numHits = 0;  
  m_startPart = -1;
  m_endPart = -1;
  m_brLastLayer = -1;
  m_ecLastLayer = -1;
  m_brFirstLayer = -1;
  m_ecFirstLayer = -1;
  m_ecPart = -1;
  m_numLayers = 0;
  m_maxHitsInLayer = 0;
  m_depth = -99;
  m_dof = 0;
  m_chi2 = 0.0;
  m_rms = 0.0;
  m_deltaPhi = 0.0; 

  m_xPosSigma = 0.0;
  m_yPosSigma = 0.0;
  m_zPosSigma = 0.0;

  m_changeUnit = false;
  m_recmode = 0;
  //added by LI Chunhua
  m_kalrechi2 =0. ;
  m_kaldof =0;
  m_kaldepth = -99;
  m_kalbrLastLayer = -1;
  m_kalecLastLayer = -1;
}

// Assignment constructor.
RecMucTrack&
RecMucTrack::operator=(const RecMucTrack& orig)
{
  // Assignment operator.
  if ( this != &orig ) {             // Watch out for self-assignment!
    //m_trackId         = orig.m_trackId;    //--------------->
    m_ExtTrackID    = orig.m_ExtTrackID;
    m_MdcPos        = orig.m_MdcPos;
    m_MdcMomentum   = orig.m_MdcMomentum;
    m_MucPos        = orig.m_MucPos;
    m_MucPosSigma   = orig.m_MucPosSigma;
    m_MucMomentum   = orig.m_MucMomentum;
    m_CurrentPos    = orig.m_CurrentPos;
    m_CurrentDir    = orig.m_CurrentDir;
    m_CurrentInsct  = orig.m_CurrentInsct;
    m_id            = orig.m_id;
    m_status        = orig.m_status;
    m_type          = orig.m_type;
    m_numHits       = orig.m_numHits;    //--------------->
    m_startPart     = orig.m_startPart;
    m_endPart       = orig.m_endPart;
    m_brLastLayer   = orig.m_brLastLayer;
    m_ecLastLayer   = orig.m_ecLastLayer;
    m_numLayers     = orig.m_numLayers;
    m_maxHitsInLayer= orig.m_maxHitsInLayer;
    m_depth         = orig.m_depth;
    m_dof           = orig.m_dof;
    m_chi2          = orig.m_chi2;
    m_rms           = orig.m_rms;
    m_deltaPhi      = orig.m_deltaPhi;
    m_pHits         = orig.m_pHits;
    m_pExpectedHits = orig.m_pExpectedHits;
    m_Intersections = orig.m_Intersections;
    m_Directions    = orig.m_Directions;
    m_xPosSigma     = orig.m_xPosSigma;
    m_yPosSigma     = orig.m_yPosSigma;
    m_zPosSigma     = orig.m_zPosSigma;
    m_changeUnit    = orig.m_changeUnit;
    m_recmode       = orig.m_recmode;
    //*******
    m_kalrechi2     = orig.m_kalrechi2;
    m_kaldof        = orig.m_kaldof;
    m_kaldepth      =orig.m_kaldepth;
    m_kalbrLastLayer=orig.m_kalbrLastLayer;
    m_kalecLastLayer=orig.m_kalecLastLayer;
  }

  return *this;
}

// Copy constructor.
RecMucTrack::RecMucTrack(const RecMucTrack& source)
  : //m_trackId        (source.m_trackId),    //--------------->
    m_ExtTrackID   (source.m_ExtTrackID),
    m_MdcPos       (source.m_MdcPos),
    m_MdcMomentum  (source.m_MdcMomentum),
    m_MucPos       (source.m_MucPos),
    m_MucPosSigma  (source.m_MucPosSigma),
    m_MucMomentum  (source.m_MucMomentum),
    m_CurrentPos   (source.m_CurrentPos),
    m_CurrentDir   (source.m_CurrentDir),
    m_CurrentInsct (source.m_CurrentInsct),
    m_pHits        (source.m_pHits),
    m_pExpectedHits(source.m_pExpectedHits),
    m_Intersections(source.m_Intersections),
    m_Directions   (source.m_Directions)
{ 
    m_id           = source.m_id;
    m_status       = source.m_status;
    m_type         = source.m_type;
    m_numHits      = source.m_numHits;    //--------------->
    m_startPart    = source.m_startPart;
    m_endPart      = source.m_endPart;
    m_brLastLayer  = source.m_brLastLayer;
    m_ecLastLayer  = source.m_ecLastLayer;
    m_numLayers    = source.m_numLayers;
    m_maxHitsInLayer= source.m_maxHitsInLayer;
    m_depth        = source.m_depth;
    m_dof          = source.m_dof;
    m_chi2         = source.m_chi2;
    m_rms          = source.m_rms;
    m_deltaPhi     = source.m_deltaPhi;
    m_xPosSigma    = source.m_xPosSigma;
    m_yPosSigma    = source.m_yPosSigma;
    m_zPosSigma    = source.m_zPosSigma;
    m_changeUnit   = source.m_changeUnit;
    m_recmode      = source.m_recmode;
    //******
    m_kalrechi2     = source.m_kalrechi2;
    m_kaldof        = source.m_kaldof;
    m_kaldepth      = source.m_kaldepth;
    m_kalbrLastLayer= source.m_kalbrLastLayer;
    m_kalecLastLayer= source.m_kalecLastLayer;
}

RecMucTrack::RecMucTrack(const DstMucTrack& dstTrack)
  :DstMucTrack(dstTrack)
{

  SetDefault();
  
}

RecMucTrack& RecMucTrack::operator=(const DstMucTrack& dstTrack)
{
  SetDefault();
  DstMucTrack::operator=(dstTrack);
  return *this;
}

void RecMucTrack::SetDefault()
{
  m_brFirstLayer = -99;
  m_ecFirstLayer = -99;
  m_Good3DPart   = -99;
}

// Destructor.
RecMucTrack::~RecMucTrack()
{ 
for(int i = 0 ; i < m_pExpectedHits.size(); i++)
          delete m_pExpectedHits[i];
}

// Set the index for this track.
void
RecMucTrack::setTrackId(const int trackId)
{
  if(trackId >= 0) {
    m_trackId = trackId;
  }
}

//now, what we get is cm, so it' multiplied by 10 to convert to mm
//and GeV -> MeV
void
RecMucTrack::SetMdcPos(const float x,
		    const float y,
		    const float z)
{
  m_MdcPos.set(x*10, y*10, z*10);
}

void
RecMucTrack::SetMdcMomentum(const float px,
			 const float py,
			 const float pz)
{
  m_MdcMomentum.set(px*1000, py*1000, pz*1000);
}

void
RecMucTrack::SetMucPos(const float x,
		    const float y,
		    const float z)
{
  m_MucPos.set(x*10, y*10, z*10);
  m_xPos = x*10;
  m_yPos = y*10;
  m_zPos = z*10;
}

void
RecMucTrack::SetMucPosSigma(const float x,
        const float y,
        const float z)
{
  m_MucPosSigma.set(x*10, y*10, z*10);
  m_xPosSigma = x*10;
  m_yPosSigma = y*10;
  m_zPosSigma = z*10;
}

void
RecMucTrack::SetMucMomentum(const float px,
			 const float py,
			 const float pz)
{
  m_MucMomentum.set(px*1000, py*1000, pz*1000);
  m_px = px*1000;
  m_py = py*1000;
  m_pz = pz*1000;
}

void
RecMucTrack::SetExtMucPos(const float x,
		       const float y,
		       const float z)
{
  m_ExtMucPos.set(x*10, y*10, z*10);
}

void
RecMucTrack::SetExtMucMomentum(const float px,
			    const float py,
			    const float pz)
{
  m_ExtMucMomentum.set(px*1000, py*1000, pz*1000);
}

void
RecMucTrack::SetCurrentPos(const float x,
			const float y,
			const float z)
{
  m_CurrentPos.set(x*10, y*10, z*10);
}

void
RecMucTrack::SetCurrentDir(const float x,
			const float y,
			const float z)
{
  m_CurrentDir.set(x*1000, y*1000, z*1000);  //unnecessary, because it's dir, not mom
}

void 
RecMucTrack::SetCurrentInsct(const float x,
			  const float y,
			  const float z)
{
  m_CurrentInsct.set(x, y, z);  /////this is intenal function, it receive "mm", so need not convert.
}

// set corresponding monte carlo track pointer.
//RecMucTrack::SetMCTrack(const BesPersTrack* mcTrack);
//{
//  m_MCTrack = mcTrack;
//}

void
RecMucTrack::SetExtTrack(RecExtTrack *extTrack)
{
  m_ExtTrack = extTrack;
  if (m_ExtTrack) m_ExtTrackID = extTrack->GetTrackId(); 
}

//reload setVecHits

void
RecMucTrack::setVecHits(vector<MucRecHit*>& pHits)
{
  for(int i = 0 ; i < pHits.size(); i++)
    m_vecHits.push_back((pHits[i]->GetID()).get_value());
}

void
RecMucTrack::setExpHits(vector<MucRecHit*>& pHits)
{ 
    for(int i = 0 ; i < pHits.size(); i++)
          m_expHits.push_back((pHits[i]->GetID()).get_value());
}

void
RecMucTrack::GetMdcExtTrack(Hep3Vector  mdcStartPos, Hep3Vector  mdcStartMomentum, int charge,
			 Hep3Vector &mucStartPos, Hep3Vector &mucStartMomentum)
{

  //cm->mm; GeV->MeV//
  mdcStartPos*=10;
  mdcStartMomentum*=1000;
  ////////////////////


  Hep3Vector pt = mdcStartMomentum;
  pt.setZ(0);
  //cout << "pt " << pt.mag() << endl;
  double radius = (pt.mag() * 1e6 / kvC * 1e3) / (fabs(charge * kMagnetField)) ;
  //double startPhi = startP.phi();
  double deltaPhi = -1.0 * (charge / abs(charge)) * kDeltaPhi;
  double deltaZ = (mdcStartMomentum.z() * 1e6 / kvC * 1e3) * kDeltaPhi / (abs(charge) * kMagnetField);

  //cout << "r = " << radius << endl;
  mucStartPos = mdcStartPos;
  mucStartMomentum = mdcStartMomentum;
  double phi;
  int    iter = 0;
  do {
    phi = mucStartMomentum.getPhi() + deltaPhi;
    mucStartPos.setX(mucStartPos.x() + radius * kDeltaPhi * cos(phi));
    mucStartPos.setY(mucStartPos.y() + radius * kDeltaPhi * sin(phi));
    mucStartPos.setZ(mucStartPos.z() + deltaZ);
    
    mucStartMomentum.setPhi(mucStartMomentum.phi() + deltaPhi);
    iter++;
    //cout << endP << "  " << mucStartPos << endl;
  }
  while(IsInsideSuperConductor(mucStartPos) && iter < kMdcExtIterMax);

  //mm->cm; MeV->GeV//
  mucStartPos/=10;
  mucStartMomentum/=1000;
  ////////////////////
}

bool 
RecMucTrack::IsInsideSuperConductor(Hep3Vector pos)
{
  if(pos.mag() < kSuperConductorR && fabs(pos.z()) < kSuperConductorZ) {
    return true;
  }
  else {
    return false;
  }
}

// Attach the given hit to this track.
// Assume that this hit has been verified to be consistent with the road.
void
RecMucTrack::AttachHit(MucRecHit* hit)
{
  //  cout << "Muc2DRoad::AttachHit-I0  hit = " << hit << endl;

  if (!hit) {
    cout << "RecMucTrack::AttachHit-E1  null hit pointer!" << endl;
    return ;
  }
  
  int part = hit->Part();
  int gap  = hit->Gap();
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    // The gap number of the hit is out of range.
    cout << "Muc2DRoad::AttachHit(MucRecHit*), bad gap number = " << gap
	 << endl;
    return;
  }

  // Attach the hit to the road.
  m_pHits.push_back(hit);
  
  //  m_HitDistance[gap] = dX;

  // Now recalculate the total number of hits and the max. number of
  // hits per gap.
  //CountHits();
}

// Where does the trajectory of this track intersect a specific gap?
void
RecMucTrack::Project(const int& part, const int& gap, 
		  float& x, float& y, float& z,
		  int& seg)
{
  seg = -1;
  x = 0.0; y = 0.0; z = 0.0;
  
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    cout << "Muc2DRoad::Project(), invalid gap = " << gap << endl;
    return;
  }

  HepPoint3D pos = m_CurrentPos;
  Hep3Vector dir = m_CurrentDir;

  vector<HepPoint3D> insctCon = MucGeoGeneral::Instance()->FindIntersections(part, gap, pos, dir);
  if( insctCon.size() == 0 ) {
    return;
  }

  HepPoint3D intersection = insctCon[0];
  
  x = intersection.x();
  y = intersection.y();
  z = intersection.z();

  //cout << " x " << x << " y " << y << " z " << z << endl;

  float phi;
  if( (x*x+y*y+z*z) < kMinor) {
    return;
  }
  else {
    if(part == 1) {
      phi = acos(x/sqrt(x*x+y*y));
      if(y < 0) phi = 2.0*kPi - phi;
      phi = fmod((phi + kPi/8.0), 2.0*kPi);
      seg = int(phi/(kPi/4.0)); 
    }
    else {
      if(x >= 0.0) {
	if(y >= 0.0) {
	  seg = 0;
	} 
	else {
	  seg = 3;
	}
      }
      else {
	if(y >= 0.0) {
	  seg = 1;
	}
	else {
	  seg = 2;
	}
      }
    }
  }
}

float 
RecMucTrack::GetHitDistance(const MucRecHit* hit)
{
  if (!hit) {
    cout << "RecMucTrack:GetHitDistance-E1  null hit pointer!" << endl;
    return kInfinity;
  }

  int part = hit->Part();
  int gap  = hit->Gap();
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    cout << "RecMucTrack::GetHitDistance()  bad gap number = " << gap << endl;
    return kInfinity;
  }

  HepPoint3D posHit        = hit->GetCenterPos();
  HepPoint3D posHitLocal   = hit->GetGap()->TransformToGap(posHit);
  HepPoint3D posInsctLocal = hit->GetGap()->TransformToGap(m_CurrentInsct);
  int orient = hit->GetGap()->Orient();

  float distance = -9990;
  if(orient == 1) distance = fabs(posInsctLocal.x() - posHitLocal.x());
  if(orient == 0) distance = fabs(posInsctLocal.y() - posHitLocal.y());

  return distance;
}

//not abs value
float
RecMucTrack::GetHitDistance2(const MucRecHit* hit)
{
  if (!hit) {
    cout << "RecMucTrack:GetHitDistance-E1  null hit pointer!" << endl;
    return kInfinity;
  } 

  int part = hit->Part();
  int gap  = hit->Gap();
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    cout << "RecMucTrack::GetHitDistance()  bad gap number = " << gap << endl;
    return kInfinity;
  }
  
  HepPoint3D posHit        = hit->GetCenterPos();
  HepPoint3D posHitLocal   = hit->GetGap()->TransformToGap(posHit);
  HepPoint3D posInsctLocal = hit->GetGap()->TransformToGap(m_CurrentInsct);
  int orient = hit->GetGap()->Orient();
      
  float distance = -9990;
  if(orient == 1) distance = (posInsctLocal.x() - posHitLocal.x());
  if(orient == 0) distance = (posInsctLocal.y() - posHitLocal.y());
 
  //cout<<"========in RecMucTrack: line insct: "<<posInsctLocal.x()<<" "<<posInsctLocal.y()<<"   ->  "<<posHitLocal.x()<<" "<<posHitLocal.y()<<endl;
 
  return distance;
} 

/// Calculate intersection from all hits attached on this gap.
Hep3Vector 
RecMucTrack::CalculateInsct(const int part,
			 const int seg,
			 const int gap)
{
  MucGeoGap *gapPtr = MucGeoGeneral::Instance()->GetGap(part, seg, gap);
  vector<int> hitSeq;
  for(int i = 0; i < (int)m_pHits.size(); i++) {
    MucRecHit *aHit = m_pHits[i];
    if(aHit->Part() == part &&
       aHit->Seg()  == seg  &&
       aHit->Gap()  == gap) {
      hitSeq.push_back(i);
    }  
  }    
  int nHitInGap = hitSeq.size();
  //cout << "nHitInGap " << nHitInGap << endl;
  
  HepPoint3D insctLocal = gapPtr->TransformToGap(m_CurrentInsct);
  //HepPoint3D newInsct(0.0, 0.0, 0.0);
  HepPoint3D newInsctLocal = insctLocal;

  vector<float> x;
  for(int i = 0; i < nHitInGap; i++) x.push_back(0);
  float xInsct = 0, xNewInsct = 0;


  int orient = gapPtr->Orient();
  if(orient == 1) xInsct = insctLocal.x();
  if(orient == 0) xInsct = insctLocal.y();
  
  for(int i = 0; i < nHitInGap; i++) {
    float xStrip, yStrip, zStrip;
    (m_pHits[hitSeq[i]])->GetStrip()->GetCenterPos(xStrip, yStrip, zStrip);
    if(orient == 1) x[i] = xStrip;
    if(orient == 0) x[i] = yStrip;
  }
  //cout << "local Old" << insctLocal << endl;

  //if == 0, no direction change;
  if(nHitInGap > 0) {
    xNewInsct = xInsct * kInsctWeight;

    //float minDist = kInfinity;
    for(int i = 0; i < nHitInGap; i++) {
      //if(fabs(x[i] - xInsct) < minDist) {
      //xNewInsct = x[i];    //}
      xNewInsct += x[i] * ((1.0 - kInsctWeight) / nHitInGap);
    }
    
    if(orient == 1) {
      newInsctLocal.setX(xNewInsct);
      newInsctLocal.setY(insctLocal.y());
    }
    if(orient == 0) {
      newInsctLocal.setX(insctLocal.x());
      newInsctLocal.setY(xNewInsct);
    }
  }

  m_CurrentInsct = gapPtr->TransformToGlobal(newInsctLocal);
  //cout << "local  New" << newInsctLocal << endl;
  //cout << "global New" << m_CurrentInsct << endl;
  
  return m_CurrentInsct;
}

void 
RecMucTrack::AttachInsct(Hep3Vector insct)
{
  m_Intersections.push_back(insct);
}

void 
RecMucTrack::AttachDirection(Hep3Vector dir)
{
  m_Directions.push_back(dir);
}

void 
RecMucTrack::CorrectDir()
{
  //cout << "Before CorrectDir(), fCurrentDir " << m_CurrentDir << endl;
  m_CurrentDir = m_CurrentInsct - m_CurrentPos;
  m_CurrentDir.setMag(1.0);
  //cout << "After CorrectDir(), fCurrentDir " << m_CurrentDir << endl;
}

void 
RecMucTrack::CorrectPos()
{
  m_CurrentPos = m_CurrentInsct;
}

void
RecMucTrack::ComputeLastGap() 
{
  int lastGap1, lastGap2;
  int firstGap1, firstGap2;
  lastGap1 = lastGap2 = -1;
  firstGap1 = firstGap2 = 99;
  vector<MucRecHit*>::const_iterator iHit;
  iHit = m_pHits.begin();
  for( ;iHit != m_pHits.end(); iHit++) 
  {
    if(*iHit) 
    {  // Check for a null pointer.
      int part = (*iHit)->Part();
      int gap  = (*iHit)->Gap();
      
      if(part == 1) {
        if(gap > lastGap1)  lastGap1 = gap;      
        if(gap < firstGap1) firstGap1 = gap;
      }
      else {
        if(gap > lastGap2)  { m_ecPart = part; m_endPart = part; lastGap2 = gap; }
        if(gap < firstGap2) firstGap2 = gap;
      }
    }
  }
  
  m_brLastLayer = lastGap1;
  if(firstGap1 == 99) m_brFirstLayer = -1;
  else if(firstGap1>=0&&firstGap1<9) m_brFirstLayer = firstGap1;

  m_ecLastLayer = lastGap2;
  if(firstGap2 == 99) m_ecFirstLayer = -1;
  else if(firstGap2>=0&&firstGap2<8) m_ecFirstLayer = firstGap2;

  //cout<<"MucTrack, br: "<<m_brFirstLayer<<", "<<m_brLastLayer<<"\tec: "<<m_ecFirstLayer<<", "<<m_ecLastLayer<<endl;
}

int
RecMucTrack::ComputeDepth(int method)
{
  if( m_numLayers == 0 ) {
    m_depth = m_depth_3 = -99; return 0; 
  } 
  else if( m_numLayers == 1 && (m_brLastLayer == 0 || m_ecLastLayer == 0) ) {
    m_depth = m_depth_3 = 0; return 0;
  }
  
  m_depth_3 = 0.0;
  
  float brThickness = 0.0;  float ecThickness = 0.0;  float deltaPhi = 0.0;
  int betweenSeg = 0;

  float phi = m_MucMomentum.phi();
  float theta = m_MucMomentum.theta();

  vector<MucRecHit*>::const_iterator iHit;
  int ngap = MucID::getGapMax();
  //cout<<"ngap:\t"<<ngap<<endl;
  
  int Seg[9]; int part, seg, gap, strip;
  for(int gap = 0; gap < ngap; gap++){Seg[gap] = -1;}
  for(iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if(*iHit) {  // Check for a null pointer.
      part   = (*iHit)->Part();
      seg    = (*iHit)->Seg();
      gap    = (*iHit)->Gap();
      strip  = (*iHit)->Strip();
      if(part==1) Seg[gap] = seg;
    }
  }

  int segTrackBr = -1;
  for(int gap = 0; gap <= brLastLayer(); gap++){
    if(Seg[gap] != -1) segTrackBr = Seg[gap];
  }
  // BR, the innermost layer is RPC module
  for(int gap = 0; gap <= brLastLayer(); gap++){
    float thickness = 0.0;
    if(Seg[gap] != -1 && Seg[brLastLayer()-1] != -1 && Seg[gap] != Seg[brLastLayer()-1]) betweenSeg = 1; 
    thickness = MucGeoGeneral::Instance()->GetGap(1, 0, gap)->GetIronThickness();
    //cout<<"RecMucTrack  gap="<<gap<<" brlastgap="<<brLastLayer()<<" "<<thickness<<endl;
    if(sin(m_MucMomentum.theta()) != 0 ) thickness /= sin(m_MucMomentum.theta());
    else ;//cout<<"RecMucTrack::ComputeDepth, In Barrel,theta=0?"<<endl;
    deltaPhi = m_MucMomentum.phi() - segTrackBr*kPi/4;
    if(Seg[gap] == -1 && betweenSeg == 1) {
      thickness += 40;  // gap width
    }
    if(cos(deltaPhi) != 0 ) thickness /= cos(deltaPhi);
    else ;//cout<<"RecMucTrack::ComputeDepth, In Barrel,Cos(phi)=0?"<<endl;
    if(deltaPhi == 0 && Seg[brLastLayer()-1]==2 ) thickness = 0;
    //cout<<"in muctrack "<<thickness<<" "<<brThickness<<" theta "<<m_MucMomentum.theta()<<" phi="<<deltaPhi<<" "<<m_MucMomentum.phi()<<endl;

    if(brFirstLayer()<brLastLayer())  brThickness += thickness; 
    else if(brFirstLayer()==brLastLayer()) {  //only one gap
      if(m_MucMomentum.mag()>1000 || brFirstLayer()<2)  brThickness += thickness;  //high momentum or only one or two gap
      //cout<<"mom="<<m_MucMomentum.mag()<<" "<<brFirstLayer()<<" "<<brThickness<<" "<<thickness<<endl;
    }
    else cout<<"in RecMucTrack: Wrong Gap Info"<<endl;

    //cout<<brThickness<<endl;
  }

  //cout<<"eclastgap= "<<ecLastLayer()<<"  ecfirstgap= "<<ecFirstLayer()<<endl;

  // EC, the innermost layer is Iron
  //for (int gap = ecFirstLayer(); gap!=-1&&gap <= ecLastLayer(); gap++) {
  for (int gap = 0; gap!=-1&&gap <= ecLastLayer(); gap++) {
    ecThickness += MucGeoGeneral::Instance()->GetGap(0, 0, gap)->GetIronThickness();
  }
  
  if (cos(theta) != 0.0) ecThickness /= cos(theta);
  else ;//cout << "RecMucTrack::ComputeDepth, In EndCap, Track theta = 90.0 ? " << endl;
  ecThickness  = fabs(ecThickness);
  
  //cout<<"eclastgap= "<<ecLastLayer()<<" ecthickness="<<ecThickness<<endl;

  if(method == 2){
    //barrel first
    if((m_Good3DLine == 1) &&(m_Good3DPart == 1)){
      if(m_IntersectionInner[0].x()!=-9999){
	for(int gap = 1;gap <= brLastLayer(); gap++)//Inner[gap1]-Outer[gap0] ...
	  {
	  if(m_IntersectionInner[gap].x() != -9999 && m_IntersectionInner[gap-1].x() != -9999)
	    m_depth_3 += (m_IntersectionInner[gap] - m_IntersectionOuter[gap-1]).mag();
	  }
	//may be pass some gap in endcap!
	m_depth_3 += ecThickness;
      }
      else m_depth_3 = brThickness + ecThickness;
    }
    if((m_Good3DLine == 1) &&(m_Good3DPart != 1)){
      for(int gap = 1;gap <= ecLastLayer(); gap++)//Inner[gap1]-Outer[gap0] ...
      {
	if(m_IntersectionInner[gap].x() != -9999 && m_IntersectionInner[gap-1].x() != -9999)
	  m_depth_3 += (m_IntersectionInner[gap] - m_IntersectionOuter[gap-1]).mag();
      }
      //may be pass some gap in barrel!
      m_depth_3 += brThickness;
      if (cos(theta) != 0.0) m_depth_3 += 40/cos(theta); //there is 1 absorber before first gap in endcap!
    }
    if(m_Good3DLine == 0) m_depth_3 = brThickness + ecThickness;
    if(m_depth_3>2000) m_depth_3 = brThickness + ecThickness;  //unreasonable depth! so use previous method!
  }
  else  //method == 1 linefit
    {
      m_depth_3 = brThickness + ecThickness;

    }
  
  double offset = 50.0;
  //if(GetTotalHits() > 0) m_depth_3 += offset;

  m_depth = m_depth_3; 

  //if(m_depth<0||m_depth>2000) m_depth = 0;
  if(m_depth>2000) m_depth = -99;
  //cout<<"depth= "<<m_depth<<endl;

}

void 
RecMucTrack::ComputeDepth()
{
  m_depth = -99.0;
  // Part 1 first.
  float brThickness = 0.0;
  for (int gap = 0; gap <= brLastLayer(); gap++) {
    brThickness += MucGeoGeneral::Instance()->GetGap(1, 0, gap)->GetIronThickness();
  }

  // second alg
  float brThickness_2 = 0.0;
  for (int gap = 0; gap <= brLastLayer(); gap++) {
    if(HasHitInGap(1,gap)){
      brThickness_2 += MucGeoGeneral::Instance()->GetGap(1, 0, gap)->GetIronThickness();
    }
  }
  // third alg
  float brThickness_3 = 0.0;
  vector<MucRecHit*>::const_iterator iHit;
  int ngap = MucID::getGapMax();
  int Seg[9]; int part, seg, gap, strip;
  for(int gap = 0; gap < ngap; gap++){Seg[gap] = -1;}
  for(iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if(*iHit) {  // Check for a null pointer.
      part   = (*iHit)->Part();
      seg    = (*iHit)->Seg();
      gap    = (*iHit)->Gap();
      strip  = (*iHit)->Strip();
      if(part==1) Seg[gap] = seg;
    }
  }
  
  float deltaPhi_3 = 0.0;
  int betweenSeg = 0;

  int segTrackBr = -1;
  for(int gap = 0; gap <= brLastLayer(); gap++){
    if(Seg[gap] != -1) segTrackBr = Seg[gap];
  }

  for(int gap = 0; gap <= brLastLayer(); gap++){
    float thickness = 0.0;
    if(Seg[gap] != -1 && Seg[brLastLayer()-1] != -1 && Seg[gap] != Seg[brLastLayer()-1]) betweenSeg = 1; 
    thickness = MucGeoGeneral::Instance()->GetGap(1, 0, gap)->GetIronThickness();
    if(sin(m_MucMomentum.theta()) != 0 ) thickness /= sin(m_MucMomentum.theta());
    else cout<<"RecMucTrack::ComputeDepth, In Barrel,theta=0?"<<endl;
    //if(Seg[gap] != -1) deltaPhi_3 = m_MucMomentum.phi() - Seg[gap]*kPi/4;
    deltaPhi_3 = m_MucMomentum.phi() - segTrackBr*kPi/4;   //some times, no hits in a gap, but a good track exist!
    if(Seg[gap] == -1 && betweenSeg == 1) {
      cout<<"between segment"<<endl;
      thickness += 40;  // gap width
    }

    if(cos(deltaPhi_3) != 0 ) thickness /= cos(deltaPhi_3);
    else cout<<"RecMucTrack::ComputeDepth, In Barrel,Cos(phi)=0?"<<endl;

    if(deltaPhi_3 == 0 && Seg[brLastLayer()-1]==2 ) thickness = 0;
    //cout<<"in muctrack "<<thickness<<" "<<brThickness_3<<" theta "<<m_MucMomentum.theta()<<" phi="<<deltaPhi_3<<" "<<m_MucMomentum.phi()<<endl;
    brThickness_3 += thickness;
    
  }

  //cout<<"in RecMucTrack: compare thickness "<<brThickness<<" "<<brThickness_2<<" "<<brThickness_3<<endl;

  float phi = m_MucMomentum.phi();
  float deltaPhi = phi - kPi/4*(int)(phi/(kPi/4));
  if (deltaPhi > kPi/8) deltaPhi = kPi/4 - deltaPhi;
  float theta = m_MucMomentum.theta();
//   cout << "br LastGap " << brLastLayer() << " Thick " << brThickness 
//     << " 1/sin(theta) " << 1/sin(theta) << " 1/cos(deltaPhi) " << 1/cos(deltaPhi) << endl;

  if (sin(theta) != 0.0) brThickness /= sin(theta);
  else cout << "RecMucTrack::ComputeDepth, In Barrel, Track theta = 0.0 ? " << endl; 

  brThickness /= cos(deltaPhi);
  brThickness  = fabs(brThickness);
  //cout << "br Depth " << brThickness << endl;
  
  // EC, then
  float ecThickness = 0.0;
  for (int gap = 0; gap <= ecLastLayer(); gap++) {
    ecThickness += MucGeoGeneral::Instance()->GetGap(0, 0, gap)->GetIronThickness();
  }
  //cout << "ec LastGap " << ecLastLayer() << " Thick " << ecThickness
  //   << " 1/cos(theta) " << 1/cos(theta) << endl;
  
  if (cos(theta) != 0.0) ecThickness /= cos(theta);
  else ; //cout << "RecMucTrack::ComputeDepth, In EndCap, Track theta = 90.0 ? " << endl;
  ecThickness  = fabs(ecThickness);
  //cout << "ec Depth " << ecThickness << endl;

  m_depth = brThickness + ecThickness;
  m_depth_3 = brThickness_3 + ecThickness;
  cout << "Depth " << m_depth << " Depth3 = "<<m_depth_3<<endl;
  m_depth = m_depth_3;
  double offset = 50.0;
  if(GetTotalHits() > 0) m_depth += offset; // since brThickness on gap 0 is zero, give an offset for track arriving muc.

}

void 
RecMucTrack::ComputeDistanceMatch()
{
  bool firstHitFound = false;
  MucGeoGap *firstGap = 0; 
  vector<MucRecHit*>::const_iterator iHit;
  vector<MucRecHit*> hitsGap0;
  float stripLocal[3]={0.0, 0.0, 0.0};
  float stripGlobal[3]={0.0, 0.0, 0.0};
  int nStrip = 0;

  int part, seg, gap, strip;
  int barrel_gap0_exist = 0;

  for(iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if(*iHit) {  // Check for a null pointer.
      part   = (*iHit)->Part();
      seg    = (*iHit)->Seg();
      gap    = (*iHit)->Gap();
      strip  = (*iHit)->Strip();
      if(!firstHitFound && gap == 0) {
	firstGap = MucGeoGeneral::Instance()->GetGap(part, seg, gap);
	firstHitFound = true;
      }
      if(firstGap && part == firstGap->Part() && seg == firstGap->Seg() && gap == firstGap->Gap()) {
	//cout<<"in RecMucTrack "<<part<<" "<<seg<<" "<<gap<<" "<<strip<<endl;
	HepPoint3D posHit        = (*iHit)->GetCenterPos();
	HepPoint3D posHitLocal   = (*iHit)->GetGap()->TransformToGap(posHit);
	if(part==1&&gap==0) barrel_gap0_exist = 1;  //exist
	
	stripLocal[0] += posHitLocal.x();
	stripLocal[1] += posHitLocal.y();
	stripLocal[2] += posHitLocal.z(); 

	stripGlobal[0] += posHit.x();   //to calc phi of this strip 
	stripGlobal[1] += posHit.y();
	stripGlobal[2] += posHit.z();
 
	nStrip++;
      }
    }
  }

  //cout<<"in RecMucTrack: extpos "<<m_ExtMucPos<<" mucpos "<< m_MucPos<<endl;
  
  int apart = -1, aseg = -1;
  int nHits = FindSegWithMaxHits(apart, aseg);
  MucGeoGap *fakefirstGap = 0;   // maybe not exist!
  if(apart == -1 && aseg== -1)
    {m_Dist_muc_ext.set(0,0,0);}
  else {
    fakefirstGap = MucGeoGeneral::Instance()->GetGap(apart, aseg, 0);
    HepPoint3D fextLocal = fakefirstGap->TransformToGap(m_ExtMucPos);
    HepPoint3D fmucLocal = fakefirstGap->TransformToGap(m_MucPos);
    float dist_x = fextLocal.x() - fmucLocal.x();
    float dist_y = fextLocal.y() - fmucLocal.y();
    float dist_z = fextLocal.z() - fmucLocal.z();
    m_Dist_muc_ext.set(dist_x,dist_y,dist_z);
    //cout<<"in RecMucTrack dist = "<<dist_x<<" "<<dist_y<<" "<<dist_z<<endl;
    if (fakefirstGap->Orient() == 0) { // part 0,2
      //cout<<"in RecMucTrack "<< extLocal.y()<<" "<<stripLocal[1]<<endl;
    }
    else {
      //cout<<"in RecMucTrack1 "<< extLocal.x()<<" "<<stripLocal[0]<<endl;
    }
  }

  float distance = -9990;

  if (nStrip == 0 || !firstGap) {
    distance = -9990;
    //cout << "no hits on first gap" << endl;
  }  
  else{
    for (int k = 0; k < 3; k++) stripLocal[k] /= nStrip;
    for (int k = 0; k < 3; k++) stripGlobal[k] /= nStrip;
    
    m_StripPhi.set(stripGlobal[0],stripGlobal[1],stripGlobal[2]);

    HepPoint3D extLocal = firstGap->TransformToGap(m_ExtMucPos);
    // extmucpos may be can be used to identify mu/pion

//     cout<<"in RecMucTrack mom "<<m_MucMomentum.x()<<" "<<m_MucMomentum.y()<<" "<<m_MucMomentum.z()<<endl;
//     cout<<"in RecMucTrack extpos "<<m_ExtMucPos.x()<<" "<<m_ExtMucPos.y()<<" "<<m_ExtMucPos.z()<<endl;
//     cout<<"in RecMucTrack extpos2 "<<ExtMucPos_2.x()<<" "<<ExtMucPos_2.y()<<" "<<ExtMucPos_2.z()<<endl;
//     cout<<"in RecMucTrack extloc "<<extLocal.x()<<" "<<extLocal.y()<<" "<<extLocal.z()<<endl;
    if (firstGap->Orient() == 0) { // part 0,2
      distance = extLocal.y() - stripLocal[1];
      //cout<<"in RecMucTrack "<< extLocal.y()<<" "<<stripLocal[1]<<endl;
    }
    else {
      distance = extLocal.x() - stripLocal[0];
      //cout<<"in RecMucTrack1 "<< extLocal.x()<<" "<<stripLocal[0]<<endl;
    }
  }

  m_distance = distance;

  //use m_chi2 temporary 
  //m_chi2 = distance;
  // cout<<"in RecMucTrack  distance= "<<m_distance<<" n= "<<nStrip<<endl;
}

void 
RecMucTrack::Extend()
{
  float x = m_ExtMucPos.x(), y = m_ExtMucPos.y(), z = m_ExtMucPos.z();
  //cout<<"in MucTrac, MucPos= "<<x<<" "<<y<<" "<<z<<endl;
  float step = 0.005;
  float myMucR = 1720.0;
  float myMucZ = 2110.0;
   int count_=0; //added by LI Chunhua to avoid loop infinitely
   while ( ( (fabs(x)<=myMucR) && (fabs(y)<=myMucR) && ((fabs(x-y)/sqrt(2.))<=myMucR) && ((fabs(x+y)/sqrt(2.))<=myMucR) && (fabs(z)<=myMucZ) )&&count_<1000 ) {
    x += step * m_MucMomentum.x();
    y += step * m_MucMomentum.y();
    z += step * m_MucMomentum.z();
    count_++;
    //cout << "in RecMucTrack+ x " << x << " y " << y << " z " << z <<" mom "<< m_MucMomentum.x()<< " "<<m_MucMomentum.y()<<" "<<m_MucMomentum.z()<<endl;
  }

   int count = 0; 
  while( !( (fabs(x)<=myMucR) && (fabs(y)<=myMucR) && ((fabs(x-y)/sqrt(2.))<=myMucR) && ((fabs(x+y)/sqrt(2.))<=myMucR) && (fabs(z)<=myMucZ) )&&count<1000 ) {
    x -= step * m_MucMomentum.x();
    y -= step * m_MucMomentum.y();
    z -= step * m_MucMomentum.z();
    count++;
    //cout << "in RecMucTrack- x " << x << " y " << y << " z " << z <<" mom "<< m_MucMomentum.x()<< " "<<m_MucMomentum.y()<<" "<<m_MucMomentum.z()<<endl;
  } 
 
  if(count<1000&&count_<1000){
    if(fabs(x)<2600&&fabs(y)<2600&&fabs(z)<2800){
      m_ExtMucPos.set(x, y, z);
      m_MucPos.set(x,y,z);
      m_xPos = x;
      m_yPos = y;
      m_zPos = z;
    }
  }
 
}

void 
RecMucTrack::LineFit(int fittingMethod)
{
  Extend();

  int part = -1, seg = -1;
  int nHits = FindSegWithMaxHits(part, seg);
//   cout << "RecMucTrack::ComputeDepth(), part " << part << " seg " << seg 
//     << " contains most hits " << nHits << endl;
  if (part < 0 || seg < 0) {
    m_depth = 0;
    //cout << "No hit in this track" << endl;
    return;
  }

  float startPos[3] = {0.0, 0.0, 0.0};
  MucRec2DRoad road2D[2];
  vector<MucRecHit*>::const_iterator iHit;
  for (int orient = 0; orient <= 1; orient++) {
    road2D[orient] = MucRec2DRoad(part, seg, orient, startPos[0], startPos[1], startPos[2],fittingMethod );
    for(iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
      if(*iHit) {  // Check for a null pointer.
        int hitPart   = (*iHit)->Part();
        int hitSeg    = (*iHit)->Seg();
        int hitOrient = (*iHit)->GetGap()->Orient();
        if (hitPart == part && hitSeg == seg && hitOrient == orient) {
          road2D[orient].AttachHit(*iHit);
        }
      }
    }
    //cout << "Orient " << orient
    // << " Slope " << road2D[orient].GetSlope()
    // << " Intercept " << road2D[orient].GetIntercept()
    // << " LastGap " << road2D[orient].GetLastGap()
    // << endl;

  }
  MucRec3DRoad road3D(&road2D[0], &road2D[1]);
  float vx, vy, x0, y0;
  if ( road3D.GetPart() == 1 ){
    road3D.TransformPhiRToXY( road2D[1].GetSlope(),     road2D[0].GetSlope(),     
        road2D[1].GetIntercept(), road2D[0].GetIntercept(),
        vx, vy, x0, y0);
  }
  else {
    vx = road2D[1].GetSlope();
    x0 = road2D[1].GetIntercept();
    vy = road2D[0].GetSlope();
    y0 = road2D[0].GetIntercept();
  }
  
  //cout << "road3D Last Gap " << road3D.GetLastGap() << endl;
  //cout << "vx " << vx << " x0 " << x0 << " vy " << vy << " y0 " << y0 << endl;

  // if number of gaps with hits >= 2 on both orient, change muc pos and momentum
  if (road2D[0].GetNGapsWithHits() >= 2 && road2D[1].GetNGapsWithHits() >= 2) {

    //good 3d road!!!
    m_Good3DLine = 1;
    m_status     = 1;

    m_Good3DPart = road3D.GetPart();
    road3D.SetSimpleFitParams(vx, x0, vy, y0);
    //road3D.PrintHitsInfo();
    
    float startx = 0.0, starty = 0.0, startz = 0.0;
    float startxSigma = 0.0, startySigma = 0.0, startzSigma = 0.0;
    float x1 = 0.0, y1 = 0.0, z1 = 0.0, x2 = 0.0, y2 = 0.0, z2 = 0.0;
    int gap = 0;
    
    if(fittingMethod==2){    //if choose quadratic fitting method!
      for(int igap=0; igap<9;igap++){
        road3D.Project(igap, x1, y1, z1, x2, y2, z2);
        m_IntersectionInner[igap].set(x1,y1,z1);
        m_IntersectionOuter[igap].set(x2,y2,z2);
        //cout<<"3dproject sur "<<x1<<" "<<y1<<" "<<z1<<" "<<x2<<" "<<y2<<" "<<z2<<endl;      
      }
    }

    do {
      //road3D.Project(gap, startx, starty, startz);
      road3D.ProjectWithSigma(gap, startx, starty, startz, startxSigma, startySigma, startzSigma);
      gap++;
    }
    while ( (startx*startx + starty*starty + startz*startz) < kMinor &&
	      gap < (int)MucID::getGapNum(part) );
    
    if(fabs(startx)<2600&&fabs(starty)<2600&&fabs(startz)<2800){
      Hep3Vector MucPos_self;
      MucPos_self.set(startx, starty, startz);
      float dist = (MucPos_self - m_MucPos).mag();
    
      if(dist < 1000 ){  // (mm) maybe the fit is bad, if the dist is too big.
        m_MucPos.set(startx, starty, startz);
        m_xPos = startx;
        m_yPos = starty;
        m_zPos = startz;
      }
    }
    m_MucPosSigma.set(startxSigma, startySigma, startzSigma);
    m_xPosSigma = startxSigma;
    m_yPosSigma = startySigma;
    m_zPosSigma = startzSigma;
    
    //cout<<"in RecMucTrack gap= "<<gap<<" start= "<<startx<<" "<<starty<<" "<<startz<<" "<<endl;
    float momentum = m_MucMomentum.mag();
    float zDir = 1.0;
    if (m_MucMomentum.z() != 0.0) zDir  = m_MucMomentum.z() / fabs(m_MucMomentum.z());
    
    float px = road3D.GetSlopeZX()*zDir;
    float py = road3D.GetSlopeZY()*zDir;
    float pz = zDir;
    float segPhi = 0.25*kPi*seg;
    // if vr is opposite to original, but both are very small, e.x. -0.01 -> + 0.01, or you can say, pt~p, change it to guarantee vx, vy is correct.
    if (part == 1 && px*cos(segPhi)+py*sin(segPhi) < 0.0) { // when in barrel, pt dot segPhi < 0, angle between them > 90deg
      px *= -1.0;
      py *= -1.0;
      pz *= -1.0;
    }

    //if(sqrt(px*px+py*py)>0.01)m_MucMomentum.set(px, py, pz);
    m_MucMomentum.set(px, py, pz);
    m_MucMomentum.setMag(momentum);

    m_px = px; m_py = py; m_pz = pz;
    Hep3Vector phi_mdc, phi_muc;
    phi_mdc.set(m_MdcMomentum.x(),m_MdcMomentum.y(),0);
    phi_muc.set(px,py,0);
    double deltaPhi = phi_mdc.angle(phi_muc);

    m_deltaPhi = deltaPhi;
    m_dof  = road3D.GetDegreesOfFreedom();
    m_chi2 = road3D.GetReducedChiSquare();
    if(m_chi2<0||m_chi2>1000)  m_chi2 = 0;
    m_rms  = 0.0; // road3D.GetReducedChiSquare(); // ??? in MucRecRoad3D


   if(road2D[0].GetNGapsWithHits() >= 3 && road2D[1].GetNGapsWithHits() >= 3){ //need 3+ gap to do refit
      //calc distance between each hit with this track.
      //use GetHitDistance(), so we should provide m_CurrentInsct. 
      float xInsct = 0.0, yInsct = 0.0, zInsct = 0.0, sigmax = 0.0, sigmay = 0.0, sigmaz = 0.0;
      float quad_x1 = 0.0, quad_y1 = 0.0, quad_z1 = 0.0, quad_x2 = 0.0, quad_y2 = 0.0, quad_z2 = 0.0;
      for(int ihit = 0; ihit < m_pHits.size(); ihit++){
        int igap = (m_pHits[ihit])->Gap();
        road3D.ProjectNoCurrentGap(igap, xInsct, yInsct, zInsct, sigmax, sigmay, sigmaz, 
                                   quad_x1, quad_y1, quad_z1, quad_x2, quad_y2, quad_z2);

        SetCurrentInsct(xInsct, yInsct, zInsct);

        float dist_hit_track = GetHitDistance2(m_pHits[ihit]);
        m_distHits.push_back(dist_hit_track);

	//cout<<"===========in RecMucTrack: line dist: "<<dist_hit_track<<endl;

	if(fittingMethod==2) // ------calc local insct in a gap with quad line.
	  {
	   
            Hep3Vector center = m_pHits[ihit]->GetCenterPos();
	    int iPart = m_pHits[ihit]->Part();
	    int iSeg  = m_pHits[ihit]->Seg();
            int iGap  = m_pHits[ihit]->Gap();
	    float xHit, yHit, zHit = 0.;
	    if (iPart == 1) {
		    if ( iGap %2  == 1) {
			    xHit = center.z();
			    yHit = sqrt(center.x()*center.x()
					    + center.y()*center.y());
			    if(iSeg==2) yHit = center.y();  //deal with seg2 seperately! because there is a hole here. 2006.11.9
		    } 
		    else {
			    xHit = center.x();
			    yHit = center.y();
		    }
	    }
	    else {
		    if ( iGap %2 == 0) {
			    xHit = center.z();
			    yHit = center.y();
		    }
		    else {
			    xHit = center.z();
			    yHit = center.x();
		    }
	    }
	    
	    float distance1 = fabs(xHit - quad_x1)/(xHit - quad_x1) * sqrt((xHit - quad_x1)*(xHit - quad_x1) + (yHit - quad_y1)*(yHit - quad_y1));
            float distance2 = fabs(xHit - quad_x2)/(xHit - quad_x2) * sqrt((xHit - quad_x2)*(xHit - quad_x2) + (yHit - quad_y2)*(yHit - quad_y2));

	    float dist_quad = distance1;
	    if(fabs(distance1) > fabs(distance2)) dist_quad = distance2;
	    
      //cout<<"============in RecMucTrack: quad xyz: "<<quad_x1<<" "<<quad_y1<<" "<<quad_z1<<" "<<quad_x2<<" "<<quad_y2<<endl;
	    //cout<<"============in RecMucTrack: hit pos: "<<xHit<<" "<<yHit<<" "<<zHit<<endl;
	    //cout<<"============in RecMucTrack: dist1 = "<<distance1<<" dist2 = "<<distance2<<" dist ="<<dist_quad<<endl; 

	    if(quad_x1 == -9999) m_distHits_quad.push_back(-99);
	    else m_distHits_quad.push_back(dist_quad);

          }
      }
    }
    else{
	for(int ihit = 0; ihit < m_pHits.size(); ihit++){
            m_distHits.push_back(-99);
            m_distHits_quad.push_back(-99);
        }
    }
    ////////////////////////////////////// 

    ///////////////find expected strips for this 3D road
    ComputeLastGap();
    //HepPoint3D initPos(startx, starty,startz);
    HepPoint3D initPos(startx-m_MucMomentum.x()/momentum, starty-m_MucMomentum.y()/momentum,startz-m_MucMomentum.z()/momentum);

    //for(int igap = 0; igap <= m_brLastLayer; igap++){
    for(int igap = 0; igap < 9 ; igap++){
      vector<int> padID;
      vector<float> intersect_x;
      vector<float> intersect_y;
      vector<float> intersect_z;

      //refit---------------------------------
      float zx, zy, x0, y0;
      int status = road3D.RefitNoCurrentGap(igap,zx,x0,zy,y0);
      Hep3Vector mom_refit;
      if(status == 0){ //refit succeed
	      float zDir = 1.0;
	      if (m_MucMomentum.z() != 0.0) zDir  = m_MucMomentum.z() / fabs(m_MucMomentum.z());
	      float px_refit = zx * zDir;  
	      float py_refit = zy * zDir;  	
	      float pz_refit = zDir;
	      float segPhi = 0.25*kPi*seg;
	      // if vr is opposite to original, but both are very small, e.x. -0.01 -> + 0.01, or you can say, pt~p, change it to guarantee vx, vy is correct.
	      if (part == 1 && px_refit*cos(segPhi)+py_refit*sin(segPhi) < 0.0) { // when in barrel, pt dot segPhi < 0, angle between them > 90deg
		      px_refit *= -1.0;
		      py_refit *= -1.0;
		      pz_refit *= -1.0;
	      }

	      mom_refit.setX(px_refit);
	      mom_refit.setY(py_refit);
	      mom_refit.setZ(pz_refit);
	      mom_refit.setMag(momentum);
      }
      else mom_refit = m_MucMomentum;  //use the former momentum

      HepPoint3D initPos_refit;
      if(status == 0){
	      float initPosx_refit, initPosy_refit, initPosz_refit;
	      float sigmax_refit, sigmay_refit, sigmaz_refit;
	      road3D.Project(0, zx, x0, zy, y0, initPosx_refit, initPosy_refit, initPosz_refit);
	      initPos_refit.setX(initPosx_refit-mom_refit.x()/momentum);
	      initPos_refit.setY(initPosy_refit-mom_refit.y()/momentum);
	      initPos_refit.setZ(initPosz_refit-mom_refit.z()/momentum);
      }
      else initPos_refit = initPos;

      //cout<<"initPos: "<<initPos<<"  "<<initPos_refit<<endl;
      //if((initPos - initPos_refit).mag()>100) cout<<"--------=====------to far"<<endl;
      //refit---------------------------------
      //cout<<"no gap: "<<igap<<"  mom: "<<m_MucMomentum<<"  refit: "<<mom_refit<<endl;	
      
      //vector<Identifier> MuId = road3D.ProjectToStrip(1,igap,initPos,m_MucMomentum,padID,intersect_x,intersect_y,intersect_z);
      vector<Identifier> MuId = road3D.ProjectToStrip(1,igap,initPos_refit,mom_refit,padID,intersect_x,intersect_y,intersect_z);

      //vector<Identifier> MuId = road3D.ProjectToStrip(1,igap,initPos,mom_refit,padID,intersect_x,intersect_y,intersect_z);

      vector<Identifier>::const_iterator mucid;
      int i = 0;
      for(mucid = MuId.begin(); mucid != MuId.end(); mucid++,i++)
      {
        MucRecHit *pHit = new MucRecHit(MucID::part(*mucid),MucID::seg(*mucid),MucID::gap(*mucid),MucID::strip(*mucid));
        pHit->SetPadID(padID[i]);
        pHit->SetIntersectX(intersect_x[i]);
        pHit->SetIntersectY(intersect_y[i]);
        pHit->SetIntersectZ(intersect_z[i]);

        m_pExpectedHits.push_back(pHit);
      }
    }

    if(m_endPart!=-1&&m_ecLastLayer!=-1)
    {
      //for(int igap = 0; igap <= m_ecLastLayer; igap++){
      for(int igap = 0; igap < 8; igap++){
        vector<int> padID;
        vector<float> intersect_x;
        vector<float> intersect_y;
        vector<float> intersect_z;

	//refit---------------------------------
	float zx, zy, x0, y0;
	//int status = road3D.RefitNoCurrentGap(igap,zy,y0,zx,x0);  //!!!!!different sequence
	int status = road3D.RefitNoCurrentGap(igap,zx,x0,zy,y0); 
	Hep3Vector mom_refit;
	if(status == 0){ //refit succeed
		float zDir = 1.0;
		if (m_MucMomentum.z() != 0.0) zDir  = m_MucMomentum.z() / fabs(m_MucMomentum.z());
		float px_refit = zx * zDir;
		float py_refit = zy * zDir;
		float pz_refit = zDir;
		float segPhi = 0.25*kPi*seg;
		// if vr is opposite to original, but both are very small, e.x. -0.01 -> + 0.01, or you can say, pt~p, change it to guarantee vx, vy is correct.
		if (part == 1 && px_refit*cos(segPhi)+py_refit*sin(segPhi) < 0.0) { // when in barrel, pt dot segPhi < 0, angle between them > 90deg
			px_refit *= -1.0;
			py_refit *= -1.0;
			pz_refit *= -1.0;
		}

		mom_refit.setX(px_refit);
		mom_refit.setY(py_refit);
		mom_refit.setZ(pz_refit);
		mom_refit.setMag(momentum);
	}
	else mom_refit = m_MucMomentum;  //use the former momentum

	HepPoint3D initPos_refit;
	if(status == 0){
		float initPosx_refit, initPosy_refit, initPosz_refit;
		float sigmax_refit, sigmay_refit, sigmaz_refit;
		road3D.Project(0, zx, x0, zy, y0, initPosx_refit, initPosy_refit, initPosz_refit);
		initPos_refit.setX(initPosx_refit-mom_refit.x()/momentum);
		initPos_refit.setY(initPosy_refit-mom_refit.y()/momentum);
		initPos_refit.setZ(initPosz_refit-mom_refit.z()/momentum);
	}
	else initPos_refit = initPos;
        //refit---------------------------------
        //cout<<"mom: "<<m_MucMomentum<<" "<<mom_refit<<" pos: "<<initPos<<"  "<<initPos_refit<<endl;  
        //vector<Identifier> MuId = road3D.ProjectToStrip(m_endPart,igap,initPos,m_MucMomentum,padID,intersect_x,intersect_y,intersect_z);

        vector<Identifier> MuId = road3D.ProjectToStrip(m_endPart,igap,initPos_refit,mom_refit,padID,intersect_x,intersect_y,intersect_z);

        vector<Identifier>::const_iterator mucid;
	int i = 0;
        for(mucid = MuId.begin(); mucid != MuId.end(); mucid++,i++)
        {
          MucRecHit *pHit = new MucRecHit(MucID::part(*mucid),MucID::seg(*mucid),MucID::gap(*mucid),MucID::strip(*mucid));
          pHit->SetPadID(padID[i]);
          pHit->SetIntersectX(intersect_x[i]);
          pHit->SetIntersectY(intersect_y[i]);
          pHit->SetIntersectZ(intersect_z[i]);

          m_pExpectedHits.push_back(pHit);
        }
      }
    }

    //cout<<"in RecMucTrack push back expected hits  size= "<<m_pExpectedHits.size()<<" ? "<<m_pHits.size()<<endl;
    for(int i=0; i< m_pExpectedHits.size(); i++)
    {
      MucRecHit *ihit = m_pExpectedHits[i];
      //cout<<"expected Hits: "<<ihit->Part()<<"  "<<ihit->Seg()<<"  "<<ihit->Gap()<<"  "<<ihit->Strip()<<endl;
      //cout<<"pad: "<<ihit->GetPadID()<<"  pos: "<<ihit->GetIntersectX()<<" "<<ihit->GetIntersectY()<<" "<<ihit->GetIntersectZ()<<" "<<endl;
    }

    for(int i=0; i< m_pHits.size(); i++)
    {
      MucRecHit *ihit = m_pHits[i];
      //cout<<"attachedd Hits: "<<ihit->Part()<<"  "<<ihit->Seg()<<"  "<<ihit->Gap()<<"  "<<ihit->Strip()<<" isseed: "<<ihit->HitIsSeed()<<endl;
    }
    //cout<<"1st: "<<brFirstLayer()<<"  "<<ecFirstLayer()<<"  last: "<<brLastLayer()<<"  "<<ecLastLayer()<<endl;
  
  }
  else {
    //calc distance between each hit with this track.
    //initialize distHits
    for(int ihit = 0; ihit < m_pHits.size(); ihit++){
      m_distHits.push_back(-99);
      m_distHits_quad.push_back(-99);
    }
  }
  //cout << "Muc Pos: x " << m_MucPos.x() << " y " << m_MucPos.y() << " z " << m_MucPos.z() << endl;
}

void
RecMucTrack::ComputeNGapsWithHits() 
{
  int ngap = MucID::getGapMax();
  int gap, count = 0;
  vector<int> firedGap;
  for(gap = 0; gap < ngap; gap++) firedGap.push_back(0);

  vector<MucRecHit*>::const_iterator iHit;
  for (iHit=m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      gap = (*iHit)->Gap();
      firedGap[gap] = 1;
    }
  }
  
  for ( gap = 0; gap < ngap; gap++) {
    count += firedGap[gap];
  }

  m_numHits   = m_pHits.size();
  m_numLayers = count;
  //cout<<"nLayer:\t"<<m_numLayers<<endl;
}

int
RecMucTrack::GetTotalHits() const
{ 
  return m_pHits.size();
}

int
RecMucTrack::GetHitInGap(const int part, const int gap) const
{
  if ( part < 0 || part > 2 ) {
    cout << "RecMucTrack::GetHitInGap(), invalid part " << part << endl;
    return 0;
  }
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    cout << "RecMucTrack::GetHitInGap(), invalid gap " << gap << endl;
    return 0;
  } 
  
  vector<MucRecHit*>::const_iterator iHit;
  int hitsInGap = 0;
  
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    
    if ( !(*iHit) ) {
      cout << "RecMucTrack::GetHitInGap() null hit pointer !"  << endl;
      return 0;
    }
    else {
      if( part == (*iHit)->Part() && gap  == (*iHit)->Gap() ) hitsInGap++;
    }
  }
  
  return hitsInGap;
}

int
RecMucTrack::GetHitInSeg(const int part, const int seg) const
{
  int num = GetHitInSegOrient(part, seg, 0) + GetHitInSegOrient(part, seg, 1);
  return num;
}

int
RecMucTrack::GetHitInSegOrient(const int part, const int seg, const int orient) const
{
  if ( part < 0 || part > 2 ) {
    cout << "RecMucTrack::GetHitInSeg(), invalid part " << part << endl;
    return 0;
  }
  if ( (seg < 0) || (seg >= (int)MucID::getSegNum(part)) ) {
    cout << "RecMucTrack::GetHitInSeg(), invalid seg = " << seg << endl;
    return 0;
  } 
  
  vector<MucRecHit*>::const_iterator iHit;
  int hitsInSeg = 0;
  
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    
    if ( !(*iHit) ) {
      cout << "RecMucTrack::GetHitInSeg(), null hit pointer !"  << endl;
      return 0;
    }
    else {
      if( part == (*iHit)->Part() && seg == (*iHit)->Seg() && 
	  orient == (*iHit)->GetGap()->Orient() ) hitsInSeg++;
    }
  }
  
  return hitsInSeg;
}

int
RecMucTrack::FindSegWithMaxHits(int &findPart, int &findSeg)
{
  int maxHits = 0;
  int hits = 0;
  for(int part = 0; part < (int)MucID::getPartNum(); part++) {
    for(int seg = 0; seg < (int)MucID::getSegNum(part); seg++) {
      hits = GetHitInSeg(part, seg);
      if (hits > maxHits) {
	maxHits  = hits;
	findPart = part;
	findSeg  = seg;
      }
    }
  }

  return maxHits;
}

void
RecMucTrack::ComputeMaxHitsInGap() 
{
  int maxHits = 0;
  int hits = 0;
  for(int part = 0; part < (int)MucID::getPartNum(); part++) {
    for(int gap = 0; gap < (int)MucID::getGapNum(part); gap++) {
      hits = GetHitInGap(part, gap);
      if(hits > maxHits) maxHits = hits;
    }
  }

  m_maxHitsInLayer = maxHits;
}

bool 
RecMucTrack::HasHit(const int part, const int seg, const int gap, const int strip) const
{
  bool found = false;
  vector<MucRecHit*>::const_iterator iHit;

  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      if ( (*iHit)->Part()  == part &&
	   (*iHit)->Seg()   == seg &&
	   (*iHit)->Gap()   == gap &&
	   (*iHit)->Strip() == strip ) {
	found = true;
      }
    }
  }

  return found;
}

bool
RecMucTrack::HasHitInGap(const int part, const int gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    cout << "RecMucTrack::HasHitInGap-E2  invalid gap = " << gap << endl;
    return false;
  }
  
  bool found = false;
  vector<MucRecHit*>::const_iterator iHit;

  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      if ( (*iHit)->Part() == part &&
	   (*iHit)->Gap() == gap ) {
	found = true;
      }
    }
  }

  return found;
}

int
RecMucTrack::GetNSharedHits(const RecMucTrack* track2) const
{
  if (!track2) {
    return 0;
  }

  int count = 0;
  vector<MucRecHit*>::const_iterator iHit1;
  vector<MucRecHit*>::const_iterator iHit2;
  MucRecHit *hit1, *hit2;

  for( iHit1 = m_pHits.begin(); iHit1 != m_pHits.end(); iHit1++){
    for( iHit2 = track2->m_pHits.begin(); 
	 iHit2 != track2->m_pHits.end(); iHit2++){
      hit1 = (*iHit1);
      hit2 = (*iHit2);
      
      if ( (hit1 != 0) && (hit2 != 0) ) {
	if (hit1->GetID() == hit2->GetID()) {
	  count++;
	}
      }
    }
  }
  
  return count;
}

MucRecHit*
RecMucTrack::GetHit(const int part, const int gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    cout << "RecMucTrack::Hit-E1  invalid gap = " << gap << endl;
    return 0;
  }
  
  vector<MucRecHit*>::const_iterator iHit;
  
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      if ( (*iHit)->Part() == part &&
	   (*iHit)->Gap()  == gap) { 
	return (*iHit);
      }
    }
  }
  
  return 0L;
}

vector<MucRecHit*>
RecMucTrack::GetHits() const
{
  return m_pHits;
}

vector<MucRecHit*>
RecMucTrack::GetExpectedHits() const
{
    return m_pExpectedHits;
}

vector<long> 
RecMucTrack::GetHitIndices() const
{
  vector<long> v;
  
  vector<MucRecHit*>::const_iterator iHit;
  for ( iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      long index = (*iHit)->GetID();
      v.push_back(index);
      /*
      cout << " Muc2DRoad::HitIndices  gap  orientation  twopack= "
	   <<  (*iHit)->ChannelID().Plane() << "   "
	   <<  (*iHit)->ChannelID().Orient()  << "   "
	   <<  (*iHit)->ChannelID().TwoPack() << endl; 
      */
    }
  }
  return v;
}

void 
RecMucTrack::ComputeTrackInfo(int fittingmethod)
{
  setVecHits(m_pHits);
  setExpHits(m_pExpectedHits);
  ComputeLastGap();
  ComputeNGapsWithHits(); 
  ComputeMaxHitsInGap();
  //ComputeDepth();
  ComputeDepth(fittingmethod);
  ComputeDistanceMatch();
  OutputUnitChange();
}

void
RecMucTrack::PrintHitsInfo() const
{
  vector<MucRecHit*>::const_iterator iHit;
  for ( iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      float xl, yl, zl;
      (*iHit)->GetStrip()->GetCenterPos(xl, yl, zl);
      HepPoint3D vl(xl, yl, zl);
      HepPoint3D vg = (*iHit)->GetGap()->TransformToGlobal(vl);
      
      cout << " part "   << (*iHit)->Part() 
	   << " seg  "   << (*iHit)->Seg()
	   << " gap  "   << (*iHit)->Gap()
	   << " strip "  << (*iHit)->Strip()
	   << " pos ("   << vg.x() << ", " << vg.y() << ", " << vg.z() << ")"  
	   << endl;
    }
  }

}

void
RecMucTrack::OutputUnitChange()
{
  if(m_changeUnit == false){
    m_depth/=10;

    m_xPos /=10;
    m_yPos /=10;
    m_zPos /=10; 

    m_xPosSigma/=10;
    m_yPosSigma/=10;
    m_zPosSigma/=10;

    m_px /= 1000;
    m_py /= 1000;
    m_pz /= 1000;

    m_distance /= 10;
  }

  m_changeUnit = true;

}
