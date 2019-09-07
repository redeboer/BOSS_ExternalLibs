//                              -*- Mode: C++ -*- 
//
// PdtEntry.h - data class for a particle
//
// Copyright (C)  1993  The Board of Trustees of The Leland Stanford
// 
// History:
//	Migration for BESIII MDC
//                      Junior University.  All Rights Reserved.
//
// $Id: PdtEntry.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// A PdtEntry is an almost pure data class. It holds the data for a particle,
// ie. mass, width, lifetime, decay modes, etc. The sign of quantities is
// defined to be that of the particle (not antiparticle).
//
// Modified:
//    Luca Lista     04 oct 96    lookup by different types, not by integer
//
// See Also
//    AntiPData, PData, Pdt, DecayMode

#ifndef _PDTENTRY_HH_
#define _PDTENTRY_HH_

#include "MdcRecoUtil/PdtLund.h"
#include "MdcRecoUtil/PdtPdg.h"
#include "MdcRecoUtil/PdtGeant.h"
#include "MdcRecoUtil/PdtPid.h"
#include <vector>
using std::vector;

#include <iosfwd>
class DecayMode;

class PdtEntry
{
public:
  // in the constructor, spin is in units of hbar, charge in unit of e.
  // mass and width are in GeV.
  PdtEntry(const char *name, PdtLund::LundType code, float spin,
	   float charge, float mass, float width=0, float massCut=0);
  PdtEntry(const char *name, PdtGeant::GeantType code, float spin,
	   float charge, float mass, float width=0, float massCut=0);
  PdtEntry(const char *name, PdtPdg::PdgType code, float spin,
	   float charge, float mass, float width=0, float massCut=0);
  
  virtual ~PdtEntry();
  
  void printOn(std::ostream& str) const;
  void printBFOn(std::ostream& str) const;
  
  const char *name()     const {return _name;}
  float       charge()   const {return _charge;}
  float       mass()     const {return _mass;}
  float       width()    const {return _width;}
  float       widthCut() const {return _widthCut;}
  float       lifetime() const {return _lifetime;}
  float       spin()     const {return _spin;}
  float       sumBR()    const {return _sumBR;}
  const vector<DecayMode*> *decayList() const { return _decayList; }
  void addDecay(float bf, vector<PdtEntry*> *kids );
  PdtLund::LundType   lundId()  const { return _lundId;};
  PdtPdg::PdgType     pdgId()   const { return _pdgId; }
  PdtGeant::GeantType geantId() const { return _geantId; };
  PdtPid::PidType     pidId() const   { return _pidId; };
  PdtPid::PidNeutralType    pidNeutId() const   { return _pidNeutId; };
  bool operator==(const PdtEntry &) const;
  bool operator<(const PdtEntry &) const;
  
  // new functionality (GHM 05/99) : get the conjugate PdtEntry
  const PdtEntry* conjugate() const;

protected:
  char *_name;            	// name
  float _mass;		// nominal mass (GeV)
  float _width;		// width (0 if stable) (GeV)
  float _lifetime;		// c*tau, in standard length units (cm)
  float _spin;		// spin, in units of hbar
  float _charge;		// charge, in units of e
  float _widthCut;		// used to limit range of B-W
  float _sumBR;		// total branching ratio (should be 1.)
  vector<DecayMode*>* _decayList; // pointer to linked list of decayers
  PdtLund::LundType   _lundId;
  PdtPdg::PdgType     _pdgId;
  PdtGeant::GeantType _geantId;
  PdtPid::PidType     _pidId;
  PdtPid::PidNeutralType  _pidNeutId;
  /*mutable*/ const PdtEntry* _conjugate; // this particle conjugate
};

#endif
