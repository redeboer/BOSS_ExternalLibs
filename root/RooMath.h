/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooMath.h,v 1.16 2007/05/11 09:11:30 verkerke Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_MATH
#define ROO_MATH

#include <cmath>
#include <complex>

#include "RooComplex.h"

#include <fstream>

typedef RooComplex* pRooComplex ;
typedef Double_t* pDouble_t ;

class RooMath {
public:

  virtual ~RooMath() {} ;

  /** @brief evaluate Faddeeva function for complex argument
   *
   * @author Manuel Schiller <manuel.schiller@nikhef.nl>
   * @date 2013-02-21
   *
   * Calculate the value of the Faddeeva function @f$w(z) = \exp(-z^2)
   * \mathrm{erfc}(-i z)@f$.
   *
   * The method described in
   *
   * S.M. Abrarov, B.M. Quine: "Efficient algotithmic implementation of
   * Voigt/complex error function based on exponential series approximation"
   * published in Applied Mathematics and Computation 218 (2011) 1894-1902
   * doi:10.1016/j.amc.2011.06.072
   *
   * is used. At the heart of the method (equation (14) of the paper) is the
   * following Fourier series based approximation:
   *
   * @f[ w(z) \approx \frac{i}{2\sqrt{\pi}}\left(
   * \sum^N_{n=0} a_n \tau_m\left(
   * \frac{1-e^{i(n\pi+\tau_m z)}}{n\pi + \tau_m z} -
   * \frac{1-e^{i(-n\pi+\tau_m z)}}{n\pi - \tau_m z}
   * \right) - a_0 \frac{1-e^{i \tau_m z}}{z}
   * \right) @f]
   * 
   * The coefficients @f$a_b@f$ are given by:
   *
   * @f[ a_n=\frac{2\sqrt{\pi}}{\tau_m}
   * \exp\left(-\frac{n^2\pi^2}{\tau_m^2}\right) @f]
   *
   * To achieve machine accuracy in double precision floating point arithmetic
   * for most of the upper half of the complex plane, chose @f$t_m=12@f$ and
   * @f$N=23@f$ as is done in the paper.
   *
   * There are two complications: For Im(z) negative, the exponent in the
   * equation above becomes so large that the roundoff in the rest of the
   * calculation is amplified enough that the result cannot be trusted.
   * Therefore, for Im(z) < 0, the symmetry of the erfc function under the
   * transformation z --> -z is used to avoid accuracy issues for Im(z) < 0 by
   * formulating the problem such that the calculation can be done for Im(z) > 0
   * where the accuracy of the method is fine, and some postprocessing then
   * yields the desired final result.
   *
   * Second, the denominators in the equation above become singular at
   * @f$z = n * pi / 12@f$ (for 0 <= n < 24). In a tiny disc around these
   * points, Taylor expansions are used to overcome that difficulty.
   *
   * This routine precomputes everything it can, and tries to write out complex
   * operations to minimise subroutine calls, e.g. for the multiplication of
   * complex numbers.
   *
   * In the square -8 <= Re(z) <= 8, -8 <= Im(z) <= 8, the routine is accurate
   * to better than 4e-13 relative, the average relative error is better than
   * 7e-16. On a modern x86_64 machine, the routine is roughly three times as
   * fast than the old CERNLIB implementation and offers better accuracy.
   */
  static std::complex<double> faddeeva(std::complex<double> z);
  /** @brief evaluate Faddeeva function for complex argument (fast version)
   *
   * @author Manuel Schiller <manuel.schiller@nikhef.nl>
   * @date 2013-02-21
   *
   * Calculate the value of the Faddeeva function @f$w(z) = \exp(-z^2)
   * \mathrm{erfc}(-i z)@f$.
   *
   * This is the "fast" version of the faddeeva routine above. Fast means that
   * is takes roughly half the amount of CPU of the slow version of the
   * routine, but is a little less accurate.
   *
   * To be fast, chose @f$t_m=8@f$ and @f$N=11@f$ which should give accuracies
   * around 1e-7.
   *
   * In the square -8 <= Re(z) <= 8, -8 <= Im(z) <= 8, the routine is accurate
   * to better than 4e-7 relative, the average relative error is better than
   * 5e-9. On a modern x86_64 machine, the routine is roughly five times as
   * fast than the old CERNLIB implementation, or about 30% faster than the
   * interpolation/lookup table based fast method used previously in RooFit,
   * and offers better accuracy than the latter (the relative error is roughly
   * a factor 280 smaller than the old interpolation/table lookup routine).
   */
  static std::complex<double> faddeeva_fast(std::complex<double> z);

  /** @brief complex erf function
   *
   * @author Manuel Schiller <manuel.schiller@nikhef.nl>
   * @date 2013-02-21
   *
   * Calculate erf(z) for complex z.
   */
  static std::complex<double> erf(const std::complex<double> z);

  /** @brief complex erf function (fast version)
   *
   * @author Manuel Schiller <manuel.schiller@nikhef.nl>
   * @date 2013-02-21
   *
   * Calculate erf(z) for complex z. Use the code in faddeeva_fast to save some time.
   */
  static std::complex<double> erf_fast(const std::complex<double> z);
  /** @brief complex erfc function
   *
   * @author Manuel Schiller <manuel.schiller@nikhef.nl>
   * @date 2013-02-21
   *
   * Calculate erfc(z) for complex z.
   */
  static std::complex<double> erfc(const std::complex<double> z);
  /** @brief complex erfc function (fast version)
   *
   * @author Manuel Schiller <manuel.schiller@nikhef.nl>
   * @date 2013-02-21
   *
   * Calculate erfc(z) for complex z. Use the code in faddeeva_fast to save some time.
   */
  static std::complex<double> erfc_fast(const std::complex<double> z);


  // CERNLIB complex error function
  static RooComplex ComplexErrFunc(Double_t re, Double_t im= 0);
  static RooComplex ComplexErrFunc(const RooComplex& z);

  // Interpolated CERF with automatic interpolation order selection
  static RooComplex FastComplexErrFunc(const RooComplex& z) ;
  
  // Interpolated Re(CERF) with automatic interpolation order selection
  static Double_t FastComplexErrFuncRe(const RooComplex& z) ;

  // Interpolated Im(CERF) with automatic interpolation order selection
  static Double_t FastComplexErrFuncIm(const RooComplex& z) ;

  // Interpolated complex error function at specified interpolation order
  static RooComplex ITPComplexErrFunc(const RooComplex& z, Int_t nOrder) ;
  static Double_t ITPComplexErrFuncRe(const RooComplex& z, Int_t nOrder) ;
  static Double_t ITPComplexErrFuncIm(const RooComplex& z, Int_t nOrder) ;

  // Switch to use file cache for CERF lookup table
  static void cacheCERF(Bool_t flag=kTRUE) ;

  // 1-D nth order polynomial interpolation routines
  static Double_t interpolate(Double_t yArr[],Int_t nOrder, Double_t x) ;
  static Double_t interpolate(Double_t xa[], Double_t ya[], Int_t n, Double_t x) ;

  static Double_t erf(Double_t x) ;
  static Double_t erfc(Double_t x) ;
  
  static void cleanup() ;

  // Allocate and initialize CERF lookup grid
  static void initFastCERF(Int_t reBins= 800, Double_t reMin=-4.0, Double_t reMax=4.0, 
			   Int_t imBins=1000, Double_t imMin=-4.0, Double_t imMax=6.0) ;

private:

  ClassDef(RooMath,0) // math utility routines
};

#endif