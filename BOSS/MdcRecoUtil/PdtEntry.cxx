//
// PdtEntry.cc - data class for a particle
//
// Copyright (C)  1993  The Board of Trustees of The Leland Stanford
// 
// History:
//	Migration for BESIII MDC
//                      Junior University.  All Rights Reserved.
//
// $Id: PdtEntry.cxx,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// A PdtEntry holds the Particle Data Table data for a single particle.
//
// Modified:
//    Luca Lista     04 oct 96    lookup by different types, not by integer
//
// See Also
//    Pdt, DecayMode

extern "C" {
#include <float.h>
#include <string.h>
}

// This is for systems that define the BSD string functions as macros:
#ifdef index
#undef index
#endif
#ifdef rindex
#undef rindex
#endif

#include <iostream>
#include <iomanip>
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtEntry.h"
#include "MdcRecoUtil/DecayMode.h"
#include <vector>
using std::endl;
using std::ios;
using std::ostream;
using std::setw;
using std::vector;

#define HBARC (197.327*1.e-3*1.e-13) // GeV*cm

PdtEntry::PdtEntry(const char *name, PdtLund::LundType code, float spin, float charge, 
		   float mass, float width, float massCut ) :
  _name(new char[strlen(name) + 1]),
  _mass(mass),
  _width(width),
  _lifetime( (width>0.0) ? (HBARC / width) : FLT_MAX ),
  _spin(spin),
  _charge(charge),
  _widthCut( (massCut > 0.0) ? massCut : 2.0*width ),
  _sumBR(0.0),
  _decayList(new vector<DecayMode*>),
  _lundId(code),
  _pdgId(Pdt::pdgId(code)),
  _geantId(Pdt::geantId(code)),
  _pidId(Pdt::pidId(code)),
  _pidNeutId(Pdt::pidNeutId(code)),
  _conjugate(0)
{ strcpy(_name, name); }


PdtEntry::PdtEntry(const char *name, PdtGeant::GeantType code, float spin, float charge, 
		   float mass, float width, float massCut ) :
  _name(new char[strlen(name) + 1]),
  _mass(mass),
  _width(width),
  _lifetime( (width>0.0) ? (HBARC / width) : FLT_MAX ),
  _spin(spin),
  _charge(charge),
  _widthCut( (massCut > 0.0) ? massCut : 2.0*width ),
  _sumBR(0.0),
  _decayList(new vector<DecayMode*>),
  _lundId(Pdt::lundId(code)),
  _pdgId(Pdt::pdgId(code)),
  _geantId(code),
  _pidId(Pdt::pidId(code)),
  _pidNeutId(Pdt::pidNeutId(code)),
  _conjugate(0)
{ strcpy(_name, name); }

PdtEntry::PdtEntry(const char *name, PdtPdg::PdgType code, float spin, float charge, 
		   float mass, float width, float massCut ) :
  _name(new char[strlen(name) + 1]),
  _mass(mass),
  _width(width),
  _lifetime( (width>0.0) ? (HBARC / width) : FLT_MAX ),
  _spin(spin),
  _charge(charge),
  _widthCut( (massCut > 0.0) ? massCut : 2.0*width ),
  _sumBR(0.0),
  _decayList(new vector<DecayMode*>),
  _lundId(Pdt::lundId(code)),
  _pdgId(code),
  _geantId(Pdt::geantId(code)),
  _pidId(Pdt::pidId(code)),
  _pidNeutId(Pdt::pidNeutId(code)),
  _conjugate(0)
{ strcpy(_name, name); }

void PdtEntry::addDecay(float bf, vector<PdtEntry*> *kids )
{
  _decayList->push_back(new DecayMode(bf, kids));
  _sumBR += bf;
}

#ifndef HP1022
void PdtEntry::printOn(ostream& str) const
{
  // pkg coordinator:  we need to see if form() really is part of
  // iostream.  In any case, i've recoded things below simply to
  // avoid it.  Perhaps this is good enough.
  //
  
  //   str.form("%6d %-8s %8g ", (int) _lundId, _name, _mass);
  //   Not sure if the is exactly the same as using form(), but on
  //   sun, it's claimed that you get %g behavior from ios as default.
  //   I'm ignoring the left-field justification for now.
  //
  str << setw(6) << (int) _lundId << " " << setw(8) << _name << " "
      << setw(8) << _mass << " ";
  
  if (_width > 0.0)
    str << " " << _width;
  else 
    str << "Stable      ";
  str << setw(3) << _spin << "   " << setw(2) <<_charge
      << "   ";
  if (_width > 0.0)
    str << " " << _lifetime << endl;
  else
    str << " Stable\n";
}
#else
void PdtEntry::printOn(ostream& str) const
{
  ios::sync_with_stdio();
  printf("%6d %-8s %8g ", (int) _lundId, _name, _mass);
  if (_width > 0.0)
    { printf("%g ", _width); str <<" ";}
  else 
    str << "Stable      ";
  
  printf( "%3g   %2g   ", _spin, _charge);
  if (_width > 0.0)
    {  printf( " %g\n", _lifetime); str <<" ";}
  else
    str << " Stable\n";
}
#endif


void PdtEntry::printBFOn(ostream& str) const
{
  int l = _decayList->size();
  for (int i=0; i<l; i++)
    (*_decayList)[i]->printOn(str);
}

PdtEntry::~PdtEntry()
{
  vector<DecayMode*>::iterator iter=_decayList->begin();
  while ( iter != _decayList->end() ) { delete *iter; ++iter; }
  _decayList->clear(); 
  delete _decayList;
  delete [] _name;
}

const PdtEntry*
PdtEntry::conjugate() const
{
  if ( ! _conjugate ) {
    const_cast<PdtEntry*>(this)->_conjugate = Pdt::conjugate( this );
  }
  return _conjugate ;
}


bool PdtEntry::operator==(const PdtEntry &theOther) const 
{
  return (theOther._lundId==_lundId && theOther._geantId==_geantId);
}

bool PdtEntry::operator<(const PdtEntry &theOther) const 
{
  return (_lundId < theOther._lundId || 
	  (_lundId == theOther._lundId && _geantId < theOther._geantId));
}

