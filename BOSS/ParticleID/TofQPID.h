#ifndef ParticleID_TofQPID_H
#define ParticleID_TofQPID_H
//
// TofQPID package: particle Identification with Barrel TOF Q value
//
//
// In 1st version, nothing is done since the Q0 calibarions are not done
// Author: K.L. He     date: 11/12/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class TofQPID : public ParticleIDBase {

 public:
  static TofQPID *instance();
  ~TofQPID(){;} 

  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof > 0);}
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  int ndof() const {return m_ndof;}
  double hitstatus() const {return m_hitstatus;}
//  int q1() const{return m_q1;}
//  int q2() const{return m_q2;}
  int neuronPID() const {return -1;}

 protected:

  int neuronPIDCalculation() { return -1;}
  int particleIDCalculation();
  int LikelihoodCalculation() {return -1;}

 private:
  double m_chi[5];
  double m_prob[5];
  double m_chimin;
  int m_ndof;
  double m_hitstatus; 
//  int m_q1;
//  int m_q2;

 private:
  TofQPID();
  static TofQPID *m_pointer;
};

#endif
