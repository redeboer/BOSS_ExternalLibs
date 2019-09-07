//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixUtils.cxx,v 1.4 2007/12/07 07:04:14 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner, code stolen from TrkParms class
//
//------------------------------------------------------------------------
//#include "BaBar/BaBar.h"
#include "TrkBase/TrkHelixUtils.h"
#include "TrkBase/NeutParams.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <math.h>
#include "TrkBase/TrkExchangePar.h"
#include "BField/BField.h"
#include "MdcGeom/BesAngle.h"
#include "MdcRecoUtil/BesPointErr.h"
#include "MdcRecoUtil/BesVectorErr.h"
#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifArray.h"
#include "MdcGeom/Constants.h"
const double pi = Constants::pi;

//------------------------------------------------------------------------
HepMatrix TrkHelixUtils::jacobianExtrapolate(const TrkExchangePar& par, 
					  double fltNew) {
//------------------------------------------------------------------------
  
//----------------------------------------------------------------------
//      Compute and return the jacobian that takes the covariance matrix
//	from fltOld to fltNew
//
//      "fltLen" -- the signed 3-d pathlength a particle travels 
//                     along the orbit starting from the point on the orbit 
//                     close to the origin.
//
//      Each element in this matrix is a partial derivative of an orbit
//      parameter at some reference point to an orbit parameter at
//      the fit point.  What is kept fixed in taking these partial 
//      derivatives is that the orbit parameters are those at the point
//      on the orbit that is closest in the x-y plane to the reference point.
//      This transform matrix has the property that transform(-arclength) 
//      is the inverse of transform(arclength).
//      (repaired by Gerry Lynch, I think -- sfs)
//----------------------------------------------------------------------

  HepMatrix transform(5, 5, 0);

  double tanDip = par.tanDip();
  double omega = par.omega();
  // Convert to 2-d arclength
  double darc = fltNew / sqrt(1. + tanDip * tanDip);
  double dphi    = omega * darc;
  double cosDphi = cos(dphi);
  double sinDphi = sin(dphi);

  // partials of d0
 
  transform[0][0] = cosDphi;
  transform[0][1] = sinDphi / omega;
  transform[0][2] = (1.0-cosDphi) / (omega * omega);

  // partials of phi0  

  transform[1][0] = -sinDphi * omega;
  transform[1][1] =  cosDphi;
  transform[1][2] =  sinDphi / omega;

  // partials of omega  

  transform[2][2] = 1.0;

  // partials of z0 
 
  transform[3][0] = -tanDip * sinDphi;
  transform[3][1] = -tanDip * (1.0-cosDphi) / omega;
  transform[3][2] = -tanDip * (dphi-sinDphi) / (omega*omega);
  transform[3][3] =  1.;
  transform[3][4] =  darc;

  // partials of tanDip

  transform[4][4] = 1.;

  return transform;
}


//----------------------------------------------------------------------
HepSymMatrix TrkHelixUtils::extrapolateCov(TrkExchangePar& par, 
					   double fltNew) {
//----------------------------------------------------------------------

  return par.covariance().similarity(jacobianExtrapolate(par, fltNew));  
}

//----------------------------------------------------------------------
TrkExchangePar TrkHelixUtils::helixFromMom(const HepPoint3D& pos, 
       	      const Hep3Vector& pmom, double sign, const BField& fieldmap) {
//----------------------------------------------------------------------
  //As documented in 
  //R.~Luchsinger and C.~Grab, Comp.~Phys.~Comm.~{\bf 76}, 263-280 (1993).
  //Equation 14 on page 270.
  //Note: We use the opposite sign for d0.
  //We use tandip and not the dip angle itself.
  
  register double phip,gamma,rho,pt;
  static HepVector pars(5);
  
  double px = pmom.x();
  double py = pmom.y();
  pt=sqrt(px*px+py*py);

  if (pt < 1.e-7)   pt = 1.e-7;  // hack to avoid pt=0 tracks
  if (fabs(px) < 1.e-7) px = (px<0.0) ? -1.e-7 : 1.e-7; // hack to avoid pt=0 tracks
  
  double Bval = fieldmap.bFieldNominal();
  
  pars(3)=-BField::cmTeslaToGeVc*Bval*sign/pt;  //omega
  pars(5)=pmom.z()/pt;  //tandip
  
  rho=1./pars(3);
  phip=atan2(py,px);
  double cosphip=cos(phip); double sinphip=sin(phip);
  
  gamma=atan((pos.x()*cosphip+pos.y()*sinphip)/
	     (pos.x()*sinphip-pos.y()*cosphip-rho)); // NOTE: do NOT use atan2 here!

  
  pars(2)=BesAngle(phip+gamma).rad();  //phi0
  pars(1)=(rho+pos.y()*cosphip-pos.x()*sinphip)/cos(gamma)-rho;  //d0
  pars(4)=pos.z()+rho*gamma*pars(5);  //z0
  
  return TrkExchangePar(pars);
}

