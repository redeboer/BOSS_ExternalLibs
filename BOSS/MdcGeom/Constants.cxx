//--------------------------------------------------------------------------
//
// Description:
//	Class Constants; encapsulates static constants
//
//      See header for more info
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII
//
// Author List:
// 	Zhang Yao
//
// Copyright Information:
//	Copyright (C) 2006
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcGeom/Constants.h"
#include "CLHEP/Units/PhysicalConstants.h"

const double Constants::pi           =  CLHEP::pi;
const double Constants::twoPi        =  CLHEP::twopi;
const double Constants::halfPi       =  CLHEP::halfpi;
const double Constants::radToDegrees =  180./pi;
 
const double Constants::c            =  2.99792458E10;       // cm/sec
const double Constants::epsilon      =   0.000000001;
const double Constants::vpropInner   =  22.0;//cm/nsec
const double Constants::vpropOuter   =  24.0;//cm/nsec

const int Constants::nSuperLayer  =  11;
const int Constants::nWireBeforeLayer[43] = {0,40,84,132,188,252,324,404,484,560,636,724,812,912,1012,1124,1236,1364,1492,1632,1772,1932,2092,2252,2412,2588,2764,2940,3116,3324,3532,3740,3948,4188,4428,4668,4908,5164,5420,5676,5932,6220,6508};
const int Constants::maxCell[43] = { 
  40,44,48,56,     64,72,80,80,     76,76,88,88,
  100,100,112,112, 128,128,140,140, 160,160,160,160,
  176,176,176,176, 208,208,208,208, 240,240,240,240,
  256,256,256,256, 288,288,288 };

const int Constants::viewOfsLayer[11] = {1,-1,0,0,0,1,-1,1,-1,0,0};//FIXME

/// MDC Geometry
const double Constants::firstStLayerRadius = 23.6;
const double Constants::maxMdcRadius = 81.0;
const double Constants::maxMdcZLen   = 150.;
