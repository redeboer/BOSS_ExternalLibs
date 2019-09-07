//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: Constants.h,v 1.7 2010/07/19 08:51:07 zhangy Exp $
//
// Description:
//	The Constants class contains various constant values as
//      static members in MdcPatRec package.
//
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//      Zhang Yao
//
// Copyright Information:
//	Copyright (C) 2006
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef CONSTANTS_HH
#define CONSTANTS_HH

//-----------------
// Headers --
//-----------------

class Constants {

  public:

  // Initialization of these is done in the .cc file. Although this
  // could be done here, it is not supported by the DEC C++ compiler
  // and is a recent change to the language reference.
  static const double pi;
  static const double twoPi;
  static const double halfPi;
  static const double radToDegrees;

  static const double c;

  // small value
  static const double epsilon;

  // pulse propagation velocity in cm/nsec
  static const double vpropInner;//cm/nsec
  static const double vpropOuter;//cm/nsec

  // for global wire no. calc.
  static const int nSuperLayer;
  static const int nWireBeforeLayer[43];
  static const int maxCell[43];
  static const int viewOfsLayer[11];

  /// MDC Geometry
  static const double firstStLayerRadius;
  static const double maxMdcRadius;
  static const double maxMdcZLen;

};

#endif

