//
// Collect EmcRec information and calculate acoplanarity, etc.
//
// Created by Chengdong Fu, May 10,2007
//
// Change data format according EventFilter framework, June 8,2007

#ifndef EFProcessCluster_H
#define EFProcessCluster_H

#include "HltDataTypes/CriteriaItemValue.h"
#include "HltAlgorithms/IEFAlgorithm.h"

class EFProcessCluster: public IEFAlgorithm{
 public:
  EFProcessCluster(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFProcessCluster();
  virtual StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void reset();

 private:
  CriteriaItemValue* m_nshower;
  CriteriaItemValue* m_acop;
  CriteriaItemValue* m_acole;
  CriteriaItemValue* m_emax1;
  CriteriaItemValue* m_emax2;
  CriteriaItemValue* m_emax3;
  CriteriaItemValue* m_emax12;
  CriteriaItemValue* m_coste1;
  CriteriaItemValue* m_coste2;
  CriteriaItemValue* m_coste3;
  CriteriaItemValue* m_phi1;
  CriteriaItemValue* m_phi2;
  CriteriaItemValue* m_phi3;

};
#endif //EFProcessCluster_H
