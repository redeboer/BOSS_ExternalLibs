#ifndef ParticleID_TofCPID_H
#define ParticleID_TofCPID_H
//
// TofCPID package: particle Identification with Endcap TOF
//
//
// In 1st version, nothing is done since Endcap data is invalid
// Author: K.L. He     date: 11/12/2005, created
//

#include "ParticleID/ParticleIDBase.h"

class TofCPID : public ParticleIDBase {

 public:
  static TofCPID *instance();
  ~TofCPID(){;} 

  void init();
  void calculate();
  bool IsPidInfoValid() const {return (m_ndof > 0); }
  double chi(int n) const {return m_chi[n];}
  double prob(int n) const {return m_prob[n];}
  double sigma(int n) const{return m_sigma[n];}
  double offset(int n) const{return m_offset[n];}
  int ndof() const {return m_ndof;}
  double mass2() const {return m_mass2;}
  double phc() const {return m_phc;}
  double zhitc() const {return m_zhitc;}
  double pathc() const{return m_pathc;}
  double tofc() const{return m_tofc;}
  int neuronPID() const {return -1;}


 protected:

  int neuronPIDCalculation() { return -1;}
  int particleIDCalculation();
  int LikelihoodCalculation() {return -1;}
//  double offsetTofC(int n, int cntr, double ptrk, double rtof, double ph,double charge);
//  double sigmaTofC(int n, int cntr,  double ptrk, double rtof, double ph,double charge);
double offsetTofC(int n, double ptrk, double cost);
double sigmaTofC(int n, double ptrk, double cost);
  
  double sampleQ0(double betagamma,double beta);
  double mypol3(double x, double par0, double par1, double par2, double par3);
  double mypol5(double x, double par0, double par1, double par2, double par3, double par4,double par5);
  
 private:
  double m_chi[5];
  double m_prob[5];
  double m_sigma[5];
  double m_offset[5];
  double m_chimin;
  double m_pdfmin;
  int m_ndof;
  double m_mass2;
  double m_phc;
  double m_zhitc;
  double m_pathc;
  double m_tofc;
  double m_pars[15];
  double m_momentpara[5][12];
  double m_thetapara[5][8];
  double m_endcappara[5][4];
  int m_readstate;
 private:
  TofCPID();
  static TofCPID *m_pointer;
};

#endif
