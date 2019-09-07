#ifndef Analysis_McCor_H
#define Analysis_McCor_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "TGraphErrors.h"

class McCor : public Algorithm {

public:
  McCor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  double corEnergyMc(double eg,double theid);
private:
  bool ntOut;

  NTuple::Tuple*  m_tuple1;
  NTuple::Item<double>  m_ef;
  NTuple::Item<double>  m_e5;
  NTuple::Item<double>  m_ct;
  NTuple::Item<double>  m_ec;

};
#endif 
