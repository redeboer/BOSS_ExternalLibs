#ifndef Physics_analysis_BbEmc_H
#define Physics_Analysis_BbEmc_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
class BbEmc : public Algorithm
{
 public:
   BbEmc(const std::string& name, ISvcLocator* pSvcLocator);
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
 private:
   //control flag
   bool m_hist;
   bool m_trigger_flag;
   bool m_hlt_flag;
   bool m_est_flag;
   bool m_kalTrk_flag;
   //  event selection cut
   double m_energy_cut;
   double m_mom_cut;
   double m_dangCut;
   double m_vr0cut;
   double m_vz0cut;
   
   NTuple::Tuple *m_tuple1;
   NTuple::Item<long>  m_run;
   NTuple::Item<long>  m_event;
   NTuple::Item<long>  m_num_Ctrk;
   NTuple::Item<long>  m_num_Ntrk;
   NTuple::Item<long>  m_num_Gtrk;
   NTuple::Item<long>  m_index;
   NTuple::Array<double>  m_ene;
   NTuple::Array<double>  m_theta;
   NTuple::Array<double>  m_phi;
   NTuple::Item<double>  m_dang;

   NTuple::Item<long>  m_trig_index;
   NTuple::Array<long>  m_trig_cond;
   NTuple::Array<long> m_trig_chan;
   NTuple::Item<long> m_trig_timewindow;
   NTuple::Item<long> m_trig_timetype;

   NTuple::Item<double>  m_est_start;
   NTuple::Item<long>  m_est_status;
   NTuple::Item<double>  m_est_quality;
   NTuple::Item<long> m_hlt_type;
};
#endif
