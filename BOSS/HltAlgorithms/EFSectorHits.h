//
// Count hits number of certain sector in MDC, for Bhabha
//
// Created by Chengdong Fu, June 8,2007
//

#ifndef EFSectorHits_H
#define EFSectorHits_H

#include "HltDataTypes/CriteriaItemValue.h"
#include "HltAlgorithms/IEFAlgorithm.h"

class EFSectorHits: public IEFAlgorithm{
 public:
  EFSectorHits(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFSectorHits();
  virtual StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void reset();
 private:
  bool WhetherSector(float,float=0.,float=6.2831852);
 private:
  CriteriaItemValue* m_nsec;
  static int idmax[43];

};
#endif //EFSectorHits_H
