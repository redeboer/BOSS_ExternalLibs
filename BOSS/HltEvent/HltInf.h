/***********************************************************
 * 
 * EventFilter information
 *   
 * ********************************************************/

#ifndef EVENT_HLT_INF_H
#define EVENT_HLT_INF_H

#include "Identifier/Identifier.h"
#include "HltEvent/DstHltInf.h"
#include <vector>

extern const CLID& CLID_HltInf;  

class HltInf : public DstHltInf{
 public:
  virtual const CLID& clID() const{ return  HltInf::classID();}
  static const CLID& classID(){return CLID_HltInf;} ;

  //ctor and dector
  HltInf(const unsigned int type, const unsigned int alg, const unsigned int cri,
	    const unsigned int ver, const float etot);
  HltInf();
  ~HltInf();

  HltInf(const DstHltInf& aHlt);
  HltInf& operator=(const DstHltInf& aHlt);
  
  //modifiers
  virtual void push_mdc(uint32_t data) { m_mdcData.push_back(data);};
  virtual void push_tof(uint32_t data) { m_tofData.push_back(data);};
  virtual void push_emc(uint32_t data) { m_emcData.push_back(data);};
  virtual void push_muc(uint32_t data) { m_mucData.push_back(data);};
  virtual void push_con(uint32_t data) { m_conData.push_back(data);};
  virtual void push_mdc(float data) { m_mdcData.push_back(*((uint32_t*)&data));};
  virtual void push_tof(float data) { m_tofData.push_back(*((uint32_t*)&data));};
  virtual void push_emc(float data) { m_emcData.push_back(*((uint32_t*)&data));};
  virtual void push_muc(float data) { m_mucData.push_back(*((uint32_t*)&data));};
  virtual void push_con(float data) { m_conData.push_back(*((uint32_t*)&data));};
  virtual bool setData(const Identifier& id, float data){return setData(id,(*((uint32_t*)&data)));};
  virtual bool setData(const Identifier&, uint32_t);
  virtual void setMdcData(const std::vector<uint32_t>& vec) {m_mdcData=vec;};
  virtual void setTofData(const std::vector<uint32_t>& vec) {m_tofData=vec;};
  virtual void setEmcData(const std::vector<uint32_t>& vec) {m_emcData=vec;};
  virtual void setMucData(const std::vector<uint32_t>& vec) {m_mucData=vec;};
  virtual void setConData(const std::vector<uint32_t>& vec) {m_conData=vec;};

  //
  std::vector<uint32_t>&  getDataVec(const Identifier&) ;
  std::vector<uint32_t>&  getDataVec(const uint32_t) ;
  virtual bool getData(const Identifier&, uint32_t*) ;

  virtual void print() const;
   
 private:
  std::vector<uint32_t>   m_mdcData;
  std::vector<uint32_t>   m_tofData;
  std::vector<uint32_t>   m_emcData;
  std::vector<uint32_t>   m_mucData;
  std::vector<uint32_t>   m_conData;
};

ostream& operator<<(ostream & os, const HltInf& aHlt);

#endif//EVENT_HLT_INF_H
