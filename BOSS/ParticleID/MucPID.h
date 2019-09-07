#ifndef ParticleID_MucPID_H
#define ParticleID_MucPID_H


//
// MucPID package: particle identification with Muc energy deposits 
// 
// In 1st version, MucPID is not ready
//
// Author: K.L. He    11/13/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class TFile;
class TTree;
class TMultiLayerPerceptron;
class TMLPAnalyzer;

class MucPID : public ParticleIDBase {

 public:
  static MucPID *instance();
//  ~MucPID(){;} 
  
  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof> 0);}
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  int ndof() const {return m_ndof;}
  double hits() const{return m_hits;}
  double depth() const{return m_depth;}
  double chi2() const{return m_chi2;}
  double distance() const{return m_distance;}
  double delta_phi() const{return m_muc_delta_phi;}
  double val_muc1() const{return m_val_muc;}
  
  int neuronPID() const {return -1;}


 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() {return -1;}
  int LikelihoodCalculation() {return -1;}

 private:
  double m_chi[5];
  double m_prob[5];
  double m_chimin;
  int m_ndof;
  double m_hits;
  double m_depth;
  double m_chi2;
  double m_distance;
  double m_muc_delta_phi;
  double m_val_muc;
  double params_muc1[8];
  double m_p_h[13][400];
  double m_m_h[13][400];

 private:
  TFile *m_trainFile_muc;
  TTree *m_trainTree_muc;
  TMultiLayerPerceptron *m_mlp_muc;
  TMLPAnalyzer* m_mlpa_muc;

 private:
  MucPID();
  static MucPID *m_pointer;
};

#endif
