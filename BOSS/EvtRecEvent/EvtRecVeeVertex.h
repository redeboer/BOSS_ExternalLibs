#ifndef Event_EvtRecVeeVertex_H
#define Event_EvtRecVeeVertex_H

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "EventModel/EventModel.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include <utility>

using CLHEP::HepVector;
using CLHEP::HepSymMatrix;
using namespace EventModel;
extern const CLID & CLID_EvtRecVeeVertex;

// 
// For "V" Type Vertex : Ks, Lambda, gamma conversion
// more than 1 vee vertex for every event
// Author : Kanglin He, Min Xu   Date: 2008.11.13
//
class EvtRecVeeVertex : virtual public ContainedObject {
 public:
  virtual const CLID& clID() const {return EvtRecVeeVertex::classID();}
  static  const CLID& classID() {return CLID_EvtRecVeeVertex;}

  EvtRecVeeVertex();
  ~EvtRecVeeVertex() {;}

  EvtRecVeeVertex(const EvtRecVeeVertex&);
  EvtRecVeeVertex & operator = (const EvtRecVeeVertex &);

  int vertexId() const {return m_vertexId;}
  int vertexType() const {return m_vertexType;}
  double chi2() const {return m_chi2;}
  int ndof() const {return m_ndof;}
  double mass() const {return m_mass;}
  const HepVector& w() const {return m_w;}
  const HepSymMatrix& Ew() const {return m_Ew;}
  const std::pair<int, int>& pair() const {return m_pair;}
  inline int pair(int i) const;
  int nCharge() const {return m_nCharge;}
  int nTracks() const {return m_nTracks;}
  std::pair<SmartRef<EvtRecTrack>, SmartRef<EvtRecTrack> >& pairDaughters() { 
    return m_daughters;
  }
  inline SmartRef<EvtRecTrack>& daughter(int i);  
 
  void setVertexId(int vtxId) {m_vertexId = vtxId;}
  void setVertexType(int vtxType) {m_vertexType = vtxType;}
  void setChi2(double chi2) {m_chi2 = chi2;}
  void setNdof(int ndof) {m_ndof = ndof;}
  void setMass(double mass) {m_mass = mass;}
  void setW(const HepVector& w) {m_w = w;} 
  void setEw(const HepSymMatrix& Ew) {m_Ew = Ew;}
  void setPair(const std::pair<int, int>& pair) {m_pair = pair;}
  void setNCharge(int nCharge) {m_nCharge = nCharge;}
  void setNTracks(int nTracks) {m_nTracks = nTracks;}
  inline void addDaughter(const SmartRef<EvtRecTrack>& track, int i);
  void setDaughter(const std::pair<SmartRef<EvtRecTrack>, SmartRef<EvtRecTrack> >& p) {
    m_daughters = p;
  }

 private:
  int m_vertexId;
  int m_vertexType; // 0 : Ks;  1 : Lambda; 2 : gamma conversion
  double m_chi2;  // chi square of vertex fitting
  int m_ndof;     // degree of freedom
  double m_mass; // mass of "V" type decay particle
  HepVector m_w; // (px, py, pz, E, x, y, z) of "V" type decay particle
  HepSymMatrix m_Ew; // error matrix
  std::pair<int, int> m_pair; // identification of daughter particles
                              // 1:e   2:mu   3:pion  4:kaon  5:proton
  int m_nCharge;  // total charges of daughter particles 
  int m_nTracks; // total tracks of daughter particles
  std::pair<SmartRef<EvtRecTrack>, SmartRef<EvtRecTrack> > m_daughters;
};

inline int EvtRecVeeVertex::pair(int i) const {
  if (i == 0) {
    return m_pair.first;
  } 
  assert(i == 1);
  return m_pair.second;
}

inline SmartRef<EvtRecTrack>& EvtRecVeeVertex::daughter(int i) {
  if (i == 0) {
    return m_daughters.first;
  }
  assert(i == 1);
  return m_daughters.second;
}

inline void EvtRecVeeVertex::addDaughter(const SmartRef<EvtRecTrack>& track, int i) {
  if (i == 0) {
    m_daughters.first = track;
  } else if (i == 1) {
    m_daughters.second = track;
  } else {
    assert(false);
  }
}

typedef ObjectVector<EvtRecVeeVertex> EvtRecVeeVertexCol;
#endif
  
