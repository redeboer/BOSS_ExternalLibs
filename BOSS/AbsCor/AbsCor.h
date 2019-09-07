#ifndef Analysis_AbsCor_H
#define Analysis_AbsCor_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "TGraphErrors.h"

class TGraph2DErrors;
class AbsCor : public Algorithm {

public:
  AbsCor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  double corEnergyPi0(double eg,double theid);
private:
  bool ntOut;
  int mccor;
  int edgecor;
  int usetof;
  int dodatacor;
  int dopi0Cor;
  int MCuseTof;
  int hotcellmask;
  int hrunstart[10];
  int hrunend[10];
  int hcell[10];


  int **m_index;
  double **m_par;
  double **m_parphi;


  double ECorrMC(double eg, double theid) const;
  double ErrMC(double eg, double theid) const;
  double E25min(int n) const;
  double E25max(int n) const;

  double e25min[28];
  double e25max[28];
  // Shower energy correction
  TGraph2DErrors *dt;
  // Energy error
  TGraph2DErrors *dtErr;




  NTuple::Tuple*  m_tuple1;
  NTuple::Item<double>  m_ef;
  NTuple::Item<double>  m_e5;
  NTuple::Item<double>  m_ct;
  NTuple::Item<double>  m_cedge;
  NTuple::Item<double>  m_ec;

};
#endif 
