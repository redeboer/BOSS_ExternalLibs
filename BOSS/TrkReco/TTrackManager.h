//-----------------------------------------------------------------------------
// $Id: TTrackManager.h,v 1.17 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TTrackManager.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A manager of TTrack information to make outputs as Reccdc_trk.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TTRACKMANAGER_FLAG_
#define TTRACKMANAGER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/TUpdater.h"
#include "TrkReco/THelixFitter.h"
#include "TrkReco/TCosmicFitter.h"
#include "MdcRecEvent/RecMdcTrack.h" 
#include "MdcRecEvent/RecMdcHit.h" 
class TMDCWireHit;
class TTrack;
//struct reccdc_trk;
//struct reccdc_trk_add;
//struct reccdc_mctrk;
//struct rectrk;
#include "MdcTables/MdcTables.h"
#include "MdcTables/TrkTables.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"

class StatusCode;

/// A manager of TTrack information to make outputs as MdcRec_trk.
class TTrackManager : public TUpdater {

  public:
    /// Constructor.
    TTrackManager();

    /// Destructor.
    virtual ~TTrackManager();

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// to access information
    /// returns name.
    std::string name(void) const;

    /// returns version.
    std::string version(void) const;

    /// returns a list of all reconstructed tracks including junks.
    const AList<TTrack> & allTracks(void) const;

    /// returns a list of reconstructed tracks.
    const AList<TTrack> & tracks(void) const;

    /// returns a list of 2D tracks.
    const AList<TTrack> & tracks2D(void) const;

    /// returns a list of tracks writen to MdcRec_trk.
    const AList<TTrack> & tracksFinal(void) const;

  public:// parameters
    /// sets fitting flag.
    void fittingFlag(unsigned);

    /// sets the max. momentum.
    double maxMomentum(double);

    /// returns/sets debug level.
    int debugLevel(void) const;
    int debugLevel(int);

    //6para T0... Liuqg
    float paraT0(void) const;

  public:// track manipulations
    /// clears all internal information.
    void clear(void);

    /// checks goodness of a track.
    static bool goodTrack(const TTrack &, bool track2D = false);

    /// appends (2D) tracks. 'list' will be cleaned up.
    void append(AList<TTrack> & list);
    void append2D(AList<TTrack> & list);

    /// refits tracks.
    void refit(void);

    /// finishes tracks.
    void finish(void);

    /// moves pivot of tracks.
    void movePivot(void);

    /// determines T0 and refit all tracks.
    StatusCode  determineT0(unsigned level, unsigned nMaxTracks);

    /// tests for curlers.
    void setCurlerFlags(void);

    /// sorts tracks.
    void sortTracksByQuality(void);
    void sortTracksByPt(void);

  public:// table manipulations
    /// clears tables.
    void clearTables(void) const;

    /// stores track info. into TDS. by Zang Shilei
    //yzhang changed 2010-01-12 
    //StatusCode makeTds(bool doClear = true, int tkStat = 0); 
     StatusCode makeTds(RecMdcTrackCol* trackList, RecMdcHitCol* hitList, int tkStat = 1, int runge=0, int cal=0);
    
    /// stores track info. into Panther table.
    void saveTables(void);

    /// stores MC track info. into Panther table.
    void saveMCTables(void) const;

    /// sorts RECMDC_TRK tables.
    void sortBanksByPt(void) const;

  private:// table manipulations in private
    /// copies a track. Non-zero will be returned if error happens.
    int copyTrack(TTrack & t,
		  MdcRec_trk ** r,
		  MdcRec_trk_add ** a) const;
    /// sorts banks.
    void swapReccdc(MdcRec_trk & cdc0,
		    MdcRec_trk_add & add0,
		    MdcRec_mctrk & mc0,
		    MdcRec_trk & cdc1,
		    MdcRec_trk_add & add1,
		    MdcRec_mctrk & mc1) const;
    void swapRectrk(MdcTrk & trk0, MdcTrk & tkr1) const;
    void tagReccdc(unsigned * id, unsigned n) const;
    void tagRectrk(unsigned * id, unsigned n) const;

  public:// functions for after trak
    /// final decision for a curler.
    void treatCurler(MdcTrk & curl, MdcRec_trk_add & cdc, unsigned flag) const;

    /// determines IP.
    void determineIP(void);

