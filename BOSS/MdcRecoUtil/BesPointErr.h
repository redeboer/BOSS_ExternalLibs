//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: BesPointErr.h,v 1.4 2010/03/25 09:55:57 zhangy Exp $
//
// Description:
//      Endow a Point class with a covariance matrix
//      
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Forest Rouse
//      Victoria Novotny  
//      Ed Frank             University of Penn., efrank@upenn5.hep.upenn.edu
//
// History
//      14 Oct 96 Ed Frank   Simple mod to make unary - compile under
//                           native SunOS5 compiler.
//
// Copyright Information:
//	Copyright (C) 1996 U.C. Davis
// 
// History:
//	Migration for BESIII MDC
//	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//	Zhang Yao(zhangyao@ihep.ac.cn)  Upgrade to CLHEP2.0.4.5 2010-03-25 
//
//------------------------------------------------------------------------
#ifndef BESPOINTERR_H
#define BESPOINTERR_H

#include "MdcRecoUtil/BesVectorErr.h"
#include "MdcRecoUtil/BesError.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace CLHEP;


class BesPointErr : public HepPoint3D
{
  public:
    enum PolarCoordinateIndex {   
      Rho = 0, 
      Theta = 1, 
      Phi = 2,
      NUM_PCOORDINATES = 3
    };
    enum CylindricalCoordinateIndex {   
      C_Rho = 0, 
      C_Zeta = 1, 
      C_Phi = 2,
      NUM_CCOORDINATES = 3
    };
    BesPointErr() : HepPoint3D(), _covMatrix(3)		{}
    BesPointErr(const HepPoint3D &p) : HepPoint3D(p),
				       _covMatrix(3)						{}
    BesPointErr(const HepPoint3D &p, const BesError& covMat) : HepPoint3D(p),
							       _covMatrix(3)						{ _covMatrix=covMat; }

    // new constructors for this class
    BesPointErr(const BesPointErr& v) : HepPoint3D(),_covMatrix(3)	{*this = v;}

    double determineChisq(const HepPoint3D& diffPoint) const
    {
      HepVector temp(3);
      temp[0] = diffPoint.x()-this->x();
      temp[1] = diffPoint.y()-this->y();
      temp[2] = diffPoint.z()-this->z();
      return _covMatrix.determineChisq(temp);
    }

    BesError covRTPMatrix() const;
    // returns the covariance Matrix in spherical coordinate
    // use   PolarCoordinateIndex enum to get the components
    BesError covRZPMatrix() const;
    // returns the covariance Matrix in cylindrical coordinate
    // use   CylindricalCoordinateIndex enum to get the components

    inline const BesError& covMatrix() const		{ return _covMatrix; }
    inline void setCovMatrix(const BesError& v)		{ _covMatrix = v; }

    //  void printOn(ostream& out=cout) const;

  private:

    BesError _covMatrix;
};

BesPointErr operator + (const BesPointErr&, const BesVectorErr&);

BesPointErr operator + (const BesVectorErr&, const BesPointErr&);

BesPointErr operator - (const BesPointErr&, const BesVectorErr&);

BesVectorErr operator - (const BesPointErr&, const BesPointErr&);

std::ostream & operator<<(std::ostream & stream, const BesPointErr & verr);
std::istream & operator>>(std::istream & stream, BesPointErr & verr);


#endif





