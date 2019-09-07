//$id$
//
//$log$

/*
 *    2003/12/21   Zhengyun You      Peking University
 * 
 *    2005/02/24   Zhengyun You      Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_2DROAD_H
#define MUC_REC_2DROAD_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/ObjectVector.h"

#include "MucRecEvent/MucRecHit.h"

using CLHEP::Hep3Vector;       // forward declaration

/**
 * Describes a "two-dimensional" road found in the muon chamber.
 * part and orient decides dimension of the road.
 * part     orient         dimension
 *    1          0         Z-R
 *    1          1         Phi-R
 *  0,2          0         Y-Z
 *  0,2          1         X-Z
 * A full "three-dimensional" road is composed of 
 * two 2D roads with different orients on the same part.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucRec2DRoad : virtual public ContainedObject  
{
public:

  /// Constructor.
  MucRec2DRoad(const int &part = 0, 
	       const int &seg  = 0, 
	       const int &orient = 0,
	       const float &xVertex = 0.0,
	       const float &yVertex = 0.0,
	       const float &zVertex = 0.0,
	       const int &fittingMethod = 0);
  
  /// Assignment constructor.
  MucRec2DRoad &operator=(const MucRec2DRoad &orig);
  
  /// Copy constructor.
  MucRec2DRoad(const MucRec2DRoad &source);

  /// Destructor.
  ~MucRec2DRoad();
  
  /// Set the index for this road.
  void SetIndex(const int &index);

  /// Attach the given hit to this road.
  void AttachHit(MucRecHit *hit);

  /// Attach the given hit to this road, but not fit this road.
  void AttachHitNoFit(MucRecHit *hit);

  /// Max number of consecutive gaps allowed with no hits attached.
  /// This parameter affects the calculation of the last gap.
  void SetMaxNSkippedGaps(const int &nGaps);

  /// Calculate the best-fit straight line with "simple" weights.
  int SimpleFit(float &slope,      float &intercept,
		float &sigmaSlope, float &sigmaIntercept,
		float &chisq,      int &ndof);

  /// Calculate the best-fit straight line with "simple" weights. not use current gap!!!
  int SimpleFitNoCurrentGap(int currentgap,   float &slope,      float &intercept,
    float &sigmaSlope, float &sigmaIntercept,
    float &chisq,      int &ndof);

  /// Fit (with weights) the hit center to a straight line.
  //  This is a helper function for the Project methods.
  //  Give an estimated position (x,y,z) of the point to which we will
  //  project.
  int Fit(const float &x,
	  const float &y,
	  const float &z,
	  float &slope,      float &intercept,
	  float &sigmaSlope, float &sigmaIntercept,
	  float &chisq,      int &ndof);

  /// Where does the trajectory of this road intersect a specific gap?
  void Project(const int &gap, float &x, float &y, float &z, float &x2, float &y2, float &z2);

  //// Weight Function.
  float WeightFunc(const float &chisq, const float &distance) const; 

  //// Window Function. 
  float WindowFunc(const float &chisq, const float &distance) const; 

  /// A unique identifier for this road in the current event.
  int GetIndex() const;

  /// In which part was this road found?
  int GetPart() const;

  /// In which segment was this road found?
  int GetSeg() const;
  
  /// In which orientation was this road found?
  int GetOrient() const;

  /// Position of the vertex point.
  void GetVertexPos(float &x, float &y, float &z) const;
  
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

  /// Does this road contain any hits in the given gap?
  bool HasHitInGap(const int &gap) const;
  
  /// How many hits do two roads share?
  int GetNSharedHits(const MucRec2DRoad *road) const;

  /// Slope of trajectory.
  float GetSlope() const;

  /// Intercept of trajectory.
  float GetIntercept() const;

  /// How many degrees of freedom in the trajectory fit?
  int GetDegreesOfFreedom() const;

  /// Chi-square parameter (per degree of freedom) of the trajectory fit.
  float GetReducedChiSquare() const;

  /// Get the parameters from the simple fit.
  void GetSimpleFitParams(float &slope,      float &intercept,
			  float &sigmaSlope, float &sigmaIntercept,
			  float &chisq,      int &ndof) const;

  /// Get the parameters from the simple quad fit.
  void GetSimpleFitParams(float &a,      float &b,  float &c, int &whichhalf,
			  float &sigmaa, float &sigmab, float &sigmac,
			  float &chisq,      int &ndof) const;
  bool GetQuadFitOk(){return m_QuadFitOK;}

  void GetPosSigma(float &possigma)const;

  /// Get a pointer to the first found hit attached in a particular gap.
  MucRecHit *GetHit(const int &gap) const;
  
  /// Distance to a hit.
  float GetHitDistance(const MucRecHit *hit);
  
  /// Determine the size of the search window in the given gap.
  float GetSearchWindowSize(const int &gap) const;

  /// Does the hit exist in the road  .
  bool HasHit(MucRecHit *hit) const;

  /// Get indices of all hits in the road.
  vector<Identifier> GetHitsID() const;

  vector<MucRecHit*> GetHits() const;

  /// Print Hits Infomation.
  void PrintHitsInfo() const;

private:

  /// Calculate the hit counts and last gap quantities.
  void CountHits();

  Hep3Vector m_VertexPos;    // Event vertex position.
  Hep3Vector m_VertexSigma;  // Uncertainty of event vertex position.

  int   m_Index;                 // uniquely identifies this road in the event
  int   m_Part;                  // Part in which the road was found.
  int   m_Seg;                   // Segment in which the road was found.
  int   m_Orient;                // orientation of hits attached to this road.

  float m_SimpleSlope;           // slope of fit with simple weights
  float m_SimpleIntercept;       // intercept of fit with simple weights
  float m_SimpleSlopeSigma;      // uncert. in slope of fit with simple weights
  float m_SimpleInterceptSigma;  // uncert. in intercept of fit with simple weights
  float m_SimplePosSigma;        // uncert. in pos of this 2D line with first gap of MUC

  
  float m_SimpleChi2;            // Chi-square of fit with simple weights

  float m_SimpleQuad_a;          //y = a*x*x + b*x +c
  float m_SimpleQuad_b;     
  float m_SimpleQuad_c;
  int   m_SimpleQuad_whichhalf;
  float m_SimpleQuad_aSigma;          //y = a*x*x + b*x +c
  float m_SimpleQuad_bSigma;     
  float m_SimpleQuad_cSigma;     
  int   m_fittingMethod;


  float m_Chi2;                  // Chi-square parameter of trajectory fit.

  int   m_SimpleDOF;             // degrees of freedom of fit with simple weights
  int   m_DOF;                   // Number of degrees of freedom of fit.

  int   m_MaxHitsPerGap;         // Max. number of hits attached in a single gap.
  int   m_LastGap;               // Last gap with attached clusters.
  int   m_TotalHits;             // Total number of hits attached.
  int   m_MaxNSkippedGaps;       // Max consecutive gaps allowed with no hits.

  bool  m_SimpleFitOK;           // If == true, we have a good "simple" fit.
  bool  m_FitOK;                 // If == true, we have a good fit.
  bool  m_QuadFitOK;

  vector<float> m_HitDistance;   // distance from best hit to road
  vector<MucRecHit*> m_pHits;    // Pointers to attached hits.
};

typedef ObjectVector<MucRec2DRoad> MucRec2DRoadContainer;

#endif  /* MUC_REC_2DROAD_H */
