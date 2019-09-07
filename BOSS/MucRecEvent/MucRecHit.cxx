//$id$
//
//$log$

/*
 *    2003/12/13   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#include "MucRecEvent/MucRecHit.h"

// IMPLEMENTATION OF MucRecHit METHODS.  ================================

// Constructors.
MucRecHit::MucRecHit(const Identifier &id)
  : m_MucID(id)
{
  m_pMucGeoGap   = MucGeoGeneral::Instance()->GetGap(m_MucID);
  m_pMucGeoStrip = MucGeoGeneral::Instance()->GetStrip(m_MucID);
  m_IsSeed = false;
  if (m_pMucGeoGap && m_pMucGeoStrip) {
    float x, y, z, sx, sy, sz;
    
    m_pMucGeoStrip->GetCenterPos(x, y, z);
    m_pMucGeoStrip->GetCenterSigma(sx, sy, sz);
    HepPoint3D pos(x, y, z);
    HepPoint3D sigma(sx, sy, sz);
    m_CenterPos   = m_pMucGeoGap->TransformToGlobal(pos);
    m_CenterSigma = m_pMucGeoGap->RotateToGlobal(sigma);
  }
  else {
    cout << "MucRecHit::MucRecHit(const Identifier&), gap or strip pointer lost" << endl;
  }
}

MucRecHit::MucRecHit(const int part,
		     const int seg,
		     const int gap,
		     const int strip)
{
  m_MucID = MucID::channel_id(part, seg, gap, strip);
  //cout << "part " << part << " seg " << seg << " gap " << gap << " strip " << strip << endl;

  m_pMucGeoGap   = MucGeoGeneral::Instance()->GetGap(m_MucID);
  m_pMucGeoStrip = MucGeoGeneral::Instance()->GetStrip(m_MucID);
  m_recmode      = -1;
  m_IsSeed = false;
  //m_pMucGeoGap   = MucGeoGeneral::Instance()->GetGap(part, seg, gap);
  //m_pMucGeoStrip = MucGeoGeneral::Instance()->GetStrip(part, seg, gap, strip);
  if (m_pMucGeoGap && m_pMucGeoStrip) {
    float x, y, z, sx, sy, sz;
    
    m_pMucGeoStrip->GetCenterPos(x, y, z);
    m_pMucGeoStrip->GetCenterSigma(sx, sy, sz);
    HepPoint3D pos(x, y, z);
    HepPoint3D sigma(sx, sy, sz);
    m_CenterPos   = m_pMucGeoGap->TransformToGlobal(pos);
    //m_CenterSigma = m_pMucGeoGap->RotateToGlobal(sigma); //comment out at 2006.11.30
    m_CenterSigma = sigma; // do not transform
  }
  else {
    cout << "MucRecHit::MucRecHit(), gap or strip pointer lost" << endl;
  }

}

// Copy constructor.
MucRecHit::MucRecHit(const MucRecHit& source)
  : m_MucID(source.m_MucID), 
  m_pMucGeoGap(source.m_pMucGeoGap),
  m_pMucGeoStrip(source.m_pMucGeoStrip),
  m_CenterPos(source.m_CenterPos),
  m_CenterSigma(source.m_CenterSigma)
{ }

// Assignment operator.
MucRecHit& 
MucRecHit::operator=(const MucRecHit& orig)
{
  // Assignment operator.
  if (this != &orig) {             // Watch out for self-assignment!
    m_MucID        = orig.m_MucID;
    m_pMucGeoGap   = orig.m_pMucGeoGap;
    m_pMucGeoStrip = orig.m_pMucGeoStrip;
    m_CenterPos    = orig.m_CenterPos;
    m_CenterSigma  = orig.m_CenterSigma;
  }
  return *this;
}

// Destructor.
MucRecHit::~MucRecHit()
{ 
  // No need to delete MucGeo pointer objects; the allocation/deallocation
  // is done elsewhere.
  m_pMucGeoGap   = 0L;
  m_pMucGeoStrip = 0L;
}

// Get Center position of the strip (global coords).
Hep3Vector
MucRecHit::GetCenterPos() const
{
  Hep3Vector x(m_CenterPos.x(),
	       m_CenterPos.y(),
	       m_CenterPos.z());
  return x;
}

// Get Center position uncertainty of the strip (global coords).
Hep3Vector
MucRecHit::GetCenterSigma() const
{
  Hep3Vector s(m_CenterSigma.x(),
	       m_CenterSigma.y(),
	       m_CenterSigma.z());
  return s;
}

