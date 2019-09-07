//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkCircleTraj.cxx,v 1.4 2006/03/31 07:29:23 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include "TrkFitter/TrkCircleTraj.h"
#include "TrkBase/TrkVisitor.h"
#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifPoint.h"
#include "MdcRecoUtil/DifVector.h"
#include "TrkBase/TrkExchangePar.h"
//
//  Fix for some machines
//
#ifndef M_2PI
#define M_2PI 2*M_PI
#endif
using CLHEP::Hep3Vector;
using CLHEP::HepSymMatrix;

//-----------------------------------------------------------------
TrkCircleTraj::TrkCircleTraj(const HepVector& pvec, const HepSymMatrix& pcov, 
    		     double lowlim, double hilim, const HepPoint3D& refpoint) :
  TrkSimpTraj(pvec, pcov, lowlim,hilim,refpoint)
//-----------------------------------------------------------------
{ 
  //  Make sure the dimensions of the input matrix and vector are correct
  if( pvec.num_row() != nCirPrm() ||
      pcov.num_row() != nCirPrm()  ){
    std::cout<<"ErrMsg(fatal)" << 
      "CircleTraj: incorrect constructor vector/matrix dimension" << std::endl;
  }

  if (omega() == 0.0) parameters()->parameter()[omegaIndex()] = 1.e-9; 
}


//-----------------------------------------------------------------
TrkCircleTraj::TrkCircleTraj(const TrkExchangePar& inpar,
    		     double lowlim, double hilim, const HepPoint3D& refpoint) :
  TrkSimpTraj(inpar.params(), inpar.covariance(), lowlim,hilim,refpoint) {
//-----------------------------------------------------------------
 
  if (omega() == 0.0) parameters()->parameter()[omegaIndex()] = 1.e-9;   
}

TrkCircleTraj::TrkCircleTraj(const TrkCircleTraj& h )
  : TrkSimpTraj(h.parameters()->parameter(), h.parameters()->covariance(),
                h.lowRange(),h.hiRange(),h.referencePoint())
{ 
	
}

TrkCircleTraj*
TrkCircleTraj::clone() const
{
  return new TrkCircleTraj(*this);
}

TrkCircleTraj&
TrkCircleTraj::operator=(const TrkCircleTraj& h)
{
  if( &h != this ){
    Trajectory::operator=(h);
    _dtparams = *(h.parameters());
    _refpoint = h._refpoint;
  }
  return *this;
}

TrkCircleTraj::~TrkCircleTraj()
{
}

double
TrkCircleTraj::x( const double& f ) const
{
  double sphi0 = sin(phi0());
  return ( sin(angle(f))-sphi0)/omega() - d0()*sphi0 + referencePoint().x();
}

double
TrkCircleTraj::y( const double& f ) const
{
  double cphi0 = cos(phi0());
  return -(cos(angle(f))-cphi0)/omega() + d0()*cphi0 + referencePoint().y();
}

HepPoint3D
TrkCircleTraj::position( double f) const
{
  double sphi0 = sin(phi0());
  double cphi0 = cos(phi0());
  double ang = angle(f);
  double xt =  ( sin(ang)-sphi0)/omega() - d0()*sphi0 + referencePoint().x();
  double yt =  -(cos(ang)-cphi0)/omega() + d0()*cphi0 + referencePoint().y();
  return HepPoint3D(xt, yt, referencePoint().z());
}

Hep3Vector
TrkCircleTraj::direction( double f) const
{
  // Angle formed by tangent vector after
  // being rotated 'arclength' around orbit.
  double alpha = angle( f );
  // Construct 3-D tangent vector of unit magnitude.
  return Hep3Vector ( cos(alpha),
		      sin(alpha),
		             0.0);
}

Hep3Vector
TrkCircleTraj::delDirect( double fltLen ) const
{
  double delX = -omega() * sin(angle(fltLen));
  double delY =  omega() * cos(angle(fltLen));
  return Hep3Vector(delX, delY, 0.0);
}

double
TrkCircleTraj::distTo1stError(double , double tol, int) const
{
  double arg = 2. * tol / fabs(omega());
  assert (arg >= 0.);
  return sqrt(arg);
}

double
TrkCircleTraj::distTo2ndError(double , double tol, int) const
{
  //return pow(6.*tol / sqr(omega()), 0.33333333);//yzhang changed sqr
  return pow(6.*tol / (omega()*omega()), 0.33333333);
}

