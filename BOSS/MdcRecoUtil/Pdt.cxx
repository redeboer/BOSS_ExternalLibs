//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: Pdt.cxx,v 1.5 2010/03/25 09:55:57 zhangy Exp $
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
//     PdtEntry, DecayMode

#include "MdcRecoUtil/AstStringMap.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtEntry.h"

#include <string.h>     
#include <stdio.h>
#include <assert.h>

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <iostream>
#include <strstream>
using std::cerr;
using std::cout;
using std::endl;
using std::istrstream;
using std::ostream;
using std::strstream;

#define HBARC (197.327*1.e-3*1.e-13) // GeV*cm

PdtEntry* Pdt::_positiveEntries[5]={0,0,0,0,0};
PdtEntry* Pdt::_negativeEntries[5]={0,0,0,0,0};
PdtEntry* Pdt::_neutralEntries[5]={0,0,0,0,0};
  
//By pdt number
map<int, PdtEntry*>* 
Pdt::_entries = new map<int, PdtEntry*>; 

//By name
AstStringMap< PdtEntry >* 
Pdt::_entriesn = new AstStringMap< PdtEntry >;

//By geant number
map<int, PdtEntry*>*
Pdt::_entriesg = new map<int, PdtEntry*>;  

//By lund number
map<int, PdtEntry*>*
Pdt::_entriesl = new map<int, PdtEntry*>;  

PdtLund::LundType Pdt::antiCode(PdtLund::LundType kf_id){

  int kf = (int) kf_id;
  
  int i2 = (kf/10) % 10;
  int i3 = (kf/100) % 10;
  int i4 = (kf/1000) % 10;
  
  if (i2 == i3 && i4 == 0 && kf > 100)
    return lundId(kf);
  else if (kf >= 21 && kf <= 23) return lundId(kf);
  
  else if (kf == 310) return lundId(130);
  else if (kf == 130) return lundId(310);
  
  else return lundId(-kf);
}

void Pdt::addParticle(const char *pname, PdtLund::LundType id,
		      float spin, float charge, float mass, 
		      float width, float cut){

  PdtEntry* nentry = new PdtEntry(pname, id, spin, charge, mass, width, cut);
  (*_entries)[pdgId(id)]=nentry;

  if ( (*_entriesn)[pname] != 0 ) {
    //cout << "Pdt::addParticle:adding existing particle:"<<pname<<endl;
  }

  _entriesn->insert( pname )=nentry;
  if(PdtGeant::null!=nentry->geantId()){
    (*_entriesg)[nentry->geantId()]=nentry;
  }

  (*_entriesl)[id]=nentry;

  if(PdtPid::null!=nentry->pidId()) {
    if(nentry->charge()>0) {
      _positiveEntries[nentry->pidId()]=nentry;
    }else{
      _negativeEntries[nentry->pidId()]=nentry;
    }
  }

  if(PdtPid::none!=nentry->pidNeutId()){
    if(nentry->charge()==0){
      _neutralEntries[nentry->pidNeutId()]=nentry;
    }
  }

}

void Pdt::addParticle(const char *pname, PdtGeant::GeantType id,
		      float spin, float charge, float mass, 
		      float width, float cut){

  PdtEntry* nentry = new PdtEntry(pname, id, spin, charge, mass, width, cut);

  (*_entries)[pdgId(id)]=nentry;
  if ( (*_entriesn)[pname] != 0 ) {
    //cout << "Pdt::addParticle:adding existing particle:"<<pname<<endl;
  }

  _entriesn->insert( pname )=nentry;
  if(PdtGeant::null!=nentry->geantId()){
    (*_entriesg)[id]=nentry;
  }

  (*_entriesl)[lundId(id)]=nentry;

  if(PdtPid::null!=nentry->pidId()) {
    if(nentry->charge()>0) {
      _positiveEntries[nentry->pidId()]=nentry;
    }else{
      _negativeEntries[nentry->pidId()]=nentry;
    }

  }

  if(PdtPid::none!=nentry->pidNeutId()){
    if(nentry->charge()==0) {
      _neutralEntries[nentry->pidNeutId()]=nentry;
    }
  }

}

