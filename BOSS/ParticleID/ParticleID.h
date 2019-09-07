#ifndef ParticleID_ParticleID_H
#define ParticleID_ParticleID_H

//
// ParticleID package: particle identification with combined info from
//   dE/dx, TOF, TOFQ ,Muc and Emc system. 
// 
// How to Use: 
//  +++++++++++++++++++++++++++++++++++++++++
//  ParticleID *pid = 0;
//  for (int i = 0; i < ncharg; i++) {
//      if(pid) delete pid;
//      pid = new ParticleID(dstTrk);
//      pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2() | ...);
//      pid->identify(pid->onlyPionKaon()); // pid->onlyPionKaonProton(), etc
//      pid->calculate();
//      if(!(pid->IsPidInfoValid())) continue;
//      // user's selections
//  }
//  if(pid) delete pid;
//  -----------------------------------------
//
// Author: K.L.He & L.L. Wang & Gang.Qin    01/07/2007, created
//

#include "ParticleID/ParticleIDBase.h"
#include "ParticleID/DedxPID.h"
#include "ParticleID/TofPID.h"
#include "ParticleID/TofEPID.h"
#include "ParticleID/TofQPID.h"
#include "ParticleID/TofCPID.h"
#include "ParticleID/TofCorrPID.h"
#include "ParticleID/EmcPID.h"
#include "ParticleID/MucPID.h"

class ParticleID : public ParticleIDBase{

 public:

  static ParticleID *instance();
  ~ParticleID();

  void init();
  //
  //  PID info from each sub system
  //
  //                 Is Pid subsys info Valid
  inline bool IsDedxInfoValid() const;
  inline bool IsTofInfoValid() const;
  inline bool IsTofEInfoValid() const;
  inline bool IsTofQInfoValid() const;
  inline bool IsTofCInfoValid() const;
  inline bool IsTofCorrInfoValid() const;
  inline bool IsEmcInfoValid()  const;
  inline bool IsMucInfoValid()  const;
  //                 get subsys Chi 
  inline double chiDedx(int n) const;
  inline double chiTof(int n) const;
  inline double chiTof1(int n) const;
  inline double chiTof2(int n) const;
  inline double chiTofE(int n) const;
  inline double chiTofQ(int n) const;
  inline double chiTofC(int n) const;
  inline double chiTofCorr(int n) const;
  inline double chiEmc(int n) const;
  inline double chiMuc(int n) const;
  //                 get subsys Prob
  inline double probDedx(int n) const;
  inline double probTof(int n) const;
  inline double probTofE(int n) const;
  inline double probTofQ(int n) const;
  inline double probTofC(int n) const;
  inline double probTofCorr(int n) const;
  inline double probEmc(int n)  const;
  inline double probMuc(int n)  const;
  //huangb add 
//  get the subsys Pdf
  inline double pdfDedx(int n);
  inline double pdfTof(int n);
  inline double pdfTofE(int n);
  inline double pdfTofQ(int n);
  inline double pdfTofC(int n);
  inline double pdfTofCorr(int n);
  inline double pdfEmc(int n);
  inline double pdfMuc(int n);  


  int getNhitCut() const {return m_nhitcut;}
  void setNhitCut(const int nhitcuth = 5) {m_nhitcut = nhitcuth;}
  
  // setMethod(methodProbability()); 
  // setMethod(methodLikelihood());
  // setMethod(methodNeuronNetwork());
  void setMethod(const int method) {m_method = (m_method | method);}
  // specify sub pid sys
  // accessed by  useDedx() | useTof1() | useTof2() | useTofE() | useTofQ() | useEmc()
  void usePidSys(const int pidsys) {m_pidsys = (m_pidsys | pidsys);}
  // identify pi/K   accessed by onlyPionKaon() or onlyPion() | onlyKaon()
  // identify pi/K/p accessed by onlyPionKaonProton()  or onlyPion() | onlyKaon() | onlyProton()
  // identify pi/K/e accessed by onlyPionKaonElectron()  or onlyPion() | onlyKaon() | onlyElectron()
  // identify pi/K/e/p  accessed by all() 
  // identify muon   accessed by onlyMuon() 
  void identify(const int pidcase) {m_pidcase = (m_pidcase | pidcase);}
  // Calculation
  void calculate();
#ifdef BEAN
  void calculate(int run); // BEAN: use this instead of calculate()
#endif

  // output of ParticleID 

