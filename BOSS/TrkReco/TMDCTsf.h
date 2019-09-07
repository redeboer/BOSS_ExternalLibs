//-----------------------------------------------------------------------------
// $Id: TMDCTsf.h,v 1.7 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDCTsf.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a Track Finder Segment(TSF).
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDCTsf_FLAG_
#define TMDCTsf_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include "TrkReco/TMLink.h"
#include "TrkReco/TSegment.h"

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"

//class TMDCWire;

#define TMDCTSF_MIN_LAYERS 3

/// A class to represent a Track Finder Segment(TSF).
class TMDCTsf {

  public:
    /// Constructor of fixed shape.
//    TMDCTsf(const TMDCWire * const);

    /// Constructor for user defined shape.
/*    TMDCTsf(const AList<int> & offset,
	    const AList<int> & width,
	    const TMDCWire * const);
*/
    TMDCTsf(unsigned sl);

    /// Destructor
    virtual ~TMDCTsf();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Modifiers
    /// finds segments.
    AList<TSegment> segments(const AList<TMLink> & links);
    AList<TSegment> createTsf(unsigned) const;

    /// solves left-right ambiguityies.
    void solveLeftRight(void);

  private:// Modifiers
    /// creates a wire list, returns false if failed.
    bool create(const AList<int> & offset,
		const AList<int> & width,
		const TMDCWire * const);

    /// find Tsf.
//    int  createTsf(TMLink *, AList<TMLink> &, AList<TMLink> &);
//    AList<TSegment> createTsf(void) const;
    double alpha(unsigned, TMLink *) const;

  private:
    unsigned _sl;
    double _angle;   // 0.775--1.102

    AList<TMLink> _all;
    AList<TMLink> _links;
//    AList<TMLink> _unusedHitsOnLocallayer[4];
//    AList<TMLink> _usedHitsOfSecond; //for the (23) layers;

    AList<TMDCWire> _wires;

    static TMDC * _cdc;

  private: //parameters.
//    const float _maxDriftOfHit;
//    float _maxdDistance;// maybe need five parameters here.
//    float _averR[11] = {9.7, 14.5, 22.14, 28.62, 35.1, 42.39, 48.87, 55.35, 61.83, 69.12, 74.79};
//    float _singleSigma; //unit cm.
};

//-----------------------------------------------------------------------------

#ifdef TMDCTsf_NO_INLINE
#define inline
#else
#undef inline
#define TMDCTsf_INLINE_DEFINE_HERE
#endif

#ifdef TMDCTsf_INLINE_DEFINE_HERE



#endif

#undef inline

#endif /* TMDCTsf_FLAG_ */
