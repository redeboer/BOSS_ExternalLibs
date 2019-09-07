#ifndef Event_EvtRecEvent_H
#define Event_EvtRecEvent_H

#include "GaudiKernel/DataObject.h"
#include "EventModel/EventModel.h"

using namespace EventModel;

extern const  CLID  &CLID_EvtRecEvent;

class EvtRecEvent : public DataObject {

 public:

  EvtRecEvent(): m_tottks(0), m_nchrg(0), m_nneu(0), m_nvee(0), m_ndtag(0) {}
  virtual ~EvtRecEvent() {}

  virtual const CLID& clID()  const { return  EvtRecEvent::classID();}
  static  const CLID& classID() { return CLID_EvtRecEvent; }
  //        Global event info: number of track, vertex, fill after reconstruction 
  int totalTracks() const {return m_tottks;}
  void setTotalTracks(const int tottks) {m_tottks = tottks;}

  int totalCharged() const {return m_nchrg;}
  void setTotalCharged(const int nchrg) {m_nchrg = nchrg;}

  int totalNeutral() const {return m_nneu;}
  void setTotalNeutral(const int nneu) {m_nneu = nneu;}

  int numberOfVee() const {return m_nvee;}
  void setNumberOfVee(const int nvee) {m_nvee = nvee;}

  int numberOfPi0() const {return m_npi0;}
  void setNumberOfPi0(const int npi0) {m_npi0 = npi0;}

  int numberOfDTag() const {return m_ndtag;}
  void setNumberOfDTag(const int ndtag) {m_ndtag = ndtag;}

 private:
  // Global event info: number of track, vertex in events
  int m_tottks; // total tracks
  int m_nchrg;  // total charged tracks
  int m_nneu;   // total neutral tracks
  int m_nvee;   // number of VEEs: Ks, Lambda, gamma conversion
   int m_npi0;  // number of Pi0
  int m_ndtag;  // number of DTags 

};
#endif

