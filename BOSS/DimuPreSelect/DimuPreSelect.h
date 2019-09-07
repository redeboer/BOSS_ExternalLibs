#ifndef Physics_analysis_DimuPreSelect_H
#define Physics_Analysis_DimuPreSelect_H

//#include "GaudiKnernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
class DimuPreSelect : public Algorithm
{
 public:
   DimuPreSelect(const std::string& name, ISvcLocator* pSvcLocator);
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
 private:
//  event selection
   bool m_selectFlag; // Flag of use DimuPreSelect or not
   bool m_output;
   bool m_mdcPass, m_tofPass, m_emcPass, m_mucPass; // for subdetector flag
   bool m_useFlag[4];  //MDC, TOF, EMC, MUC, for configuration
   bool m_passFlag[4]; //MDC, TOF, EMC, MUC, for filtering output

   long m_totevent;   
   long m_currun;
   long m_curevent;
   double m_ecms;
   double m_vr0cut;
   double m_vz0cut;
   double m_pcut_up;
   double m_pcut_down;
   double m_psymcut;
   double m_tcut;
   double m_ecut_up;
   double m_ecut_down;
   double m_dthetacut;
   double m_dphicut;
   double m_partselect;  //0: all, 1 : barrel only, 2 : endcap only
   double m_mudigicut;
   double m_mutrkcut;
   
   double m_cutpass[20];
   double m_subpass[4];
   double m_totalpass;
   
// Event selection
   NTuple::Tuple*  m_passtuple;   
   NTuple::Item<double>  m_run;
   NTuple::Item<double>  m_event;
   NTuple::Item<double>  m_part; // 1: barrel, 0: endcap
   NTuple::Item<double>  m_c1;
   NTuple::Item<double>  m_c2;
   NTuple::Item<double>  m_r1;
   NTuple::Item<double>  m_r2;
   NTuple::Item<double>  m_z1;
   NTuple::Item<double>  m_z2;
   NTuple::Item<double>  m_p1;
   NTuple::Item<double>  m_p2;
   NTuple::Item<double>  m_t1;
   NTuple::Item<double>  m_t2;
   NTuple::Item<double>  m_e1;
   NTuple::Item<double>  m_e2;
   NTuple::Item<double>  m_theta1;
   NTuple::Item<double>  m_theta2;
   NTuple::Item<double>  m_phi1;
   NTuple::Item<double>  m_phi2;
   NTuple::Item<double>  m_mudigi;
   NTuple::Item<double>  m_mutrk;
   NTuple::Item<double>  m_zeroCFlag;
   NTuple::Item<double>  m_vtRZFlag;
   NTuple::Item<double>  m_pLimFlag;
   NTuple::Item<double>  m_pSymFlag;
   NTuple::Item<double>  m_tLimFlag;
   NTuple::Item<double>  m_eLimFlag;
   NTuple::Item<double>  m_eBBFlag;
   NTuple::Item<double>  m_partFlag;
   NTuple::Item<double>  m_mudigiFlag;
   NTuple::Item<double>  m_mutrkFlag;
   NTuple::Item<double>  m_mdcFlag;
   NTuple::Item<double>  m_tofFlag;
   NTuple::Item<double>  m_emcFlag;
   NTuple::Item<double>  m_mucFlag;
   
};
#endif
