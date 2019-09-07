//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: BField.h,v 1.4 2008/09/17 08:30:39 zhangy Exp $
//
// Description:
//	The BField class contains the magnetic field map.  This class is now a
//      virtual class
//
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Bob Jacobsen
//      Dave Brown, 12-23-96; Added Kalman filter tracking functions, prepared
//                            for a full field map implementation
//      Fri Sep 25 11:57:57 PDT 1998 - A. Snyder
//      Remove bFieldVectDF and bFieldZDF
//      The are replaced by derivative
//      Dave Brown, 12/20/99  Make this a virtual class, remove the
//      obsolete and dangerous static bFieldNom;
//
// Copyright Information:
//	Copyright (C) 1995, 1996
//
//------------------------------------------------------------------------

#ifndef BFIELD_H
#define BFIELD_H

#include "MagneticField/IMagneticFieldSvc.h"

class BField {

public:

  BField(IMagneticFieldSvc* magSvc);
  ~BField();  

  // return the Z component of the magnetic field at a point
  double bFieldZ() const;

  // return nominal field
  double bFieldNominal()const;

  //print 
  void print()const;

  // for converting radius*field to GeV/c of transverse momentum
  static const double cmTeslaToGeVc;
  static const double gaussToTesla;

private:
  // nominal field only
  double _nomFieldZ;
  IMagneticFieldSvc* m_pIMF;
};

#endif

