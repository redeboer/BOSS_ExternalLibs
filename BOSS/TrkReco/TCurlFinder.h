#ifndef TCURLFINDER_FLAG_
#define TCURLFINDER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES
#include "CLHEP/Matrix/DiagMatrix.h"

#include "TrkReco/TFinderBase.h"
#include "TrkReco/TBuilderCurl.h"
#include "TrkReco/TMSelector.h"
#include "TrkReco/TCurlFinderParameters.h"

using CLHEP::HepDiagMatrix;

class TMDCWire;
class TMLink;
class TTrack;
class TMDCWireHit;
class TCircle;
class TSegmentCurl;

class TCurlFinder : public TFinderBase {

public:
  TCurlFinder(void);
  ~TCurlFinder(void);
  std::string name(void) const;
  std::string version(void) const;

  TCurlFinder(const unsigned min_segment,
	      const unsigned min_salvage,
	      const double bad_distance_for_salvage,
	      const double good_distance_for_salvage,
	      const unsigned min_sequence,
	      const unsigned min_fullwire,
	      const double range_for_axial_search,
	      const double range_for_stereo_search,
	      const unsigned superlayer_for_stereo_search,
	      const double range_for_axial_last2d_search,
	      const double range_for_stereo_last2d_search,
	      const double trace2d_distance,
	      const double trace2d_first_distance,
	      const double trace3d_distance,
	      const unsigned determine_one_track,
	      const double selector_max_impact,
	      const double selector_max_sigma,
	      const double selector_strange_pz,
	      const double selector_replace_dz,
	      const unsigned stereo_2dfind,
	      const unsigned merge_exe,
	      const double merge_ratio,
	      const double merge_z_diff,
	      const double mask_distance,
	      const double ratio_used_wire,
	      const double range_for_stereo1,
	      const double range_for_stereo2,
	      const double range_for_stereo3,
	      const double range_for_stereo4,
	      const double range_for_stereo5,
              const double range_for_stereo6,
	      const double z_cut,
	      const double z_diff_for_last_attend,
	      const unsigned svd_reconstruction,
	      const double min_svd_electrons,
	      const unsigned on_correction,
	      const unsigned output_2dtracks,
	      const unsigned curl_version,
		  //jialk
	      const  double minimum_seedLength,
	      const  double minimum_2DTrackLength,
		  const  double minimum_3DTrackLength,
	      const  double minimum_closeHitsLength,
	      const  double MIN_RADIUS_OF_STRANGE_TRACK,
          const  double ULTIMATE_MIN_RADIUS_OF_STRANGE_TRACK);
							  

  /// main function
  int doit(const AList<TMDCWireHit> & axialHits,
	   const AList<TMDCWireHit> & stereoHits,
	   AList<TTrack> & tracks,
	   AList<TTrack> & tracks2D);

  /// cleans all members of this class
  void clear(void);

private:
  IMagneticFieldSvc* m_pmgnIMF;
  /// Utility Section
  double   distance(const double, const double) const;
  unsigned offset(const unsigned) const;
  unsigned layerId(const double &) const;
  unsigned maxLocalLayerId(const unsigned) const;
  int      nextSuperAxialLayerId(const unsigned, const int) const;
  int      nextSuperStereoLayerId(const unsigned, const int) const;
  void makeList(AList<TMLink>&, const AList<TSegmentCurl>&, const AList<TMLink>&);
  void makeList(AList<TMLink>&, const AList<TMLink>&, const AList<TMLink>&);
  unsigned nAxialHits(const double&) const;

  /// Sub Main Section #1
  void makeWireHitsListsSegments(const AList<TMDCWireHit>&, const AList<TMDCWireHit>&);
  
  /// Sub Main Section #2
  int checkSortSegments(void);
  
  /// Sub Main Section #3
  void makeCurlTracks(AList<TTrack> &tracks,
		      AList<TTrack> &tracks2D);

  /// Utility of #1
  void linkNeighboringWires(AList<TMLink>*, const unsigned);
  void setNeighboringWires(TMLink*, const TMLink*);
  void createSuperLayer(void);
  void createSegments(AList<TMLink>&);
  void searchSegment(TMLink*, AList<TMLink>&, AList<TMLink>&, TSegmentCurl*);
  TMLink * findLink(const TMLink*, const AList<TMLink>&);

