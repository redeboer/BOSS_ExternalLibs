//
// Calculate time differece between top and bottom
//
// Created by Chengdong Fu, May 10,2007
//
// Change data format according EventFilter framework, June 8,2007

#ifndef EFFlightTime_H
#define EFFlightTime_H 1

#include "HltDataTypes/CriteriaItemValue.h"
#include "HltAlgorithms/IEFAlgorithm.h"
#include "HltAlgorithms/EFTofHitsCol.h"

class EFFlightTime : public IEFAlgorithm{
 public:
  EFFlightTime(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFFlightTime();
  virtual StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void reset();
 private:
  VTOF::iterator FindHit(VTOF&,const unsigned int,const unsigned int,const unsigned int);
 private:
  CriteriaItemValue* m_dtof1;
  CriteriaItemValue* m_dphi1;
  CriteriaItemValue* m_dtof2;
  CriteriaItemValue* m_dphi2;
};
#endif //EFFlightTime_H
