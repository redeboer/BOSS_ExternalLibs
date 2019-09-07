#ifndef Physics_Analysis_DQAJpsi2PPbarAlg_H
#define Physics_Analysis_DQAJpsi2PPbarAlg_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class ITHistSvc;
/**********************************************************************/
/** <project name> - 
 ** <description> Head file for event selection program 
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: DQAJpsi2PPbarAlg.h,v 1.2 2009/07/17 03:01:22 lixl Exp $
 **
 **
*************************************************************************/


class DQAJpsi2PPbarAlg : public Algorithm {

public:
  DQAJpsi2PPbarAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

  // Declare r0, z0 cut for charged tracks
    double m_vr0cut;
    double m_vz0cut;
    double m_vr1cut;
    double m_vz1cut;  
    double m_cthcut;

  bool   m_useKalTrk; 

  //Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaPhiCut;
  double m_gammaThetaCut;
  double m_gammaAngCut ;

  // 
  int m_test4C;
  int m_test5C;

  // 
  int m_checkDedx;
  int m_checkTof;
  
    ITHistSvc* m_thsvc;

  // define Ntuples here

  NTuple::Tuple*  m_tuple;      // charged track vertex

  NTuple::Item<long>  m_runNo  ;
  NTuple::Item<long>  m_event;
  NTuple::Item<long>  m_nchrg;
  NTuple::Item<long>  m_nneu ;
  NTuple::Item<long>  m_ngch ;

  NTuple::Array<long> m_charge ;
  NTuple::Array<double> m_vx0   ;
  NTuple::Array<double> m_vy0   ;
  NTuple::Array<double> m_vz0   ;
  NTuple::Array<double> m_vr0   ;

  NTuple::Array<double> m_vx    ;
  NTuple::Array<double> m_vy    ;
  NTuple::Array<double> m_vz    ;
  NTuple::Array<double> m_vr    ;

  NTuple::Array<double> m_px    ;
  NTuple::Array<double> m_py    ;
  NTuple::Array<double> m_pz    ;
  NTuple::Array<double> m_p     ;
  NTuple::Array<double> m_cos   ;

  NTuple::Array<double> m_bst_px    ;
  NTuple::Array<double> m_bst_py    ;
  NTuple::Array<double> m_bst_pz    ;
  NTuple::Array<double> m_bst_p     ;
  NTuple::Array<double> m_bst_cos   ;

  NTuple::Array<double> m_vtx_px    ;
  NTuple::Array<double> m_vtx_py    ;
  NTuple::Array<double> m_vtx_pz    ;
  NTuple::Array<double> m_vtx_p     ;
  NTuple::Array<double> m_vtx_cos   ;

  NTuple::Array<double> m_probPH;
  NTuple::Array<double> m_normPH;
  NTuple::Array<double> m_chie  ;
  NTuple::Array<double> m_chimu ;
  NTuple::Array<double> m_chipi ;
  NTuple::Array<double> m_chik  ;
  NTuple::Array<double> m_chip  ;
  NTuple::Array<double> m_ghit  ;
  NTuple::Array<double> m_thit  ;

  NTuple::Array<double> m_e_emc ;

  NTuple::Array<double> m_tof_b1;
  NTuple::Array<double> m_beta_b1;
  NTuple::Array<double> m_tof_b2;
  NTuple::Array<double> m_beta_b2;
  NTuple::Array<double> m_tof_e;
  NTuple::Array<double> m_beta_e;

  NTuple::Array<double>   m_cntr_etof  ;
  NTuple::Array<double>   m_ptot_etof  ;
  NTuple::Array<double>   m_ph_etof    ;
  NTuple::Array<double>   m_rhit_etof  ;
  NTuple::Array<double>   m_qual_etof  ;
  NTuple::Array<double>   m_tof_etof   ;
  NTuple::Array<double>   m_te_etof    ;
  NTuple::Array<double>   m_tmu_etof   ;
  NTuple::Array<double>   m_tpi_etof   ;
  NTuple::Array<double>   m_tk_etof    ;
  NTuple::Array<double>   m_tp_etof    ;

  NTuple::Array<double>   m_cntr_btof1 ;
  NTuple::Array<double>   m_ptot_btof1 ;
  NTuple::Array<double>   m_ph_btof1   ;
  NTuple::Array<double>   m_zhit_btof1 ;
  NTuple::Array<double>   m_qual_btof1 ;
  NTuple::Array<double>   m_tof_btof1  ;
  NTuple::Array<double>   m_te_btof1   ;
  NTuple::Array<double>   m_tmu_btof1  ;
  NTuple::Array<double>   m_tpi_btof1  ;
  NTuple::Array<double>   m_tk_btof1   ;
  NTuple::Array<double>   m_tp_btof1   ;

  NTuple::Array<double>   m_cntr_btof2 ;
  NTuple::Array<double>   m_ptot_btof2 ;
  NTuple::Array<double>   m_ph_btof2   ;
  NTuple::Array<double>   m_zhit_btof2 ;
  NTuple::Array<double>   m_qual_btof2 ;
  NTuple::Array<double>   m_tof_btof2  ;
  NTuple::Array<double>   m_te_btof2   ;
  NTuple::Array<double>   m_tmu_btof2  ;
  NTuple::Array<double>   m_tpi_btof2  ;
  NTuple::Array<double>   m_tk_btof2   ;
  NTuple::Array<double>   m_tp_btof2   ;

  NTuple::Array<double>    m_ptrk_pid ;
  NTuple::Array<double>    m_cost_pid ;
  NTuple::Array<double>    m_dedx_pid ;
  NTuple::Array<double>    m_tof1_pid ;
  NTuple::Array<double>    m_tof2_pid ;
  NTuple::Array<double>    m_prob_pi  ;
  NTuple::Array<double>    m_prob_k   ;
  NTuple::Array<double>    m_prob_p   ;

  NTuple::Item<long>   m_np ;
  NTuple::Item<long>   m_npb;

  NTuple::Item<double>   m_m2p   ;
  NTuple::Item<double>   m_angle ;
  NTuple::Item<double>   m_deltatof ;
  
  NTuple::Item<double>   m_vtx_m2p   ;
  NTuple::Item<double>   m_vtx_angle ;
  
  NTuple::Item<double>   m_chi2_4c  ;
  NTuple::Item<double>   m_m2p_4c   ;
  NTuple::Item<double>   m_angle_4c ;
  
  
};

#endif 
