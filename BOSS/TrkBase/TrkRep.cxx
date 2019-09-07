//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkRep.cxx,v 1.3 2005/12/01 06:18:42 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
// Revision History (started 2002/05/22)
//	20020522  M. Kelsey -- Remove assert() from resid(HOT*...).  Replace
//		  with sanity checks on HOT/Rep association and whether HOT
//		  has already-computed residual.  Return value used to
//		  flag sanity checks and "trustworthiness" of results.
//------------------------------------------------------------------------
#include "MdcGeom/Constants.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
#include "TrkBase/TrkRep.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtEntry.h"
#include "TrkBase/TrkDifTraj.h"
#include "TrkBase/TrkHotListFull.h"
#include "TrkBase/TrkHotListEmpty.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkFunctors.h"
#include "TrkBase/TrkErrCode.h"
#include "MdcRecoUtil/DifPoint.h"
#include "MdcRecoUtil/DifVector.h"
#include "MdcRecoUtil/DifIndepPar.h"
#include "ProbTools/ChisqConsistency.h"
#include "ProxyDict/IfdIntKey.h"
#include "TrkBase/TrkExchangePar.h"
using std::cout;
using std::endl;

TrkRep::TrkRep(TrkRecoTrk* trk, PdtPid::PidType hypo,bool createHotList)
  : _hotList( createHotList?new TrkHotListFull:0 )
{
  init(trk, hypo);
}

TrkRep::TrkRep(const TrkHotList& hotlist, TrkRecoTrk* trk,
               PdtPid::PidType hypo)
  :_hotList( hotlist.clone(TrkBase::Functors::cloneHot(this)) )
{
  init(trk, hypo);
}

TrkRep::TrkRep(TrkHotList& hotlist, TrkRecoTrk* trk,
               PdtPid::PidType hypo, bool stealHots)
{
  init(trk, hypo);
  if (!stealHots) {
    _hotList.reset( hotlist.clone(TrkBase::Functors::cloneHot(this)) );
  } else {
    _hotList.reset( new TrkHotListFull(hotlist,setParent(this)) );
  }
}

TrkRep::TrkRep(const TrkHotList* hotlist, TrkRecoTrk* trk,
               PdtPid::PidType hypo)
{
	//yzhang SegGrouperAx::storePar newTrack come here
	//yzhang SegGrouperSt::storePar addZValue come here too and hotlist!=0 do clone()
  init(trk, hypo);
  _hotList.reset( hotlist!=0?
                  hotlist->clone(TrkBase::Functors::cloneHot(this)):
                  new TrkHotListFull );
}

TrkRep::TrkRep(TrkHotList* hotlist, TrkRecoTrk* trk,
               PdtPid::PidType hypo,bool takeownership)
{
  init(trk, hypo);
  if (!takeownership) {
    _hotList.reset( hotlist!=0?
                    hotlist->clone(TrkBase::Functors::cloneHot(this)):
                    new TrkHotListFull );
  } else {
    assert(hotlist!=0);
    _hotList.reset( hotlist->resetParent(setParent(this)) );
  }
}

// Ctor for reps without hits
TrkRep::TrkRep(TrkRecoTrk* trk, PdtPid::PidType hypo, int nact, int nsv,
               int ndc, double stFndRng, double endFndRng)
  :_hotList(new TrkHotListEmpty(nact, nsv, ndc, stFndRng, endFndRng))
{
//        cout << " in TrkRep copy ctor 1"   << endl;//yzhang debug
	
  init(trk, hypo);
}

// copy ctor
TrkRep::TrkRep(const TrkRep& oldRep, TrkRecoTrk* trk, PdtPid::PidType hypo) :
  TrkFitStatus(oldRep)
{
//	cout << " in TrkRep copy ctor 2"   << endl;//yzhang debug
	
  init(trk, hypo);
  // Hots and hotlist have to be cloned in the derived classes
}

TrkRep&
TrkRep::operator= (const TrkRep& right)
{
  if(&right != this){
    init(right._parentTrack, right._partHypo);
    _hotList.reset( right._hotList->clone(this) );
    TrkFitStatus::operator=(right);
  }
  return *this;
}

void
TrkRep::init(TrkRecoTrk* trk, PdtPid::PidType hypo)
{
  _parentTrack = trk;
  _partHypo = hypo;
  _betainv = -999999.;
}

TrkRep::~TrkRep()
{
}

bool
TrkRep::operator== (const TrkRep& rhs)
{
  return (&rhs == this);
}

void
TrkRep::addHot(TrkHitOnTrk *newHot)
{
  if (newHot->isActive()) setCurrent(false);
  hotList()->append(newHot);
}

void
TrkRep::removeHot(TrkHitOnTrk *theHot)
{
  if(theHot->isActive()) setCurrent(false);     // fit no longer current
  hotList()->remove(theHot);
}

void
TrkRep::activateHot(TrkHitOnTrk* hot)
{
  if(!hot->isActive()){
// make sure this is my hot we're talking about
    if(this == hot->getParentRep()){
      setCurrent(false);
// actually activate the hot; this is now the rep's job
      hot->setActive(true);
    }
  }
}

