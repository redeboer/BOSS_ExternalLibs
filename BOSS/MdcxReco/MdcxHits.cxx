//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxHits.cxx,v 1.8 2009/12/16 08:59:13 zhangy Exp $
//
// Description: 
//	Class |MdcxHits| - container for |MdcxHit|s
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//      S. Wagner
//      Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
// Copyright Information:
//	Copyright (C) 1994	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include "MdcxReco/MdcxHits.h"
#include "CLHEP/Alist/AIterator.h"
#include "MdcData/MdcHit.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

using std::cout;
using std::endl;
using std::ostream;

double MdcxHits::timecut = -50.0;

MdcxHits::MdcxHits() : m_debug(0) { }
/*
MdcxHits::MdcxHits(MdcDigiCol* mdcDigiCol, float c0,  float cresol)
  : m_debug(0) {

  int ij = mdcDigiCol->size(); 
  if (ij <= 0) return;
  MdcDigi* theDigi;
  MdcDigiCol::iterator iter = mdcDigiCol->begin();
  for (; iter != mdcDigiCol->end(); iter++ ) {
    theDigi = (*iter);
    MdcxHit* temp = new MdcxHit(theDigi, c0, cresol);
    mdcxHitList.append(temp);
  }
  return;
}
*/

MdcxHits::~MdcxHits() { KillList(); }

void MdcxHits::reset() {
  HepAListDeleteAll(mdcxHitList);
}

void MdcxHits::create(MdcDigiVec mdcDigiVec, float c0, float cresol) {
  MdcDigiVec::iterator iter = mdcDigiVec.begin();
  for (; iter != mdcDigiVec.end(); iter++ ) {
    const MdcDigi* aDigi = *iter;
    MdcxHit* temp = new MdcxHit(aDigi, c0, cresol);
    //const Identifier id= aDigi->identify();
    //int layer = MdcID::layer(id);
    //int wire = MdcID::wire(id);
    //std::cout<< " ("<<layer<<","<<wire<<") "<<aDigi->getTimeChannel()<<" "<<aDigi->getChargeChannel() << std::endl;//yzhang debug
    mdcxHitList.append(temp);
  }    
}

void MdcxHits::print(ostream &o, int pmax) const {
  int mcheck = pmax;
  if (mdcxHitList.length() < pmax) mcheck = mdcxHitList.length();
  o << " First " << mcheck << " Main Drift Chamber Hits:" << endl;
  for(int i = 0; i < mcheck; i++) {
    mdcxHitList[i]->print(o, i);
  }
}
