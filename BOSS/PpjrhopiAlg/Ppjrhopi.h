#ifndef Physics_Analysis_Ppjrhopi_H
#define Physics_Analysis_Ppjrhopi_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
//#include "VertexFit/ReadBeamParFromDb.h"

class Ppjrhopi : public Algorithm {

public:
  Ppjrhopi(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

//  ReadBeamParFromDb m_reader;
  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;
  double m_vr1cut;
  double m_vz1cut;  
  double m_cthcut;
  
  //Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaAngCut;
//  double m_gammaPhiCut;
//  double m_gammaThetaCut;

  // 
  int m_test4C;
  int m_test5C;

  // 
  int m_checkDedx;
  int m_checkTof;

  // define Ntuples here

  NTuple::Tuple*  m_tuple4;     // rhopi 4C
  NTuple::Item<long>  m_run;
  NTuple::Item<long>  m_rec;
  NTuple::Item<long>  m_nch;
  NTuple::Item<long>  m_nneu;
  NTuple::Item<long>  m_gdgam;
  NTuple::Item<double>  m_recpp;
  NTuple::Item<double>  m_chi1;
  NTuple::Item<double>  m_mpi0; 
  NTuple::Item<double>  m_mprho0;  
  NTuple::Item<double>  m_mprhop;
  NTuple::Item<double>  m_mprhom;
  NTuple::Item<double>  m_mpjjj;
  NTuple::Item<double>  m_bepi0;
  NTuple::Item<double>  m_be4cjpsi; 
  NTuple::Item<double>  m_mp2pi1; 
  NTuple::Item<double>  m_mf2pi1g1;
  NTuple::Item<double>  m_mf2pi1g2;
  NTuple::Item<double>  m_mf2pi1pi0;
  NTuple::Item<double>  m_mt2pi2g1;
  NTuple::Item<double>  m_mt2pi2g2;
  NTuple::Item<double>  m_mp2pi3; 
  NTuple::Item<double>  m_mf2pi3g1;
  NTuple::Item<double>  m_mf2pi3g2;
  NTuple::Item<double>  m_mf2pi3pi0;
  NTuple::Item<double>  m_mp2pi4; 
  NTuple::Item<double>  m_mf2pi4g1;
  NTuple::Item<double>  m_mf2pi4g2;
  NTuple::Item<double>  m_mf2pi4pi0;
  NTuple::Item<double>  m_mp4pi; 
  NTuple::Item<double>  m_mppptot;
  NTuple::Item<double>  m_mp4pig1;
  NTuple::Item<double>  m_mp4pig2;    

  NTuple::Item<double>  m_mpx1;
  NTuple::Item<double>  m_mpy1;
  NTuple::Item<double>  m_mpz1;
  NTuple::Item<double>  m_mpe1;
  NTuple::Item<double>  m_mpx2;
  NTuple::Item<double>  m_mpy2;
  NTuple::Item<double>  m_mpz2;
  NTuple::Item<double>  m_mpe2;
  NTuple::Item<double>  m_mpx3;
  NTuple::Item<double>  m_mpy3;
  NTuple::Item<double>  m_mpz3;
  NTuple::Item<double>  m_mpe3;
  NTuple::Item<double>  m_mpx4;
  NTuple::Item<double>  m_mpy4;
  NTuple::Item<double>  m_mpz4;
  NTuple::Item<double>  m_mpe4;
  NTuple::Item<double>  m_mpxg1;
  NTuple::Item<double>  m_mpyg1;
  NTuple::Item<double>  m_mpzg1;
  NTuple::Item<double>  m_mpeg1;
  NTuple::Item<double>  m_mpxg2;
  NTuple::Item<double>  m_mpyg2;
  NTuple::Item<double>  m_mpzg2;
  NTuple::Item<double>  m_mpeg2;
  NTuple::Item<double>  m_chikk;
/*  
  NTuple::Item<double>  m_p1vx;
  NTuple::Item<double>  m_p1vy;
  NTuple::Item<double>  m_p1vz;
  NTuple::Item<double>  m_p1vr;
  NTuple::Item<double>  m_p1vct;
  NTuple::Item<double>  m_m1vx;
  NTuple::Item<double>  m_m1vy;
  NTuple::Item<double>  m_m1vz;
  NTuple::Item<double>  m_m1vr;
  NTuple::Item<double>  m_m1vct;
  NTuple::Item<double>  m_p2vx;
  NTuple::Item<double>  m_p2vy;
  NTuple::Item<double>  m_p2vz;
  NTuple::Item<double>  m_p2vr;
  NTuple::Item<double>  m_p2vct;
  NTuple::Item<double>  m_m2vx;
  NTuple::Item<double>  m_m2vy;
  NTuple::Item<double>  m_m2vz;
  NTuple::Item<double>  m_m2vr;
  NTuple::Item<double>  m_m2vct;
  NTuple::Item<double>  m_p1ptot;
  NTuple::Item<double>  m_emcTp1;
  NTuple::Item<double>  m_m1ptot;
  NTuple::Item<double>  m_emcTm1;
  NTuple::Item<double>  m_p2ptot;
  NTuple::Item<double>  m_emcTp2;
  NTuple::Item<double>  m_m2ptot;
  NTuple::Item<double>  m_emcTm2;
  NTuple::Item<double>  m_p1pxy;
  NTuple::Item<double>  m_m1pxy;
  NTuple::Item<double>  m_p2pxy;
  NTuple::Item<double>  m_m2pxy; 
  NTuple::Item<double>  m_laypip1;
  NTuple::Item<double>  m_laypim1;
  NTuple::Item<double>  m_laypip2;
  NTuple::Item<double>  m_laypim2; 
*/

  NTuple::Item<long>  m_good;
  NTuple::Item<long>  m_gam;
  NTuple::Item<long>  m_pip;
  NTuple::Item<long>  m_pim;
  NTuple::Item<long>    m_pidpip;
  NTuple::Array<double> m_ipipin;  
  NTuple::Item<long>    m_pidpim;  
  NTuple::Array<double> m_ipimin; 
   
  NTuple::Item<double>  m_angle;
  NTuple::Item<double>  m_cosuubr;    
  NTuple::Item<double>  m_cosmupbr; 
  NTuple::Item<double>  m_cosmumbr; 
  NTuple::Item<double>  m_phimupbr; 
  NTuple::Item<double>  m_phimumbr; 
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
