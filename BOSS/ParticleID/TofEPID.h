#ifndef ParticleID_TofEPID_H
#define ParticleID_TofEPID_H
//
// TofEPID package: particle Identification with Endcap TOF
//
//
// In 1st version, nothing is done since Endcap data is invalid
// Author: K.L. He     date: 11/12/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class TofEPID : public ParticleIDBase {

 public:
  static TofEPID *instance();
  ~TofEPID(){;} 

  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof > 0); }
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  double sigma(int n) const{return m_sigma[n];}
  double offset(int n) const{return m_offset[n];}
  int ndof() const {return m_ndof;}
  double mass2() const {return m_mass2;}
  int part() const {return m_part;}
  double rhit() const {return m_rhit;}
  int neuronPID() const {return -1;}
 protected:

  int neuronPIDCalculation() { return -1;}
  int particleIDCalculation();
  int LikelihoodCalculation() {return -1;}
  double offsetTofE(int n, int cntr, double ptrk, double rtof, double ph,double charge);
  double sigmaTofE(int n, int cntr,  double ptrk, double rtof, double ph,double charge);
//  void readSigPar();
//  double getSigbyRun(int tofid);
 private:
  double m_chi[5];
  double m_prob[5];
  double m_sigma[5];
  double m_offset[5];
//  double m_sigmaRundata[4][96];
//  double m_sigmaRunmc[4][96];
  double m_chimin;
  double m_pdfmin;
  int m_ndof;
  double m_mass2;
  int m_part; 
  double m_rhit; 
 private:
  TofEPID();
  static TofEPID *m_pointer;
};

#endif
