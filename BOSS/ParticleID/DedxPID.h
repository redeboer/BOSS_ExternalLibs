#ifndef ParticleID_DedxPID_H
#define ParticleID_DedxPID_H
//
// DedxPID package:  particle identification with dE/dx 
// read original chi from DstDedx, 
// make beta*gamma = p/m  and dip angle cos(theta) dependent correction   
// 
// In 1st version, only the momentum dependent is taken into account
// The corrections are made by: M.Yang, M.S. Chen, L.L. Wang, J.Y. Zhang and Y.Z. Sun
// Author: K.L. He     date: 11/12/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class DedxPID : public ParticleIDBase {

 public:

  static DedxPID * instance();
  // constructor & desconstructor
  ~DedxPID(){;} 
  
  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof>0);}
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  double offset(int n) const {return m_offset[n];}
  double sigma(int n) const {return m_sigma[n];}
  int    ndof() const {return m_ndof;}
  double normPH() const { return m_normPH;}
  double goodHits() const {return m_goodHits;}
  int    neuronPID() const {return -1;}
  int getNhitCutDx() const {return m_nhitcutdx;}
  void setNhitCutDx(const int nhitcuthdx = 5) {m_nhitcutdx = nhitcuthdx;}
 
 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() {return -1;}
  double offsetDedx(int n, double ptrk, double cost);
  double sigmaDedx(int n, double ptrk, double cost);
  int LikelihoodCalculation() {return -1;}
  double mypol3(double x, double par0, double par1, double par2, double par3); 
  double mypol5(double x, double par0, double par1, double par2, double par3, double par4,
              double par5); 
  void inputpar();
  double CorrDedx(int n, double ptrk, double cost,double chi,int charge); 
  double iterate(double ptrk,double *mean,double *p);
  double cal_par(int index1,double *m_jpsi_pip_ptrk_offset,double ptrk,double begin,double bin);
 private:
  double m_chi[5];
  double m_prob[5];
  double m_offset[5];
  double m_sigma[5];
  double m_chimin;
  double m_pdfmin;
  int m_ndof;
  int m_nhitcutdx;
  int m_readstate;
  double m_normPH;
  double m_probPH;
  double m_goodHits;
//lixr add 
    int m_runxx; 
    
    double m_jpsi_kap_ptrk_offset[12];
    double m_jpsi_kap_ptrk_sigma[12];
    double m_jpsi_kam_ptrk_offset[12];
    double m_jpsi_kam_ptrk_sigma[12];
    double m_jpsi_kap_theta_offset[18];
    double m_jpsi_kap_theta_sigma[18];
    double m_jpsi_kam_theta_offset[18];
    double m_jpsi_kam_theta_sigma[18];

    double m_jpsi_mc_kap_ptrk_offset[12];
    double m_jpsi_mc_kap_ptrk_sigma[12];
    double m_jpsi_mc_kam_ptrk_offset[12];
    double m_jpsi_mc_kam_ptrk_sigma[12];
    double m_jpsi_mc_kap_theta_offset[18];
    double m_jpsi_mc_kap_theta_sigma[18];
    double m_jpsi_mc_kam_theta_offset[18];
    double m_jpsi_mc_kam_theta_sigma[18];


    
    double m_jpsi_protonp_ptrk_offset[8];
    double m_jpsi_protonp_ptrk_sigma[8];
    double m_jpsi_protonm_ptrk_offset[8];
    double m_jpsi_protonm_ptrk_sigma[8];
    double m_jpsi_protonp_theta_offset[18];
    double m_jpsi_protonp_theta_sigma[18];
    double m_jpsi_protonm_theta_offset[18];
    double m_jpsi_protonm_theta_sigma[18];


    double m_jpsi_mc_protonp_ptrk_offset[8];
    double m_jpsi_mc_protonp_ptrk_sigma[8];
    double m_jpsi_mc_protonm_ptrk_offset[8];
    double m_jpsi_mc_protonm_ptrk_sigma[8];
    double m_jpsi_mc_protonp_theta_offset[18];
    double m_jpsi_mc_protonp_theta_sigma[18];
    double m_jpsi_mc_protonm_theta_offset[18];
    double m_jpsi_mc_protonm_theta_sigma[18];



//psip
    double m_psip_kap_ptrk_offset[9];
    double m_psip_kap_ptrk_sigma[9];
    double m_psip_kam_ptrk_offset[9];
    double m_psip_kam_ptrk_sigma[9];

    double m_psip_mc_kap_ptrk_offset[9];
    double m_psip_mc_kap_ptrk_sigma[9];
    double m_psip_mc_kam_ptrk_offset[9];
    double m_psip_mc_kam_ptrk_sigma[9];


    double m_psip_protonp_ptrk_offset[9];
    double m_psip_protonp_ptrk_sigma[9];
    double m_psip_protonm_ptrk_offset[9];
    double m_psip_protonm_ptrk_sigma[9];

    double m_psip_mc_protonp_ptrk_offset[9];
    double m_psip_mc_protonp_ptrk_sigma[9];
    double m_psip_mc_protonm_ptrk_offset[9];
    double m_psip_mc_protonm_ptrk_sigma[9];

//psipp
    
    double m_psipp_pi_ptrk_offset[18];
    double m_psipp_pi_ptrk_sigma[18];
    double m_psipp_pi_theta_offset[16];
    double m_psipp_pi_theta_sigma[16];

    double m_psipp_mc_pi_ptrk_offset[18];
    double m_psipp_mc_pi_ptrk_sigma[18];
    double m_psipp_mc_pi_theta_offset[16];
    double m_psipp_mc_pi_theta_sigma[16];

    double m_psipp_ka_ptrk_offset[17];
    double m_psipp_ka_ptrk_sigma[17];
    double m_psipp_ka_theta_offset[16];
    double m_psipp_ka_theta_sigma[16];

    double m_psipp_mc_ka_ptrk_offset[17];
    double m_psipp_mc_ka_ptrk_sigma[17];
    double m_psipp_mc_ka_theta_offset[16];
    double m_psipp_mc_ka_theta_sigma[16];

    double m_psipp_proton_ptrk_offset[18];
    double m_psipp_proton_ptrk_sigma[18];
    double m_psipp_proton_theta_offset[18];
    double m_psipp_proton_theta_sigma[18];

    double m_psipp_mc_proton_ptrk_offset[9];
    double m_psipp_mc_proton_ptrk_sigma[9];
    double m_psipp_mc_proton_theta_offset[18];
    double m_psipp_mc_proton_theta_sigma[18];





    
 private:
  DedxPID();
  static DedxPID *m_pointer;
};

#endif
