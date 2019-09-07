//
// Collect MdcFastTrkAlg information and calculate acollinearity, etc.
//
// Created by Chengdong Fu, May 10,2007
//
// Change data format according EventFilter framework, June 8,2007

#ifndef EFChargedTrack_H
#define EFChargedTrack_H

//#include "GaudiKernel/Algorithm.h"

#include "HltDataTypes/CriteriaItemValue.h"
#include "HltAlgorithms/IEFAlgorithm.h"

class EFChargedTrack: public IEFAlgorithm{
 public:
  EFChargedTrack(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFChargedTrack();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  virtual void reset();

 private:
  CriteriaItemValue* m_ntrk;
  CriteriaItemValue* m_acol;
  CriteriaItemValue* m_mbal;
  CriteriaItemValue* m_pmax1;
  CriteriaItemValue* m_pmax2;
  CriteriaItemValue* m_cost1;
  CriteriaItemValue* m_cost2;
  CriteriaItemValue* m_vr;
  CriteriaItemValue* m_vz;

};
#endif//EFChargedTrack_H
