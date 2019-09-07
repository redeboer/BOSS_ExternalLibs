//#include "BaBar/BaBar.h"
#include <assert.h>
#include <iostream>
#include "TrkBase/TrkCompTrk.h"
#include "TrkBase/TrkSimpTraj.h"
#include "TrkBase/HelixTraj.h"
#include "TrkBase/NeutTraj.h"
#include "TrkBase/NeutParams.h"
#include "TrkBase/TrkMomCalculator.h"
//#include "AbsEnv/AbsEnv.h"
//#include "ProxyDict/Ifd.h"
#include "ProxyDict/IfdStrKey.h"
#include "BField/BField.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkPoca.h"
#include "MdcRecoUtil/BesPointErr.h"
#include "MdcRecoUtil/BesVectorErr.h"
#include "MdcRecoUtil/DifPoint.h"
#include "MdcRecoUtil/DifVector.h"
#include "MdcRecoUtil/DifIndepPar.h"
#include "TrkBase/TrkHelixUtils.h"
#include "MdcGeom/BesAngle.h"
using std::endl;
using std::ostream;

// Parameters Constructor
//----------------------------------------------------------------------------
TrkCompTrk::TrkCompTrk(const BesPointErr& pos, 
		       const BesVectorErr& mom, 
		       const HepMatrix& xpCov, 
		       int charge, double chisq, int nDof, const BField* bf) :
  _chisq(chisq),_nDof(nDof) {
//---------------------------------------------------------------------------
  _bf = bf;
  _charge=charge;
  if(_charge!=0) {
    TrkExchangePar par1 = TrkHelixUtils::helixFromMomErr(pos,mom,xpCov,charge,bField());
    _traj.reset( new HelixTraj(par1.params(), par1.covariance()) );
  }
  else {
    _traj.reset( new NeutTraj(TrkHelixUtils::lineFromMomErr(pos,mom,xpCov,1,bField())) );
  }
}

//-- Copy constructor
//---------------------------------------------
TrkCompTrk::TrkCompTrk(const TrkCompTrk& rhs) : _bf(rhs._bf),_chisq(rhs._chisq),_nDof(rhs._nDof) {
//---------------------------------------------
  _traj.reset((TrkSimpTraj*)rhs.traj().clone());
  _charge=rhs.charge();
}

//----------------------------------------------------------------------
TrkCompTrk::~TrkCompTrk() {
//----------------------------------------------------------------------
}

//----------------------------------------------------------------------
const TrkCompTrk& 
TrkCompTrk::operator=(const TrkCompTrk &right) {
//----------------------------------------------------------------------
  if (&right == this) return *this;
  _bf = right._bf;
  _traj.reset((TrkSimpTraj*)right.traj().clone());
  _chisq=right.chisq();
  _nDof=right.nDof();
  _charge=right.charge();
  return *this;
}

//------------------------------------------------------------------------
int 
TrkCompTrk::nDof() const {
//------------------------------------------------------------------------
  return _nDof;
}

//------------------------------------------------------------------------
double 
TrkCompTrk::chisq() const {
//------------------------------------------------------------------------
  return _chisq;
}



//------------------------------------------------------------------------
int 
TrkCompTrk::charge() const {
//------------------------------------------------------------------------
  return _charge;
}


//------------------------------------------------------------------------
const TrkDifTraj& 
TrkCompTrk::traj() const {
//------------------------------------------------------------------------
  return *_traj;
}

//------------------------------------------------------------------------
HepPoint3D 
TrkCompTrk::position( double fltL) const {
//------------------------------------------------------------------------
  return traj().position(fltL);
}

//------------------------------------------------------------------------
Hep3Vector 
TrkCompTrk::direction(double fltL) const {
//------------------------------------------------------------------------
  return traj().direction(fltL);
}

Hep3Vector 
TrkCompTrk::momentum(double fltL) const
{
  return TrkMomCalculator::vecMom(*_traj, bField(), fltL);

}

BesPointErr
TrkCompTrk::positionErr(double fltL) const
{
  // If getDFInfo is split up, use faster version - done JEA 020300
  DifPoint posD;
  DifVector dirD;
  traj().getDFInfo2(fltL, posD, dirD);
  HepMatrix err = posD.errorMatrix( posD.x.indepPar()->covariance() );
  BesError symErr(3);
  symErr.assign(err);
  HepPoint3D point(posD.x.number(), posD.y.number(), posD.z.number());
  return BesPointErr(point, symErr);
}

