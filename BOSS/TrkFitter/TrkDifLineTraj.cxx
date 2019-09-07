//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkDifLineTraj.cxx,v 1.5 2007/12/07 07:06:34 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#include "TrkFitter/TrkDifLineTraj.h"
#include <assert.h>
#include <math.h>
#include "MdcGeom/BesAngle.h"
#include "TrkFitter/TrkDifLineTraj.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkVisitor.h"
#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifPoint.h"
#include "MdcRecoUtil/DifVector.h"

using CLHEP::Hep3Vector;

TrkDifLineTraj::TrkDifLineTraj(const HepVector& pvec, const HepSymMatrix& pcov,
                               double lowlim, double hilim, const HepPoint3D& refpoint) :
  TrkSimpTraj(pvec, pcov, lowlim,hilim,refpoint)
{
}

TrkDifLineTraj::TrkDifLineTraj(const TrkExchangePar& inpar,
                               double lowlim, double hilim, const HepPoint3D& refpoint) :
  TrkSimpTraj(HepVector(NLINPRM,1),HepSymMatrix(NLINPRM,1), lowlim,hilim,refpoint)
{
// the above constructor actually screws up z0 and tandip.  I can't fix it in
// the initializer, so I'll just overwrite the track here.  Ugly and inefficient
  HepVector subvect(NLINPRM,1);
  HepSymMatrix submat(NLINPRM,1);
  const HepSymMatrix covar = inpar.covariance();
  subvect[d0Ind] = inpar.d0();
  subvect[phi0Ind] = inpar.phi0();
  subvect[z0Ind] = inpar.z0();
  subvect[tanDipInd] = inpar.tanDip();

  submat.fast(d0Ind+1,d0Ind+1) = covar.fast(TrkExchangePar::ex_d0+1,TrkExchangePar::ex_d0+1);
  submat.fast(d0Ind+1,phi0Ind+1) = covar.fast(TrkExchangePar::ex_d0+1,TrkExchangePar::ex_phi0+1);
  submat.fast(d0Ind+1,z0Ind+1) = covar.fast(TrkExchangePar::ex_d0+1,TrkExchangePar::ex_z0+1);
  submat.fast(d0Ind+1,tanDipInd+1) = covar.fast(TrkExchangePar::ex_d0+1,TrkExchangePar::ex_tanDip+1);
  submat.fast(phi0Ind+1,phi0Ind+1) = covar.fast(TrkExchangePar::ex_phi0+1,TrkExchangePar::ex_phi0+1);
  submat.fast(phi0Ind+1,z0Ind+1) = covar.fast(TrkExchangePar::ex_phi0+1,TrkExchangePar::ex_z0+1);
  submat.fast(phi0Ind+1,tanDipInd+1) = covar.fast(TrkExchangePar::ex_phi0+1,TrkExchangePar::ex_tanDip+1);
  submat.fast(z0Ind+1,z0Ind+1) = covar.fast(TrkExchangePar::ex_z0+1,TrkExchangePar::ex_z0+1);
  submat.fast(z0Ind+1,tanDipInd+1) = covar.fast(TrkExchangePar::ex_z0+1,TrkExchangePar::ex_tanDip+1);
  submat.fast(tanDipInd+1,tanDipInd+1) = covar.fast(TrkExchangePar::ex_tanDip+1,TrkExchangePar::ex_tanDip+1);
// reset the track
  (*parameters()) = TrkParams(subvect,submat);
}

TrkDifLineTraj::TrkDifLineTraj( const TrkDifLineTraj& h )
  : TrkSimpTraj(h.parameters()->parameter(), h.parameters()->covariance(),
                h.lowRange(),h.hiRange(),h.referencePoint())
{
}

TrkDifLineTraj*
TrkDifLineTraj::clone() const
{
  return new TrkDifLineTraj(*this);
}

TrkDifLineTraj&
TrkDifLineTraj::operator=(const TrkDifLineTraj& h)
{
  if( &h != this ){
    Trajectory::operator=(h);
    _dtparams = *h.parameters();
    _refpoint = h._refpoint;
  }
  return *this;
}

