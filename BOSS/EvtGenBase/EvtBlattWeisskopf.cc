#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtBlattWeisskopf.cc,v 1.2 2015/07/01 12:28:24 pingrg Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#include <iostream>
#include <assert.h>
#include <math.h>
#include "EvtGenBase/EvtBlattWeisskopf.hh"
#include "EvtGenBase/EvtReport.hh"
using std::endl;

EvtBlattWeisskopf::EvtBlattWeisskopf(int LL, double R, double p0)
  : _LL(LL), _radial(R), _p0(p0)
{
  if(R < 0) {

    report(INFO,"EvtGen") << "Radius " << R << " negative" << endl;
    assert(0);
  }

  _radial = R;

  // compute formula for nominal momentum

  _F0 = compute(_p0);
  if(_F0 <= 0) {
    
    report(INFO,"EvtGen") << "Invalid nominal form factor computed " << _F0 << endl;
    assert(0);
  } 
}

EvtBlattWeisskopf::EvtBlattWeisskopf(const EvtBlattWeisskopf& other)
  : _LL(other._LL), _radial(other._radial), _p0(other._p0), _F0(other._F0)
{}

EvtBlattWeisskopf::~EvtBlattWeisskopf()
{}

double EvtBlattWeisskopf::operator()(double p) const
{
  double ret = compute(p)/_F0;
  //  report(INFO,"EvtGen") << p << " " << _p0 << " " << _F0 << " " << _LL << " " << _radial << " " << ret << endl;
  return ret;
}

// Blatt-Weisskopf form factors
// see e.g. hep-ex/0011065
// Dalitz Analysis of the Decay D0->K-pi+pi0 (CLEO)
//
// p   - momentum of either daugher in the meson rest frame,
//       the mass of the meson is used
// pAB - momentum of either daughter in the candidate rest frame
//       the mass of the candidate is used
// R - meson radial parameter
// 
// In the CLEO paper R=5 GeV-1 for D0, R=1.5 for intermediate resonances

double EvtBlattWeisskopf::compute(double p) const
{
  if(p < 0) {
    
    report(INFO,"EvtGen") << "Momentum " << p << " negative in form factor calculation" << endl;
    assert(0);
  }
  else {
  double rp = p*_radial;
  double rp2 = rp*rp;
  double rp4 = rp2*rp2;
  double rp6 = rp2 * rp4;
  double rp8 = rp4 * rp4;

    double x = p*p*_radial*_radial;
    
    if(0 == _LL) return 1.;
    else
      if(1 == _LL) return sqrt(1.0/(1.0+x));
      else
	if(2 == _LL) return sqrt(1.0/(1.0+x/3.0+x*x/9.0));
	else
	  if(3 == _LL) return 1.0/sqrt(225 + 45*rp2 + 6*rp4 + rp6 );
	  else
	    if(4 == _LL) return 1.0/sqrt(11025 + 1575*rp2 + 135*rp4 + 10*rp6 + rp8 );
	    else {
	      report(INFO,"EvtGen") << "Angular momentum " << _LL << " not implemented" << endl;
	      assert(0);
	    }
  }
}

