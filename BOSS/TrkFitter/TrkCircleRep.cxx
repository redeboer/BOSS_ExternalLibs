// File and Version Information:
//      $Id: TrkCircleRep.cxx,v 1.3 2011/09/26 00:56:56 zhangy Exp $
//
// Description:
//      Implements circle track representation 
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
//#include "BaBar/BaBar.h"
#include "TrkFitter/TrkCircleRep.h"
#include "TrkBase/TrkSimpTraj.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkExchangePar.h"
#include "MdcGeom/BesAngle.h"
using std::endl;
using std::ostream;

// Construct from exchange paramters
TrkCircleRep::TrkCircleRep(const TrkExchangePar& inPar,
                           TrkRecoTrk* trk,
                           PdtPid::PidType hypo,
                           const TrkHotList *inHots):
  TrkSimpleRep(inHots, trk, hypo),
  _traj(inPar)
{
  set2d(true);
}

// Construct from exchange paramters, no hits
TrkCircleRep::TrkCircleRep(const TrkExchangePar& inPar, TrkRecoTrk* trk,
                           PdtPid::PidType hypo, int nact, int nsv, int ndc, double chi2,
                           double stFndRng, double endFndRng) :
  TrkSimpleRep(trk, hypo, nact, nsv, ndc, chi2, stFndRng, endFndRng),
  _traj(inPar)
{
  set2d(true);
}

// copy ctor
TrkCircleRep::TrkCircleRep(const TrkCircleRep& right, TrkRecoTrk* trk,
                           PdtPid::PidType hypo) :
  TrkSimpleRep(right, trk, hypo), _traj(right._traj) 
{
    set2d(true);
}

TrkCircleRep::~TrkCircleRep()
{}

TrkCircleRep*
TrkCircleRep::clone(TrkRecoTrk* theTrack) const
{
  TrkCircleRep* newRep = new TrkCircleRep(*this, theTrack, this->particleType());
  newRep->setValid(fitValid());
  newRep->setCurrent(fitCurrent());
  return newRep;
	
}

TrkCircleRep*
TrkCircleRep::cloneNewHypo(PdtPid::PidType hypo)
{
  TrkCircleRep* newRep = new TrkCircleRep(*this, parentTrack(), hypo);
  newRep->setValid(fitValid());
  newRep->setCurrent(fitCurrent());  // true for circles, anyway
  return newRep;
}

TrkExchangePar
TrkCircleRep::helix(double ) const
{
  TrkExchangePar localPar(_traj.d0(), BesAngle(_traj.phi0()).rad(), 
                          _traj.omega(), 0.0, 0.0);
  HepSymMatrix& theCov = localPar.covariance();
  for (int i = 1; i < 4; i++) {
    for (int j = 1; j < 4; j++) {
      theCov(i,j) = _traj.parameters()->covariance()(i,j);
    }
  }
  return localPar;
}

void 
TrkCircleRep::print(ostream& ostr) const
{
  ostr << "TrkCircleRep "
    << " q: "<<charge()
    << " phi0: " << BesAngle(_traj.phi0()).rad()
    << " om: "<<_traj.omega()
    << " pt: "<<pt()
    << " p: "<<momentum()
    << " d0: " << _traj.d0() 
    << " parent track:"<<parentTrack()->id()
    << endl;

}

void
TrkCircleRep::printAll(ostream& ostr) const
{
  print(ostr);
}

  TrkSimpTraj&
TrkCircleRep::simpTraj()
{
  return _traj;
}

const TrkSimpTraj&
TrkCircleRep::simpTraj() const
{
  return _traj;
}

  TrkDifTraj&
TrkCircleRep::traj()
{
  return _traj;
}
const TrkDifTraj&
TrkCircleRep::traj() const
{
  return _traj;
}
