//-----------------------------------------------------------------------------
// $Id: TBuilderCurl.h,v 1.13 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TBuilderCurl.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to build a curl track.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TBuilderCurl_FLAG_
#define TBuilderCurl_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/TMSelector.h"
#include "TrkReco/TBuilder0.h"
#include "TrkReco/TCurlFinderParameters.h"
//#include "TrkReco/TSvdFinder.h"
//#include "TrkReco/TSvdAssociator.h"

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"


//class HepPoint3D;

/// A class to build a Curl track.
class TBuilderCurl : public TBuilder0 {

  public:
    /// Constructor.
    TBuilderCurl(const std::string & name);

    /// Destructor
    virtual ~TBuilderCurl();

  public:// Selectors
    /// appends stereo hits to a track.
    TTrack * buildStereo(TTrack & track, const AList<TMLink> &) const;
    TTrack * buildStereoMC(TTrack & track, const AList<TMLink> &) const;

    //const TMSelector & trackSelector(const TMSelector &);// not used
    void setParam(const TCurlFinderParameter&);

    TTrack * buildStereo(TTrack & track, const AList<TMLink> &, const AList<TMLink> &) const;
    bool buildStereo(TTrack & track, double &dZ, double &tanL) const;

//  void setSvdClusters(void){
//    m_svdAssociator->clearClusters();
//    m_svdAssociator->fillClusters();
//  }

  private:
    //
    void setArcZ(TTrack&, AList<TMLink>&) const;
    void setArcZ(TTrack&, AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 unsigned) const;
    void setArcZ(TTrack&, AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 AList<TMLink>&,
		 unsigned) const;
    void setArcZ(TTrack&, AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 AList<TMLink>&, AList<TMLink>&,
		 unsigned) const;
    void setArcZ(TTrack&, AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 unsigned) const;
    void setArcZ(TTrack&, AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 AList<TMLink>&, AList<TMLink>&, AList<TMLink>&, AList<TMLink>&,
		 unsigned) const;

    //
    unsigned appendPoints(AList<TMLink>&, AList<TMLink>&, double, double, TTrack&, double) const;

    //
    void resetHelixFit(THelixFitter *) const;

    //
    void fitLine(AList<TMLink>&, double&, double&, double&, AList<TMLink>&, AList<HepPoint3D>&, int&) const;
    void fitLine2(const AList<TMLink>&, double&, double&, double&, AList<TMLink>&, AList<HepPoint3D>&, int&) const;

    //
    unsigned check(const TTrack&) const;

    //
    bool fitWDD(double &xc, double &yc, double &r,
                AList<TMLink> &list) const;
    int stereoHit(double &xc, double &yc, double &r, double &q,
                  AList<TMLink> & list) const;

    void makeLine(TTrack&, AList<TMLink>&, const AList<TMLink>&,
		  AList<TMLink>&, double&, double&, double&, AList<HepPoint3D>&) const;
    int sortByLocalId(AList<TMLink> &list) const;

    //
    void plotArcZ(AList<TMLink>&, double = 0., double = 0., const int = 1) const;

  private:
    mutable THelixFitter _fitter;
    TCurlFinderParameter m_param;
//    TSvdFinder * m_svdFinder;
//    TSvdAssociator * m_svdAssociator;
  private:
    IMagneticFieldSvc* m_pmgnIMF;

};

//-----------------------------------------------------------------------------

#ifdef TBuilderCurl_NO_INLINE
#define inline
#else
#undef inline
#define TBuilderCurl_INLINE_DEFINE_HERE
#endif

#ifdef TBuilderCurl_INLINE_DEFINE_HERE

#endif

#undef inline

#endif /* TBuilderCurl_FLAG_ */