  public:// hit manipulations
    /// masks hits on found curl tracks.
    void maskCurlHits(const AList<TMDCWireHit> & axial,
		      const AList<TMDCWireHit> & stereo,
		      const AList<TTrack> & tracks) const;

    /// masks hits with large chisq as associated hits. Pull in TMLink is used.
    static void maskBadHits(const AList<TTrack> &, float maxSigma2);

    /// salvages hits for dE/dx(not for track fitting).
    void salvageAssociateHits(const AList<TMDCWireHit> &,
			      float maxSigma2);

    /// associates SVD and then adds track information.
    //    void addSvd(const int) const;

  private:// internal functions
    /// checks track quality.
    AList<TTrack> selectGoodTracks(const AList<TTrack> &,
				   bool track2D = false) const;
    static bool checkNumberOfHits(const TTrack &, bool track2D = false);

    /// names tracks.
    void nameTracks(void);

    /// returns T0 by old methode(expensive).
    float T0(unsigned n);

    /// returns T0 by T0 fitting.
    float T0Fit(unsigned n);
    float minimum(float y0, float y1, float y2) const;

  public:// obsolete
    TMLink & divide(const TTrack & t, AList<TMLink> * l) const;
    TMLink & divideByIp(const TTrack & t, AList<TMLink> * l) const;
    void removeHitsAcrossOverIp(AList<TMLink> &) const;
    /// returns a track which is the closest to a hit.
    TTrack * closest(const AList<TTrack> &, const TMDCWireHit &) const;
    /// salvages remaining hits.
    void salvage(const AList<TMDCWireHit> &) const;
    /// masks hits out which are in tail of curly tracks.
    void mask(void) const;
    void maskNormal(TTrack &) const;
    void maskCurl(TTrack &) const;
    void maskOut(TTrack &, const AList<TMLink> &) const;
    void maskMultiHits(TTrack &) const;
    void merge(void);


    void
    sett0bes(double t0)  {
	  t0bes = t0;
    }   
    
  private:
    IMagneticFieldSvc* m_pmgnIMF;
    RawDataProviderSvc * m_rawDataProviderSvc;
    double t0bes;
    int _debugLevel;
    double _maxMomentum;
    double _sigmaCurlerMergeTest;
    unsigned _nCurlerMergeTest;

    THelixFitter _fitter;
    THelixFitter _cFitter;

    AList<TTrack> _tracksAll;   // all tracks found by the finders
    AList<TTrack> _tracks;      // good 3D tracks determined by the mgr
    AList<TTrack> _tracks2D;    // good 2D tracks determined by the mgr
    AList<TTrack> _tracksFinal; // final tracks output to tables

    AList<TMLink> _associateHits;

    struct summary {
	unsigned _nEvents;
	unsigned _nTracks;
	unsigned _nTracksAll;
	unsigned _nTracks2D;
	unsigned _nTracksFinal;
	unsigned _nSuperMoms;
	unsigned _nToBeMerged;
	unsigned _nToBeMergedMoreThanTwo;
    };
    struct summary * _s;

    //t0 in 6 para helix fitting... Liuqg
    float _t0;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TTRACKMANAGER_INLINE_DEFINE_HERE
#endif

#ifdef TTRACKMANAGER_INLINE_DEFINE_HERE

inline
std::string
TTrackManager::name(void) const {
    return std::string("Track Manager");
}

inline
const AList<TTrack> &
TTrackManager::tracks(void) const {
    return _tracks;
}

inline
const AList<TTrack> &
TTrackManager::tracks2D(void) const {
    return _tracks2D;
}

inline
const AList<TTrack> &
TTrackManager::allTracks(void) const {
    return _tracksAll;
}

inline
double
TTrackManager::maxMomentum(double a) {
    return _maxMomentum = a;
}

inline
int
TTrackManager::debugLevel(void) const {
    return _debugLevel;
}

inline
int
TTrackManager::debugLevel(int a) {
    return _debugLevel = a;
}

inline
void
TTrackManager::fittingFlag(unsigned a) {
    if (a & 1) _fitter.sag(true);
    if (a & 2) _fitter.propagation(true);
    if (a & 4) _fitter.tof(true);
    if (a & 8) _fitter.freeT0(true);
}

inline
const AList<TTrack> &
TTrackManager::tracksFinal(void) const {
    return _tracksFinal;
}

inline
float
TTrackManager::paraT0(void) const {
  return _t0;
}


#endif

#undef inline

#endif /* TTRACKMANAGER_FLAG_ */
