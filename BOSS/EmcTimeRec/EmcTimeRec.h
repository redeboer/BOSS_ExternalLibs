//
//  Emc Time Rec
//  
//  Created by He Miao, 2009-03-17
//
#ifndef EMC_TIME_REC_A_H
#define EMC_TIME_REC_A_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class EmcTimeRec:public Algorithm {

 public:
   EmcTimeRec(const std::string& name, ISvcLocator* pSvcLocator);
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

 private:
   bool m_output;
   NTuple::Tuple*  m_tuple;
   NTuple::Item<float> m_nt_emcTime;
   NTuple::Item<float> m_nt_estTime;
   NTuple::Item<float> m_nt_corTime;
   NTuple::Item<long>  m_nt_npart;
   NTuple::Item<long>  m_nt_ntheta;
   NTuple::Item<long>  m_nt_nphi;
   NTuple::Item<float> m_nt_e5x5;
   NTuple::Item<long>  m_nt_status;
   
};

#endif /// EMC_TIME_REC_A_H

