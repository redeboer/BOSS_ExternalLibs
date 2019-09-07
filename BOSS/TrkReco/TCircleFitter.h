//-----------------------------------------------------------------------------
// $Id: TCircleFitter.h,v 1.12 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TCircleFitter.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a circle.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TCIRCLEFITTER_FLAG_
#define TCIRCLEFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES

#include "TrkReco/TMFitter.h"
//#include "helix/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"

class TMLink;
class TCircle;

/// A class to fit a TTrackBase object to a circle.
class TCircleFitter : public TMFitter {

  public:
    /// Constructor.
    TCircleFitter(const std::string & name);

    /// Destructor
    virtual ~TCircleFitter();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Modifiers
    virtual int fit(TTrackBase &) const;

  private:
    mutable double _charge;
    mutable double _radius;
    mutable HepPoint3D _center;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TCIRCLEFITTER_INLINE_DEFINE_HERE
#endif

#ifdef TCIRCLEFITTER_INLINE_DEFINE_HERE

#endif

#undef inline

#endif /* TCIRCLEFITTER_FLAG_ */
