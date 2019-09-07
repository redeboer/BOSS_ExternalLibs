//-----------------------------------------------------------------------------
// $Id: TrkReco.h,v 1.30 2011/10/08 01:56:15 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TrkReco.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A tracking module.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TrkReco_FLAG_
#define TrkReco_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/TTrackManager.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "TrkReco/TRungeFitter.h"
class TMDC;
class TMDCWireHit;
class TFinderBase;
class TConformalFinder0;
class TCurlFinder;
class TTrack;
class TTrackMC;

class MdcCalibFunSvc;
class IBesTimerSvc;
class BesTimer;

extern float TrkRecoHelixFitterChisqMax;

/// A tracking module.
class TrkReco : public  Algorithm {    

  public:
    /// returns TrkReco.
    //static TrkReco * getTrkReco(void);
    TrkReco(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    StatusCode beginRun();
    StatusCode endRun(){}; 
    
  public:// BASF interfaces
    /// initializes TrkReco.
//    void init(int * status);

    /// terminates TrkReco.
//    void term(void);

    /// processes an event.
//    void event(BesEvent *, int *);
//    void event(void);
    
    /// temporarily required 
//    void begin_run(BesEvent *, int *) {};
//    void end_run(BesEvent *, int *) {};
    void disp_stat(const char *);

  public:// TrkReco interfaces
    /// returns name.
//zsl    std::string name(void) const;

    /// returns version.
//liuqg    std::string version(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a pointer to the rphi finder.
    const TFinderBase * confFinder(void) const;

    /// returns a pointer to the curl finder.
    const TFinderBase * curlFinder(void) const;

    /// returns a pointer to TTrackManager.
    const TTrackManager & trackManager(void) const;

  public: // to access information
    /// clears all TMDC information.
    void clear(void);

    /// clears TMDC information.
    void fastClear(void);

    /// returns a list of reconstructed tracks.
    const AList<TTrack> & tracks(void) const;

  private:
    /// initiate Parameters
    void initPara(void);
    
    /// returns true if an event is MC.
    bool mcEvent(void) const;

    /// creates MC info. of reconstructed tracks.
    void mcInformation(void);

    /// cal maxdistance temporary in conformal plane
    double maxdDistance(TMLink *) const;

    TMDC * cdcInit(void);
    
    /// initiate and fill Tuple
    void InitTuple(void);
    void FillTuple(void);

  public:// public members for basf interface

    //...RawData/TUPLE...
    int b_mcPar;
    int b_mcHit;
    int b_tuple;
    int b_goodTrk;
    int b_timeTest;
    bool m_dropHot;
    bool m_combineTracking;
    bool m_keepBadTdc;
    bool m_keepUnmatch;
    int b_RungeKuttaCorrection;
    //...MDC...
    float b_cdcVersion;
    std::string _cdcVersion;
    float b_fudgeFactor;

    //...Bunch Time...
    int useESTime;

    //...TrkReco...
    int b_debugLevel;
    int b_useAllHits;
    int b_doT0Reset;
    int b_nT0ResetMax;
    int b_doMCAnalysis;
    float b_helixFitterChisqMax;

    //...Perfect Finder...
    int b_doPerfectFinder;
    int b_perfectFitting;

    //...Conformal Finder...
    int b_conformalFinder;
    int b_doConformalFinder;
    int b_doConformalFastFinder;
    int b_doConformalSlowFinder;
    int b_conformalPerfectSegmentFinding;
    int b_conformalFittingFlag;
    float b_conformalMaxSigma;
    int b_conformalMinNLinksForSegment;
    int b_conformalMinNCores;
    int b_conformalMinNSegments;
    float b_salvageLevel;
    int b_conformalSalvageLoadWidth;
    int b_conformalStereoMode;
    int b_conformalStereoLoadWidth;
    float b_conformalStereoMaxSigma;
    float b_conformalStereoSzSegmentDistance;
    float b_conformalStereoSzLinkDistance;

    //...Old Conformal Finder...
    int b_doConformalFinderStereo;
    int b_doConformalFinderCosmic;
    float b_conformalFraction;
    float b_conformalStereoZ3;
    float b_conformalStereoZ4;
    float b_conformalStereoChisq3;
    float b_conformalStereoChisq4;
    int b_conformalFittingCorrections;
    int m_CalibFlag;

    //...Track Manager...
    double b_momentumCut;
    int b_fittingFlag;
    int b_doSalvage;
    int b_doMerge;
    int b_doT0Determination;
    int b_nTracksForT0;
    int b_sortMode;
    int b_doAssociation;
    float b_associateSigma;
    int b_test;

    //...Curl Finder...
    int b_doCurlFinder;
    int    min_segment;
    int    min_salvage;
    double bad_distance_for_salvage;
    double good_distance_for_salvage;
    int    min_sequence;
    int    min_fullwire;
    double range_for_axial_search;
    double range_for_stereo_search;
    int    superlayer_for_stereo_search;
    double range_for_axial_last2d_search;
    double range_for_stereo_last2d_search;
    double trace2d_distance;
    double trace2d_first_distance;
    double trace3d_distance;
    int    determine_one_track;
    double selector_max_impact;
    double selector_max_sigma;
    double selector_strange_pz;
    double selector_replace_dz;
    int    stereo_2dfind;
    int    merge_exe;
    double merge_ratio;
    double merge_z_diff;
    double mask_distance;
    double ratio_used_wire;
    double range_for_stereo1;
    double range_for_stereo2;
    double range_for_stereo3;
    double range_for_stereo4;
    double range_for_stereo5;
    double range_for_stereo6;
    double z_cut;
    double z_diff_for_last_attend;
    int    svd_reconstruction;
    double min_svd_electrons;
    int    on_correction;
    int    output_2dtracks;
    int    curl_version;
   
	//jialk
    double minimum_seedLength;
	double minimum_2DTrackLength;
	double minimum_3DTrackLength;
	double minimum_closeHitsLength;
	double MIN_RADIUS_OF_STRANGE_TRACK;
	double ULTIMATE_MIN_RADIUS_OF_STRANGE_TRACK;

  private:
//    static TrkReco * _TrkReco;
    TMDC * _cdc;
    TFinderBase * _perfectFinder;
    TFinderBase * _confFinder;
    TCurlFinder * _curlFinder;
    TTrackManager _trackManager;
    TRungeFitter _rkfitter;
    RawDataProviderSvc * _rawDataProviderSvc;
    MdcCalibFunSvc * _mdcCalibFunSvc;
    MdcGeomSvc * _mdcGeomSvc;

    unsigned _nEvents;
    AList<TTrackMC> _mcTracks;

    //Tuple related
    IBesTimerSvc* m_timersvc;
    BesTimer* m_timer[2];
    double t0_bes;
    double t0Sta;    
    int havedigi[43][288];
    int MC_DIGI_SIZE;
    
    NTuple::Tuple*  m_tuple;  //rec3D 
    NTuple::Item<float>  t_mcphi,t_mctheta,t_mcptot,t_mcpt,t_mcpz,t_mct0,t_nDigi,
      t_pt,t_ptot,t_dr,t_dz,t_tanlmd,t_phi,t_radius,t_chi2,t_ndf,t_nHits,t_nCores,t_nSegs,
      t_length,t_length2,t_evtNo,
      t_dpt,t_dptot,t_dlmd,t_dphi,
      t_t0,t0_sta,
      t_good_theta,t_gdNLayers,t_mcNLayers,t_bestNLayers,t_bestMcNLayers;

    NTuple::Tuple*  m_tuple2;  //rec2D
    NTuple::Item<float>  t2_mctheta,t2_mcpt,t2_nDigi,t2_length,t2_ndf,t2_evtNo,t2_chi2,t2_nHits,t2_nSegs,t2_length2,t2_radius;

    NTuple::Tuple*  m_tuple3; //raw
    NTuple::Item<float>  t3_mct0,t3_mcptot,t3_mcpt,t3_mctheta,t3_mcphi, t3_mcpid, t3_evtNo;

    NTuple::Tuple*  m_tuple31; //raw/event
    NTuple::Item<float>  t3_nDigi,t3_goodLength, t3_t0Rec, t3_t0, t3_t0Sta, t3_finalLength, t3_length;

    NTuple::Tuple*  m_tuple4; //hits
    NTuple::Item<float>  t4_Dist,t4_drift,t4_dDrift,t4_mcDrift,t4_mcLR,t4_pull,t4_lyrId,t4_localId;
    NTuple::Item<float>  t4_LR,t4_tdc,t4_z,t4_bz,t4_fz,t4_fy,t4_phi, t4_nHits;

    NTuple::Tuple*  m_tuple5; //charge
    NTuple::Item<float>  t5_ptotPos, t5_ptotNeg, t5_drPos, t5_drNeg, t5_dzPos, t5_dzNeg;

    NTuple::Tuple*  m_tuple6; //unreconstructed.
    NTuple::Item<float>  u_length2,u_mcptot,u_mcpt,u_mctheta,u_nDigi,u_evtNo,u_mct0,ut_t0,ut0_sta;

    NTuple::Tuple*  m_tuple7; //time test.
    NTuple::Item<float>  ti_eventTime, ti_recTrkNum, ti_evtNo, ti_nHits, ti_nDigi;

    NTuple::Tuple* m_tuple9; //segments
    NTuple::Item<float>  t9_times, t9_nLinks, t9_nUsed, t9_nSL, t9_mctheta;

    NTuple::Tuple* m_tuple10; //raw hit
    NTuple::Item<float>  t10_tdc, t10_adc, t10_drift, t10_dDrift,t10_lyrId,t10_localId;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TrkReco_INLINE_DEFINE_HERE
#endif

#ifdef TrkReco_INLINE_DEFINE_HERE

inline
const TFinderBase *
TrkReco::confFinder(void) const {
    return (TFinderBase *) _confFinder;
}

inline
const TFinderBase *
TrkReco::curlFinder(void) const {
    return (TFinderBase *) _curlFinder;
}

inline
const AList<TTrack> &
TrkReco::tracks(void) const {
    return _trackManager.tracksFinal();
}

/*
inline
std::string
TrkReco::name(void) const {
    return std::string("TrkReco");
}
*/

inline
const TTrackManager &
TrkReco::trackManager(void) const {
    return _trackManager;
}

#endif

#undef inline

#endif /* TrkReco_FLAG_ */
