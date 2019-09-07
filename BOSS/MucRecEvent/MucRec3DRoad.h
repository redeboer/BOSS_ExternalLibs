//$id$
//
//$log$

/*
 *    2003/12/25   Zhengyun You      Peking University
 * 
 *    2005/02/27   Zhengyun You      Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_3DROAD_H
#define MUC_REC_3DROAD_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/ObjectVector.h"

#include "MucRecEvent/MucRec2DRoad.h"

/**
 * Describes a road found in the muon chamber.
 *
 *  A 3Droad is a set of hits in muon chamber strips that we expect to 
 *  be associated with a single particle traversing the muon chamber.
 *  (Roads are found by the MucRecRoadFinder class.)
 *  Methods include adding hits, calculating and retrieving the
 *  trajectory indicated by the hits, projecting along the trajectory
 *  to an arbitrary gap, etc.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucRec3DRoad : virtual public ContainedObject 
{
public:

  /// Constructor.
  MucRec3DRoad(MucRec2DRoad *road0, MucRec2DRoad *road1);

  /// Assignment constructor.
  MucRec3DRoad &operator=(const MucRec3DRoad &orig);
  
  /// Copy constructor.
  MucRec3DRoad(const MucRec3DRoad &source);
  /// Destructor.
  ~MucRec3DRoad();

  /// set the index for this road
  void SetIndex(const int &index);

  /// set the group index for this road
  void SetGroup(const int &Group);

  /// set the fit parameters : slope and intercept for XZ and YZ.
  void SetSimpleFitParams(const float &m_SlopeZX, 
			  const float &m_InterceptZX,
			  const float &m_SlopeZY, 
			  const float &m_InterceptZY);
  /// refit the 3D road without the assigned gap
  int RefitNoCurrentGap(const int &gap,
			 float &slopeZX,
                         float &interceptZX,
                         float &slopeZY,
                         float &interceptZY);

  vector<Identifier> ProjectToStrip(const int& part, const int& gap, const HepPoint3D& gPoint, const Hep3Vector&gDirection);
  
  vector<Identifier> ProjectToStrip(const int& part, const int& gap, const HepPoint3D& gPoint, const Hep3Vector&gDirection, vector<int>&padID, vector<float>&intersect_x, vector<float>&intersect_y, vector<float>&intersect_z);
  
  /// Where does the trajectory of this road intersect a specific gap?
  void ProjectWithSigma(const int &gap, float &x, float &y, float &z, float &sigmaX, float &sigmaY, float &sigmaZ);

  /// Where does the trajectory of this road intersect a specific gap when refit without current gap!!!
  void ProjectNoCurrentGap(const int &gap, float &x, float &y, float &z,  float &sigmaX, float &sigmaY, float &sigmaZ,
			   float& quad_x1, float& quad_y1, float& quad_z1, float& quad_x2, float& quad_y2, float& quad_z2);

  /// Where does the trajectory of this road intersect two surfaces of a specific gap?
  void Project(const int &gap, float &x1, float &y1, float &z1, float &x2, float &y2, float &z2);

  /// project with assigned pos and dir
  void Project(const int &gap, const float zx, const float x0,const float zy,const float y0 , float &x1, float &y1, float &z1);

  /// Where does the trajectory of this road intersect the reference plane?
  //void ProjectToReference(float &x, float &y, float &z);

  /// Where does the trajectory of this road intersect a z=const plane?
  //void ProjectToZ(float &x, float &y, float &zplane);

  /// Transform the Phi, ZR cord. to ZX, ZY cord.
  void TransformPhiRToXY(const float &vk, const float &vr,
			 const float &k0, const float &r0,
			 float &vx, float &vy, 
			 float &x0, float &y0) const;

  /// Get sign of vx in TransformPhiRToXY
  float GetVxSign(const float vk) const;

  /// A unique identifier for this road in the current event.
  int GetIndex() const;
  
  /// In which part was this road found?
  int GetPart() const;

  /// In which segment was this road found?
  int GetSeg() const;

  /// unique index of group this road belongs to
  int GetGroup() const;
  
  /// Which gap is the last one with hits attached to this road?
  int GetLastGap() const;
  
  /// How many gaps provide hits attached to this road?
  int GetNGapsWithHits() const;

  /// How many hits in all does this road contain?
  int GetTotalHits() const;

  /// How many hits per gap does this road contain?
  int GetHitsPerGap(const int &gap) const;

  /// How many hits were attached in the gap with the most attached hits?
  int GetMaxHitsPerGap() const;

  /// Does this road contain any hits in the given segment?
  bool HasHitInGap(const int &gap) const;
  
  /// How many hits do two roads share?
  int GetNSharedHits(const MucRec3DRoad *road) const;

  // Are the two X and Y clusters in each gap in the same panel?
  // bool ClusterPanelsMatch() const;

  /// Difference between the last gap in the two 1-D roads.
  int GetLastGapDelta() const;

  /// Difference between the number of hits in the two 1-D roads.
  int GetTotalHitsDelta() const;
  
  /// How many degrees of freedom in the trajectory fit?
  int GetDegreesOfFreedom() const;

  /// Chi-square parameter (per degree of freedom) of the trajectory fit.
  float GetReducedChiSquare() const;

  /// Get Z-X dimension slope.
  float GetSlopeZX() const;

  /// Get Z-X dimension intercept.
  float GetInterceptZX() const;

  /// Get Z-Y dimension slope.
  float GetSlopeZY() const;

  /// Get Z-Y dimension intercept.
  float GetInterceptZY() const;

  /// Get a pointer to the first hit attached in a particular gap.
  MucRecHit *GetHit(const int &gap) const;
  
  /// Is the intersection of a pair of H and V clusters inside a panel?
  //bool IsIntersectionOK(const TMuiCluster *hcluster,
  //		const TMuiCluster *vcluster) const;

  /// get a pointer to the 2D road in the 3D road.
  MucRec2DRoad *Get2DRoad(const int &orient = 0) const;
  
  /// Get indices of all hits in the road.
  vector<Identifier> GetHitsID() const;

  /// Print Hits Infomation.
  void PrintHitsInfo();

private:

  Hep3Vector m_VertexPos;    // Event vertex position.
  Hep3Vector m_VertexSigma;  // Uncertainty of event vertex position.

  MucRec2DRoad *m_Road0;  // object describing the 2-D road orient 0
  MucRec2DRoad *m_Road1;  // object describing the 2-D road orient 1

  int   m_Index;            // uniquely identifies this road in the event
  int   m_Part;            // the part in which this road was found
  int   m_Seg;             // the segment in which this road was found
  int   m_LastGap;         // Last gap with attached hits.
  float m_Chi2;            // Chi-square parameter of trajectory fit.
  int   m_DOF;             // Number of degrees of freedom of fit.

  float m_SlopeZX;         // X = m_InterceptZX + m_SlopeZX * Z.
  float m_InterceptZX;     // X = m_InterceptZX + m_SlopeZX * Z.
  float m_SlopeZY;         // Y = m_InterceptZY + m_SlopeZY * Z.
  float m_InterceptZY;     // Y = m_InterceptZY + m_SlopeZY * Z.

  // == true if X and Y clusters are in the same panels in all gaps.
  //bool  m_ClusterPanelsMatch;
  int m_Group;            // Which group does this road belong to?
};

typedef ObjectVector<MucRec3DRoad> MucRec3DRoadContainer;

#endif  /* MUC_REC_3DROAD_H */

