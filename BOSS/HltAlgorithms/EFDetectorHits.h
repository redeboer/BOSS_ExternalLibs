//
// Hits number
//
// Created by Chengdong Fu, May 10,2007
//
// Change data format according EventFilter framework, June 8,2007

#ifndef EFDetectorHits_H
#define EFDetectorHits_H

#include "GaudiKernel/Algorithm.h"

#include "HltDataTypes/CriteriaItemValue.h"
#include "HltAlgorithms/IEFAlgorithm.h"

class EFDetectorHits: public IEFAlgorithm {
 public:
  EFDetectorHits(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFDetectorHits();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  virtual void reset();

 private:
  CriteriaItemValue* m_nmdc;
  CriteriaItemValue* m_nmuc;
  CriteriaItemValue* m_ntof;
  CriteriaItemValue* m_nemc;
  CriteriaItemValue* m_derr;
  CriteriaItemValue* m_rndm;

};
#endif //EFDetectorHits_H