void Pdt::addDecay(const char* pname, float bf,
		   const char* child1, const char* child2, const char* child3,
		   const char* child4, const char* child5){
 
  PdtEntry *primary = lookup(pname);
  if (primary == 0) return;
  const char *children[5] = {child1, child2, child3, child4, child5};

  vector<PdtEntry*> *kids = new vector<PdtEntry*>;
  
  int nkids;
  for(nkids=0; nkids<5 && children[nkids]!=0; nkids++ )
  {  
    PdtEntry* secondary = lookup(children[nkids]);
    if( secondary ==0 ) break;
    kids->push_back(secondary);
  }
  
  primary->addDecay(bf, kids );

}

void Pdt::addDecay(PdtLund::LundType id, float bf,
		   PdtLund::LundType child1, 
		   PdtLund::LundType child2, 
		   PdtLund::LundType child3,
		   PdtLund::LundType child4, 
		   PdtLund::LundType child5){
 
  PdtEntry *primary = lookup(id);
  if (primary == 0) return;

  PdtLund::LundType children[5] = {child1, child2, child3, child4, child5};
  vector<PdtEntry*> *kids = new vector<PdtEntry*>;
  
  int nkids;
  for(nkids=0; nkids<5 && children[nkids]!=0; nkids++ )
  {  
    PdtEntry* secondary = lookup(children[nkids]);
    if( secondary ==0 ) break;
    kids->push_back(secondary);
  }
  
  primary->addDecay(bf, kids );

}

PdtEntry* Pdt::lookup( const std::string& name ){
  return (*_entriesn)[name];
}

PdtEntry* Pdt::lookup(PdtLund::LundType id){
  // look for the particle by Particle Data Group id
  return (*_entriesl)[id];
}

PdtEntry* Pdt::lookup(PdtGeant::GeantType id){
  // look for the particle by GEANT id
  return (*_entriesg)[id];
}

PdtEntry* Pdt::lookup(PdtPdg::PdgType id){
  // look for the particle by PDG id
  return (*_entries)[id];
}

PdtEntry* Pdt::lookup(PdtPid::PidType id, int charge){

  // look for the particle by PID code id and charge
  if(id==PdtPid::null) return lookup(PdtLund::null);
  if(charge>0) return _positiveEntries[id];
  return _negativeEntries[id];

}

PdtEntry* Pdt::lookup(PdtPid::PidNeutralType id, int/* charge*/){

  // look for the particle by PID code id and charge

  if(id==PdtPid::none) return lookup(PdtLund::null);
  return _neutralEntries[id];

}

void Pdt::printOn(ostream& cout){

  //lange move printouts here
  cout << "Number of particles in tables: "<<endl
       << "By pdt number   :" << _entries->size() <<endl
       << "By name         :" << _entriesn->size()<<endl
       << "By geant number :" << _entriesg->size()<<endl
       << "By lund number  :" <<_entriesl->size()<<endl;

  cout << '\n'; 
  int i;
  for (i=0;i<72;i++) cout << '-'; 
  cout<<'\n';
  
  cout << "                 Particle Data Table\n\n";
  cout << "   id  name       mass    width    spin  charge  lifetime\n";
  cout << "                  (GeV)   (GeV)                  (cm)    \n";
  for (i=0; i<72; i++) cout << '-';
  cout << '\n'; 
  
  PdtEntry *p;

  map<int, PdtEntry*>::iterator iter=_entries->begin();

  while ( iter != _entries->end()) {
    p = iter->second;
    assert(0!=p);
    p->printOn(cout);
    p->printBFOn(cout);
    ++iter;
  }
  cout << '\n'; 
  for (i=0; i<72; i++) cout << '-';
  cout << '\n';

  map<int, PdtEntry*>::iterator iter1=_entries->begin();

  while ( iter1 != _entries->end()) {
    p=iter1->second;
    cout << "The conjuate of "<< p->name() << " is ";
    cout << conjugate(p)->name() <<endl;
    iter1++;
  }  
}



