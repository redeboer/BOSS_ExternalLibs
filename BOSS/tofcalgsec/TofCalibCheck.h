#ifndef TOFCALIBCHECK_H
#define TOFCALIBCHECK_H

#include "GaudiKernel/NTuple.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"

class TofCalibCheck{
 public:
  TofCalibCheck( NTuple::Tuple*& tuple );
  ~TofCalibCheck() {}

 public:
  void FillBarrelCol( RecBTofCalHitCol& );
  void FillEndcapCol( RecETofCalHitCol& );
  void FillEtfCol( RecBTofCalHitCol& );

 private:
  NTuple::Tuple*&      m_tuple;
  NTuple::Item<long>   m_run;
  NTuple::Item<long>   m_event;
  NTuple::Item<long>   m_tofid;
  NTuple::Item<long>   m_strip;
  NTuple::Item<double> m_q1;
  NTuple::Item<double> m_q2;
  NTuple::Item<double> m_t1;
  NTuple::Item<double> m_t2;
  NTuple::Item<double> m_zrhit;
  NTuple::Item<double> m_dt;
  NTuple::Item<double> m_texp;
  NTuple::Item<double> m_path;
  NTuple::Item<double> m_phi;
  NTuple::Item<double> m_theta;
  NTuple::Item<double> m_p;
  NTuple::Item<double> m_t0;
  NTuple::Item<double> m_q0;
  NTuple::Item<long>   m_hitcase;

};

#endif
