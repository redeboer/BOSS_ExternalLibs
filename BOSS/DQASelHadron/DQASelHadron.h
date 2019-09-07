#ifndef Physics_ControlSample_DQASelHadron_H
#define Physics_ControlSample_DQASelHadron_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"

/**********************************************************************/
/** <project name> - 
 ** <description> Head file for event selection program 
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: DQASelHadron.h,v 1.2 2017/12/22 03:47:45 jixb Exp $
 **
 **
*************************************************************************/

class DQASelHadron : public Algorithm {

public:

    DQASelHadron(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();  

private:
  ITHistSvc *m_thistsvc;

 TH1F* m_ha_costheta;
 TH1F* m_ha_phi;
     TH1F *m_ha_pmax;
     TH1F *m_ha_emax;
     TH1F *m_ha_etot;
     TH1I *m_ha_nneu;
     TH1I *m_ha_nchg;
     TH1F *m_ha_br;
     TH1F *m_ha_bz;
     TH1F *m_ha_vx;
     TH1F *m_ha_vy;
     TH1F *m_ha_vz; 

 
  bool   m_writentuple;
  bool   m_useVertexDB;
  // Declare r0, z0 cut for charged tracks
  double m_ecms;
  double m_beamangle;
  double m_vr0cut;
  double m_vz0cut;
  double m_coscut;


  bool   m_tagHadron;

  bool   m_useTOF;
  bool   m_usePID;
  bool   m_useEMC; 
  bool   m_useMUC;     
  bool   m_useMDC;
  bool   m_useDEDX;
  bool   m_useEMConly;

  //Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaPhiCut;
  double m_gammaThetaCut;
  double m_gammaTrkCut;
  double m_gammathCut;
  double m_gammatlCut;



  double  m_acoll_h_cut;
  double   m_poeb_h_cut;  
  double   m_dtof_h_cut;
  double   m_eop_h_cut;
  double  m_etotal_h_cut;
  int   m_ngam_h_cut;
  double  m_br_h_cut;  
  double  m_bz_h_cut; 
  double  m_thr_h_cut;



  // define Ntuples here

   NTuple::Tuple*  m_tuple1; 
  NTuple::Item<long>  m_run  ;
  NTuple::Item<long>  m_rec;
  NTuple::Item<long>  m_ncharg;
  NTuple::Item<long>  m_nneu ;
  NTuple::Item<long>  m_ngch ;
  NTuple::Item<long>  m_nGam ;
  NTuple::Item<long>  m_hadrontag ;

  NTuple::Item<double>  	m_evis;
  NTuple::Item<double>  	m_thr;
  NTuple::Item<double>  	m_br;
  NTuple::Item<double>  	m_bz;

  NTuple::Item<double>  	m_acoll; 
  NTuple::Item<double>   	m_acopl;
  NTuple::Item<double>   	m_deltatof;		
  NTuple::Item<double>  m_eop1;
  NTuple::Item<double>  m_eop2;
  NTuple::Item<double>  m_eoeb1;
  NTuple::Item<double>  m_eoeb2;
  NTuple::Item<double>  m_poeb1;
  NTuple::Item<double>  m_poeb2;
  NTuple::Item<double>  m_etoeb1;
  NTuple::Item<double>  m_etoeb2;
  NTuple::Item<long>  m_mucinfo1;
  NTuple::Item<long>  m_mucinfo2;


  NTuple::Array<double> m_delang   ;
  NTuple::Array<double> m_delphi   ;
  NTuple::Array<double> m_delthe   ;
  NTuple::Array<long> m_nSeed;
  NTuple::Array<long> m_npart;
  NTuple::Array<long> m_module;
  NTuple::Array<long> m_nemchits;
  NTuple::Array<double> m_x;
  NTuple::Array<double> m_y;
  NTuple::Array<double> m_z;
  NTuple::Array<double> m_dx;
  NTuple::Array<double> m_dy;
  NTuple::Array<double> m_dz;
  NTuple::Array<double> m_dtheta;
  NTuple::Array<double> m_dphi;
  NTuple::Array<double> m_theta;
  NTuple::Array<double> m_phi;
  NTuple::Array<double> m_energy;
  NTuple::Array<double> m_dE;
  NTuple::Array<double> m_eSeed;
  NTuple::Array<double> m_e3x3;
  NTuple::Array<double> m_e5x5;
  NTuple::Array<double> m_secondMoment;
  NTuple::Array<double> m_latMoment;
  NTuple::Array<double> m_a20Moment;
  NTuple::Array<double> m_a42Moment;
  NTuple::Array<double> m_getTime;
  NTuple::Array<double> m_getEAll;




  NTuple::Array<long>   m_charge ;
  NTuple::Array<double> m_vx0   ;
  NTuple::Array<double> m_vy0   ;
  NTuple::Array<double> m_vz0   ;


  NTuple::Array<double> m_px    ;
  NTuple::Array<double> m_py    ;
  NTuple::Array<double> m_pz    ;
  NTuple::Array<double> m_p     ;


  NTuple::Array<double> m_kal_vx0   ;
  NTuple::Array<double> m_kal_vy0   ;
  NTuple::Array<double> m_kal_vz0   ;


  NTuple::Array<double> m_kal_px    ;
  NTuple::Array<double> m_kal_py    ;
  NTuple::Array<double> m_kal_pz    ;
  NTuple::Array<double> m_kal_p     ;




  NTuple::Array<float> m_probPH;
  NTuple::Array<float> m_normPH;
  NTuple::Array<float> m_chie  ;
  NTuple::Array<float> m_chimu ;
  NTuple::Array<float> m_chipi ;
  NTuple::Array<float> m_chik  ;
  NTuple::Array<float> m_chip  ;
  NTuple::Array<float> m_ghit  ;
  NTuple::Array<float> m_thit  ;

  NTuple::Array<float> m_e_emc ;
  NTuple::Array<float> m_theta_emc ;
  NTuple::Array<float> m_phi_emc ;
  	
  NTuple::Array<float>    m_nhit_muc ; 
 NTuple::Array<float>     m_nlay_muc; 	
  	  	
  NTuple::Array<double> m_qual_etof  ;
  NTuple::Array<double> m_tof_etof   ;
  NTuple::Array<double> m_te_etof    ;
  NTuple::Array<double> m_tmu_etof   ;
  NTuple::Array<double> m_tpi_etof   ;
  NTuple::Array<double> m_tk_etof    ;
  NTuple::Array<double> m_tp_etof    ;

  NTuple::Array<double> m_qual_btof1 ;
  NTuple::Array<double> m_tof_btof1  ;
  NTuple::Array<double> m_te_btof1   ;
  NTuple::Array<double> m_tmu_btof1  ;
  NTuple::Array<double> m_tpi_btof1  ;
  NTuple::Array<double> m_tk_btof1   ;
  NTuple::Array<double> m_tp_btof1   ;
  NTuple::Array<double> m_t_btof   ;
  NTuple::Array<double> m_t_etof   ;
  NTuple::Array<double> m_qual_btof2 ;
  NTuple::Array<double> m_tof_btof2  ;
  NTuple::Array<double> m_te_btof2   ;
  NTuple::Array<double> m_tmu_btof2  ;
  NTuple::Array<double> m_tpi_btof2  ;
  NTuple::Array<double> m_tk_btof2   ;
  NTuple::Array<double> m_tp_btof2   ;
  NTuple::Array<long> m_pidcode;  	
  NTuple::Array<double> m_pidprob;
  NTuple::Array<double> m_pidchiDedx;
  NTuple::Array<double> m_pidchiTof1;
  NTuple::Array<double> m_pidchiTof2;  	
  	
  NTuple::Item<double> m_px_cms_ep       ;
  NTuple::Item<double> m_py_cms_ep       ;
  NTuple::Item<double> m_pz_cms_ep       ;
  NTuple::Item<double> m_e_cms_ep       ;
  NTuple::Item<double> m_cos_ep       ; 
  NTuple::Item<double> m_px_cms_em	    ;
  NTuple::Item<double> m_py_cms_em       ;
  NTuple::Item<double> m_pz_cms_em       ;
  NTuple::Item<double> m_e_cms_em       ;  	
  NTuple::Item<double> m_cos_em       ; 
  NTuple::Item<double> m_mass_ee       ; 
   NTuple::Item<double> m_emax       ; 
  NTuple::Item<double> m_esum       ;  	
  NTuple::Item<long>   m_npip ;
  NTuple::Item<long>   m_npim ;
  NTuple::Item<long>   m_nkp  ;
  NTuple::Item<long>   m_nkm  ; 
  NTuple::Item<long>   m_np   ;
  NTuple::Item<long>   m_npb  ;
  NTuple::Item<long>   m_nep ;
  NTuple::Item<long>   m_nem ;
  NTuple::Item<long>   m_nmup ;
  NTuple::Item<long>   m_nmum ;





};


#endif