//----------------------------------------------------------------------
TrkExchangePar TrkHelixUtils::helixFromMomErr(const BesPointErr& pos,
              const BesVectorErr& pmom,const HepMatrix& cxp, double sign, 
              const BField& fieldmap) {
//----------------------------------------------------------------------

  DifNumber xDF(pos.x(),1,6), yDF(pos.y(),2,6), zDF(pos.z(),3,6);
  DifNumber pxDF(pmom.x(), 4, 6);
  DifNumber pyDF(pmom.y(), 5, 6);
  DifNumber pzDF(pmom.z(), 6, 6);

  static DifNumber phip, cosphip, sinphip, gamma;
  static DifArray pars(5,6);

  DifNumber invpt = pxDF;
  invpt *= pxDF;
  invpt += pyDF*pyDF;
  invpt = sqrt(invpt);

  if (invpt < 1.e-7) invpt = 1.e-7;  // hack to avoid pt=0 tracks
  if (fabs(pxDF) < 1.e-7) pxDF = pxDF<0?-1e-7:1e-7; // hack to avoid pt=0 tracks
  invpt = 1./invpt;  
    
  //omega
  double Bval = fieldmap.bFieldNominal();
  //  if (fabs(Bval) < 1.e-7) {  // hack to avoid B=0 tracks (very far Z)
  //    Bval = 1.e-7;
  //  }

//  pars(3) = -BField::cmTeslaToGeVc*Bval*sign/pt;
  pars(3) = invpt;
  pars(3) *= sign;
  pars(3) *= Bval;
  pars(3) *= -BField::cmTeslaToGeVc;

//  pars(5) = pzDF / pt;  //tandip
  pars(5) = pzDF;
  pars(5) *= invpt;

  DifNumber rho = 1./pars[2];
  phip=atan2(pyDF,pxDF);
  phip.cosAndSin(cosphip,sinphip);

//  pars(1) = (rho + yDF*cosphip - xDF*sinphip)/cos(gamma) - rho;//d0
  pars(1) = rho;
  pars(1) += yDF*cosphip;
  pars(1) -= xDF*sinphip;  // continued below ...

  gamma=atan((xDF*cosphip+yDF*sinphip)/ -pars(1)); // NOTE: do NOT use atan2 here
    
  pars(1) /= cos(gamma);
  pars(1) -= rho;

//  pars(2) = phip+gamma;  //phi0
  pars(2) = phip;
  pars(2) += gamma;
  //  pars(2).mod(0., Constants::twoPi);


//  pars(4) = zDF + rho*gamma*pars[4];  //z0
  pars(4) = pars[4];  // weird
  pars(4) *= gamma;
  pars(4) *= rho;
  pars(4) += zDF;


// Get error matrix for position and momentum

  static HepSymMatrix posandmomErr(6);
  static HepVector parsVec(5);

  int i;
  for (i = 1; i <= 3; ++i) {
    int j;
    for (j = 1; j <= i; ++j) {
      // with "fast" access, row should be >= col
      posandmomErr.fast(i,j) = pos.covMatrix().fast(i,j);
      posandmomErr.fast(i+3,j+3) = pmom.covMatrix().fast(i,j);
    }
    for (j = 1; j <= 3; ++j) {
      posandmomErr.fast(j+3,i) = cxp(i,j);
    }
  }
  for (i = 1; i <= 5; ++i) {
 // make the array of DifNums into a HepVector
 // (needed for TrkExchangePar init)
    parsVec(i) = pars(i).number();
  }
// Now calculate error on the helix pars--the real calculation

  return TrkExchangePar(parsVec,posandmomErr.similarity(pars.jacobian()) );
}
//----------------------------------------------------------------------
NeutParams TrkHelixUtils::lineFromMomErr(const BesPointErr& pos,
              const BesVectorErr& pmom,const HepMatrix& cxp, double sign, 
              const BField& fieldmap) {
//----------------------------------------------------------------------

  DifNumber xDF(pos.x(),1,6), yDF(pos.y(),2,6), zDF(pos.z(),3,6);
  DifNumber pxDF(pmom.x(), 4, 6);
  DifNumber pyDF(pmom.y(), 5, 6);
  DifNumber pzDF(pmom.z(), 6, 6);

  static DifArray pars(6,6);

  DifNumber pt = pxDF;
  pt *= pxDF;
  pt += pyDF*pyDF;

  if (pt < 1.e-14) pt = 1.e-14;               // hack to avoid pt=0 tracks
  if (fabs(pxDF) < 1.e-7) pxDF = pxDF<0?-1e-7:1e-7; // hack to avoid pt=0 tracks

//  pars(3) = sqrt(pt*pt+pzDF*pzDF); // Magnitude of p 
  pars(3) = pzDF;
  pars(3) *= pzDF;
  pars(3) += pt;
  pars(3) = sqrt(pars(3));

  pt = sqrt(pt);
  DifNumber invpt = 1./pt;

  //Next lines modified by Eugenio Paoloni 19-XI-98

//  DifNumber pvxDF=pxDF/pt; // versor along pt x
  DifNumber pvxDF=pxDF;
  pvxDF *= invpt;

//  DifNumber pvyDF=pyDF/pt; // and y component
  DifNumber pvyDF=pyDF;
  pvyDF *= invpt;


//  pars(5) = pzDF / pt;  //tandip
  pars(5) = pzDF;
  pars(5) *= invpt;

  pars(2) = atan2(pyDF,pxDF);  //phi0

//  pars(1) = yDF*pvxDF - xDF*pvyDF;//d0
  pars(1) = yDF;
  pars(1) *= pvxDF;
  pars(1) -= xDF*pvyDF;

//  pars(4) = zDF -pars(5)*(xDF*pvxDF+yDF*pvyDF) ;  //z0
  pars(4) = xDF;
  pars(4) *= pvxDF;
  pars(4) += yDF*pvyDF;

// insert this in the middle for speed
//  pars(6) = (xDF*pvxDF+yDF*pvyDF)*pars(3)/pt;//s0
  pars(6) = pars(4);
  pars(6) *= pars(3);
  pars(6) *= invpt;


  pars(4) *= -pars(5);
  pars(4) += zDF;




// Get error matrix for position and momentum

  static HepSymMatrix posandmomErr(6);
  static HepVector parsVec(6);

  int i;
  for (i = 1; i <= 3; ++i) {
    int j;
    for (j = 1; j <= i; ++j) {
      // with "fast" access, row should be >= col
      posandmomErr.fast(i,j) = pos.covMatrix().fast(i,j);
      posandmomErr.fast(i+3,j+3) = pmom.covMatrix().fast(i,j);
    }
    for (j = 1; j <= 3; ++j) {
      posandmomErr.fast(j+3,i) = cxp(i,j);
    }
  }
  for (i = 1; i <= 6; ++i) {
 // make the array of DifNums into a HepVector
 // (needed for TrkExchangePar init)
    parsVec(i) = pars(i).number();
  }
// Now calculate error on the helix pars--the real calculation
  return NeutParams(parsVec,posandmomErr.similarity(pars.jacobian()) );
}

