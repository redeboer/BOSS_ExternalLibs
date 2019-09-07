#ifndef ParticleID_Tof1PID_H
#define ParticleID_Tof1PID_H
//
// Tof1PID package: particle Identification with Barrel TOF inner layer
//
//
// In 1st version, only the momentum dependent is taken into account
// The corrections are made by: M.Yang, M.S. Chen, L.L. Wang, J.Y. Zhang and Y.Z. Sun
// Author: K.L. He     date: 11/12/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class Tof1PID : public ParticleIDBase {

 public:
  
  static Tof1PID * instance();
  ~Tof1PID(){;} 
  
  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof > 0); }
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  double offset(int n) const {return m_offset[n];}
  double sigma(int n) const {return m_sigma[n];}
  int ndof() const {return m_ndof;}
  double mass2() const {return m_mass2;}
  double ph1() const {return m_ph1;}
  double zhit1() const {return m_zhit1;}
  double path1() const{return m_path1;}
  double tof1() const{return m_tof1;}
  int neuronPID() const {return -1;}
 
 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() { return -1;}
  int LikelihoodCalculation() {return -1;}
  double offsetTof1(int n, int cntr, double ptrk, double ztof, double m_ph1, double charge);
  double sigmaTof1(int n, int cntr,  double ptrk, double ztof, double m_ph1, double charge);
  double sampleQ0(double betagamma,double beta);

 private:
  double m_chi[5];
  double m_sigma[5];
  double m_offset[5];
  double m_prob[5];
  double m_chimin;
  double m_pdfmin;
  int m_ndof;
  double m_mass2;
  double m_ph1;
  double m_zhit1;
  double m_path1;
  double m_tof1;
  double m_pars[15];
 private:
  Tof1PID();
  static Tof1PID *m_pointer;
};

#endif
