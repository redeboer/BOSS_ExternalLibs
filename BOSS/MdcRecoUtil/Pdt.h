//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: Pdt.h,v 1.3 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      Searchable Particle Lists for BaBar
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      John LoSecco            Original Author
//	Zhang Yao(zhangyao@ihep.ac.cn)
//      Anders Ryd              Reimplemented using RW hash tables
//
// Copyright Information:
//      Copyright (C) 1998      The University of Notre Dame
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
//
// Based on Pdt.h by Luca Lista
//
// See Also
//     PdtbEntry, DecayMode

#ifndef PDT_HH
#define PDT_HH

#include "MdcRecoUtil/PdtLund.h"
#include "MdcRecoUtil/PdtGeant.h"
#include "MdcRecoUtil/PdtPid.h"
#include "MdcRecoUtil/PdtEntry.h"
#include <string>
#include <map>

//Hash size, should atleast be a prime and 
//of about the same size as the number of particles
#define PdtHashSize 503

//typedef PdtEntry* PdtEntryPtr;

#include <iosfwd>

template < class T > class AstStringMap;

class Pdt {

public:
  // return PDT entry pointer given the particle id or name
  static PdtEntry* lookup(const std::string& name);
  static PdtEntry* lookup(PdtLund::LundType id);
  static PdtEntry* lookup(PdtGeant::GeantType id);
  static PdtEntry* lookup(PdtPdg::PdgType id);
  static PdtEntry* lookup(PdtPid::PidType id, int charge=-1);
  static PdtEntry* lookup(PdtPid::PidNeutralType id,int charge=0);
  //
  // LL:  the following method is suggested by Gautier, but I think 
  // LL:  could not be completely safe.
   // LL:  I suggest to use lookup(pdtId(int id), int charge) in replacement. 
  // 
  //  static PdtEntry* lookup(int id, int charge=-1); 

  //lange Feb 11,2003
  static bool sameOrConj( PdtLund::LundType id1, PdtLund::LundType id2);
  static bool sameOrConj( PdtPdg::PdgType id1, PdtPdg::PdgType id2);
  static bool sameOrConj( PdtEntry *id1, PdtEntry *id2);

  // GHM 05/19/99 : get the conjugate of a PdtEntry
  static const PdtEntry* conjugate( const PdtEntry* );
  
  // get mass value 
  static float mass(PdtLund::LundType id)      { return lookup(id)->mass(); }
  static float mass(PdtGeant::GeantType id)    { return lookup(id)->mass(); }
  static float mass(PdtPid::PidType id)        { return lookup(id)->mass();}
  static float mass(PdtPid::PidNeutralType id) { return lookup(id)->mass();}
  static float mass(const char* name)          { return lookup(name)->mass(); }

  // get width value 
  static float width(PdtLund::LundType id)     { return lookup(id)->width(); }
  static float width(PdtGeant::GeantType id)   { return lookup(id)->width(); }
  static float width(PdtPid::PidType id)       { return lookup(id)->width(); }
  static float width(PdtPid::PidNeutralType id) { return lookup(id)->width(); }
  static float width(const char* name)         { return lookup(name)->width(); }

  // get lifetime value 
  static float lifetime(PdtLund::LundType id)  { return lookup(id)->lifetime(); }
  static float lifetime(PdtGeant::GeantType id){ return lookup(id)->lifetime(); }
  static float lifetime(PdtPid::PidType id)    { return lookup(id)->lifetime(); }
  static float lifetime(PdtPid::PidNeutralType id) { return lookup(id)->lifetime(); }
  static float lifetime(const char* name)      { return lookup(name)->lifetime(); }

  // get spin value 
  static float spin(PdtLund::LundType id)      { return lookup(id)->spin(); }
  static float spin(PdtGeant::GeantType id)    { return lookup(id)->spin(); }
  static float spin(PdtPid::PidType id)        { return lookup(id)->spin(); }
  static float spin(PdtPid::PidNeutralType id) { return lookup(id)->spin(); }
  static float spin(const char* name)          { return lookup(name)->spin(); }
  
  // get charge. Not available for Pid code 
  static float charge(PdtLund::LundType id)    { return lookup(id)->charge(); }
  static float charge(PdtGeant::GeantType id)  { return lookup(id)->charge(); }
  static float charge(const char* name)        { return lookup(name)->charge(); }

  // get antiparticle code
  static PdtLund::LundType antiCode(PdtLund::LundType);

  // read the MC++ input file
  static void readMCppTable(std::string filenm);

  // remove all the entries
  static void deleteAll();

