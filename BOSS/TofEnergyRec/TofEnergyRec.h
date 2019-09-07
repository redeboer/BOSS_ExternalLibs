//
//  Package: TofEnergyRec
//  BESIII Tof Energy Reconstruction Algorithm
//  Created by He Miao (EPC IHEP)
//
#ifndef TOF_ENERGY_REC_A_H
#define TOF_ENERGY_REC_A_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include <string>

class TofShower;

class TofEnergyRec : public Algorithm {

  public:
    TofEnergyRec(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    int m_event;
    int m_eventNb;  //for output;
    double m_calibConst; //energy calibration constant
    double m_isData; //data or MC flag
    TofShower *m_tofShower;

   
  private:
    bool m_output;
    NTuple::Tuple*  m_tuple;
    NTuple::Tuple*  m_tuple1;
    NTuple::Tuple*  m_tuple2;
};
#endif // Tof_ENERGY_REC_A_H