//------------------------------------------------------------------------
double TrkHelixUtils::fltToRad(const TrkExchangePar& hel, double rad) {
//------------------------------------------------------------------------
  double d0 = hel.d0();
  double omega = hel.omega();
  double tanDip = hel.tanDip();
  //std::cout<< "omega  "<<omega << std::endl;
  // GS To be able to use this with straight lines:  
//  if( fabs(omega) < 0.0001 ) omega=copysign(0.0001,omega);  // assume the pt= 1000 GeV  
  if( fabs(omega) < 0.0001 ) omega = (omega<0.0) ? -0.0001 : 0.0001 ;
  
  double stuff =  ( rad*rad - d0*d0 ) / (1 + omega * d0);
  if (stuff <= 0.0) return 0.;
  //std::cout<< "in TrkHelixUtils::fltToRad  "<< stuff << std::endl;
  if (omega == 0.) return sqrt(stuff);
  double sinAngle = 0.5 * omega * sqrt(stuff);
  double dist2d = 0;
  if (sinAngle < -1.0 || sinAngle > 1.0) {
    dist2d = Constants::pi / fabs(omega);
  } else {
    dist2d = 2. * asin(sinAngle) / omega;
  }
  //std::cout<< "in TrkHelixUtils::fltToRad  dist2d "<< dist2d << std::endl;
  return dist2d * sqrt(1. + tanDip*tanDip);
}
