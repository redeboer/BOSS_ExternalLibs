//$id$
//
//$log$

/*
 *    2003/12/12   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_HIT_H
#define MUC_REC_HIT_H

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"
#include "MucGeomSvc/MucGeomSvc.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

using namespace std;
using namespace CLHEP;

extern const  CLID &CLID_MucRecHit;

class MucRecHit : virtual public ContainedObject 
{
 public:

  /// Constructor.
  MucRecHit() {;}

  /// Constructor.
  MucRecHit(const Identifier& id);

  /// Constructor.
  MucRecHit(const int part  = -1,
	    const int seg   = -1,
	    const int gap   = -1,
	    const int strip = -1);

  /// Copy constructor.
  MucRecHit(const MucRecHit& source);
  
  /// Assignment operator.
  MucRecHit& operator=(const MucRecHit& orig);
  
  /// Destructor.
  ~MucRecHit();

  virtual const CLID& clID() const { return MucRecHit::classID(); }
  static  const CLID& classID() { return CLID_MucRecHit; }

 public:
  
  /// Get soft identifier of this hit.
  Identifier GetID() const { return m_MucID; }
  
  /// Get Part.
  int Part() const { return MucID::part(m_MucID); }

  /// Get Seg.
  int Seg() const { return MucID::seg(m_MucID); }
  
  /// Get Gap.
  int Gap() const { return MucID::gap(m_MucID); }
  
  /// Get Strip.
  int Strip() const { return MucID::strip(m_MucID); }

  /// Get geometry data for the gap containing this hit.
  MucGeoGap *GetGap() const { return m_pMucGeoGap; }
  
  /// Get geometry data for the strip containing this hit.
  MucGeoStrip *GetStrip() const { return m_pMucGeoStrip; } 

  /// Get Center position of the strip (global coords).
  Hep3Vector GetCenterPos() const;

  /// Get Center position uncertainty of the strip (global coords).
  Hep3Vector GetCenterSigma() const;

  void SetHitMode(int recmode){ m_recmode = recmode;}
  
  int GetHitMode() const {return m_recmode;}

  void SetHitSeed(int seed ){m_IsSeed = seed;}

  int HitIsSeed() const {return m_IsSeed;}

  void SetPadID(int padID){m_padID = padID;}
  void SetIntersectX(float x){m_intersect_x = x;}
  void SetIntersectY(float y){m_intersect_y = y;}
  void SetIntersectZ(float z){m_intersect_z = z;}

  int GetPadID() const {return m_padID;}
  float GetIntersectX() const{return m_intersect_x;}
  float GetIntersectY() const{return m_intersect_y;}
  float GetIntersectZ() const{return m_intersect_z;}

 private:
  
  Identifier   m_MucID;
  int          m_padID;       // Which pad is expected, Only used in expected hit,
  float        m_intersect_x; // intersection position of expected hit
  float        m_intersect_y;
  float        m_intersect_z;
  MucGeoGap   *m_pMucGeoGap;
  MucGeoStrip *m_pMucGeoStrip;
  HepPoint3D   m_CenterPos;   // Position of strip which was hit.
  HepPoint3D   m_CenterSigma; // Uncert. in position of strip.
  int          m_recmode;     // 1: mdc ext ; 2 emc ext
  int          m_IsSeed;
};

typedef ObjectVector<MucRecHit> MucRecHitCol;

#endif  /* MUC_REC_HIT_H */