  inline bool IsPidInfoValid() const;
  double chi(int n) const {return m_chisq[n];}
  double prob(int n) const { return m_prob[n];}
  int ndof() const {return m_ndof;}
 // huangb add
  //---------------------------------------
  double pdf(int n) const { return m_pdf[n];}
  double lhf(int n) const {return m_likelihoodfraction[n];}

  double probElectron() const {return m_prob[0];}
  double probMuon() const {return m_prob[1];}
  double probPion() const {return m_prob[2];}
  double probKaon() const {return m_prob[3];}
  double probProton() const {return m_prob[4];}
  int discard() const {return m_discard;}
  //huangb add
  //-------------------------------------------------------
  double pdfElectron() const {return m_pdf[0];}
  double pdfMuon() const {return m_pdf[1];}
  double pdfPion() const {return m_pdf[2];}
  double pdfKaon() const {return m_pdf[3];}
  double pdfProton() const {return m_pdf[4];}
  // Neuron Network PID
/*  int neuronPID() const {return m_neuronPid;}
  int neuronStat() const {return m_neuronStat;}
  double neuronVal() const {return m_neuronVal;}
  void setCutEm_mu(double cut1, double cut2) {m_cut_em_mu[0]=cut1; m_cut_em_mu[1]=cut2;} 

  void setNeuronPidCuts();
*/
  //tricks for nn
  //int neuronPID() const {return m_neuronPid;}
  int neuronStat() const {return IsPidInfoValid();}
  double neuronVal() const {
    if(lhf(0)>0.02) return 1.0;
    if(lhf(1)>0.02) return 8.5;
    if(lhf(2)>0.02) return 2.0;
    if(lhf(3)>0.02) return 4.0;
    if(lhf(4)>0.02) return 5.0;
    }
//  void setCutEm_mu(double cut1, double cut2) {m_cut_em_mu[0]=cut1; m_cut_em_mu[1]=cut2;}
  int    ipmt() {
    return m_tofcorrpid->ipmt();
  }
  double dt(int ipar, int ipmt) {
    return m_tofcorrpid->dt(ipar, ipmt);
  }
  double dtCorr(int ipar, int ipmt) {
    return m_tofcorrpid->dtCorr(ipar, ipmt);
  }
  double sigCorr(int ipar, int ipmt) {
    return m_tofcorrpid->sigCorr(ipar, ipmt);
  }
  double chiCorr(int ipar, int ipmt) {
    return m_tofcorrpid->chiCorr(ipar, ipmt);
  }
  double offset(int ipar) {
    return m_tofcorrpid->offset(ipar);
  }
  double sigma(int ipar) {
    return m_tofcorrpid->sigma(ipar);
  }

 protected:

  int particleIDCalculation();
  int LikelihoodCalculation();
  // Neuron Network PID
  int neuronPIDCalculation();
 private:

  bool IsDedxInfoUsed() const {return ((m_pidsys & useDedx()) == useDedx());}
  bool IsTofInfoUsed() const {return ((m_pidsys & useTof()) == useTof());}
  bool IsTof1InfoUsed() const {return ((m_pidsys & useTof1()) == useTof1());}
  bool IsTof2InfoUsed() const {return ((m_pidsys & useTof2()) == useTof2());}
  bool IsTofEInfoUsed() const {return ((m_pidsys & useTofE()) == useTofE());}
  bool IsTofQInfoUsed() const {return ((m_pidsys & useTofQ()) == useTofQ());}
  bool IsTofCInfoUsed() const {return ((m_pidsys & useTofC()) == useTofC());}
  bool IsTofCorrInfoUsed() const {return ((m_pidsys & useTofCorr()) == useTofCorr());}
  bool IsEmcInfoUsed() const {return ((m_pidsys & useEmc()) == useEmc());}
  bool IsMucInfoUsed() const {return ((m_pidsys & useMuc()) == useMuc());}

 private:

  DedxPID* m_dedxpid;
  TofEPID* m_tofepid;
  TofPID* m_tofpid;
  TofQPID* m_tofqpid;
  TofCPID* m_tofcpid;
  TofCorrPID* m_tofcorrpid;
  EmcPID* m_emcpid;
  MucPID* m_mucpid;
  double m_chisq[5];
  double m_prob[5];
  //huangb add
 double m_pdf[5];
  double m_likelihoodfraction[5];
  double m_TotalLikelihood;
  
  int m_nhitcut;
  double m_chicut;
  int m_ndof;
  int m_pidsys;   
  int m_pidcase;   
  int m_method;
  int m_discard;  



 private:
  //constructor & desconstructor
  ParticleID();
  static ParticleID *m_pointer;

};

#include "ParticleID/ParticleID.icc"

#endif
