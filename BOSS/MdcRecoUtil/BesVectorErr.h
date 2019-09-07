//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: BesVectorErr.h,v 1.5 2010/03/25 09:55:57 zhangy Exp $
//
// Description:
//	Add errors to a vector.  Used for direction errors 
//      BaBar native class
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Forest Rouse	                February 1996
//      Victoria Novotny                August   1996
//      Ed Frank        University of Pennsylvania, efrank@upenn5.hep.upenn.edu
//
// History
//      14 Oct 96  Ed Frank      Simple mod to make unary - compile under
//                               Sun's CC 4.0.1
//      19 Jan 2002  Sasha Telnov  Added operator * (scaling by a real number)
//
// Copyright Information:
//	Copyright (C) 1996
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef BESVECTORERR_H
#define BESVECTORERR_H

#include <iosfwd>
#include "MdcRecoUtil/BesError.h"
#include "CLHEP/Vector/Rotation.h"

using namespace CLHEP;


class BesVectorErr : public Hep3Vector {
  
public:
  // polar coordinates
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
  // argumentless constructor:
  BesVectorErr() : Hep3Vector(), _covMatrix(NUM_COORDINATES) {}
  
  // auto casting constructor
  BesVectorErr(const Hep3Vector &p) : Hep3Vector(p), _covMatrix(NUM_COORDINATES)	{}
  BesVectorErr(const Hep3Vector &p, const BesError& covMat) : Hep3Vector(p),
  _covMatrix(NUM_COORDINATES)				{ _covMatrix=covMat; }

  // copy constructor:
  BesVectorErr(const BesVectorErr& v) : Hep3Vector(v),
    _covMatrix(v.covMatrix())	{}

  // destructor MAY be needed later
  // virtual ~BesVectorErr() {};

  // assignment operator:
  BesVectorErr& operator=(const BesVectorErr& v)
    {
      if (this != &v) {
	Hep3Vector::operator=(v);
	_covMatrix = v.covMatrix();
      }
      return *this;
    }

  BesVectorErr operator - () {
      Hep3Vector t = *this;
      return BesVectorErr( -t, _covMatrix);  // _covMatrix remains unaltered
  }

  BesVectorErr& operator += (const BesVectorErr& v){
      Hep3Vector::operator+=(v);
      _covMatrix += v.covMatrix();
      return *this;
  }
  
  BesVectorErr& operator -= (const BesVectorErr& v){
      Hep3Vector::operator-=(v);
      _covMatrix += v.covMatrix();
      return *this;
  }
/*
  BesVectorErr& transform(const HepTranslation& trans){
      Hep3Vector::transform(trans);
      return *this;
  }

  BesVectorErr& transform(const HepRotation& rot){
      Hep3Vector::transform(rot);
      _covMatrix = _covMatrix.similarity(rot);
      return *this;
  }

  BesVectorErr& transform(const HepTransformation& transf){
      Hep3Vector::transform(transf);
      _covMatrix = _covMatrix.similarity(transf.rot_mat());
      return *this;
  }
*/
  double determineChisq(const Hep3Vector& refVector) const;   
  // returns Chisquare
  // refVector refers to the same origin as the Hep3Vector of this
  // ie refVector is not relative to this Vector


  inline const BesError & covMatrix() const		{ return _covMatrix; }

  BesError covRTPMatrix() const;
  // returns the covariance Matrix in spherical coordinate
  // use   PolarCoordinateIndex enum to get the components

  BesError covRZPMatrix() const;
  // returns the covariance Matrix in cylindrical coordinate
  // use   CylindricalCoordinateIndex enum to get the components

  inline void setCovMatrix(const BesError& v)		{ _covMatrix = v; }

//  void printOn(ostream& out=cout) const;

private:
  
  BesError _covMatrix;
};


BesVectorErr operator + (const BesVectorErr&, const BesVectorErr&);

BesVectorErr operator - (const BesVectorErr&, const BesVectorErr&);

// Added by Sasha Telnov
BesVectorErr operator * (const BesVectorErr &, double a);
BesVectorErr operator * (double a, const BesVectorErr &);

std::ostream & operator<<(std::ostream & stream, const BesVectorErr & verr);
std::istream & operator>>(std::istream & stream, BesVectorErr & verr);

#endif






