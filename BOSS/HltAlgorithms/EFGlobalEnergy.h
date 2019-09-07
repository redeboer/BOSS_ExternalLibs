//
// Collect EmcDigi information and calculate total energy, etc.
//
// Created by Chengdong Fu, May 10,2007
//
// Change data format according EventFilter framework, June 8,2007/

#ifndef EFGlobalEnergy_H
#define EFGlobalEnergy_H

#include "EmcCalibConstSvc/EmcCalibConstSvc.h"

#include "HltDataTypes/CriteriaItemValue.h"
#include "HltAlgorithms/IEFAlgorithm.h"

class EFGlobalEnergy: public IEFAlgorithm{
 public:
  EFGlobalEnergy(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFGlobalEnergy();
  virtual StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void reset();

 private:
  CriteriaItemValue* m_etot;
  CriteriaItemValue* m_ebar;
  CriteriaItemValue* m_eend;
  CriteriaItemValue* m_ebal;

  IEmcCalibConstSvc* m_emcCalibConstSvc;

};
#endif //EFGlobalEnergy_H
