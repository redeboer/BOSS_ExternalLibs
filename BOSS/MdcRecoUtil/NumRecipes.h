//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: NumRecipes.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      A collection of commonly used algorithms from "Numerical Recipes"
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Bob Jacobsen, Ed Iskander
//
// Copyright Information:
//	Copyright (C) 1996
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef NUMRECIPES_H
#define NUMRECIPES_H

#define NUMREC_ITMAX 100
#define NUMREC_EPS 3.0e-7

//-----------------
//-----------------
class NumRecipes {
public:

  // log of gamma function
  static double gammln(double x);

  static double gammq(double a, double x);  

  static double gammp(double a, double x);
  
  static void gcf(double* gammcf, double a, double x, double* gln);

  static void gser(double* gamser, double a, double x, double* gln);

private:
  static void recipesErr(const char* c);
};

#endif