void Pdt::readMCppTable(string filenm){

  string path = filenm;
  char line[512];
  FILE *ifl = fopen(path.c_str(),"r");

  if(0 == ifl) cout<<" Pdt::readMCppTable: please copy " <<
                     filenm << " in to your run directory!"<<
                     std::endl;
  assert(0!=ifl);

  while ( fgets(line,512,ifl) )
  { 
    if (strlen(line) >= 511)
      {
	cerr << "Pdt.read : input line is too long\n";
	assert(0);
      }
    istrstream linestr(line);
    string opcode;
    char subcode;
    linestr >> opcode >> subcode;
    
    if( opcode == "end" )
      break;
    
    else if( opcode == "add" )
    {
      switch (subcode)
	{
	case 'p':
	  {
	    string classname;
	    linestr >> classname;
	    // if (classname == "Collision" || classname == "Parton") 
	    if (classname == "Collision" ) 
	      continue;
	    
	    string name;
	    int type;
	    float mass, width, cut, charge, spin, lifetime;
	    
	    linestr >> name >> type;
	    linestr >> mass >> width >> cut >> charge;
	    linestr >> spin >> lifetime;
	    
	    charge /= 3.0;
	    if (classname != "Meson")
	      spin /= 2.0;
	    
	    // lifetime is c*tau (mm)
	    if (lifetime > 0.0 && width < 1e-10)
	      width = HBARC / (lifetime/10.0);
	    
	    addParticle(name.c_str(), lundId(type), spin, charge, mass, width, cut);
	    break;
	  }
	  
	case 'c':
	  {
	    int     ptype, nchild;
	    float   bf;
	    string decayer;
	    
	    linestr >> ptype >> bf >> decayer >> nchild;
	    PdtEntry *parent = lookup(lundId(ptype));
	    if (parent == 0) continue;
	    
	    vector<PdtEntry*> *kids = new vector<PdtEntry*>;
	    
	    int i;
	    for(i=0; i<nchild; i++ )
	    {  
	      int ctype;
	      linestr >> ctype;
	      PdtEntry* secondary = lookup(lundId(ctype));
	      if( secondary ==0 ) break;
	      kids->push_back(secondary);
	    }
	    
	    parent->addDecay(bf, kids );
	    break;
	  }
	  
	case 'd':
	  break;
	  
	default:
	  cerr << "Pdt.readMCppTable : unknown subcode '" << subcode 
	       << "' for operation add\n";
	  break;
	}
    }
  }
  // adding geantino for GEANT studies
  //why why why why - shouldn't you do this in the tables?
  //addParticle("geantino", PdtGeant::geantino, 0., 0., 0., 0., 0. );

  fclose(ifl);

}

void Pdt::deleteAll(){

  PdtEntry *p;

  map<int, PdtEntry*>::iterator iter=_entries->begin();

  while ( iter != _entries->end()) {
    p = iter->second;
    assert(0!=p);
    delete p;
    ++iter;
  }

  _entries->clear();
  _entriesn->clear();
  _entriesg->clear();
  _entriesl->clear();
}


PdtLund::LundType Pdt::lundId(const PdtGeant::GeantType gId){ 

  if (gId == PdtGeant::deuteron ||
      gId == PdtGeant::tritium ||
      gId == PdtGeant::alpha ||
      gId == PdtGeant::geantino ||
      gId == PdtGeant::He3 ||
      gId == PdtGeant::Cerenkov)
  { return PdtLund::null; }
  else
  { return PdtGeant::_lundId[gId - PdtGeant::_firstGeantId]; }

}