void
TrkRep::deactivateHot(TrkHitOnTrk* hot)
{
  if(hot->isActive()){
// make sure this is my hot we're talking about
    if(this == hot->getParentRep()){
      setCurrent(false);
// actually deactivate the hot; this is now the rep's job
      hot->setActive(false);
    }
  }
}

HepPoint3D
TrkRep::position(double fltL) const
{
  return traj().position(fltL);
}

Hep3Vector
TrkRep::direction(double fltL) const
{
  return traj().direction(fltL);
}

double
TrkRep::arrivalTime(double fltL) const
{
  static double cinv = 1./Constants::c;
  // Initialize cache
  if (_betainv < 0.0) {
    double mass2 = Pdt::lookup(particleType())->mass();
    mass2 = mass2 * mass2;
    double ptot2 = momentum(0.).mag2();
    assert(ptot2 != 0.0);
    _betainv = sqrt( (ptot2 +  mass2)/ ptot2);
  }
  double tof = fltL * _betainv * cinv;
  return trackT0() + tof;
}

double
TrkRep::trackT0() const
{
  return parentTrack()->trackT0();
}

BesPointErr
TrkRep::positionErr(double fltL) const
{
  static DifPoint posD;
  static DifVector dirD;
  traj().getDFInfo2(fltL, posD, dirD);
  HepMatrix err = posD.errorMatrix( posD.x.indepPar()->covariance() );
  HepPoint3D point(posD.x.number(), posD.y.number(), posD.z.number());
  BesError symErr(3);
  symErr.assign(err);

  if (false) {
#ifdef MDCPATREC_ROUTINE 
    cout<< "ErrMsg(routine) " << "Pos " 
	    << err.num_row() << " " << err.num_col() << endl
	    << "output:" << endl
      //    << err(1,1) << endl
      //    << err(2,1) << "  " << err(2,2) << endl
      //    << err(3,1) << "  " << err(3,2) << "  " << err(3,3) << endl
                    << "x deriv: " << endl
                    << posD.x.derivatives() << endl
                    << "y deriv: " << endl
                    << posD.y.derivatives() << endl
                    << endl;
#endif
    //  }

    Hep3Vector pointDir(point.x(), point.y());
    double dirMag = pointDir.mag();
    double dist = 5.e-3;
    double delx = dist * point.x() / dirMag;
    double dely = dist * point.y() / dirMag;
    int ierr = 0;
    HepMatrix weight = err.inverse(ierr);
    double chisq =     weight(1,1) * delx * delx +
      2 * weight(2,1) * delx * dely +
      weight(2,2) * dely * dely;
#ifdef MDCPATREC_DEBUG
    cout << point << endl;
    cout << symErr << endl;
    cout << "delta: " << delx << "  " << dely << endl;
    cout << "chisq: " << chisq << endl;
#endif
    double phi0 = helix(fltL).phi0();
    delx = dist * cos(phi0);
    dely = dist * sin(phi0);
    chisq =            weight(1,1) * delx * delx +
                   2 * weight(2,1) * delx * dely +
                       weight(2,2) * dely * dely;
#ifdef MDCPATREC_DEBUG
    cout << "delta: " << delx << "  " << dely << endl;
    cout << "chisq: " << chisq << endl;
    cout << endl << endl;
#endif
  }
  return BesPointErr(point, symErr);
}

BesVectorErr
TrkRep::directionErr(double fltL) const
{
  static DifPoint posD;
  static DifVector dirD;
  traj().getDFInfo2(fltL, posD, dirD);
  BesError symErr(3);
  symErr.assign( dirD.errorMatrix( dirD.x.indepPar()->covariance() ));
  Hep3Vector dir(dirD.x.number(), dirD.y.number(), dirD.z.number());
  return BesVectorErr(dir, symErr);
}

double
TrkRep::startValidRange() const
{
  return traj().lowRange();
}

double
TrkRep::endValidRange() const
{
  return traj().hiRange();
}

double
TrkRep::startFoundRange() const
{
  return hotList()->startFoundRange();
}

double
TrkRep::endFoundRange() const
{
  return hotList()->endFoundRange();
}

PdtPid::PidType
TrkRep::particleType() const
{
  return _partHypo;
}

const IfdKey&
TrkRep::myKey() const
{
  // This provides a default key (used to provide Rep-specific interfaces
  //   to TrkRecoTrk consumers).
  static IfdIntKey _theKey(0);
  return _theKey;
}

void
TrkRep::updateHots()
{
  setCurrent(false);
  hotList()->updateHots();
}

int
TrkRep::nActive() const
{
  return hotList()->nActive();
}

int
TrkRep::nSvt() const
{
  return hotList()->nSvt();
}

int
TrkRep::nMdc() const
{
  return hotList()->nMdc();
}

bool
TrkRep::resid(const TrkHitOnTrk *h,
              double& residual, double& residErr,
              bool exclude) const
{
  assert (h != 0);
  if (h->parentRep() != this) return false;	// HOT must belong to Rep
  if (!h->hasResidual()) return false;		// Residual must be available
  if (exclude) return false;  			// FIXME: Can't do unbiased residuals (yet!)

  residual=h->residual();
  residErr=h->hitRms();
  return true;
}

ChisqConsistency
TrkRep::chisqConsistency() const {
  if(fitValid())
    return ChisqConsistency(chisq(),nDof());
  else
    return ChisqConsistency();
}
