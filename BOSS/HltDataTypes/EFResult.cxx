#include <iostream>
#include "HltDataTypes/EFResult.h"

EFResult::EFResult(std::vector<std::string> ans)
  : m_answerVector(ans),
    m_answerIndex(-1), 
    m_midanswerIndex(-1),
    m_efVec()
{
}

EFResult::EFResult(std::vector<uint32_t> vec, uint32_t pos) //pos start from 0
  : m_answerVector(),
    m_answerIndex(-1),
    m_midanswerIndex(-1),
    m_efVec()
{
  if(vec.size()+pos>54) {
    std::cout <<"***EFResult:EFResult(): vector size error!" << std::endl;
    m_efVec.resize(54,0);
  }
  else{
    for(unsigned int i=0;i<54;++i){
      if(i<pos||i>=vec.size()+pos) m_efVec.push_back(0);
      else                         m_efVec.push_back(vec[i-pos]);
    }
  }
}

EFResult::EFResult():
  m_answerVector(),
  m_answerIndex(-1),
  m_midanswerIndex(-1),
  m_efVec()
{

}

EFResult::~EFResult(){}

const CLID &EFResult::classID()
{
  return CLID_EFResult; // hxt  ClassID_traits<EFResult>::ID();
}

const CLID &EFResult::clID() const
{
  return EFResult::classID();
}

const std::vector<uint32_t> EFResult::getEFVec() const
{
  return m_efVec;
}
const uint32_t EFResult::getEFVec(uint32_t pos) const
{
  if(pos>=m_efVec.size())return 0;
  else return m_efVec[pos];
}

bool EFResult::push_back(uint32_t val){
  if(m_efVec.size()>0&&m_efVec.size()<54) return false;
  if(m_efVec.size()==0) m_efVec.resize(54,0);
  m_efVec.push_back(val);
  return true;
}

bool EFResult::appToEFVec(double val, uint32_t pos)
{
  if(pos>=54) return false;
  if(m_efVec.size()!=0&&m_efVec.size()<54){
    std::cout << "EFResult::appToEFVec-->size error!" <<std::endl;
  }
  if(m_efVec.size()==0) m_efVec.resize(54,0);
  float a=val;
  float *f=(float*)&m_efVec[pos];
  *f = a;
  return true;
}

bool EFResult::addToEFVec(uint32_t val, uint32_t pos)
{
  if(pos>=54) return false;
  if(m_efVec.size()!=0&&m_efVec.size()<54){
    std::cout << "EFResult::addToEFVec-->size error!" <<std::endl;
  }
  if(m_efVec.size()==0) m_efVec.resize(54,0);

  m_efVec[pos] +=val;

  return true;
}

bool EFResult::setVecBit(uint32_t val, uint32_t vecpos,uint32_t bbegin,uint32_t bend)
{
  if(vecpos>=54) return false;
  if(bend>=32||bbegin>bend) return false;
  if(m_efVec.size()==0) m_efVec.resize(54,0);
  uint32_t bits=0xFFFFFFFF>>(31-bend+bbegin);
  m_efVec[vecpos]&=  (~((bits)<<bbegin));
  m_efVec[vecpos]|= (val&bits)<<bbegin;
  return true;
}

bool EFResult::setVecBit(bool val,uint32_t vecpos,uint32_t bitpos)
{
  if(vecpos>=54) return false;
  if(bitpos>=32) return false;
  if(m_efVec.size()==0) m_efVec.resize(54,0);
  if(val) m_efVec[vecpos]|=(1<<bitpos);
  else    m_efVec[vecpos]&=(~(1<<bitpos));
  return true;
}

void EFResult::setAnswer(std::string &answer)
{
  int iCount = 0;

  std::vector<std::string>::iterator itVs = m_answerVector.begin();
  std::vector<std::string>::iterator endVs = m_answerVector.end();
  for (;itVs != endVs && m_answerIndex == -1; ++itVs) {
    if (*itVs == answer) {
      m_answerIndex = iCount;
      
      // Store the answer index in the first data member field
      // zoujh: memory leak ... what's the use of m_efVec ?
      //m_efVec.push_back(m_answerIndex); 
    }
    iCount++;
  }
}

void EFResult::setMidAnswer(std::string &answer)
{
  int iCount = 0;
  //cout << "EFResult::setMidAnswer   "<<answer<<" ("<<m_answerVector.size()<<")"<<endl;
  std::vector<std::string>::iterator itVs = m_answerVector.begin();
  std::vector<std::string>::iterator endVs = m_answerVector.end();
  for (;itVs != endVs && m_answerIndex == -1; ++itVs) {
    if (*itVs == answer) {
      m_midanswerIndex = iCount;

      // Store the answer index in the first data member field
      // zoujh: memory leak ... what's the use of m_efVec ?
      //m_efVec.push_back(m_answerIndex);
    }
    //cout<<(*itVs)<<endl;
    iCount++;
  }
  //cout<<m_midanswerIndex<<endl;
}

void EFResult::reset()
{
  m_answerIndex = -1;
  m_midanswerIndex = -1;
  if(m_efVec.size()>=54){
    m_efVec.resize(54,0);
    for(unsigned int i=0;i<m_efVec.size();++i)  m_efVec[i]=0;
  }
  else if(m_efVec.size()!=0){
    std::cout <<"EFResult::reset(): m_efVec size check error!"<<std::endl;
  }
}

const std::string EFResult::getAnswer() const { return m_answerVector[m_answerIndex]; }

const int EFResult::getAnswerIndex() const { return m_answerIndex; }

const std::string EFResult::getMidAnswer() const { return m_answerVector[m_midanswerIndex]; }

const int EFResult::getMidAnswerIndex() const { return m_midanswerIndex; }