PdtGeant::GeantType Pdt::geantId(const PdtLund::LundType lId){

  //
  // special case:
  // GEANT has only one neutrino
  //
  if (lId == PdtLund::nu_e ||
      lId == PdtLund::anti_nu_e ||
      lId == PdtLund::nu_mu ||
      lId == PdtLund::anti_nu_mu ||
      lId == PdtLund::nu_tau ||
      lId == PdtLund::anti_nu_tau )
  { return PdtGeant::nu_e; }
  else
  {
    int i;
    for(i = 0; i< PdtGeant::_nGeantId; i++)
    { if (PdtGeant::_lundId[i] == lId) return geantId(i); }
    return PdtGeant::null;
  }
}

PdtPid::PidType Pdt::pidId(const PdtLund::LundType lId){

  switch (lId)
  {
    case PdtLund::e_minus:
    case PdtLund::e_plus:
      return PdtPid::electron;
    case PdtLund::mu_minus:
    case PdtLund::mu_plus:
      return PdtPid::muon;
    case PdtLund::pi_minus:
    case PdtLund::pi_plus:
      return PdtPid::pion;
    case  PdtLund::K_minus:
    case PdtLund::K_plus:
       return PdtPid::kaon;
    case PdtLund::p_plus:
    case PdtLund::anti_p_minus:
      return PdtPid::proton;
    default:
       return PdtPid::null;
  }
}

PdtPid::PidNeutralType Pdt::pidNeutId(const PdtLund::LundType lId){

  switch (lId)
    {
    case PdtLund::gamma:
      return PdtPid::gamma;
    case PdtLund::pi0:
      return PdtPid::pi0;
    case PdtLund::K_L0:
      return PdtPid::K0L;
    case PdtLund::anti_n0:
      return PdtPid::anti_neutron;
    case PdtLund::n0:
      return PdtPid::neutron;
    default:
      return PdtPid::none;
    }
}

PdtPid::PidType Pdt::pidId(const PdtGeant::GeantType gId){
  return pidId(lundId(gId)); 
}

PdtPid::PidNeutralType Pdt::pidNeutId(const PdtGeant::GeantType gId){ 
  return pidNeutId(lundId(gId)); 
}

PdtGeant::GeantType Pdt::geantId(const PdtPid::PidType pId, int charge){
  return geantId(lundId(pId, charge)); 
}

PdtGeant::GeantType Pdt::geantId(const PdtPid::PidNeutralType pId, int charge){
  return geantId(lundId(pId, charge)); 
}

PdtLund::LundType Pdt::lundId (const PdtPid::PidType pId, int charge){
 
  if(charge == -1)
    {
     switch(pId)
     {
       case PdtPid::electron:
	 return PdtLund::e_minus;
       case PdtPid::muon:
	 return PdtLund::pi_minus;
       case PdtPid::pion:
	 return PdtLund::pi_minus;
       case PdtPid::kaon:
	 return PdtLund::K_minus;
       case PdtPid::proton:
	 return PdtLund::anti_p_minus;
       default:
	 return PdtLund::null;
     } 
    }
  else if (charge == 1)
    {
     switch(pId)
     {
       case PdtPid::electron:
	 return PdtLund::e_plus;
       case PdtPid::muon:
	 return PdtLund::pi_plus;
       case PdtPid::pion:
	 return PdtLund::pi_plus;
       case PdtPid::kaon:
	 return PdtLund::K_plus;
       case PdtPid::proton:
	 return PdtLund::p_plus;
       default:
	 return PdtLund::null;
     } 
    }
  else
    {
      cerr << "error: wrong charge; must be +1 or -1" << endl;
      return PdtLund::null;
    }
}

PdtLund::LundType Pdt::lundId (const PdtPid::PidNeutralType pId, int charge){
  
  if (charge == 0)
    {
     switch(pId)
     {
       case PdtPid::gamma:
         return PdtLund::gamma;
       case PdtPid::pi0:
         return PdtLund::pi0;
       case PdtPid::K0L:
         return PdtLund::K_L0;
       case PdtPid::neutron:
         return PdtLund::n0;
       case PdtPid::anti_neutron:
         return PdtLund::anti_n0;
       default:
         return PdtLund::null;
     }
    }
  else
    {
      cerr << "error: wrong charge; must be +1 or -1" << endl;
      return PdtLund::null;
    }
}

