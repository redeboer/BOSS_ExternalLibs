//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifExternal.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Header for |DifExternal|
//      Define some useful constants
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

#ifndef DifExternal_H
#define DifExternal_H

#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifVector.h"
#include "MdcRecoUtil/DifPoint.h"
#include "MdcRecoUtil/DifComplex.h"

extern const DifNumber one;
extern const DifNumber zero;

extern const DifVector xhat;
extern const DifVector yhat;
extern const DifVector zhat;
extern const DifVector nullVec;

extern const DifPoint origin;

extern const DifComplex I;

#endif








