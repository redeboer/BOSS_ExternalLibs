#ifndef ParticleID_TofPID_H
#define ParticleID_TofPID_H
//
// TofPID package: particle Identification with Barrel TOF inner layer
//
//
// In 1st version, only the momentum dependent is taken into account
// The corrections are made by: M.Yang, M.S. Chen, L.L. Wang, J.Y. Zhang and Y.Z. Sun
// Author: K.L. He     date: 11/12/2005, created
//


#include "ParticleID/ParticleIDBase.h"
#include "ParticleID/Tof1PID.h"
#include "ParticleID/Tof2PID.h"

class TofPID : public ParticleIDBase {

 public:
  
  static TofPID * instance();
  ~TofPID(){;} 
  
  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof > 0); }
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  double offset(int n) const {return m_offset[n];}
  double sigma(int n) const {return m_sigma[n];}
  int ndof() const {return m_ndof;}
  double getweight1() const{ return weight1;}
  double getweight2() const{ return weight2;}
  double chilayer1(int n) const;
  double chilayer2(int n) const;

 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() { return -1;}
  int LikelihoodCalculation() {return -1;}

 private:
  bool val_tof1;
  bool val_tof2;
  double m_chi[5];
  double m_offset[5];
  double m_prob[5];
  double m_sigma[5];
  double m_chimin;
  double m_pdfmin;
  int m_ndof;
  Tof1PID* m_tof1pid;
  Tof2PID* m_tof2pid;
  double weight1;
  double weight2;
 private:
  TofPID();
  static TofPID *m_pointer;
};

#endif
