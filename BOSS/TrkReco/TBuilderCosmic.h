//-----------------------------------------------------------------------------
// $Id: TBuilderCosmic.h,v 1.6 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TBuilderCosmic.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to build a cosmic track.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TBuilderCosmic_FLAG_
#define TBuilderCosmic_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

//#ifndef LINE_COSMIC
//#define LINE_COSMIC
//#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/TMSelector.h"
#include "TrkReco/TBuilder0.h"
#include "TrkReco/TCosmicFitter.h"
#include "TrkReco/T3DLineFitter.h"

/// A class to build a cosmic track.
class TBuilderCosmic : public TBuilder0 {

  public:
    /// Constructor.
    TBuilderCosmic(const std::string & name, float salvageLevel);

    /// Destructor
    virtual ~TBuilderCosmic();

  public:// Selectors
    /// appends stereo hits to a track.
    TTrack * buildStereo(TTrack & track, const AList<TMLink> &) const;

  private:
#ifdef LINE_COSMIC
    T3DLineFitter _fitter;
#else
    //THelixFitter _fitter;
    TCosmicFitter _fitter;
#endif
};

//-----------------------------------------------------------------------------

#ifdef TBuilderCosmic_NO_INLINE
#define inline
#else
#undef inline
#define TBuilderCosmic_INLINE_DEFINE_HERE
#endif

#ifdef TBuilderCosmic_INLINE_DEFINE_HERE

#endif

#undef inline

#endif /* TBuilderCosmic_FLAG_ */