void
TrkCircleTraj::getInfo(double flt, HepPoint3D& pos, Hep3Vector& dir,
		   Hep3Vector& delDir) const
{
  double sphi0 = sin(phi0());
  double cphi0 = cos(phi0());
  double ang = angle(flt);
  double cang = cos(ang);
  double sang = sin(ang);
  double xt =   (sang-sphi0)/omega() - d0()*sphi0 + referencePoint().x();
  double yt =  -(cang-cphi0)/omega() + d0()*cphi0 + referencePoint().y();

  pos.setX(xt);
  pos.setY(yt);
  pos.setZ(referencePoint().z());

  dir.setX(cang);
  dir.setY(sang);
  dir.setZ(0.0);

  delDir.setX(-omega()*sang);
  delDir.setY( omega()*cang);
  delDir.setZ(0.);
}

void
TrkCircleTraj::getInfo( double fltLen, HepPoint3D& pos, Hep3Vector& dir ) const
{
  double sphi0 = sin(phi0());
  double cphi0 = cos(phi0());
  double ang = angle(fltLen);
  double cang = cos(ang);
  double sang = sin(ang);
  double xt =   (sang-sphi0)/omega() - d0()*sphi0 + referencePoint().x();
  double yt =  -(cang-cphi0)/omega() + d0()*cphi0 + referencePoint().y();

  pos.setX(xt);
  pos.setY(yt);
  pos.setZ(referencePoint().z());

  dir.setX(cang);
  dir.setY(sang);
  dir.setZ(0.0);
}

void
TrkCircleTraj::getDFInfo2(double fltLen, DifPoint& pos, DifVector&
			 dir) const
{
  //Provides difNum version of information for calculation of derivatives.

  // Create difNumber versions of parameters
  DifNumber phi0Df(phi0(), phi0Index()+1, nCirPrm());
  DifNumber d0Df(d0(), d0Index()+1, nCirPrm());
  DifNumber omegaDf(omega(), omegaIndex()+1, nCirPrm());
  phi0Df.setIndepPar( parameters() );
  d0Df.setIndepPar( parameters() );
  omegaDf.setIndepPar( parameters() );

  DifNumber sinPhi0, cosPhi0;
  phi0Df.cosAndSin(cosPhi0, sinPhi0);

  DifNumber alphaDf = omegaDf;
  alphaDf *= fltLen;
  alphaDf += phi0Df;

  // This is not the prettiest line imaginable for this operation:
  alphaDf.mod(-Constants::pi, Constants::pi);
  DifNumber sinAlpha, cosAlpha;
  alphaDf.cosAndSin(cosAlpha, sinAlpha);

  //  DifNumber x =  (sinAlpha - sinPhi0) / omegaDf - d0Df * sinPhi0 + px;
  //  DifNumber y = -(cosAlpha - cosPhi0) / omegaDf + d0Df * cosPhi0 + py;

  DifNumber x = sinAlpha;
  x -= sinPhi0;
  x /= omegaDf;
  x -= (d0Df * sinPhi0);

  DifNumber y = -cosAlpha;
  y += cosPhi0;
  y /= omegaDf;
  y += (d0Df * cosPhi0);


  static DifNumber zNull(0.);

  pos.x = x;
  pos.y = y;
  pos.z = zNull;

  bool lref = (referencePoint().x() != 0. || referencePoint().y() != 0. ||
	       referencePoint().z() != 0.);
  if (lref) {
    DifNumber px(referencePoint().x());
    DifNumber py(referencePoint().y());
    DifNumber pz(referencePoint().z());
    pos.x += px;
    pos.y += py;
    pos.z += pz;
  }

  dir.x = cosAlpha;
  dir.y = sinAlpha;
  dir.z = 0.;
}