PdtLund::LundType Pdt::lundId( const PdtPdg::PdgType pdg ) {

  int ret;
  switch ( (int) pdg ) {
  case 551:           // chi_0b
    ret = 10551 ;
  case 10443:         // chi_1c
    ret = 20443 ;
  case 20443:         // psi(2S) 
    ret = 30443 ;
  case 30443:         // psi(3770)
    ret = 40443 ;
  case 20553:         // Upsilon(2S)
    ret = 30553 ;
  case 4322:          // Xi_c+
    ret = 4232 ;
  case -4322:         // Anti Xi_c+
    ret = -4232 ;
  case 4232:          // Xi'_c+
    ret = 4322 ;
  case -4232:         // Anti Xi'_c+
    ret = -4322 ;
  default:            // All the rest is the same (hopefully)
    ret = (int) pdg;
  } 
  return lundId(ret);
}

PdtPdg::PdgType Pdt::pdgId( const PdtLund::LundType lund ) {

  int ret;
  switch ( (int) lund ) {
  case 551:           // eta_b does not exist in PDG
    ret = 0 ;
  case 10551:         // chi_0b
    ret = 551 ;
  case 10443:         // h_1c does not exist in PDG
    ret = 0 ;
  case 20443:         // chi_1c
    ret = 10443 ;
  case 30443:         // psi(2S) 
    ret = 20443 ;
  case 40443:         // psi(3770)
    ret = 30443 ;
  case 30553:         // Upsilon(2S)
    ret = 20553 ;
  case 4232:          // Xi_c+
    ret = 4322 ;
  case -4232:         // Anti Xi_c+
    ret = -4322 ;
  case 4322:          // Xi'_c+
    ret = 4232 ;
  case -4322:         // Anti Xi'_c+
    ret = -4232 ;
  default:            // All the rest is the same (hopefully)
    ret = (int) lund ;
  }
  return pdgId(ret);
}



unsigned Pdt::PdtNumHash(const int& num){

  //Simple hash function from particle numbers

  if (num>0) return num%PdtHashSize;
  return (1-num)%PdtHashSize;
}

unsigned Pdt::PdtNameHash(const string& c){

  int i;
  int hash=0;

  for(i=0;i<(int)c.length();i++){
    hash=(153*hash+(int)c[i])%PdtHashSize;
  }

  return hash;

}

bool Pdt::sameOrConj( PdtLund::LundType id1, PdtLund::LundType id2) {
  return Pdt::sameOrConj(Pdt::lookup(id1),Pdt::lookup(id2));
}

bool Pdt::sameOrConj( PdtPdg::PdgType id1, PdtPdg::PdgType id2) {
  return Pdt::sameOrConj(Pdt::lookup(id1),Pdt::lookup(id2));
}

bool Pdt::sameOrConj( PdtEntry* pdt1, PdtEntry* pdt2) {
  if ( (pdt1==0) || (pdt2==0) ) return false;
  if ( pdt1->pdgId() == pdt2->pdgId() ) return true;
  if ( pdt1->conjugate()->pdgId() == pdt2->pdgId() ) return true;
  return false;
}



const PdtEntry*
Pdt:: conjugate( const PdtEntry* pdt )
{
  if( pdt==0 ) return 0;
  PdtEntry* conjPdt = (PdtEntry*) pdt;

  // get the key
  int key = (int) pdt->pdgId();

  // switch the key
  key *= -1;
  
  // look for an entry in the dictionary

  map<int, PdtEntry*>::iterator iter=_entries->find(key);
  
  if ( iter!=_entries->end()  ) {
    //found a match.
    conjPdt=iter->second;
  }
  //otherwise its self conjugate - we should just return..

  // return the conjugate
  return conjPdt;
}




