//-----------------------------------------------------------------------------
// $Id: TCosmicFitter.h,v 1.8 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TCosmicFitter.h
// Section  : Tracking
// Owner    : Jun-ichi Suzuki
// Email    : jsuzuki@bmail.kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a helix.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TCOSMICFITTER_FLAG_
#define TCOSMICFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#include "TrkReco/TMFitter.h"
#define HEP_SHORT_NAMES
#include "CLHEP/Matrix/Vector.h"
//#include "helix/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"


using CLHEP::HepVector;

class TMLink;

/// A class to fit a TTrackBase object to a helix.
class TCosmicFitter : public TMFitter {

  public:
    /// Constructor.
    TCosmicFitter(const std::string & name);

    /// Destructor
    virtual ~TCosmicFitter();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Modifiers
    int fit(TTrackBase &) const;
    int fit(TTrackBase &, float t0Offset) const;

    int fitWithCathode(TTrackBase &, float t0Offset = 0.,
                        float windowSize= 0.6, int SysCorr = 0 ) ; // added by matsu ( 1999/07/05 )

  private:
    /// calculates dXda. 'link' and 'dPhi' are inputs. Others are outputs.
    int dxda(const TMLink & link,
	     const Helix & helix,
	     double dPhi,
	     HepVector & dxda,
	     HepVector & dyda,
	     HepVector & dzda,
             int doSagCorrection) const;

  private:
    IMagneticFieldSvc* m_pmgnIMF;

};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TCOSMICFITTER_INLINE_DEFINE_HERE
#endif

#ifdef TCOSMICFITTER_INLINE_DEFINE_HERE

#endif

#undef inline

#endif /* TCOSMICFITTER_FLAG_ */
