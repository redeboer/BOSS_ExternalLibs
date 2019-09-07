//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You      Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_GEO_GENERAL_H
#define MUC_GEO_GENERAL_H

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Geometry/Plane3D.h>
#include <CLHEP/Vector/Rotation.h>

#include "Identifier/Identifier.h"
#include "Identifier/MucID.h"
#include "MucGeomSvc/MucGeoStrip.h"
#include "MucGeomSvc/MucGeoGap.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

using namespace std;
using namespace CLHEP;

//class MucGeoGap;
//class MucGeoStrip;

/**
 * Class MucGeoGeneral contains all of the objects necessary to describe the
 * muon chamber geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucGeoGeneral
{
 public:

  /// Constructor.
  MucGeoGeneral();

  /// Assignment constructor.
  MucGeoGeneral& operator=(const MucGeoGeneral& orig);

  /// Copy constructor.
  MucGeoGeneral(const MucGeoGeneral& orig);

  /// Destructor.
  ~MucGeoGeneral();

  /// Initialize the instance of MucGeoGeneral.
  void Init();

  /// Initialize from database.
  void InitFromDatabase();

  /// Initialize form ASCII.
  void InitFromASCII();

  /// Initialize from xml.
  void InitFromXML();

  /// Destroy the single instance of MucGeoGeneral.
  void Destroy();

  /// Get a pointer to the single instance of MucGeoGeneral.
  static MucGeoGeneral *Instance();

  /// Get a pointer to the gap identified by (part,seg,gap).
  MucGeoGap *GetGap(const int part, const int seg, const int gap) const;

  /// Get a pointer to the gap identified by Indentifier.
  MucGeoGap *GetGap(const Identifier id) const;

  /// Get a pointer to the strip identified by (part,seg,gap,strip).
  MucGeoStrip* GetStrip(const int part, const int seg, const int gap, const int strip) const;
  
  /// Get a pointer to the strip identified by Indentifier.
  MucGeoStrip* GetStrip(const Identifier id) const;

  int GetStripNumInGap(const int part, const int seg, const int gap)
    { return m_StripNumInGap[part][seg][gap]; }

  /// Get total number of strips.
  int GetStripNumTotal();

  /// Find the intersect gap of a trajectory with the given part and gap.
  /// The trajectory is given by the position and direction in global coordinate.
  vector<Identifier> FindIntersectGaps(const int part, const int gap,
				       const HepPoint3D gPoint, const Hep3Vector gDirection);
  
  /// Find the intersect strip of a trajectory with the given part and gap.
  /// The trajectory is given by the position and direction in global coordinate.
  vector<Identifier> FindIntersectStrips(const int part, const int gap,
					 const HepPoint3D gPoint, const Hep3Vector gDirection);
 
  vector<Identifier> FindIntersectStrips(const int part, const int gap,
                                         const HepPoint3D gPoint, const Hep3Vector gDirection,
					 vector<int> &padID, vector<float> &intersection_x,
					 vector<float> &intersection_y,vector<float> &intersection_z);
 
  /// Find the intersection position of a trajectory with the given part and gap.
  /// The trajectory is given by the position and direction in global coordinate.
  vector<HepPoint3D> FindIntersections(const int part, const int gap,
				       const HepPoint3D gPoint, const Hep3Vector gDirection);

  /// Find the intersection position of a trajectory with the given gap.
  /// The trajectory is given by unit vector (vx,vy,vz) and intercept (x0,y0,z0)
  /// in global coordinates, such that (x-x0)/vx = (y-y0)/vy = (z-z0)/vz .
  /// If more than one seg lies along the trajectory, take the first one
  /// intersect with the trajectory.
  void FindIntersection(const int part,      const int seg,       const int gap,
			const float vx,      const float vy,      const float vz,
			const float x0,      const float y0,      const float z0,
			const float sigmaVx, const float sigmaVy, const float sigmaVz,
			const float sigmaX0, const float sigmaY0, const float sigmaZ0,
			float& x,            float& y,            float& z,
			float& sigmaX,       float& sigmaY,       float& sigmaZ);
  

  void FindIntersectionQuadLocal(const int part,      const int seg,       const int gap,
                        const float a,       const float b,       const float c,
                        const int whichhalf,   //help to emit one solution.
                        float& x1,            float& y1,            float& z1,
                        float& x2,            float& y2,            float& z2,
                        float& sigmaX,       float& sigmaY,       float& sigmaZ);




  /// Find the intersection position of a trajectory with the given gap.
  /// The trajectory is given by a parabola and a line in global coordinates,
  /// If more than one seg lies along the trajectory, take the first one
  /// whichhalf help to judge which intersection position is better, x1,y1,z1 return the 
  /// better one . now reserver x2,y2,z2 temp.
  void FindIntersection(const int part,      const int seg,       const int gap,
			const float vy,      
			const float x0,      const float y0,      const float z0,
			const float a,       const float b,       const float c,
			const int whichhalf,   //help to emit one solution.
			const float sigmaVx, const float sigmaVy, const float sigmaVz,
			const float sigmaX0, const float sigmaY0, const float sigmaZ0,
			float& x1,            float& y1,            float& z1,
			float& x2,            float& y2,            float& z2,
			float& sigmaX,       float& sigmaY,       float& sigmaZ);


  void FindIntersectionSurface(const int part,      const int seg,       const int gap,
			       const float vx,      const float vy,      const float vz,
			       const float x0,      const float y0,      const float z0,
			       const float sigmaVx, const float sigmaVy, const float sigmaVz,
			       const float sigmaX0, const float sigmaY0, const float sigmaZ0,
			       float& x1,           float& y1,           float& z1,
			       float& x2,           float& y2,           float& z2,
			       float& sigmaX1,      float& sigmaY1,      float& sigmaZ1,
			       float& sigmaX2,      float& sigmaY2,      float& sigmaZ2);

  /// Find the intersection position of a trajectory with two surfaces of the given gap.
  /// The trajectory is given by a parabola and a line in global coordinates,
  /// If more than one seg lies along the trajectory, take the first one
  /// whichhalf help to judge which intersection position is better,and return the better one
  /// only. x1,y1,z1 is the intersection position with inner surface and x2,y2,z2 be the outer! 
  void FindIntersectionSurface(const int part,      const int seg,       const int gap,
			const float vy,      
			const float x0,      const float y0,      const float z0,
			const float a,       const float b,       const float c,
			const int whichhalf,   //help to emit one solution.
			const float sigmaVx, const float sigmaVy, const float sigmaVz,
			const float sigmaX0, const float sigmaY0, const float sigmaZ0,
			float& x1,            float& y1,            float& z1,
			float& x2,            float& y2,            float& z2,
			float& sigmaX,       float& sigmaY,       float& sigmaZ);


  /// Clear the fgpMucGeoGap and fgpMucGeoStrip vector containers.
  void Clear();

 private:

  // Have we initialize the geometry.
  static int m_gGeometryInit;                    
  // Pointer to the instance of MucGeoGeneral.
  static MucGeoGeneral *m_gpMucGeoGeneral;  

  // map containing the pointers to gaps.
  static map<Identifier, MucGeoGap*> m_gpMucGeoGap;   
  // map containing the pointers to strips.
  static map<Identifier, MucGeoStrip*> m_gpMucGeoStrip;
  
  static const int m_kPartNum  = 3;
  static const int m_kSegMax   = 8;
  static const int m_kGapMax   = 9;
  static const int m_kStripMax = 112;
  
  // number of strips on each gap.
  //int m_StripNumInGap[MucID::getPartNum()][MucID::getSegMax()][MucID::getGapMax()];
  int m_StripNumInGap[m_kPartNum][m_kSegMax][m_kGapMax];
};

ostream& operator << (ostream& s, const MucGeoGeneral& geom);

#endif   /* MUC_GEO_GENERAL_H */
