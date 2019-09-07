//$id$
//
//$log$

/*
 *    2004/03/08   Zhengyun You      Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef REC_MUC_TRACK_H
#define REC_MUC_TRACK_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"
#include "ExtEvent/RecExtTrack.h"
#include "DstEvent/DstMucTrack.h"

#include "MucRecEvent/MucRecHit.h"

/**
 * Describes a track found in the muon chamber.
 *
 *  A track is originated from a track reconstructed in MDC, and has its 
 *  intersection and direction on MUC start surface, which is extrapolated
 *  from MDC track. Methods include projecting along the trajectory to each
 *  gap, including hits in search window, modifing intersectins, and then 
 *  projecting the trajectory to the next gap.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

extern const  CLID &CLID_RecMucTrack;

class RecMucTrack : public DstMucTrack
{
 public:

  virtual const CLID& clID() const
    {
      return RecMucTrack::classID();
    }

  static const CLID& classID()
    {
      return CLID_RecMucTrack;
    }


  /// Constructor.
  RecMucTrack();

  /// Assignment constructor.
  RecMucTrack& operator=(const RecMucTrack& orig);
  
  /// Copy constructor.
  RecMucTrack(const RecMucTrack& source);

  /// Assignment constructor from DstMucTrack.
  RecMucTrack& operator=(const DstMucTrack& dstTrack);

  /// Copy constructor from DstMucTrack.
  RecMucTrack(const DstMucTrack& dstTrack);
  
  /// Destructor.
  ~RecMucTrack();

  void SetDefault();
  
  /// set the index for this track.
  void setTrackId(const int trackId);

  /// set start position of the track in Mdc.
  void SetMdcPos(const float x,
		 const float y,
		 const float z);

  /// set start moment of the track in Mdc.
  void SetMdcMomentum(const float px,
		      const float py,
		      const float pz);

  /// set start position of ext track in Muc. (compute from MdcPos MdcMomentum or get from ExtTrack)
  void SetExtMucPos(const float x,
		    const float y,
		    const float z);

  /// set start moment of ext track in Muc.
  void SetExtMucMomentum(const float px,
			 const float py,
			 const float pz);

  /// set start position of the track in Muc. (after line fit and correction)
  void SetMucPos(const float x,
		 const float y,
		 const float z);

  void SetMucPosSigma(const float sigmax,
      const float sigmay,
      const float sigmaz);

  
  /// set start moment of the track in Muc.
  void SetMucMomentum(const float px,
		      const float py,
		      const float pz);

  /// set current position of the trajectory.
  void SetCurrentPos(const float x,
		     const float y,
		     const float z);

  /// set current direction of the trajectory.
  void SetCurrentDir(const float x,
		     const float y,
		     const float z);

  /// set current intersection of the trajectory with strip plane.
  void SetCurrentInsct(const float x,
		       const float y,
		       const float z);

  /// set Ext track point.
  void SetExtTrack(RecExtTrack* extTrack);
  
  /// set Ext track id. for compute from mdc myself
  void SetExtTrackID(int id) { m_ExtTrackID = id; }

  /// compute ext track myself from mdc.
  void GetMdcExtTrack(Hep3Vector  mdcStartPos, Hep3Vector  mdcStartMomentum, int charge,
		      Hep3Vector& mucStartPos, Hep3Vector& mucStartMomentum);

 
  bool IsInsideSuperConductor(Hep3Vector pos);

  /// set corresponding monte carlo track pointer.
  //void SetMCTrack(const BesPersTrack* mcTrack);
  
  /// Attach the given hit to this track.
  void AttachHit(MucRecHit* hit);

  /// Where does the trajectory of this track intersect a specific gap?
  void Project(const int& part, const int& gap,
	       float& x, float& y, float& z,
	       int& seg);
  
  /// Calculate the distance of the hit to the intersection in read direction.
  float GetHitDistance(const MucRecHit* hit);

  /// no abs value
  float GetHitDistance2(const MucRecHit* hit);

  /// Calculate intersection from all hits attached on this gap.
  Hep3Vector CalculateInsct(const int part,
			    const int seg,
			    const int gap);
  
  /// Attach the intersection to this trajectory.
  void AttachInsct(Hep3Vector insct);
  
  /// Attach the direction to this trajectory.
  void AttachDirection(Hep3Vector dir);

  /// Correct direction of this trajectory.
  void CorrectDir();
  
  /// Correct current position of this trajectory.
  void CorrectPos();

  /// Where does the trajectory of this track intersect the reference plane?
  //void ProjectToReference(float& x, float& y, float& z);

  /// Where does the trajectory of this track intersect a z=const plane?
  //void ProjectToZ(float& x, float& y, float& zplane);

  /// Transform the Phi, ZR cord. to ZX, ZY cord.
  //void TransformPhiRToXY(const float& vk, const float& vr,
  //		 const float& k0, const float& r0,
  //		 float& vx, float& vy, 
  //		 float& x0, float& y0) const;


 /// The identifier of the Ext track as seed.
  int getExtTrackID() const { return m_ExtTrackID; }
  
  /// start position of the Ext track in Muc.
  Hep3Vector getExtMucPos() const { return m_ExtMucPos/10; }
  
  /// Start momentum of the Ext track in Muc.
  Hep3Vector getExtMucMomentum() const { return m_ExtMucMomentum/1000; }
  
  /// start position of this track in Muc.
  Hep3Vector getMucPos() const { return m_MucPos/10; }
  
  Hep3Vector getMucPosSigma() const { return m_MucPosSigma/10; }

    /// Start momentum of this track in Muc.
  Hep3Vector getMucMomentum() const { return m_MucMomentum/1000; }
  
  /// momentum of this track in Mdc
  Hep3Vector getMdcMomentum() const { return m_MdcMomentum/1000; }
  
  /// Current position.
  Hep3Vector GetCurrentPos() const { return m_CurrentPos/10; }  

  /// Current direction.
  Hep3Vector GetCurrentDir() const { return m_CurrentDir/1000; }  
  
  /// Current intersection.
  Hep3Vector GetCurrentInsct() const { return m_CurrentInsct; } //internal  

  /// Comute last gap in barrel and endcap.
  void ComputeLastGap();

  /// Which gap on Barrel is the first one with hits attached to this track?
  int brFirstLayer() const { return m_brFirstLayer; }
  
  /// Which gap on EndCap is the first one with hits attached to this track?
  int ecFirstLayer() const { return m_ecFirstLayer; }


  /// Line fit with hits on a seg with max hits.
  void LineFit(int fittingMethod);

  /// chi2 of line fit
  //float GetChi2() const { return m_Chi2; }

  /// degree of freedom in line fit
  //float GetDof() const { return m_Dof; }

  /// rms of distance from hit to track
  //float GetRms() const { return m_Rms; }

  /// Compute depth.
  void ComputeDepth();

  /// Compute depth.
  int ComputeDepth(int method);

  /// Compute distance match //2006.11.08
  void ComputeDistanceMatch();
  
  /// Extend mucpos and extmucpos to first layer of muc
  void Extend();

  /// Length of the track penetrating in iron absorber.
  float GetDepth3() const { return m_depth_3/10; }

  void  SetRecMode(int recmode) {m_recmode = recmode;}
  
  int   GetRecMode() const {return m_recmode;}
  
  /// Distance match of the ext track with muc track in first layer
  Hep3Vector  GetExtMucDistance() const { return m_Dist_muc_ext;}

  Hep3Vector  GetMucStripPos() const { return m_StripPhi;}
  
  /// ComputeNGapsWithHits;
  void ComputeNGapsWithHits(); 

  /// How many gaps provide hits attached to this track?
  int GetNGapsWithHits() const { return m_numLayers; }

  /// How many hits in all does this track contain?
  int GetTotalHits() const;

  /// How many hits per gap does this track contain?
  int GetHitInGap(const int part, const int gap) const;

  /// How many hits does a segment contains.
  int GetHitInSeg(const int part, const int seg) const;

  /// How many hits does a segment contains in one orient.
  int GetHitInSegOrient(const int part, const int seg, const int orient) const;

  /// Find the segment which contains most hits, return max hits number.
  int FindSegWithMaxHits(int &part, int &seg);

  /// ComputeMaxHitsInGap;
  void ComputeMaxHitsInGap();

  /// How many hits were attached in the gap with the most attached hits?
  //int GetMaxHitsInGap() const { return m_MaxHitsInLayer; }

  /// Does this track contains an assigned hit?
  bool HasHit(const int part, const int seg, const int gap, const int strip) const;

  /// Does this track contain any hits in the given gap?
  bool HasHitInGap(const int part, const int gap) const;
  
  /// How many hits do two tracks share?
  int GetNSharedHits(const RecMucTrack* track) const;

  /// Get a pointer to the first hit attached in a particular gap.
  MucRecHit* GetHit(const int part, const int gap) const;

  /// Get all hits on this track
  vector<MucRecHit*> GetHits() const;

  vector<int> getVecHits() const { return  m_vecHits ;}
  
  vector<MucRecHit*> GetExpectedHits() const;
 
  vector<int> getExpHits() const { return  m_expHits ;}

  vector<float> getDistHits() const { return  m_distHits ;}
 
  vector<float> getQuadDistHits() const { return  m_distHits_quad ;}

  vector<float> getExtDistHits() const { return  m_distHits_ext ;} 
 
  /// Get indices of all hits in the track.
  vector<long> GetHitIndices() const;

  /// Get corresponding monte carlo track pointer.
  //BesPersTrack* GetMCTrack() const { return m_MCTrack; }

  /// Compute all infomation of this track;
  void ComputeTrackInfo(int fittingmethod);
  
  /// Print Hits Infomation.
  void PrintHitsInfo() const;

  /// change unit
  void OutputUnitChange();

  /// reload setVecHits
  void setVecHits(vector<MucRecHit*>& pHits);

  void setExpHits(vector<MucRecHit*>& pHits);

  void setVecHits(vector<int>& vechits){ m_vecHits = vechits;}

  void setExpHits(vector<int>& exphits){ m_expHits = exphits;}  

  void setDistHits(vector<float>& disthits){ m_distHits = disthits;}

  void setQuadDistHits(vector<float>& disthits){ m_distHits_quad = disthits;}

  void setExtDistHits(vector<float>& disthits){ m_distHits_ext = disthits;}

  void pushExtDistHits(float dist){m_distHits_ext.push_back(dist);}
private:

  int       m_ExtTrackID;      // identifier of the Ext track used as seed of this track
  RecExtTrack *m_ExtTrack;        // pointer to the Ext track as seed of this track

  Hep3Vector m_MdcPos;         // Start position of the ext track in Mdc
  Hep3Vector m_MdcMomentum;    // Start Momentum of the ext track in Mdc

  Hep3Vector m_ExtMucPos;      // position of the ext track in Muc
  Hep3Vector m_ExtMucMomentum; // Momentum of the ext track in Muc

  Hep3Vector m_MucPos;         // Start position of the track in Muc
  Hep3Vector m_MucMomentum;    // Start Momentum of the track in Muc
  Hep3Vector m_MucPosSigma;
  
  Hep3Vector m_CurrentPos;     // track current position
  Hep3Vector m_CurrentDir;     // track current direction
  Hep3Vector m_CurrentInsct;   // track current intersection
  Hep3Vector m_Dist_muc_ext;
  Hep3Vector m_StripPhi;       // use to calc phi of strip in first layer

  int   m_brFirstLayer;      // first gap of the track in barrel. 20070604
  int   m_ecFirstLayer;      // first gap of the track in endcap. 20070604
  int   m_ecPart;          // whic part of endcap

  float m_depth_3;             // compare with m_depth
  int   m_recmode;             // 0: mdc ext; 1: emc ext
  int   m_Good3DLine;
  int   m_Good3DPart;
  bool  m_changeUnit;
  Hep3Vector m_IntersectionInner[9]; //intersection of this track with inner surface this gap
  Hep3Vector m_IntersectionOuter[9]; //intersection of this track with outer surface this gap

  //BesPersTrack *m_MCTrack;   // Pointer to monte carlo track
  vector<int> m_vecHits;
  vector<int> m_expHits;
  vector<float> m_distHits;
  vector<float> m_distHits_quad;
  vector<float> m_distHits_ext;
  vector<MucRecHit*> m_pHits;           // Pointers to attached hits
  vector<MucRecHit*> m_pExpectedHits;   // Pointers to expected hits
  vector<Hep3Vector> m_Intersections;   // Container of intersection points
  vector<Hep3Vector> m_Directions;      // Container of track direction on intersection points
  
};

typedef ObjectVector<RecMucTrack> RecMucTrackCol;

#endif  /* MUC_TRACK_H */

