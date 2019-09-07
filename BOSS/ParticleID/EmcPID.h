#ifndef ParticleID_EmcPID_H
#define ParticleID_EmcPID_H
//
// EmcPID package: particle identification with Emc energy deposits 
// 
// In 1st version, EmcPID is not ready
//
// Author: K.L. He    11/13/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class TMultiLayerPerceptron;
class TTree;

class EmcPID : public ParticleIDBase {

 public:
  
  static EmcPID *instance();
  ~EmcPID(){;} 
  
  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof> 0);}
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  int ndof() const {return m_ndof;}
  double energy() const{return m_energy;}
  double eseed() const{return m_eseed;}
  double e3x3() const{return m_e3x3;}
  double e5x5() const{return m_e5x5;}
  double delta_theta() const{return m_delta_theta;}
  double delta_phi() const{return m_delta_phi;}
  double secondmoment() const{return m_secondmoment;}
  double val_emc1() const{return m_val_emc;}
  int neuronPID() const {return -1;}
  double pars(int n) const{return params_emc1[n];}
  
 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() {return -1;}
  int LikelihoodCalculation() {return -1;}

 private:
  double m_chi[5];
  double m_prob[5];
  double m_chimin;
  int m_ndof;
  double m_ptrk;
  double m_pt;
  int m_type;
  double m_energy;
  double m_eseed;
  double m_e3x3;
  double m_e5x5;
  double m_delta_theta;
  double m_delta_phi;
  double m_secondmoment;
  double m_latmoment;
  double m_a20moment;
  double m_a42moment;
  double m_val_emc;
  double params_emc1[9];
  double m_e_h[18][300];
  double m_m_h[18][300];
  double m_p_h[18][300];
 private:
  static TMultiLayerPerceptron *m_mlp_emc;
  static TTree *m_trainTree_emconly;
   

 private:
  EmcPID();
  static EmcPID *m_pointer;
};

#endif
