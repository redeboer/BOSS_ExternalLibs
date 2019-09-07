#ifndef Event_EvtRecPrimaryVertex_H
#define Event_EvtRecPrimaryVertex_H

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "EventModel/EventModel.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

using CLHEP::HepVector;
using CLHEP::HepSymMatrix;
using namespace EventModel;
extern const CLID & CLID_EvtRecPrimaryVertex;

// 
// For Primary Vertex information
// only 1 primary vertex for every event
// Author : Kanglin He, Min Xu    Date : 2008.11.13
//
class EvtRecPrimaryVertex : public DataObject {
 public:
  virtual const CLID& clID() const {return EvtRecPrimaryVertex::classID();}
  static  const CLID& classID() {return CLID_EvtRecPrimaryVertex;}

  EvtRecPrimaryVertex();
  ~EvtRecPrimaryVertex() {;}

  EvtRecPrimaryVertex(const EvtRecPrimaryVertex&);
  EvtRecPrimaryVertex & operator = (const EvtRecPrimaryVertex &);

  bool isValid() const {return m_isValid;}
  int nTracks() const {return m_nTracks;}
  const std::vector<int>& trackIdList() const {return m_trackIdList;}
  double chi2() const {return m_chi2;}
  int ndof() const {return m_ndof;}
  int fitMethod() const {return m_fitMethod;}
  const HepVector& vertex() const {return m_vtx;}
  const HepSymMatrix& errorVertex() const {return m_Evtx;}

  void setIsValid(bool isValid) {m_isValid = isValid;}
  void setNTracks(int nTracks) {m_nTracks = nTracks;}
  void setTrackIdList(const std::vector<int>& trackIdList) {m_trackIdList = trackIdList;}
  void setChi2(double chi2) {m_chi2 = chi2;}
  void setNdof(int ndof) {m_ndof = ndof;}
  void setFitMethod(int fitMethod) {m_fitMethod = fitMethod;}
  void setVertex(const HepVector& vtx) {m_vtx = vtx;} 
  void setErrorVertex(const HepSymMatrix& Evtx) {m_Evtx = Evtx;}

 private:
  bool m_isValid;
  int m_nTracks; //number of tracks contributing to vertex fitting
  std::vector<int> m_trackIdList; // id list of tracks contributing to vertex fitting
  double m_chi2; // chi square of vertex fitting
  int m_ndof; // degree of freedom 
              // for a fixed position => 2n; for unknown position => 2n-3;
  int m_fitMethod; // vertex fitting algorithm 
                  // 0 => global method; 1 => kalman filter method;
  HepVector m_vtx; // primary vertex position : (Vx, Vy, Vz)
  HepSymMatrix m_Evtx; // error matrix of primary vertex 
};

//typedef ObjectVector<EvtRecPrimaryVertex> EvtRecPrimaryVertexCol;
#endif
  
