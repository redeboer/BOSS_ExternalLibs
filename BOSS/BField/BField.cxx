//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: BField.cxx,v 1.2 2008/09/17 08:30:39 zhangy Exp $
//
// Description:
//	Class BField; encapsulates the magnetic field.
//
//      See header for more info.  
//
// Environment:
//	Software developed for the BESIII Detector at BEPCII
//
// Author List:
//	Bob Jacobsen		Original Author
//	Zhang Yao		Only keep nominal field
//
// Copyright Information:
//	Copyright (C) 1995	Lawrence Berkeley Laboratory
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include <iostream>
#include "BField/BField.h"
#include "MdcGeom/Constants.h"

const double BField::cmTeslaToGeVc = Constants::c/1.0E13;
const double BField::gaussToTesla = 1000.;

//constructor
BField::BField(IMagneticFieldSvc* magSvc)

{ m_pIMF = magSvc;}

// destroy
BField::~BField() {}	    

// field z of nominal field 
double
BField::bFieldZ() const
{
  return m_pIMF->getReferField()*gaussToTesla;
}

void BField::print()const
{
  std::cout << "Default constant field" << std::endl;
  std::cout << "nominal Field: " << bFieldNominal() << std::endl;
}

double
BField::bFieldNominal()const
{
  return bFieldZ();
}
