#ifndef Event_EvtRecTrack_H
#define Event_EvtRecTrack_H

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "EventModel/EventModel.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "ExtEvent/RecExtTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "MucRecEvent/RecMucTrack.h"

using namespace EventModel;

extern const CLID &CLID_EvtRecTrack;

class EvtRecTrack:virtual public ContainedObject {
 public:
  virtual const CLID& clID() const {return  EvtRecTrack::classID();}
  static  const CLID& classID() { return CLID_EvtRecTrack;}
  
  EvtRecTrack();
  ~EvtRecTrack() {;}
  
  EvtRecTrack(const EvtRecTrack&);
  
  //get Track ID, Particle ID and Data Quality
  int trackId() const {return m_trackId;}
  int partId() const {return m_partId;}
  int quality() const { return m_quality; }

  bool isElectron() const { return (m_partId & 0x01); }
  bool isMuon()     const { return (m_partId & 0x02); }
  bool isPion()     const { return (m_partId & 0x04); }
  bool isKaon()     const { return (m_partId & 0x08); }
  bool isProton()   const { return (m_partId & 0x10); }

  //Sub detector infomation is Valid or not
  bool isMdcTrackValid() {return (m_originMdcTrack != 0);}
  bool isMdcKalTrackValid() {return (m_originMdcKalTrack != 0);}
  bool isMdcDedxValid() {return (m_originMdcDedx != 0);}
  bool isTofTrackValid() {return (!m_originTofTrack.empty());}
  bool isEmcShowerValid() {return (m_originEmcShower!= 0);}
  bool isMucTrackValid() {return (m_originMucTrack != 0);}
  bool isExtTrackValid() {return (m_originExtTrack != 0);}
  
  //pointer to sub detector infomation
  
  RecMdcTrack* mdcTrack()  {return m_originMdcTrack;}
  RecMdcKalTrack* mdcKalTrack()  {return m_originMdcKalTrack;}
  RecMdcDedx*  mdcDedx()  {return m_originMdcDedx;}
  RecExtTrack* extTrack()  {return m_originExtTrack;}
  SmartRefVector<RecTofTrack> tofTrack()  {return m_originTofTrack;}
  RecEmcShower* emcShower()  {return m_originEmcShower;}
  RecMucTrack* mucTrack()  {return m_originMucTrack;}


  // set Track ID, Particle ID and Data Quality

  void setTrackId(const int trkId) {m_trackId = trkId;}
  void setPartId(const int partId) {m_partId = partId;}
  void setQuality(const int quality) { m_quality = quality; }

  void tagElectron() { m_partId |= 0x01; }
  void tagMuon()     { m_partId |= 0x02; }
  void tagPion()     { m_partId |= 0x04; }
  void tagKaon()     { m_partId |= 0x08; }
  void tagProton()   { m_partId |= 0x10; }
  
  // set sub detector pointer
  
  void setMdcTrack(const RecMdcTrack* trk) {m_originMdcTrack = trk;}
  void setMdcKalTrack(const RecMdcKalTrack* trk) {m_originMdcKalTrack = trk;}
  void setMdcDedx(const RecMdcDedx* trk) {m_originMdcDedx = trk;}
  void setExtTrack(const RecExtTrack* trk) {m_originExtTrack = trk;}
  void setTofTrack(const SmartRefVector<RecTofTrack> trk) {m_originTofTrack = trk;}
  void addTofTrack(const SmartRef<RecTofTrack> trk) {m_originTofTrack.push_back(trk);}
  void setEmcShower(const RecEmcShower* shower) {m_originEmcShower = shower;}
  void setMucTrack(const RecMucTrack* trk) {m_originMucTrack = trk;}

  // assign operator
		
  EvtRecTrack & operator = (const EvtRecTrack &);

		
		
 private:

  int m_trackId;
  // Pid: 0x01 - electron, 0x02 - muon, 0x04 - pion, 0x08 - kaon, 0x10 - proton
  int m_partId;
  //data quality: 1 - very good, 2 - available, 3 - normal
  int m_quality;

  SmartRef<RecMdcTrack> m_originMdcTrack;
  SmartRef<RecMdcKalTrack> m_originMdcKalTrack;
  SmartRef<RecMdcDedx> m_originMdcDedx;
  SmartRef<RecExtTrack> m_originExtTrack;
  SmartRefVector<RecTofTrack> m_originTofTrack;
  SmartRef<RecEmcShower> m_originEmcShower;
  SmartRef<RecMucTrack> m_originMucTrack;

};

typedef ObjectVector<EvtRecTrack> EvtRecTrackCol;

typedef EvtRecTrackCol::iterator EvtRecTrackIterator;



#endif