BesVectorErr
TrkCompTrk::directionErr( double fltL) const
{
  // If getDFInfo is split up, use faster version - done JEA 020300
  DifPoint posD;
  DifVector dirD;
  traj().getDFInfo2(fltL, posD, dirD);
  HepMatrix err = dirD.errorMatrix( dirD.x.indepPar()->covariance() );
  BesError symErr(3);
  symErr.assign(err);
  Hep3Vector dir(dirD.x.number(), dirD.y.number(), dirD.z.number());
  return BesVectorErr(dir, symErr);
}

BesVectorErr
TrkCompTrk::momentumErr(double fltL) const
{
  return TrkMomCalculator::errMom(*_traj, bField(), fltL);
}

double
TrkCompTrk::pt(double fltL) const
{
  return TrkMomCalculator::ptMom(*_traj, bField(), fltL);
  // This could probably be made more efficient.
//  Hep3Vector pVec = momentum(fltL);
//  return sqrt( pVec.x() * pVec.x() + pVec.y() * pVec.y() );
}


double
TrkCompTrk::startValidRange() const
{
  return traj().lowRange();
}

double
TrkCompTrk::endValidRange() const
{
 return traj().hiRange();
}

void
TrkCompTrk::print(ostream& ostr) const
{
  ostr << "Traj: ";
  if(_charge==-1 || _charge==1)  {
     ostr << "Charged Particle -> Helix Trajectory" << endl;
     HelixTraj& theTraj=(HelixTraj&)traj();
     ostr << "      phi0=" << BesAngle(theTraj.phi0()).rad() << endl;
     ostr << "      d0=" << theTraj.d0() << endl;
     ostr << "      z0=" << theTraj.z0() << endl;
     ostr << "      omega=" << theTraj.omega() << endl;
     ostr << "      tanDip=" << theTraj.tanDip() << endl;
  }
    else  {
      ostr << "Neutral Particle -> NeutTraj" << endl;
      NeutTraj& theTraj=(NeutTraj&)traj();
      ostr << "      phi0=" << BesAngle(theTraj.params().phi0()).rad() << endl;
      ostr << "      d0=" << theTraj.params().d0() << endl;
      ostr << "      z0=" << theTraj.params().z0() << endl;
      ostr << "      p=" << theTraj.params().p() << endl;
      ostr << "      tanDip=" << theTraj.params().tanDip() << endl;
    }
}

void
TrkCompTrk::printAll(ostream& ostr) const
{
  print(ostr);
}

HepMatrix TrkCompTrk::posmomCov(double fltL) const
{
  const BField& theField = bField();
  return TrkMomCalculator::posmomCov(*_traj, theField, fltL);
}

void TrkCompTrk::getAllCovs(double fltL,
                            HepSymMatrix& xxCov,
                            HepSymMatrix& ppCov,
                            HepMatrix&    xpCov)      const
{
  const BField& theField = bField();
  TrkMomCalculator::getAllCovs(*_traj, theField, fltL,
                               xxCov,ppCov,xpCov);
}

void TrkCompTrk::getAllWeights(double fltL,
                               HepVector& pos,
                               HepVector& mom,
                               HepSymMatrix& xxWeight,
                               HepSymMatrix& ppWeight,
                               HepMatrix&    xpWeight) const
{
  const BField& theField = bField();
  TrkMomCalculator::getAllWeights(*_traj, theField, fltL,
                                  pos,mom,xxWeight,ppWeight,xpWeight);

}


void TrkCompTrk::getAllWeights(const HepPoint3D& pt,
                               HepVector& pos,
                               HepVector& mom,
                               HepSymMatrix& xxWeight,
                               HepSymMatrix& ppWeight,
                               HepMatrix&    xpWeight)const
{
  double fltL=0;
  TrkPoca poca(traj(),fltL, pt);
  fltL = poca.flt1();
  getAllWeights(fltL,pos,mom,xxWeight,ppWeight,xpWeight);
}
