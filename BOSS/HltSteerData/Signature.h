#ifndef _SIGNATURE_
#define _SIGNATURE_

#include <utility>
#include <map>
#include <vector>
#include <string>
#include <list>
#include "HltTools/MessageSvcProvider.h"
#include "HltSteerData/HltCriteria.h"

namespace HltProcessor {

class Signature
{
 public:
  Signature();
  Signature(std::vector<HltCriteria*>);
  virtual ~Signature();
  int id() const;
  void setId(int); 
  void print();
  void makeVectors(HltCriteria* criteria);
  //inline const std::vector<HltCriteria*>& criteriaVector() const{
  //  return m_criteriaVec;
  //}
  bool isVeto() const;
  void setVeto(bool);
  float forcedAccept() const;
  void setForcedAccept(float);
  int prescale() const;
  void setPrescale(int);
  int predecessorId();
  int successorId();
  void setPredecessorId(int);
  void setSuccessorId(int);
  void setSigLabel(std::string label);
  std::string label() const;
 
  /// For trigger
  int isCriteriaMatched();
  const std::string& result() const {
    return m_result;
  }
  const std::string& midresult() const {
    return m_midMark;
  }

 private:
  std::vector<HltCriteria *> m_criteriaVec;
  int m_SignaturePrescale;
  float m_SignatureForcedRate;
  bool m_SignatureVeto;
  std::string m_Sig_label;
  std::string m_result;
  std::string m_midMark;

  int m_Id; 
  int m_Predecessor_Id; 
  int m_Successor_Id; 
  MessageSvcProvider* m_msgSvcProvider;
};
}
#endif
