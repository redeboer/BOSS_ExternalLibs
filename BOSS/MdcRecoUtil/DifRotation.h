//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifRotation.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Header for |DifRotation|
//      Rotation matrix and rotate things
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//
// Copyright Information:
//	Copyright (C) 1996	SLAC
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef DifRotation_H
#define DifRotation_H

#include "MdcRecoUtil/DifVector.h"

class DifRotation {

public:

  //constructors
  DifRotation();		// unit matrix 
  DifRotation			// construct from euler
  (const DifNumber& alpha,
   const DifNumber& beta,
   const DifNumber& gamma);

  DifRotation			// constructionr from axes
  (const DifVector& xp,const DifVector& yp,const DifVector& zp);
  DifRotation			// z=x X y
  (const DifVector& xp,const DifVector& yp);

  ~DifRotation() {};

  //access
  inline DifVector xnew()const {return _xnew;}
  inline DifVector ynew()const {return _ynew;}
  inline DifVector znew()const {return _znew;}

  //rotate a vector
  void rotate(DifVector& v)const;

  //error check
  int fail()const;		// check for orthonormality failure

private:

  //data - store as vector
  DifVector _xnew;
  DifVector _ynew;
  DifVector _znew;
};

#endif