  /// Utility of #2
  void checkExceptionalSegmentsType01(void);
  void checkExceptionalSegmentsType02(void);
  void checkExceptionalSegmentsType03(void);

  /// Utility of #3
  /// 3D Track
  TTrack * make3DTrack(const TCircle*);
  TTrack * make3DTrack(const TCircle*, AList<TSegmentCurl>&);
  void findCloseHits(AList<TMLink>&, TTrack&, AList<TMLink>&);
  void salvage3DTrack(TTrack*, bool = true);
  TTrack* merge3DTrack(TTrack*, AList<TTrack>&);
  bool check3DTrack(TTrack*);
  int  trace3DTrack(TTrack*);
  void mask3DTrack(TTrack*, AList<TMLink>&);
  void assignTracks(void);
  void checkRelation(AList<TTrack>&);
  void check2DTracks(void);

  /// 2D Track
  TCircle * make2DTrack(const AList<TMLink>&, const AList<TSegmentCurl>&, const unsigned);
  void searchAxialCand(AList<TMLink>&, const AList<TMLink>&, const TCircle*, 
		       const int, const unsigned, const double);
  void searchStereoCand(AList<TMLink>&, const AList<TMLink>&, const TCircle*, 
			const int, const unsigned, const double);
  unsigned searchHits(const TMLink*, const TCircle*, const double) const;
  unsigned searchHits(AList<TMLink>&, const AList<TMLink>&, const TCircle*, const double) const;
  unsigned checkAppendHits(const AList<TMLink>&, AList<TMLink>&) const;
  double distance(const TTrack&, const TMLink&) const;
  int  trace2DTrack(TCircle*);
  bool check2DCircle(TCircle*);
  TCircle * dividing2DTrack(TCircle*);
  bool fitWDD(TCircle&,double&,int&) const;
  void removeStereo(TCircle&) const;

  /// MC
  int makeWithMC(const AList<TMDCWireHit>&, const AList<TMDCWireHit>&,AList<TTrack>&);

  /// Plot
  void makeCdcFrame(void);
  void plotSegment(const AList<TMLink>&, const int flag = 1);
  void plotCircle(const TCircle&, const int flag = 1);
  void plotTrack(const TTrack&, const int flag = 1);
  void writeSegment(const AList<TMLink>&, const int type = 0);

  /// Dump
  void dumpType1(TTrack*);
  void dumpType2(TTrack*);

  AList<TMLink>   m_unusedAxialHitsOriginal;
  AList<TMLink>   m_unusedAxialHits;
  AList<TMLink>   m_allAxialHitsOriginal;

  AList<TMLink>   m_unusedStereoHitsOriginal;
  AList<TMLink>   m_unusedStereoHits;
  AList<TMLink>   m_allStereoHitsOriginal;

  AList<TMLink>   m_removedHits;

  AList<TCircle> m_circles;
  AList<TCircle> m_allCircles;
  AList<TTrack>  m_tracks;
  AList<TTrack>  m_allTracks;
  AList<TTrack>  m_svdTracks;
  AList<TTrack>  m_2dTracks;

  AList<TMDCWireHit>   m_hitsOnInnerSuperLayer;

//Liuqg
  AList<TMLink> m_unusedAxialHitsOnEachLayer[19];  //origin is 32
  AList<TMLink> m_unusedStereoHitsOnEachLayer[24];  //origin is 18
  AList<TMLink> m_unusedAxialHitsOnEachSuperLayer[5];  //origin is 6
  AList<TMLink> m_unusedStereoHitsOnEachSuperLayer[6];  //origin is 5
  AList<TSegmentCurl> m_segmentList;

  TCurlFinderParameter m_param;

  TBuilderCurl m_builder;
  TMSelector m_trackSelector;

  THelixFitter m_fitter;

  //for debug
  void debugCheckSegments0(void);
  void debugCheckSegments1(void);
  void debugCheckSegments2(void);
  void debugCheckSegments(const double localId, const double layerId,
			  const double localId2,const double layerId2);

  bool m_debugCdcFrame;
  int  m_debugPlotFlag;
  int  m_debugFileNumber;
};

#endif /* TCURLFINDER_FLAG_ */