TrkDifLineTraj::~TrkDifLineTraj()
{
}

double
TrkDifLineTraj::x( const double& f ) const
{
  return -d0() * sin(phi0())  +  f * cos(phi0()) * cosDip()  +
    referencePoint().x();
}

double
TrkDifLineTraj::y( const double& f ) const
{
  return  d0() * cos(phi0())  +  f * sin(phi0()) * cosDip() +
    referencePoint().y();
}

double
TrkDifLineTraj::z( const double& f ) const
{
  return  z0()  +  f * tanDip() * cosDip() +  referencePoint().z();
}

HepPoint3D
TrkDifLineTraj::position( double f) const
{
  double cosd = cosDip();
  double cp = cos(phi0());
  double sp = sin(phi0());
  return HepPoint3D(-d0()*sp + f*cp*cosd + referencePoint().x(),
                  d0()*cp  + f*sp*cosd + referencePoint().y(),
                  z0() + f*tanDip()*cosd + referencePoint().z());
}

Hep3Vector
TrkDifLineTraj::direction( double ) const
{
  double cdip = cosDip();
  return Hep3Vector ( cos(phi0()) * cdip,
                      sin(phi0()) * cdip,
                         tanDip() * cdip);
}

Hep3Vector
TrkDifLineTraj::delDirect( double ) const
{
  return Hep3Vector(0.0, 0.0, 0.0);
}

double
TrkDifLineTraj::distTo1stError(double, double, int) const
{
  return 999.e4;
}

double
TrkDifLineTraj::distTo2ndError(double, double, int) const
{
  return 999.e4;
}

void
TrkDifLineTraj::getInfo(double fltLen, HepPoint3D& pos, Hep3Vector& dir,
                        Hep3Vector& delDir) const
{
  // This could be made much more efficient!!!!!!
  pos = position(fltLen);
  dir = direction(fltLen);
  delDir = delDirect(fltLen);
}

void
TrkDifLineTraj::getInfo( double fltLen, HepPoint3D& pos, Hep3Vector& dir ) const
{
  //  This could be made much more efficient!!!!!
  pos = position(fltLen);
  dir = direction(fltLen);
}

void
TrkDifLineTraj::getDFInfo(double flt, DifPoint& pos, DifVector& dir,
                          DifVector& delDir) const
{
  //Provides difNum version of information for calculation of derivatives.

  // Create difNumber versions of parameters
  //enum index (phi0Index(), etc) is from TrkLineParams.hh
  DifNumber phi0Df(phi0(), phi0Index()+1, nLinPrm());
  DifNumber d0Df(d0(), d0Index()+1, nLinPrm());
  DifNumber z0Df(z0(), z0Index()+1, nLinPrm());
  DifNumber tanDipDf(tanDip(), tanDipIndex()+1, nLinPrm());
  DifNumber zero(0.0, nLinPrm());
  phi0Df.setIndepPar( parameters() );
  d0Df.setIndepPar( parameters() );
  z0Df.setIndepPar( parameters() );
  tanDipDf.setIndepPar( parameters() );
  zero.setIndepPar( parameters() );

  DifNumber sphi0, cphi0;
  phi0Df.cosAndSin(cphi0, sphi0);

  DifNumber px(referencePoint().x());
  DifNumber py(referencePoint().y());
  DifNumber pz(referencePoint().z());

  DifNumber cdip =  1. / sqrt(1. + tanDipDf*tanDipDf);
  DifNumber xx = -d0Df * sphi0  +  flt * cphi0 * cdip + px;
  DifNumber yy =  d0Df * cphi0  +  flt * sphi0 * cdip + py;
  DifNumber zz =  z0Df  +  flt * tanDipDf * cdip + pz;

  pos = DifPoint(xx, yy, zz);
  dir = DifVector( cphi0 * cdip,
                   sphi0 * cdip,
                   tanDipDf * cdip);

  delDir = DifVector(zero, zero, zero);
}

double
TrkDifLineTraj::curvature(double ) const
{
  return 0.;
}

double
TrkDifLineTraj::phi0() const
{
 return BesAngle(parameters()->parameter()[phi0Index()]).rad();
}

