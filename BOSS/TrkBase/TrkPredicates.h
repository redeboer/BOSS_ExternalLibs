//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkPredicates.h,v 1.2 2007/11/13 07:27:23 codeman Exp $
//
// Description:
//      general, simple tracking predicates
//
//      All classes (or structs) here should inherit from either
//      unary_function< X, bool> or binary_function<X,Y,bool> and
//      provide bool operator()(const X&,const Y&) const. In addition,
//      this operator should be a 'pure' function, i.e. _no_ side effects...
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gerhard Raven
//------------------------------------------------------------------------
#include <functional>

#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkFitStatus.h"

namespace TrkBase { namespace Predicates {

  struct isHotActive : TrkHitOnTrk::predicate_type {
       bool operator()(const TrkHitOnTrk& h) const
       { return h.isActive(); }
  };

  class isLayer : TrkHitOnTrk::predicate_type {
  public:
       isLayer(int layer) : _layer(layer) {}
       bool operator()(const TrkHitOnTrk& h) const
       { return h.layerNumber() == _layer; }
  private:
       int _layer;
  };

  struct hasResidual : TrkHitOnTrk::predicate_type {
       bool operator()(const TrkHitOnTrk& h) const
       { return h.hasResidual(); }
  };

  class isSvtHitOnTrack : public TrkHitOnTrk::predicate_type {
  public:
       isSvtHitOnTrack(bool activeOnly=false) : _activeOnly(activeOnly) { }
       bool operator()(const TrkHitOnTrk& h) const
       { return h.svtHitOnTrack()!=0 && ( !_activeOnly || h.isActive() ); }
  private:
       bool _activeOnly;
  };

  class isMdcHitOnTrack : public TrkHitOnTrk::predicate_type {
  public:
       isMdcHitOnTrack(bool activeOnly=false) : _activeOnly(activeOnly) { }
       bool operator()(const TrkHitOnTrk& h) const
       { return h.mdcHitOnTrack()!=0 && ( !_activeOnly || h.isActive() ); }
  private:
       bool _activeOnly;
  };

  class hasView : public TrkHitOnTrk::predicate_type {
  public:
       hasView(TrkEnums::TrkViewInfo view=TrkEnums::bothView) : _view(view) { }
       bool operator()(const TrkHitOnTrk& h) const
       {
          if (_view==TrkEnums::bothView) return true;
          TrkEnums::TrkViewInfo hotview = h.whatView();
          if (h.svtHitOnTrack() != 0) {
              return _view == hotview;
          } else if (h.mdcHitOnTrack() != 0) {
// unfortunately the Mdc calls stereo hits 'both view': I'm re-interpreting it here
             return _view == TrkEnums::xyView && hotview == _view ||
                    _view == TrkEnums::zView && hotview == TrkEnums::bothView;
          }
          return false;
       }
  private:
       TrkEnums::TrkViewInfo _view;
  };


  class hotMatchesFundHit : public TrkHitOnTrk::predicate_type {
  public:
       hotMatchesFundHit(const TrkFundHit *h) : _h(h) { }
       bool operator()(const TrkHitOnTrk& h) const
       { return h.hit()==_h; }
  private:
       const TrkFundHit *_h;
  };


  struct isHotOnTrack : std::binary_function<TrkHitOnTrk,const TrkRecoTrk*, bool>
  {
       bool operator()(const TrkHitOnTrk& h, const TrkRecoTrk* const & t) const
       { return h.parentTrack() == t; }
  };

  struct isFitValid : TrkFitStatus::predicate_type {
          bool operator()( const TrkFitStatus& t) const
          { return (t.fitValid()); }
  };

  struct isFitCurrent : TrkFitStatus::predicate_type {
          bool operator()( const TrkFitStatus& t) const
          { return (t.fitCurrent()); }
  };

  // note that we use 'has' in here, not 'is' as above.... subtly distinction ;-)

  struct hasFitValid : TrkRecoTrk::predicate_type {
          bool operator()( const TrkRecoTrk& t) const
          { const TrkFitStatus *x = t.status(); return x!=0 && isFitValid()(*x); }
  };

  struct hasFitCurrent : TrkRecoTrk::predicate_type {
          bool operator()( const TrkRecoTrk& t) const
          { const TrkFitStatus *x = t.status(); return x!=0 && isFitCurrent()(*x); }
  };


} }
