#ifndef ParticleID_Tof2PID_H
#define ParticleID_Tof2PID_H
//
// Tof2PID package: particle Identification with Barrel TOF outer layer
//
//
// In 1st version, only the momentum dependent is taken into account
// The corrections are made by: M.Yang, M.S. Chen, L.L. Wang, J.Y. Zhang and Y.Z. Sun
// Author: K.L. He     date: 11/12/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class Tof2PID : public ParticleIDBase {

 public:
  static Tof2PID *instance();
  ~Tof2PID(){;} 
  
  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof > 0); }
  double chi(int n) const {return m_chi[n];}
  double sigma(int n) const{return m_sigma[n];}
  double offset(int n) const{return m_offset[n];}
  double prob(int n) const {return m_prob[n];}
  int ndof() const {return m_ndof;}
  double mass2() const {return m_mass2;}
  double ph2() const {return m_ph2;}
  double zhit2() const{return m_zhit2;}
  double path2() const{return m_path2;}
  double tof2() const{return m_tof2;}
  int neuronPID() const {return -1;}
 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() {return -1;}
  int LikelihoodCalculation() {return -1;}
  double offsetTof2(int n, int cntr, double ptrk, double ztof, double m_ph2,double charge);
  double sigmaTof2(int n, int cntr,  double ptrk, double ztof, double m_ph2, double charge);
  double sampleQ0(double betagamma,double beta);

 private:
  double m_chi[5];
  double m_prob[5];
  double m_sigma[5];
  double m_offset[5];
  double m_chimin;
  double m_pdfmin;
  int m_ndof;
  double m_mass2;
  double m_ph2;
  double m_zhit2;
  double m_path2;
  double m_tof2;
  double m_pars[15];
 private:
  Tof2PID();
  static Tof2PID *m_pointer;
};

#endif
