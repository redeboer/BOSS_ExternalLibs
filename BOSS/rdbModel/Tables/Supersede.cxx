// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Supersede.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Set.h"
#include "rdbModel/Tables/Supersede.h"
#include "rdbModel/RdbException.h"

namespace rdbModel {

  Supersede::Supersede(Table* table, Assertion* onlyIf) : 
    m_myTable(table), m_onlyIf(onlyIf), m_normalized(false) {
    m_setOld.clear();
    m_setNew.clear();
    m_oldDefaults.clear();
    m_oldForced.clear();
    m_ask.clear();
    m_fixed.clear();
  }

  void Supersede::addSet(Set* s) {
    FIELDTYPE vtype = s->getDestType();
    switch(vtype) {
    case FIELDTYPEold: 
    case FIELDTYPEoldDef:    // this shouldn't occur
      m_setOld.push_back(s); break;
    case FIELDTYPEtoBe: 
    case FIELDTYPEtoBeDef:    // nor this
      m_setNew.push_back(s); break;
    default: throw RdbException("Bad value type for <set> destination");
    }
    return;
  }


  void Supersede::normalize() {
    if (m_normalized) return;
    //    unsigned nOld = m_setOld.size();
    unsigned nNew = m_setNew.size();
    // For each object in m_setNew, store information in one of the 
    // vectors of column names or in m_fixed for easy access later
    for (unsigned iNew = 0; iNew < nNew; iNew++) {
      //      FIELDTYPE srcType = m_setOld[iOld]->getSrcType();
      //      switch (srcType) {
      switch (m_setNew[iNew]->getSrcType()) {
      case FIELDTYPEold: 
        m_oldForced.push_back(m_setNew[iNew]->getDestColName());
        break;
      case FIELDTYPEoldDef: 
        m_oldDefaults.push_back(m_setNew[iNew]->getDestColName());
        break;
      case FIELDTYPEask:
        m_ask.push_back(m_setNew[iNew]->getDestColName());
        break;
      case FIELDTYPElit:
        m_fixed.push_back(FieldVal(m_setNew[iNew]->getDestColName(),
                                   m_setNew[iNew]->getSrcValue()));
        m_fixedInterp.push_back(m_setNew[iNew]->getInterp());
        break;
      default:
        throw RdbException("Supersede::normalize Unrecognized <set>");
      }
    }

    m_fromOld.reserve(m_oldDefaults.size() + m_oldForced.size());
    m_fromOld.clear();
    for (unsigned i = 0; i < m_oldDefaults.size(); i++) {
      m_fromOld.push_back(m_oldDefaults[i]);
    }
    for (unsigned i = 0; i < m_oldForced.size(); i++) {
      m_fromOld.push_back(m_oldForced[i]);
    }
    m_normalized = true;
  }


  Supersede::~Supersede() {
    while (m_setOld.size() ) {
      Set* s  = m_setOld.back();
      m_setOld.pop_back();
      delete s;
    }
    while (m_setNew.size() ) {
      Set* s  = m_setNew.back();
      m_setNew.pop_back();
      delete s;
    }
    //    delete m_onlyIf;    probably already handled by ~Table
  }
  // For now assume visitSupersede will handle everything. Don't need
  // separate visitSet method.
  Visitor::VisitorState Supersede::accept(Visitor* v) {
    Visitor::VisitorState state = v->visitSupersede(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;
    return state;
  }

  
}