void
TrkCircleTraj::getDFInfo(double flt, DifPoint& pos, DifVector& dir,
		   DifVector& delDir) const
{
  //Provides difNum version of information for calculation of derivatives.

  // Create difNumber versions of parameters
  DifNumber phi0Df(phi0(), phi0Index()+1, nCirPrm());
  DifNumber d0Df(d0(), d0Index()+1, nCirPrm());
  DifNumber omegaDf(omega(), omegaIndex()+1, nCirPrm());
  phi0Df.setIndepPar( parameters() );
  d0Df.setIndepPar( parameters() );
  omegaDf.setIndepPar( parameters() );

  DifNumber sinPhi0, cosPhi0;
  phi0Df.cosAndSin(cosPhi0, sinPhi0);

  DifNumber alphaDf = omegaDf;
  alphaDf *= flt;
  alphaDf += phi0Df;

  // This is not the prettiest line imaginable for this operation:
  alphaDf.mod(-Constants::pi, Constants::pi);
  DifNumber sinAlpha, cosAlpha;
  alphaDf.cosAndSin(cosAlpha, sinAlpha);

  //  DifNumber x =  (sinAlpha - sinPhi0) / omegaDf - d0Df * sinPhi0 + px;
  //  DifNumber y = -(cosAlpha - cosPhi0) / omegaDf + d0Df * cosPhi0 + py;

  DifNumber x = sinAlpha;
  x -= sinPhi0;
  x /= omegaDf;
  x -= (d0Df * sinPhi0);

  DifNumber y = -cosAlpha;
  y += cosPhi0;
  y /= omegaDf;
  y += (d0Df * cosPhi0);


  static DifNumber zNull(0.);

  pos.x = x;
  pos.y = y;
  pos.z = zNull;

  bool lref = (referencePoint().x() != 0. || referencePoint().y() != 0. ||
	       referencePoint().z() != 0.);
  if (lref) {
    DifNumber px(referencePoint().x());
    DifNumber py(referencePoint().y());
    DifNumber pz(referencePoint().z());
    pos.x += px;
    pos.y += py;
    pos.z += pz;
  }

  dir.x = cosAlpha;
  dir.y = sinAlpha;
  dir.z = 0.;

  delDir.x = -omegaDf;
  delDir.x *= sinAlpha;

  delDir.y = omegaDf;
  delDir.y *= cosAlpha;

  delDir.z = 0.;
}
HepMatrix
TrkCircleTraj::derivDeflect(double fltlen,deflectDirection idirect) const
{
//  This function computes the column matrix of derivatives for the change
//  in parameters for a change in the direction of a track at a point along
//  its flight, holding the momentum and position constant.  The effects for
//  changes in 2 perpendicular directions (theta1 = dip and
//  theta2 = phi*cos(dip)) can sometimes be added, as scattering in these
//  are uncorrelated.

  HepMatrix ddflct(nCirPrm(),1,0); // initialize with zeros

//  Compute some common things

  double omeg = omega();
  double arcl = arc(fltlen);
  double dx = cos(arcl);
  double dy = sin(arcl);
  double darc = omeg*d0();

//  Go through the parameters

  switch (idirect) {
  case theta1:
    break;
  case theta2:
    ddflct[d0Index()][0] = -dy/(omeg);
    ddflct[phi0Index()][0] = dx/(1+darc);
  }

  return ddflct;
}

HepMatrix
TrkCircleTraj::derivDisplace(double fltlen,deflectDirection idirect) const
{
//  This function computes the column matrix of derivatives for the change
//  in parameters for a change in the position of a track at a point along
//  its flight, holding the momentum and direction constant.  The effects for
//  changes in 2 perpendicular directions (theta1 = dip and
//  theta2 = phi*cos(dip)) can sometimes be added, as scattering in these
//  are uncorrelated.

  HepMatrix ddflct(nCirPrm(),1,0); // initialize with zeros

//  Compute some common things

  double omeg = omega();
  double arcl = arc(fltlen);
  double dx = cos(arcl);
  double dy = sin(arcl);
  double darc = omeg*d0();

//  Go through the parameters

  switch (idirect) {
  case theta1:
    break;
  case theta2:
    ddflct[d0Index()][0] = dx;
    ddflct[phi0Index()][0] = dy*omeg/(1+darc);
  }

  return ddflct;
}

HepMatrix
TrkCircleTraj::derivPFract(double fltlen) const
{
//
//  This function computes the column matrix of derrivatives for the change
//  in parameters from a (fractional) change in the track momentum,
//  holding the direction and position constant.  The momentum change can
//  come from energy loss or bfield inhomogeneities.
//
//  For a helix, dp/P = -domega/omega,
//  dParam/d(domega/omega) = omega*dParam/ddomega

  HepMatrix dmomfrac(nCirPrm(),1);

//  Compute some common things

  double omeg = omega();
  double arcl = arc(fltlen);
  double dx = cos(arcl);
  double dy = sin(arcl);
  double darc = omeg*d0();

//  Go through the parameters
// omega
  dmomfrac[omegaIndex()][0] = -omeg;
// d0
  dmomfrac[d0Index()][0] = -(1-dx)/omeg;
// phi0
  dmomfrac[phi0Index()][0] = dy/(1+darc);

  return dmomfrac;
}

