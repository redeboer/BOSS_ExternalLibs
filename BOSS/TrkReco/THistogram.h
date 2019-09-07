//-----------------------------------------------------------------------------
// $Id: THistogram.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : THistogram.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class for a histogram used in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef THistogram_FLAG_
#define THistogram_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"

#include "TrkReco/TMLink.h"
#include "TrkReco/TSegment0.h"
#include "TrkReco/TSegment.h"

class TCircle;

/// A class for a histogram used in tracking.
class THistogram {

  public:
    /// Constructor.
    THistogram(unsigned nBins);

    /// Destructor
    virtual ~THistogram();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a pointer to i'th AList<TMLink>.
    const AList<TMLink> * const bin(unsigned i) const;

    /// returns a pointer to i'th AList<TMLink>.
    const AList<TMLink> * const bin(int i) const;

    /// returns number in i'th bin.
    unsigned nBin(unsigned i) const;

    /// returns \# of bins.
    unsigned nBins(void) const;

    /// returns an AList of TMLinks within 'center' bin +- 'width' bin region.
    AList<TMLink> contents(unsigned center, unsigned width) const;

    /// returns an AList of TMLinks from 'start' bin to 'end' bin region.
    AList<TMLink> contents(int start, int end) const;

    /// returns an AList<TMLink> of all contents.
    const AList<TMLink> & contents(void) const;

    /// returns an AList<TSegment0> of clusters.
    AList<TSegment0> clusters0(void) const;

    /// returns an AList<TSegment0> of clusters.
    AList<TSegment> clusters(void) const;

    /// returns an AList<TSegment0> using clusters() function.
    AList<TSegment0> segments0(void) const;

    /// returns an AList<TSegment0> using clusters() function.
    AList<TSegment> segments(void) const;

  public:// Modifiers
    /// fills with hits.
    void fillX(const AList<TMLink> & links);

    /// fills with hits.
    void fillY(const AList<TMLink> & links);

    /// fills with hits.
    void fillPhi(const AList<TMLink> & links);

    /// removes links.
    void remove(const AList<TMLink> & links);

    /// masks a bin. Masked bin will not be returned by ::findPeak.
    void mask(unsigned binN);

    /// remove masks.
    void unmask(void);

  private:
    unsigned _nBins;
    unsigned * _bins;
    bool * _masks;
    AList<TMLink> ** _links;
    AList<TMLink> _all;
    float _binSize;
};

//-----------------------------------------------------------------------------

#ifdef THistogram_NO_INLINE
#define inline
#else
#undef inline
#define THistogram_INLINE_DEFINE_HERE
#endif

#ifdef THistogram_INLINE_DEFINE_HERE

inline
const AList<TMLink> * const
THistogram::bin(unsigned i) const {
    if (i < _nBins) return _links[i];
    return 0;
}

inline
const AList<TMLink> * const
THistogram::bin(int i) const {
    while (i < 0) i += _nBins;
    return _links[i % _nBins];
}

inline
unsigned
THistogram::nBins(void) const {
    return _nBins;
}

inline
void
THistogram::mask(unsigned a) {
    _masks[a] = true;
}

inline
void
THistogram::unmask(void) {
    for (unsigned i = 0; i < _nBins; i++) _masks[i] = false;
}

inline
const AList<TMLink> &
THistogram::contents(void) const {
    return _all;
}

inline
unsigned
THistogram::nBin(unsigned i) const {
    if (i < _nBins) return _bins[i];
    return 0;
}

#endif

#undef inline

#endif /* THistogram_FLAG_ */
