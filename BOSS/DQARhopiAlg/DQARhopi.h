#ifndef Physics_Analysis_DQARhopi_H
#define Physics_Analysis_DQARhopi_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class ITHistSvc;

class DQARhopi : public Algorithm {

public:
  DQARhopi(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

    int Ncut0,Ncut1,Ncut2,Ncut3,Ncut4,Ncut5,Ncut6,Ncut7,Ncut8,Ncut9,Ncut10;

  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;
  double m_cthcut;

  //Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaAngCut;

  // 
  int m_test4C;
  int m_test5C;

  // 
  int m_checkDedx;
  int m_checkTof;

    ITHistSvc* m_thsvc;

  NTuple::Tuple*  m_tuple4;     // rhopi 4C
  NTuple::Item<long>  m_run;
  NTuple::Item<long>  m_rec;
  NTuple::Item<long>  m_nch;
  NTuple::Item<long>  m_nneu;  
  NTuple::Item<double>  m_chi1;
  NTuple::Item<double>  m_mpi0;
  NTuple::Item<double>  m_prho0;
  NTuple::Item<double>  m_prhop;
  NTuple::Item<double>  m_prhom;
  NTuple::Item<long>    m_good;
  NTuple::Item<long>    m_gam;
  NTuple::Item<long>    m_pip;
  NTuple::Item<long>    m_pim;
  NTuple::Item<double>  m_2gam;
  NTuple::Item<double>  m_outpi0;
  NTuple::Item<double>  m_cosang;
  NTuple::Item<double>  m_outpip;
  NTuple::Item<double>  m_outpim;
  NTuple::Item<double>  m_enpip;
  NTuple::Item<double>  m_dcpip;
  NTuple::Item<double>  m_enpim;
  NTuple::Item<double>  m_dcpim;
  NTuple::Item<double>  m_pipf;
  NTuple::Item<double>  m_pimf;
  NTuple::Item<double>  m_pi0f;
  NTuple::Item<long>    m_nangecc;
  NTuple::Array<double> m_dthec;
  NTuple::Array<double> m_dphic;
  NTuple::Array<double> m_dangc;
  NTuple::Array<double> m_mspippim;
   
  NTuple::Item<double>  m_pmax;
  NTuple::Item<double>  m_ppx;
  NTuple::Item<double>  m_ppy;
  NTuple::Item<double>  m_ppz;
  NTuple::Item<double>  m_costhep;
  NTuple::Item<double>  m_ppxkal;
  NTuple::Item<double>  m_ppykal;
  NTuple::Item<double>  m_ppzkal;
  NTuple::Item<double>  m_mpx;
  NTuple::Item<double>  m_mpy;
  NTuple::Item<double>  m_mpz;
  NTuple::Item<double>  m_costhem;
  NTuple::Item<double>  m_mpxkal;
  NTuple::Item<double>  m_mpykal;
  NTuple::Item<double>  m_mpzkal;
  NTuple::Item<double>  m_vxpin;
  NTuple::Item<double>  m_vypin;
  NTuple::Item<double>  m_vzpin;
  NTuple::Item<double>  m_vrpin;
  NTuple::Item<double>  m_costhepin;  
  NTuple::Item<double>  m_vxmin;
  NTuple::Item<double>  m_vymin;
  NTuple::Item<double>  m_vzmin;      
  NTuple::Item<double>  m_vrmin;
  NTuple::Item<double>  m_costhemin;  
  
  NTuple::Item<double>  m_vxp;
  NTuple::Item<double>  m_vyp;
  NTuple::Item<double>  m_vzp;
  NTuple::Item<double>  m_vrp;
  NTuple::Item<double>  m_vxm;
  NTuple::Item<double>  m_vym;
  NTuple::Item<double>  m_vzm;      
  NTuple::Item<double>  m_vrm;
  
  NTuple::Item<double>  dangsg;
  NTuple::Item<double>  dthesg;
  NTuple::Item<double>  dphisg;
  NTuple::Item<double>  cosgth1;
  NTuple::Item<double>  cosgth2;
   
  NTuple::Item<double>  m_chi5;
  NTuple::Item<double>  m_kpi0;
  NTuple::Item<double>  m_kpkm;
  NTuple::Item<double>  m_kpp0;
  NTuple::Item<double>  m_kmp0;
  NTuple::Item<double>  m_pgam2pi1;
  NTuple::Item<double>  m_pgam2pi2;
  NTuple::Item<double>  cosva1;
  NTuple::Item<double>  cosva2;
  NTuple::Item<double>  m_laypi1;
  NTuple::Item<double>  m_hit1; 
  NTuple::Item<double>  m_laypi2;
  NTuple::Item<double>  m_hit2; 
  NTuple::Item<double>  m_anglepm;

  NTuple::Item<long>    m_ngch;
  NTuple::Array<double> m_ptrk;
  NTuple::Array<double> m_chie;
  NTuple::Array<double> m_chimu;
  NTuple::Array<double> m_chipi;
  NTuple::Array<double> m_chik;
  NTuple::Array<double> m_chip;
  NTuple::Array<double> m_probPH;
  NTuple::Array<double> m_normPH;
  NTuple::Array<double> m_ghit;
  NTuple::Array<double> m_thit;

  NTuple::Array<double> m_ptot_etof;
  NTuple::Array<double> m_cntr_etof;
  NTuple::Array<double> m_te_etof;
  NTuple::Array<double> m_tmu_etof;
  NTuple::Array<double> m_tpi_etof;
  NTuple::Array<double> m_tk_etof;
  NTuple::Array<double> m_tp_etof;
  NTuple::Array<double> m_ph_etof;
  NTuple::Array<double> m_rhit_etof;
  NTuple::Array<double> m_qual_etof;
  NTuple::Array<double> m_ec_toff_e;
  NTuple::Array<double> m_ec_toff_mu;
  NTuple::Array<double> m_ec_toff_pi;
  NTuple::Array<double> m_ec_toff_k;
  NTuple::Array<double> m_ec_toff_p;
  NTuple::Array<double> m_ec_tsig_e;
  NTuple::Array<double> m_ec_tsig_mu;
  NTuple::Array<double> m_ec_tsig_pi;
  NTuple::Array<double> m_ec_tsig_k;
  NTuple::Array<double> m_ec_tsig_p;
  NTuple::Array<double> m_ec_tof;

  NTuple::Array<double> m_ptot_btof1;
  NTuple::Array<double> m_cntr_btof1;
  NTuple::Array<double> m_te_btof1;
  NTuple::Array<double> m_tmu_btof1;
  NTuple::Array<double> m_tpi_btof1;
  NTuple::Array<double> m_tk_btof1;
  NTuple::Array<double> m_tp_btof1;
  NTuple::Array<double> m_ph_btof1;
  NTuple::Array<double> m_zhit_btof1;
  NTuple::Array<double> m_qual_btof1;
  NTuple::Array<double> m_b1_toff_e;
  NTuple::Array<double> m_b1_toff_mu;
  NTuple::Array<double> m_b1_toff_pi;
  NTuple::Array<double> m_b1_toff_k;
  NTuple::Array<double> m_b1_toff_p;
  NTuple::Array<double> m_b1_tsig_e;
  NTuple::Array<double> m_b1_tsig_mu;
  NTuple::Array<double> m_b1_tsig_pi;
  NTuple::Array<double> m_b1_tsig_k;
  NTuple::Array<double> m_b1_tsig_p;
  NTuple::Array<double> m_b1_tof;                                                    

  NTuple::Array<double> m_dedx_pid;
  NTuple::Array<double> m_tof1_pid;
  NTuple::Array<double> m_tof2_pid;
  NTuple::Array<double> m_prob_pid;
  NTuple::Array<double> m_ptrk_pid;
  NTuple::Array<double> m_cost_pid;  
  NTuple::Item<long>    m_pnp;
  NTuple::Item<long>    m_pnm;  
  
  NTuple::Item<long>     m_nggneu;
  NTuple::Array<double>  m_numHits;    // Total number of hits
  NTuple::Array<double>  m_secondmoment;
  NTuple::Array<double>  m_x;       //  Shower coordinates and errors
  NTuple::Array<double>  m_y;
  NTuple::Array<double>  m_z;
  NTuple::Array<double>  m_cosemc;   // Shower Counter angles and errors
  NTuple::Array<double>  m_phiemc;
  NTuple::Array<double>  m_energy;  // Total energy observed in Emc
  NTuple::Array<double>  m_eSeed;
  NTuple::Array<double>  m_e3x3; 
  NTuple::Array<double>  m_e5x5; 
  NTuple::Array<double>  m_lat;
  NTuple::Array<double>  m_a20;
  NTuple::Array<double>  m_a42;  
  
};

#endif 
