//--------------------------------------------------------------------------
// File and Version Information:
//	$Id: BesError.h,v 1.5 2010/03/25 09:55:57 zhangy Exp $
//
// Description:
//      A wrapper for a covariance matrix.  A covariance matrix is
//	a symmetric n X n matrix.  Change in chisq from point
//	covariance matrix was determined is just
//
//	diff * covariance^-1 * diff_transpose
//
//	which is implemented in a similarity transform in HepSymMatrix
//	the method determineChisq carries this calculation out and requires
//	the result to be a scalar.
//      
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Forest Rouse                Jan 1996
//      Victoria Novotny            Aug 1996
//
// Copyright Information:
//	Copyright (C) 1996
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef BESERROR_HH
#define BESERROR_HH

#include <iostream>
#include <math.h>

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/LorentzRotation.h"


using namespace CLHEP;

class BesError : public HepSymMatrix {

public:

  static const double chisqUndef;
  BesError() : HepSymMatrix() {}

  BesError(int n) : HepSymMatrix(n, 0)				{}

  // autocast copy constructor.  HepSymMatrix's promoted back
  // into BesError matrices.

  BesError( const HepSymMatrix &p ) : HepSymMatrix(p)		{}

  // new constructors for this class
  BesError(const BesError& v) : HepSymMatrix() {*this = v;}

  BesError& operator=(const BesError& v)
    {
          if (this != &v) {
            HepSymMatrix::operator=(v);
      }
      return *this;
    }

  BesError& operator=(const HepSymMatrix& v)
    {
          if (this != &v) {
            HepSymMatrix::operator=(v);
      }
      return *this;
    }

  // destructor MAY be needed later
  // virtual ~BesError() {};

  //----------------------------------------------------------------------
  // determineChisq
  //	Compute v^T * V^(-1)*v - ie the chisq for this covariance
  //	matrix and the difference vector v.
  //----------------------------------------------------------------------

  double determineChisq(const HepVector& diff) const; 

  // Get right signature for all operations performed on BesError matrices
  // that should (and will) result in BesError matrices.  These include
  // similarity transforms, transpose, inverse, matrix multiplication,
  // addition, and subtraction.  HepSymMatrix's as a result of operations
  // are promoted back into BesError matrices if we start out
  // with BesError matrices in the first place. (See copy constructors)

  BesError& operator *= (double t)
    { return (BesError&) HepSymMatrix::operator*=(t); }

  BesError& operator /= (double t)
    {return (BesError&) HepSymMatrix::operator/=(t); }

  BesError& operator += (const BesError& m2)
    {
      HepSymMatrix::operator+=(m2);
      return *this;
    }

  BesError& operator -= (const BesError& m2)
    {
      HepSymMatrix::operator-=(m2);
      return *this;
    }

  BesError operator - ()
    { BesError temp(*this); 
      return temp; }
  // does nothing -- covariance Matrices have never negative entries on the 
  // main diagonal

  // Implement the signature for operators I also inherit
  BesError& operator += (const HepSymMatrix& m2)
    {
      HepSymMatrix::operator+=(m2);
      return *this;
    }

  BesError& operator -= (const HepSymMatrix& m2)
    {
      HepSymMatrix::operator-=(m2);
      return *this;
    }

  BesError operator - () const
    { BesError temp(*this); 
    return temp;}

  BesError& operator += (const HepDiagMatrix& m2)
    {
      HepSymMatrix::operator+=(m2);
      return *this;
    }

  BesError& operator -= (const HepDiagMatrix& m2)
    {
      HepSymMatrix::operator-=(m2);
      return *this;
    }

  BesError similarity(const HepRotation& rot) const;
  BesError similarity(const HepLorentzRotation& rot) const;
  // When feasible implement R * covMatrix * R_transpose (= R^-1)

  BesError similarity(const BesError& E);
  // implement E * covMatrix * E

  BesError similarity(const HepMatrix& m1) const
    {
      BesError mret(m1.num_row());
      mret.similarityWith(*this, m1);
      return mret;
    }

  BesError& similarityWith(const BesError& m, const HepMatrix& m1);

  // provide call ups to base classes similarity methods not implemented
  // here
  double similarity(const HepVector &v) const
       { return this->HepSymMatrix::similarity( v ); }
  HepSymMatrix similarity(const HepSymMatrix &m1) const
       { return this->HepSymMatrix::similarity( m1 ); }

private:
    
  friend BesError operator*(double t, const BesError& m1);

  friend BesError operator*(const BesError& m1, double t);

  friend BesError operator/(double t, const BesError& m1);

  friend BesError operator/(const BesError& m1, double t);

  friend BesError operator+(const BesError& m1, const BesError& m2);
 
  friend BesError operator-(const BesError& m1, const BesError& m2);
  
  friend std::ostream& operator<<(std::ostream& out, const BesError& mat);
  friend std::istream& operator>>(std::istream& in, BesError& mat);
  
};

#endif






