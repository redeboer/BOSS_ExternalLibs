#include "Identifier/HltID.h"
#include "HltEvent/HltInf.h"
#include <iostream>

HltInf::HltInf(const unsigned int type, const unsigned int alg, 
		     const unsigned int cri,  const unsigned int ver, const float etot):
  DstHltInf(type,alg,cri,ver,etot),
  m_mdcData(0),m_tofData(0),m_emcData(0),m_mucData(0),m_conData(0){
}

HltInf::HltInf():
  DstHltInf(),
  m_mdcData(0),m_tofData(0),m_emcData(0),m_mucData(0),m_conData(0){
}

HltInf::~HltInf(){
}

HltInf::HltInf(const DstHltInf& aHlt):
  DstHltInf(aHlt),
  m_mdcData(0),m_tofData(0),m_emcData(0),m_mucData(0),m_conData(0){
}

HltInf& HltInf::operator=(const DstHltInf& aHlt){
  DstHltInf::operator=(aHlt);
  return *this;
}

bool HltInf::setData(const Identifier& id, uint32_t data){
 
  vector<uint32_t>::iterator iter;
  uint32_t size=0;

  if(HltID::is_mdc_inf(id)) {
    iter = m_mdcData.begin();
    size = m_mdcData.size();
  }
  else if(HltID::is_tof_inf(id)) {
    iter = m_tofData.begin();
    size = m_tofData.size();
  }
  else if(HltID::is_emc_inf(id)) {
    iter = m_emcData.begin();
    size = m_emcData.size();
  }
  else if(HltID::is_muc_inf(id)) {
    iter = m_mucData.begin();
    size = m_mucData.size();
  }
  else if(HltID::is_con_inf(id)) {
    iter = m_conData.begin();
    size = m_conData.size();
  }
  else              return false;
 
  uint32_t sub=HltID::id_sub(id);

  if(sub<size) {
    (*(iter+sub))=data;
    return true;
  }
  else {
    return false;
  }
}

vector<uint32_t>& HltInf::getDataVec(const Identifier& id) {
  if(HltID::is_mdc_inf(id))      return m_mdcData;
  else if(HltID::is_tof_inf(id)) return m_tofData;
  else if(HltID::is_emc_inf(id)) return m_emcData;
  else if(HltID::is_muc_inf(id)) return m_mucData;
  else if(HltID::is_con_inf(id)) return m_conData;
  else               return m_conData;  //default: return condition number 
}

vector<uint32_t>& HltInf::getDataVec(const uint32_t part) {
  return getDataVec(HltID::data_type_id(part));                                        
}

bool HltInf::getData(const Identifier& id, uint32_t* address) {

  vector<uint32_t>::iterator iter;
  uint32_t size=0;
 
  if(HltID::is_mdc_inf(id)) {
    iter = m_mdcData.begin();
    size = m_mdcData.size();
  }
  else if(HltID::is_tof_inf(id)) {
    iter = m_tofData.begin();
    size = m_tofData.size();
  }
  else if(HltID::is_emc_inf(id)) {
    iter = m_emcData.begin();
    size = m_emcData.size();
  }
  else if(HltID::is_muc_inf(id)) {
    iter = m_mucData.begin();
    size = m_mucData.size();
  }
  else if(HltID::is_con_inf(id)) {
    iter = m_conData.begin();
    size = m_conData.size();
  }
  else              return false;

  uint32_t sub=HltID::id_sub(id);

  if(sub<size) {
    *address = *(iter+sub);
    return true;
  }
  else {
    return false;
  }
}

void HltInf::print() const{
  std::cout << *this;
  std::cout << "MDC data:" << dec << m_mdcData.size() << std::endl;
  for(uint32_t i=0;i<m_mdcData.size();i++){
    std::cout << "   #"<<i<<"# ";
    std::cout << *((int*)&m_mdcData[i]) << "(" << *((float*)&m_mdcData[i]) << ")";
    if(i==m_mdcData.size()-1) std::cout << std::endl;		   
  }
  std::cout << "TOF data:" << m_tofData.size() << std::endl;
  for(uint32_t i=0;i<m_tofData.size();i++){
    std::cout << "   #"<<i<<"# ";
    std::cout << *((int*)&m_tofData[i]) << "(" << *((float*)&m_tofData[i]) << ")";
    if(i==m_tofData.size()-1) std::cout << std::endl;
  }
  std::cout << "EMC data:" << m_emcData.size() << std::endl;
  for(uint32_t i=0;i<m_emcData.size();i++){
    std::cout << "   #"<<i<<"# ";
    std::cout << *((int*)&m_emcData[i]) << "(" << *((float*)&m_emcData[i]) << ")";
    if(i==m_emcData.size()-1) std::cout << std::endl;
  }
    std::cout << "MUC data:" << m_mucData.size() << std::endl;
  for(uint32_t i=0;i<m_mucData.size();i++){
    std::cout << "   #"<<i<<"# ";
    std::cout << *((int*)&m_mucData[i]) << "(" << *((float*)&m_mucData[i]) << ")";
    if(i==m_mucData.size()-1) std::cout << std::endl;
  }
  std::cout << "CON data:" << m_conData.size() << std::endl;
  for(uint32_t i=0;i<m_conData.size();i++){
    std::cout << "   #"<<i<<"# ";
    std::cout << *((int*)&m_conData[i]);
    if(i==m_conData.size()-1) std::cout << std::endl;
  }
  return;
}

ostream& operator<<(ostream & os, const HltInf& aHlt){
  os<<"-----HltInf-----:"<<endl;
  os<<"EventType:  "<<hex<<aHlt.getEventType()<<endl
    <<"AlgProcess: "<<aHlt.getAlgProcess()<<endl
    <<"CriteriaTab:"<<aHlt.getCriteriaTable()<<endl
    <<"EFVersion:  "<<aHlt.getVersion()<<endl
    <<"TotalEnergy:"<<aHlt.getTotalEnergy()<<endl;
  return os;
}