double
TrkCircleTraj::curvature(double ) const
{
//  Compute the curvature as the magnitude of the 2nd derrivative
//  of the position function with respect to the 3-d flight distance

  return fabs(omega());
}

double
TrkCircleTraj::phi0() const
{
 return BesAngle(parameters()->parameter()[phi0Index()]).rad();
}

void
TrkCircleTraj::paramFunc(const HepPoint3D& oldpoint,const HepPoint3D& newpoint,
			 const HepVector& oldvec,const HepSymMatrix& oldcov,
			 HepVector& newvec,HepSymMatrix& newcov,
			 double fltlen)
{
// copy the input parameter vector, in case the input and output are the same
  HepVector parvec(oldvec);
// start with the input: omega doesn't change
  newvec = parvec;
//
  double delx = newpoint.x()-oldpoint.x();
  double dely = newpoint.y()-oldpoint.y();
//
  double rad = 1./parvec[omegaIndex()];
  double rad2 = rad*rad;
  double delta = rad + parvec[d0Index()];
  double cos0 = cos(parvec[phi0Index()]);
  double sin0 = sin(parvec[phi0Index()]);
  double perp = delx*sin0-dely*cos0;
  double para = delx*cos0+dely*sin0;
  double oldphi  = parvec[phi0Index()] + fltlen*parvec[omegaIndex()] ;
// delta
  double newdelta2 = delta*delta + delx*delx + dely*dely +
    2.0*delta*perp;
// assume delta, newdelta have the same sign
  double newdelta = delta>0 ? sqrt(newdelta2) : -sqrt(newdelta2);
  double invdelta = 1.0/newdelta;
  double invdelta2 = 1.0/newdelta2;
// d0
  newvec[d0Index()] = newdelta - rad;
// phi0; check that we don't get the wrong wrapping
  double newphi = atan2(sin0+delx/delta,cos0-dely/delta);
  while(fabs(newphi - oldphi)>M_PI)
    if(newphi > oldphi)
      newphi -= M_2PI;
    else
      newphi += M_2PI;
  newvec[phi0Index()] = newphi;
  //double delphi = newphi-parvec[phi0Index()];
// now covariance: first, compute the rotation matrix
  HepMatrix covrot(nCirPrm(),nCirPrm(),0); // start with 0: lots of terms are 0
//
// omega is diagonal
  covrot[omegaIndex()][omegaIndex()] = 1.0;
// d0
  covrot[d0Index()][omegaIndex()] = rad2*(1.0 - invdelta*(delta + perp));
  covrot[d0Index()][d0Index()] = invdelta*(delta + perp);
  covrot[d0Index()][phi0Index()] = delta*para*invdelta;
// phi0
  covrot[phi0Index()][omegaIndex()] = rad2*para*invdelta2;
  covrot[phi0Index()][d0Index()] = -para*invdelta2;
  covrot[phi0Index()][phi0Index()] = delta*(delta + perp)*invdelta2;
//
//  Apply the rotation
  newcov = oldcov.similarity(covrot);
// done
}

void
TrkCircleTraj::invertParams(TrkParams* params, std::vector<bool>& flags) const
{
  // Inverts parameters and returns true if the parameter inversion
  // requires a change in sign of elements in the covariance matrix

  for (unsigned iparam = 0; iparam < NCIRPAR; iparam++) {
    switch ( iparam ) {
    case d0Ind:  // changes sign
    case omegaInd:  // changes sign
      params->parameter()[iparam] *= -1.0;
      flags[iparam] = true;
      break;
    case phi0Ind:  // changes by pi, but covariance matrix shouldn't change
      params->parameter()[iparam] =
        BesAngle(params->parameter()[iparam] + Constants::pi);
      flags[iparam] = false;
    }
  }
}
//yzhang
/*int
TrkCircleTraj::nPar() const
{
 return NCIRPAR;
}*/	
//zhangy
void
TrkCircleTraj::visitAccept(TrkVisitor* vis) const
{
// Visitor access--just use the TrkVisitor class member function
  vis->trkVisitCircleTraj(this);
}

double
TrkCircleTraj::angle(const double& f) const
{
  return BesAngle(phi0() + arc(f));
}
