#include "HltEvent/DstHltInf.h"

const std::string DstHltInf::s_chn[32]={
  "Junk","GJunk","Beamgas","GBeamgas",
  "Cosmic","GCosmic","EBhabha","GEBhabha",
  "BBhabha","GBBhabha","Dimuon","GDimuon",
  "Diphoton","GDiphoton","Hadron","GHadron",
  "Twophoton","Twophoton","","",
  "","","","",
  "","EMCEBhabha","","",
  "","Other","Error","RandomTrg"};
const std::string DstHltInf::s_err="EventFilterError";

DstHltInf::DstHltInf(const unsigned int type, const unsigned int alg, 
		     const unsigned int cri,  const unsigned int ver, const float etot):
  DataObject(),IHltInterface(),
  m_eventType(type),
  m_algProcess(alg),
  m_criTable(cri),
  m_version(ver),
  m_eTotal(etot),
  m_number(0),
  m_ncon(0){
}

DstHltInf::DstHltInf():
  DataObject(),IHltInterface(),
  m_eventType(0),
  m_algProcess(0),
  m_criTable(0),
  m_version(0),
  m_eTotal(0),
  m_number(0),
  m_ncon(0){
}

DstHltInf::DstHltInf(const DstHltInf& other):
  DataObject(),IHltInterface(),
  m_eventType(other.m_eventType),
  m_algProcess(other.m_algProcess),
  m_criTable(other.m_criTable),
  m_version(other.m_version),
  m_eTotal(other.m_eTotal),
  m_number(other.m_number),
  m_ncon(other.m_ncon) {
}

DstHltInf& DstHltInf::operator=(const DstHltInf& h) {
  if( &h != this ){
    m_eventType =h.m_eventType;
    m_algProcess=h.m_algProcess;
    m_criTable  =h.m_criTable;
    m_version   =h.m_version;
    m_eTotal    =h.m_eTotal;
    m_number    =h.m_number;
    m_ncon      =h.m_ncon;
  }
  return *this;
}

const string& DstHltInf::getEventName() const{
  for(int i=0;i<32;i++){
    if(m_eventType&(1<<i)) return (s_chn[i]);
  }
  return s_err;
}

bool DstHltInf::isType(string& type) const{
  for(int i=0;i<32;i++){
    if(type!=s_chn[i]) continue;
    if(m_eventType&(1<<i))return true;
  }
  return false;
}

ostream& operator<<(ostream & os, const DstHltInf& aHlt){
  os<<"-----DstHltInf-----:"<<endl;
  os<<"EventType:  "<<aHlt.getEventType()<<endl
    <<"AlgProcess: "<<aHlt.getAlgProcess()<<endl
    <<"CriteriaTab:"<<aHlt.getCriteriaTable()<<endl
    <<"EFVersion:  "<<aHlt.getVersion()<<endl
    <<"TotalEnergy:"<<aHlt.getTotalEnergy()<<endl;
  return os;
}