  // print the particle table
  static void printOn( std::ostream & );
  
  // create an entry for a particle that is its own antiparticle
  static void addParticle(const char* pname, PdtLund::LundType id,
			  float spin, float charge, float mass, float width=0.0,
			  float cut = 0.0
			  );
  static void addParticle(const char* pname, PdtGeant::GeantType id,
			  float spin, float charge, float mass, float width=0.0,
			  float cut = 0.0
			  );
  
  static void addDecay(const char* pname, float bf,
		       const char* child1, const char* child2=0, 
		       const char* child3=0, const char* child4=0, 
		       const char* child5=0
		       );
  
  static void addDecay(PdtLund::LundType id, float bf,
		       PdtLund::LundType child1, 
		       PdtLund::LundType child2=PdtLund::null, 
		       PdtLund::LundType child3=PdtLund::null,
		       PdtLund::LundType child4=PdtLund::null, 
		       PdtLund::LundType child5=PdtLund::null
		       );
  
  // code conversions from Lund and Geant id
  static PdtLund::LundType   lundId (const PdtGeant::GeantType);
  static PdtLund::LundType   lundId (const PdtPdg::PdgType);
  static PdtLund::LundType   lundId (const PdtPid::PidType, int charge = -1);
  static PdtLund::LundType   lundId (const PdtPid::PidNeutralType, int charge = 0);

  static PdtPdg::PdgType     pdgId  (const PdtLund::LundType);
  static PdtPdg::PdgType     pdgId  (const PdtPid::PidType pid, int charge = -1)
    { return pdgId(lundId(pid, charge)); }
  static PdtPdg::PdgType     pdgId  (const PdtPid::PidNeutralType pid, int charge = 0)
    { return pdgId(lundId(pid, charge)); }
  static PdtPdg::PdgType     pdgPid (const PdtGeant::GeantType geant)
    { return pdgId(lundId(geant)); }

  static PdtGeant::GeantType geantId(const PdtLund::LundType);
  static PdtGeant::GeantType geantId(const PdtPid::PidType, int charge = -1);
  static PdtGeant::GeantType geantId(const PdtPid::PidNeutralType, int charge = 0);
  static PdtGeant::GeantType geantId(const PdtPdg::PdgType pdg)
    { return geantId(lundId(pdg)); }

  static PdtPid::PidType     pidId  (const PdtLund::LundType);
  static PdtPid::PidType     pidId  (const PdtGeant::GeantType);
  static PdtPid::PidType     pidId  (const PdtPdg::PdgType pdg)
    { return pidId(lundId(pdg)); }
  static PdtPid::PidNeutralType     pidNeutId  (const PdtLund::LundType);
  static PdtPid::PidNeutralType     pidNeutId  (const PdtGeant::GeantType);
  static PdtPid::PidNeutralType     pidNeutId  (const PdtPdg::PdgType pdg)
    { return pidNeutId(lundId(pdg)); }

  // conversions from Pid code need the charge as an extra argument

  // conversions from integer, useful in "for" cycles and to load
  // from FORTRAN common blocks. Example:
  // int i;
  // for (i = PdtPid::electron; i <= PdtPid::proton; i++)
  //   { PdtEntry* pdt = Pdt::lookup(Pdt::pidId(i)); }
  // 
  static PdtLund::LundType   lundId (const int id) { return (PdtLund::LundType) id; }; 
  static PdtGeant::GeantType geantId(const int id) { return (PdtGeant::GeantType) id; }; 
  static PdtPid::PidType     pidId  (const int id) { return (PdtPid::PidType) id; }; 
  static PdtPid::PidNeutralType     pidNeutId (const int id) { return (PdtPid::PidNeutralType) id; };
  static PdtPdg::PdgType     pdgId  (const int id) { return (PdtPdg::PdgType) id; };



private:
  
  //Hash tables for looking up particles
  static  std::map<int,PdtEntry*>* _entries; //By pdt number
  static  AstStringMap< PdtEntry >* _entriesn;  //By name
  static  std::map<int,PdtEntry*>* _entriesg;  //By geant number
  static  std::map<int,PdtEntry*>* _entriesl;  //By lund number
  //Pid type is too small to bother. 

  //hash functions for particle numbers and name
  static unsigned PdtNumHash(const int& num);
  static unsigned PdtNameHash(const std::string& c);
  
  //cache common particles for fast access
  static PdtEntry* _positiveEntries[5];
  static PdtEntry* _negativeEntries[5];
  static PdtEntry* _neutralEntries[5];
  
};
  
#endif




