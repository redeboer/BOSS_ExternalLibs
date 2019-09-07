#ifndef Physics_Analysis_Gam4pikp_H
#define Physics_Analysis_Gam4pikp_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
//#include "VertexFit/ReadBeamParFromDb.h"

class Gam4pikp : public Algorithm {

public:
  Gam4pikp(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:
  void InitVar();
  void BubbleSort(std::vector<double> &, std::vector<int> &);


  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;
  bool m_skim4pi;
  bool m_skim4k;
  bool m_skim2pi2pr;
  bool m_rootput;
  
  //Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaPhiCut;
  double m_gammaThetaCut;
  double m_ecms;
  double m_gammadangcut;
  // 
  int m_test4C;
  int m_test5C;

  // 
  int m_checkDedx;
  int m_checkTof;

  // define Ntuples here
  NTuple::Tuple*  m_tuple0;
  NTuple::Item<double>  m_tnumber;
  NTuple::Item<double>  m_totaltrack;
  NTuple::Item<double>  m_totalcharged;
  NTuple::Item<double>  m_totalgam;
  NTuple::Item<double> m_massth;
  


  
  NTuple::Tuple*  m_tuple1;
  NTuple::Item<long>   m_run;
  NTuple::Item<long>   m_rec;
  NTuple::Item<double>   m_mpprecall;
  NTuple::Item<double>   m_meeall;
  NTuple::Item<double>   m_ncgjs;
  NTuple::Item<double>   m_cla2kpi;
  NTuple::Array<double> m_comcs2kpi;
  NTuple::Array<double> m_comcs4pi;
  NTuple::Item<long>   m_idxmc;
  NTuple::Array<long>  m_pdgid;
  NTuple::Array<long>  m_motheridx;  
  NTuple::Item<double>  m_mpprec4c;
  NTuple::Array<double>  m_x0js, m_y0js, m_z0js, m_r0js, m_Rxyjs, m_Rzjs;
  NTuple::Array<double>  m_Rnxyjs;
  NTuple::Array<double>  m_Rnzjs;
  NTuple::Array<double>  m_phinjs;
  NTuple::Item<long>   m_cy2kpi;
  NTuple::Item<long>   m_cl4pi;
  NTuple::Item<long>   m_clajs; 
  NTuple::Item<long>    m_idxmdc;
  NTuple::Array<double> m_pdg;
  NTuple::Array<double> m_cbmc;
  
  NTuple::Array<double> m_angjs5;
  NTuple::Array<double> m_nearjs5;
  NTuple::Array<double> m_angjs6;
  NTuple::Array<double> m_nearjs6;
  NTuple::Array<double> m_ang4pi5;
  NTuple::Array<double> m_near4pi5;
  NTuple::Array<double> m_ang4pi6;
  NTuple::Array<double> m_near4pi6;  
  
  NTuple::Array<double> m_ppmdcjs;
  NTuple::Array<double> m_pxmdcjs;
  NTuple::Array<double> m_pymdcjs;
  NTuple::Array<double> m_pzmdcjs;  
  NTuple::Array<double> m_ptmdcjs;
  NTuple::Array<double> m_ppkaljs;
  NTuple::Array<double> m_ptkaljs; 
  NTuple::Array<double> m_ppmdc2kpi;
  NTuple::Array<double> m_pxmdc2kpi;
  NTuple::Array<double> m_pymdc2kpi;
  NTuple::Array<double> m_pzmdc2kpi;  
    NTuple::Array<double> m_ptmdc2kpi;
      NTuple::Array<double> m_ppkal2kpi;
        NTuple::Array<double> m_ptkal2kpi;
  NTuple::Array<double> m_charge2kpi; 
  NTuple::Array<double> m_chie2kpi;
  NTuple::Array<double> m_chimu2kpi;
  NTuple::Array<double> m_chipi2kpi;
  NTuple::Array<double> m_chik2kpi;
  NTuple::Array<double> m_chip2kpi;
  NTuple::Array<double> m_ghit2kpi;
  NTuple::Array<double> m_thit2kpi;
  NTuple::Array<double> m_probph2kpi;
  NTuple::Array<double> m_normph2kpi;
 
  NTuple::Array<double> m_chiejs;
  NTuple::Array<double> m_chimujs;
  NTuple::Array<double> m_chipijs;
  NTuple::Array<double> m_chikjs;
  NTuple::Array<double> m_chipjs;
  NTuple::Array<double> m_ghitjs;
  NTuple::Array<double> m_thitjs;
  NTuple::Array<double> m_probphjs;
  NTuple::Array<double> m_normphjs;

  NTuple::Array<double> m_bjmucjs;
  NTuple::Array<double> m_bjmuc2kpi;
  NTuple::Array<double> m_bjemcjs;
  NTuple::Array<double> m_bjemc2kpi;
  NTuple::Array<double> m_bjtofjs;
  NTuple::Array<double> m_bjtof2kpi;  
  NTuple::Array<double> m_bjtofvaljs;
  NTuple::Array<double> m_bjtofval2kpi;  
  NTuple::Array<double> m_emcjs;
  NTuple::Array<double> m_evpjs;
  NTuple::Array<double> m_timecgjs;
  NTuple::Array<double> m_depthmucjs;
  NTuple::Array<double> m_layermucjs;
  NTuple::Array<double> m_emc2kpi;
  NTuple::Array<double> m_evp2kpi;
  NTuple::Array<double> m_timecg2kpi;
  NTuple::Array<double> m_depthmuc2kpi;
  NTuple::Array<double> m_layermuc2kpi;
  NTuple::Array<double> m_cosmdc2kpi;
  NTuple::Array<double> m_phimdc2kpi; 
   
  NTuple::Array<double> m_costpid2kpi;
  NTuple::Array<double> m_dedxpid2kpi;
  NTuple::Array<double> m_tof1pid2kpi;
  NTuple::Array<double> m_tof2pid2kpi;
  NTuple::Array<double> m_probe2kpi;
  NTuple::Array<double> m_probmu2kpi;
  NTuple::Array<double> m_probpi2kpi;
  NTuple::Array<double> m_probk2kpi;
  NTuple::Array<double> m_probpr2kpi;
  NTuple::Array<double> m_pidnum2kpi;
  
  NTuple::Array<double> m_chipidxpid2kpi;
  NTuple::Array<double> m_chipitof1pid2kpi;
  NTuple::Array<double> m_chipitof2pid2kpi;
  NTuple::Array<double> m_chipitofpid2kpi;
  NTuple::Array<double> m_chipitofepid2kpi;
  NTuple::Array<double> m_chipitofqpid2kpi;
  NTuple::Array<double> m_probpidxpid2kpi;
  NTuple::Array<double> m_probpitofpid2kpi;
  
  NTuple::Array<double> m_chikdxpid2kpi;
  NTuple::Array<double> m_chiktof1pid2kpi;
  NTuple::Array<double> m_chiktof2pid2kpi;
  NTuple::Array<double> m_chiktofpid2kpi;
  NTuple::Array<double> m_chiktofepid2kpi;
  NTuple::Array<double> m_chiktofqpid2kpi;
  NTuple::Array<double> m_probkdxpid2kpi;
  NTuple::Array<double> m_probktofpid2kpi;

  NTuple::Array<double> m_chiprdxpid2kpi;
  NTuple::Array<double> m_chiprtof1pid2kpi;
  NTuple::Array<double> m_chiprtof2pid2kpi;
  NTuple::Array<double> m_chiprtofpid2kpi;
  NTuple::Array<double> m_chiprtofepid2kpi;
  NTuple::Array<double> m_chiprtofqpid2kpi;
  NTuple::Array<double> m_probprdxpid2kpi;
  NTuple::Array<double> m_probprtofpid2kpi;  

  NTuple::Array<double> m_cosmdcjs;
  NTuple::Array<double> m_phimdcjs;
  NTuple::Array<double> m_dedxpidjs;
  NTuple::Array<double> m_tof1pidjs;
  NTuple::Array<double> m_tof2pidjs;
  NTuple::Array<double> m_probejs;
  NTuple::Array<double> m_probmujs;
  NTuple::Array<double> m_probpijs;
  NTuple::Array<double> m_probkjs;
  NTuple::Array<double> m_probprjs;  
  
  NTuple::Array<double> m_cotof1js;
  NTuple::Array<double> m_cotof2js;
  NTuple::Array<double> m_counterjs;
  NTuple::Array<double> m_barreljs;
  NTuple::Array<double> m_layertofjs;
  NTuple::Array<double> m_readoutjs;
  NTuple::Array<double> m_clusterjs;
  NTuple::Array<double> m_betajs;
  NTuple::Array<double> m_tofjs;
  NTuple::Array<double> m_tofpathjs;
  NTuple::Array<double> m_zhitjs;
  NTuple::Array<double> m_tofIDjs;
  NTuple::Array<double> m_clusterIDjs;
  NTuple::Array<double> m_texejs;
  NTuple::Array<double> m_texmujs;
  NTuple::Array<double> m_texpijs;
  NTuple::Array<double> m_texkjs;
  NTuple::Array<double> m_texprjs;  
  NTuple::Array<double> m_dtejs;
  NTuple::Array<double> m_dtmujs;
  NTuple::Array<double> m_dtpijs;
  NTuple::Array<double> m_dtkjs;    
  NTuple::Array<double> m_dtprjs;  

  NTuple::Array<double> m_cotof12kpi;
  NTuple::Array<double> m_cotof22kpi;
  NTuple::Array<double> m_counter2kpi;
  NTuple::Array<double> m_barrel2kpi;
  NTuple::Array<double> m_layertof2kpi;
  NTuple::Array<double> m_readout2kpi;
  NTuple::Array<double> m_cluster2kpi;
  NTuple::Array<double> m_beta2kpi;
  NTuple::Array<double> m_tof2kpi;
  NTuple::Array<double> m_tofpath2kpi;
  NTuple::Array<double> m_zhit2kpi;
  NTuple::Array<double> m_tofID2kpi;
  NTuple::Array<double> m_clusterID2kpi;
  NTuple::Array<double> m_texe2kpi;
  NTuple::Array<double> m_texmu2kpi;
  NTuple::Array<double> m_texpi2kpi;
  NTuple::Array<double> m_texk2kpi;
  NTuple::Array<double> m_texpr2kpi;
  NTuple::Array<double> m_dte2kpi;
  NTuple::Array<double> m_dtmu2kpi;
  NTuple::Array<double> m_dtpi2kpi;
  NTuple::Array<double> m_dtk2kpi;
  NTuple::Array<double> m_dtpr2kpi;  
  NTuple::Array<double> m_sigmaetof2kpi;
  NTuple::Array<double> m_sigmamutof2kpi;
  NTuple::Array<double> m_sigmapitof2kpi;
  NTuple::Array<double> m_sigmaktof2kpi;
  NTuple::Array<double> m_sigmaprtof2kpi;
  NTuple::Array<double> m_t0tof2kpi;
  NTuple::Array<double> m_errt0tof2kpi; 
 
  NTuple::Array<double> m_sigmaetofjs;
  NTuple::Array<double> m_sigmamutofjs;
  NTuple::Array<double> m_sigmapitofjs;
  NTuple::Array<double> m_sigmaktofjs;
  NTuple::Array<double> m_sigmaprtofjs;
  NTuple::Array<double> m_t0tofjs;
  NTuple::Array<double> m_errt0tofjs;
  NTuple::Item<double>  m_mchic2kpi;
  NTuple::Item<long>  m_eventtype;
  NTuple::Item<double>  m_mpsip2kpi;
  NTuple::Item<double>  m_chis2kpi;  
  NTuple::Item<double>  m_mchic4c2kpi;
    NTuple::Item<double>  m_mpsip4c2kpi;
      NTuple::Item<double>  m_chis4c2kpi;
  NTuple::Item<double>  m_mlplm2kpi;
  NTuple::Item<double>  m_mhphm2kpi;
  NTuple::Item<double>  m_mlphp2kpi;
  NTuple::Item<double>  m_mlmhm2kpi;
  NTuple::Item<double>  m_mlphm2kpi;
  NTuple::Item<double>  m_mlmhp2kpi;

  NTuple::Item<long>     m_ncy20;
  NTuple::Item<long>     m_ncy30;
  
  NTuple::Item<long>     m_idxemc;
  NTuple::Array<long>    m_numHits;    // Total number of hits 
  NTuple::Array<double>  m_secondmoment;
  NTuple::Array<double>  m_latmoment;
  NTuple::Array<double>  m_timegm;
  NTuple::Array<double>  m_cellId;
  NTuple::Array<double>  m_module;
  NTuple::Array<double>  m_a20Moment;
  NTuple::Array<double>  m_a42Moment;
  NTuple::Array<double>  m_getEAll;
  NTuple::Array<double>  m_getShowerId;
  NTuple::Array<double>  m_getClusterId;
  NTuple::Array<double>  m_x;       //  Shower coordinates and errors
  NTuple::Array<double>  m_y;
  NTuple::Array<double>  m_z;
  NTuple::Array<double>  m_cosemc;   // Shower Counter angles and errors
  NTuple::Array<double>  m_phiemc;
  NTuple::Array<double>  m_energy;  // Total energy observed in Emc
  NTuple::Array<double>  m_eSeed;
  NTuple::Array<double>  m_e3x3;
  NTuple::Array<double>  m_e5x5;
  NTuple::Array<double>  m_dang4c;
  NTuple::Array<double>  m_dthe4c;
  NTuple::Array<double>  m_dphi4c;
  NTuple::Array<double>  m_dang4crt;
  NTuple::Array<double>  m_dthe4crt;
  NTuple::Array<double>  m_dphi4crt;  
  NTuple::Matrix<double> m_phgmtof;
  NTuple::Array<double>  m_phgmtof0;
  NTuple::Array<double>  m_phgmtof1;  
  NTuple::Array<double>  m_phgmtof2;   

  
  Algorithm* m_subalg1;
  Algorithm* m_subalg2;  
  Algorithm* m_subalg3;
    
};

#endif 
