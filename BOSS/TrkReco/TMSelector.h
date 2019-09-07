//-----------------------------------------------------------------------------
// $Id: TMSelector.h,v 1.7 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TMSelector.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to select a TTrackBase object.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMSelector_FLAG_
#define TMSelector_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>
#include <iostream>

#define HEP_SHORT_NAMES


class TTrackBase;
class TCircle;

/// A class to select a TTrackBase object.
class TMSelector {

  public:
    /// Constructor.
    TMSelector();

    /// Copy constructor.
    TMSelector(const TMSelector &);

    /// Destructor
    virtual ~TMSelector();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns min. \# of hits(TMLinks) requried.
    unsigned nLinks(void) const;

    /// returns min. \# of super layers required.
    unsigned nSuperLayers(void) const;

    /// returns min. pt required.
    double minPt(void) const;

    /// returns max. impact(2D) required.
    double maxImpact(void) const;

    /// returns min. \# of stereo hits(TMLinks) requried.
    unsigned nLinksStereo(void) const;

    /// returns max. distance required for stereo hits.
    double maxDistance(void) const;

    /// returns max. sigma for each TMLink.
    double maxSigma(void) const;

    /// returns true if given track satisfys criteria after fitting.
    bool select(TTrackBase &) const;

    /// returns true if given track satisfys criteria before fitting.
    bool preSelect(const TTrackBase &) const;

  public:// Modifiers
    /// sets \# of hits(TMLinks) requried.
    unsigned nLinks(unsigned);

    /// sets \# of super layers required.
    unsigned nSuperLayers(unsigned);

    /// sets min. pt required.
    double minPt(double);

    /// sets max. impact(2D) required.
    double maxImpact(double);

    /// sets min. \# of stereo hits(TMLinks) requried.
    unsigned nLinksStereo(unsigned);

    /// sets max. distance required for stereo hits.
    double maxDistance(double);

    /// sets max. sigma for each TMLink.
    double maxSigma(double);

  private:

  private:
    bool _nLinksDefined;
    bool _nSuperLayersDefined;
    bool _minPtDefined;
    bool _maxImpactDefined;
    bool _maxSigmaDefined;

    unsigned _nLinks;
    unsigned _nSuperLayers;
    double _minPt;
    double _maxImpact;
    double _maxSigma;

    bool _nLinksStereoDefined;
    bool _maxDistanceDefined;

    unsigned _nLinksStereo;
    double _maxDistance;
};

//-----------------------------------------------------------------------------

#ifdef TMSelector_NO_INLINE
#define inline
#else
#undef inline
#define TMSelector_INLINE_DEFINE_HERE
#endif

#ifdef TMSelector_INLINE_DEFINE_HERE

inline
unsigned
TMSelector::nLinks(void) const {
#ifdef TRKRECO_DEBUG
    if (! _nLinksDefined)
	std::cout << "TMSelector !!! min. nLinks is not defined" << std::endl;
#endif
    return _nLinks;
}

inline
unsigned
TMSelector::nLinks(unsigned a) {
    _nLinksDefined = true;
    return _nLinks = a;
}

inline
unsigned
TMSelector::nSuperLayers(void) const {
#ifdef TRKRECO_DEBUG
    if (! _nSuperLayers)
	std::cout << "TMSelector !!! min. nSuperLayers is not defined" << std::endl;
#endif
    return _nSuperLayers;
}

inline
unsigned
TMSelector::nSuperLayers(unsigned a) {
    _nSuperLayersDefined = true;
    return _nSuperLayers = a;
}

inline
double
TMSelector::minPt(void) const {
#ifdef TRKRECO_DEBUG
    if (! _minPtDefined)
	std::cout << "TMSelector !!! min. pt is not defined" << std::endl;
#endif
    return _minPt;
}

inline
double
TMSelector::minPt(double a) {
    _minPtDefined = true;
    return _minPt = a;
}

inline
double
TMSelector::maxImpact(void) const {
#ifdef TRKRECO_DEBUG
    if (! _maxImpactDefined)
	std::cout << "TMSelector !!! max. impact is not defined" << std::endl;
#endif
    return _maxImpact;
}

inline
double
TMSelector::maxImpact(double a) {
    _maxImpactDefined = true;
    return _maxImpact = a;
}

inline
double
TMSelector::maxSigma(void) const {
#ifdef TRKRECO_DEBUG
    if (! _maxSigmaDefined)
	std::cout << "TMSelector !!! max. sigma is not defined" << std::endl;
#endif
    return _maxSigma;
}

inline
double
TMSelector::maxSigma(double a) {
    _maxSigmaDefined = true;
    return _maxSigma = a;
}

inline
unsigned
TMSelector::nLinksStereo(void) const {
#ifdef TRKRECO_DEBUG
    if (! _nLinksStereoDefined)
	std::cout << "TMSelector !!! min. nLinksStereo is not defined" << std::endl;
#endif
    return _nLinksStereo;
}

inline
unsigned
TMSelector::nLinksStereo(unsigned a) {
    _nLinksStereoDefined = true;
    return _nLinksStereo = a;
}

inline
double
TMSelector::maxDistance(void) const {
#ifdef TRKRECO_DEBUG
    if (! _maxDistanceDefined)
	std::cout << "TMSelector !!! max. distance is not defined" << std::endl;
#endif
    return _maxDistance;
}

inline
double
TMSelector::maxDistance(double a) {
    _maxDistanceDefined = true;
    return _maxDistance = a;
}

#endif

#undef inline

#endif /* TMSelector_FLAG_ */