HepMatrix
TrkDifLineTraj::derivDeflect(double fltlen,deflectDirection idirect) const
{
//  This function computes the column matrix of derivatives for the change
//  in parameters for a change in the direction of a track at a point along
//  its flight, holding the momentum and position constant.  The effects for
//  changes in 2 perpendicular directions (theta1 = dip and
//  theta2 = phi*cos(dip)) are uncorrelated.

  HepMatrix ddflct(nLinPrm(),1);
//  Compute some common things
  double cosd = cosDip();
//  Go through the parameters
  switch (idirect) {
  case theta1:
    ddflct[tanDipIndex()][0] = 1.0/(cosd*cosd);
    ddflct[d0Index()][0] = 0.0;
    ddflct[phi0Index()][0] =  0.0;
    ddflct[z0Index()][0] = -fltlen/cosd;;
    break;
  case theta2:
    ddflct[tanDipIndex()][0] = 0;
    ddflct[d0Index()][0] = -fltlen;
    ddflct[phi0Index()][0] = 1.0/cosd;
    ddflct[z0Index()][0] =  -(tanDip()/cosd)*d0();
    break;
  }
  return ddflct;
}

HepMatrix
TrkDifLineTraj::derivDisplace(double fltlen,deflectDirection idirect) const
{
//  This function computes the column matrix of derivatives for the change
//  in parameters for a change in the position of a track at a point along
//  its flight, holding the momentum and direction constant.  The effects for
//  changes in 2 perpendicular directions (theta1 = dip and
//  theta2 = phi*cos(dip)) are uncorrelated.

  HepMatrix ddflct(nLinPrm(),1);
//  Compute some common things
  double cosd = cosDip();
//  Go through the parameters
  switch (idirect) {
  case theta1:
    ddflct[tanDipIndex()][0] = 0.0;
    ddflct[d0Index()][0] = 0.0;
    ddflct[phi0Index()][0] =  0.0;
    ddflct[z0Index()][0] = 1.0/cosd;
    break;
  case theta2:
    ddflct[tanDipIndex()][0] = 0;
    ddflct[d0Index()][0] = 1.0;
    ddflct[phi0Index()][0] = 0.0;
    ddflct[z0Index()][0] =  0.0;
    break;
  }
  return ddflct;
}

HepMatrix
TrkDifLineTraj::derivPFract(double) const
{
//  This function computes the column matrix of derrivatives for the change
//  in parameters from a (fractional) change in the track momentum,
//  holding the direction and position constant.  The momentum change can
//  come from energy loss or bfield inhomogeneities.  Line Trajs have no
//  geometric change from energy loss.

  return HepMatrix(nLinPrm(),1, 0);
}

void
TrkDifLineTraj::paramFunc(const HepPoint3D& oldpoint,const HepPoint3D& newpoint,
                          const HepVector& oldvec,const HepSymMatrix& oldcov,
                          HepVector& newvec,HepSymMatrix& newcov,
                          double fltlen)
{
// not yet implemented
  std::cout<<"ErrMsg(fatal)" << "TrkDifLineTraj::paramFunc() is not implemented!" << std::endl;
}

void
TrkDifLineTraj::invertParams(TrkParams* params, std::vector<bool>& flags) const
{
  // Inverts parameters and returns true if the parameter inversion
  // requires a change in sign of elements in the covariance matrix
  for (unsigned iparam = 0; iparam < NLINPRM; iparam++) {
    switch ( iparam ) {
    case d0Ind:  // changes sign
    case tanDipInd:  // changes sign
      params->parameter()[iparam] *= -1.0;
      flags[iparam] = true;
      break;
    case phi0Ind:  // changes by pi, but covariance matrix shouldn't change
      params->parameter()[iparam] =
        BesAngle(params->parameter()[iparam] + Constants::pi);
      flags[iparam] = false;
      break;
    case z0Ind:  // no change
      flags[iparam] = false;
    }
  }
  return;
}

void
TrkDifLineTraj::visitAccept(TrkVisitor* vis) const
{
// Visitor access--just use the TrkVisitor class member function
  vis->trkVisitLineTraj(this);
}
