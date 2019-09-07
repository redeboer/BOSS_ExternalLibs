#ifndef Physics_Analysis_TofEnergyCalib_H
#define Physics_Analysis_TofEnergyCalib_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class TofEnergyCalib : public Algorithm {

public:
  TofEnergyCalib(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:
  NTuple::Tuple*  m_tuple;    
  NTuple::Item<long>    m_npart;
  NTuple::Item<long>    m_number;
  NTuple::Item<double>  m_adc1;     //east ADC
  NTuple::Item<double>  m_adc2;     //west ADC
  NTuple::Item<double>  m_tdc1;     //east TDC
  NTuple::Item<double>  m_tdc2;     //west TDC
  NTuple::Item<double>  m_zpos;     //z position
  NTuple::Item<double>  m_length;   //track length in TOF
  NTuple::Item<double>  m_energy;   //energy deposited
  NTuple::Item<double>  m_length_ext;   //track length in TOF
  NTuple::Item<double>  m_energy_ext;   //energy deposited
  NTuple::Item<double>  m_ztdc;     //z tdc
  NTuple::Item<double>  m_q;        //q

  int m_event;
  bool m_isData;  //MC or data flag
};

#endif 
