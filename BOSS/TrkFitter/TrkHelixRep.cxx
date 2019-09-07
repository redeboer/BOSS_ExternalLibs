// File and Version Information:
//      $Id: TrkHelixRep.cxx,v 1.4 2011/09/26 00:56:56 zhangy Exp $
//
// Description:
//      Implements helix track representation
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#include "MdcGeom/Constants.h"
#include "TrkFitter/TrkHelixRep.h"
#include "BField/BField.h"
#include "TrkBase/TrkSimpTraj.h"
#include "TrkBase/TrkExchangePar.h"
#include "MdcGeom/BesAngle.h"
#include "ProxyDict/IfdIntKey.h"
#include "TrkBase/TrkRecoTrk.h"
using std::endl;
using std::ostream;

// Construct from exchange parameters
TrkHelixRep::TrkHelixRep(const TrkExchangePar& inPar,
    TrkRecoTrk* trk,
    PdtPid::PidType hypo,
    const TrkHotList *hots) :
  TrkSimpleRep(hots, trk, hypo),
  _traj(inPar)
{}


// Construct from exchange parameters, no hits
TrkHelixRep::TrkHelixRep(const TrkExchangePar& inPar, TrkRecoTrk* trk,
    PdtPid::PidType hypo, int nact, int nsv, int ndc, double chi2,
    double stFndRng, double endFndRng) :
  TrkSimpleRep(trk, hypo, nact, nsv, ndc, chi2, stFndRng, endFndRng),
  _traj(inPar.params(), inPar.covariance())
{}

// copy ctor
TrkHelixRep::TrkHelixRep(const TrkHelixRep& old, TrkRecoTrk* trk, PdtPid::PidType hypo) :
  TrkSimpleRep(old, trk, hypo), _traj(old._traj)
{}

TrkHelixRep::~TrkHelixRep()
{}

TrkHelixRep*
TrkHelixRep::clone(TrkRecoTrk* theTrack) const
{
  TrkHelixRep* newRep = new TrkHelixRep(*this, theTrack, this->particleType());
  newRep->setCurrent(fitCurrent());
  return newRep;
}

  TrkHelixRep*
TrkHelixRep::cloneNewHypo(PdtPid::PidType hypo)
{
  TrkHelixRep* newRep = new TrkHelixRep(*this, parentTrack(), hypo);
  newRep->setValid(fitValid());
  newRep->setCurrent(fitCurrent());   // true for helices, anyway
  return newRep;
}

TrkExchangePar
TrkHelixRep::helix(double ) const
{
  HepVector outPar = _traj.parameters()->parameter();
  outPar[HelixTraj::phi0Index] = BesAngle(outPar[HelixTraj::phi0Index]).rad();
  return TrkExchangePar(outPar, _traj.parameters()->covariance());
}

void
TrkHelixRep::print(ostream& ostr) const
{
  ostr << "TrkHelixRep "
    << " q: "<<charge()
    << " phi0: " << BesAngle(_traj.phi0()).rad()
    << " om: "<<_traj.omega()
    << " pt: "<<pt()
    << " p: "<<momentum()
    << " d0: " << _traj.d0() 
    << " z0: " << _traj.z0()
    << " ct: " << _traj.tanDip() 
    << "parent track: "<<parentTrack()->id() << endl;
}

void
TrkHelixRep::printAll(ostream& ostr) const
{
  print(ostr);
}

  TrkSimpTraj&
TrkHelixRep::simpTraj()
{
  return _traj;
}

const TrkSimpTraj&
TrkHelixRep::simpTraj() const
{
  return _traj;
}

  TrkDifTraj&
TrkHelixRep::traj()
{
  return _traj;
}

const TrkDifTraj&
TrkHelixRep::traj() const
{
  return _traj;
}

//Returns arbitrary key, distinguishing this Rep from all other reps
const IfdKey&
TrkHelixRep::myKey() const
{
  static IfdIntKey _theKey(3133);
  return _theKey;
}


bool
TrkHelixRep::resid(const TrkHitOnTrk *h, double& residual, double& residErr,
    bool exclude) const
{
  if (this != h->getParentRep() ) return false;
  static HepVector derivs; double deltaChi;
  TrkErrCode errCode = h->getFitStuff( derivs, deltaChi );
  // TODO: add caching of derivatives here...
  if (errCode.failure()) return false;
  if (!exclude || !h->isActive()) {
    HepSymMatrix c = _traj.parameters()->covariance().similarityT(derivs);
    double tErr2 = c(1,1);
    double rms = h->hitRms();
    residual=h->residual();
    double e2=rms*rms+(h->isActive()?-1:1)*tErr2;
    if (e2<0) return false;
    residErr=sqrt(e2);
  } else {
    assert(h->isActive());
    // FIXME: never really checked this is really OK...
    // FIXME: should compare to HelixRefiner...
    HepSymMatrix w=_traj.parameters()->covariance();
    int ier;
    w.invert(ier);
    if (ier!=0) return false;
    HepVector p0 = _traj.parameters()->parameter();
    HepVector p1 = w*p0;
    p1 -= deltaChi*derivs;
    for (unsigned i=0; i<p0.num_row(); ++i) for (unsigned j=0; j<i+1 ;++j) {
      w.fast(i+1,j+1) -= derivs[i]*derivs[j];
    }
    w.invert(ier);
    if (ier!=0) return false;
    residual=h->residual()+h->hitRms()*dot(derivs,w*p1-p0);
    residErr=h->hitRms()*sqrt(w(1,1)+1);
  }
  return true;
}
