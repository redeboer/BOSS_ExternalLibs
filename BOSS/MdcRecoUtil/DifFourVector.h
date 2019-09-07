//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifFourVector.h,v 1.3 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Header for |DifVector|
//      A 4-vector based on differential numbers
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//
// Copyright Information:
//	Copyright (C) 2002	SLAC
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef DifFourVector_H
#define DifFourVector_H

#include <vector>

#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <iosfwd>
class DifRotation;
using CLHEP::Hep3Vector;
class DifFourVector {

public:

  //constructors
  DifFourVector();			// null - default
  DifFourVector			// construct from components
  (const DifNumber& mass,const DifVector& p);
  DifFourVector			// construct from components
  (const double& mass,const DifVector& p);
  
  DifFourVector(const DifFourVector& v); // copy

  //destructor
  ~DifFourVector() {}		// destroy

  //given error on parameters calculate error on vector
  HepSymMatrix errorMatrix		// caclulate error matrix
  (const HepSymMatrix& e)const;
  
  //give jacobian transformation matrix wrt independent parameters;
  HepMatrix jacobian()const;

  //operators 
  inline DifFourVector& operator+=(const DifFourVector& v);
  inline DifFourVector& operator-=(const DifFourVector& v);
  inline DifFourVector& operator=(const DifFourVector & v);
  inline DifFourVector operator-()const;

  inline friend DifFourVector operator+	// vector sum
  (const DifFourVector& a,const DifFourVector& b);
  inline friend DifFourVector operator-	// vector difference
  (const DifFourVector& a,const DifFourVector& b); 
  inline friend DifNumber operator*	// scalar product
  (const DifFourVector& a,const DifFourVector& b);



  //access 
  inline int nPar()const;	// return number of params
  inline DifVector direction()const;
  inline DifNumber pMag()const {return P.length();}
  inline DifNumber massSq()const {return E*E-P*P;}
  inline DifNumber mass()const {
    DifNumber temp=massSq();
    if(temp>=0) return sqrt(temp);
    return -sqrt(-massSq());
  }


  //i/o
  void print(/*std::ostream& o*/)const;	// print out


  //manipulations

  //misc
  inline DifFourVector& zeroDerivatives(); // zero derivatives
  

  //boost


  void boostTo(const DifFourVector&);
  void boostFrom(const DifFourVector&);

  void boostToMe
  (std::vector<DifFourVector*>& listToBoost)const;
  void boostFromMe
  (std::vector<DifFourVector*>& listToBoost)const;
    

  //algebra


  //data members - public .. yes, folks that's intentional!
public:

  // energy-momentum components of a 4-vector

  DifNumber E;			// energy-like component
  DifVector P;			// momentum-like compoent


};

//io 
/*SKIP
inline std::ostream& operator<<(std::ostream& o,const DifFourVector& n) {
  n.print(o);
  return o;
}
*/
#include "MdcRecoUtil/DifFourVector.icc"

#endif









