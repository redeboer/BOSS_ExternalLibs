#ifndef ParticleID_ParticleIDBase_H
#define ParticleID_ParticleIDBase_H
//
// define the standard interface of ParticleID package
// PID-sys includes: dE/dx, TOF endcap, TOF barrel inner, TOF barrel outer,
//                   TOF Q, EMC energy deposition and Maybe Muon counter
// the development of particle ID package should follow the rule defined
// in this class.
//
// Author: K.L. He    11/11/2005, created
//
//
#include <string>

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#else
#include "DstEvtRecTracks.h"
#endif

class ParticleIDBase {

 public:

  ParticleIDBase();
  virtual ~ParticleIDBase(){;}

  virtual void init() = 0;
  virtual void calculate() = 0;
  virtual bool IsPidInfoValid() const = 0;
  virtual double chi(int n) const = 0;
  virtual double prob(int n) const = 0;
  virtual int ndof() const = 0;
//  virtual int neuronPID() const = 0;       //neuron pid

  EvtRecTrack* PidTrk() const {return m_trk;}
  void setRecTrack(EvtRecTrack* trk) {m_trk = trk;}
  double chiMinCut() const {return m_chimin_cut;}
  void setChiMinCut(const double chi = 4) {m_chimin_cut = chi;}
  double chiMaxCut() const {return m_chimax_cut;}
  void setChiMaxCut(const double chi = 6) {m_chimax_cut = chi;}
  double pdfMinSigmaCut() const {return m_pdfsigmamin_cut;}
  void setPdfMinSigmaCut(const double pdf =4) {m_pdfsigmamin_cut= pdf;}
  double getRunNo() const {return m_runno;}
  void setRunNo(const double runh = 8093) {m_runno = runh;}

  double p();
  double pt();
  double charge();

 public:  // utilities

  double xmass(int n);
  double velc();
  double probCalculate(double chi2, int n);
  double pdfCalculate(double offset,double sigma);
  double interpolation(double* x, double* y,double x1);
  double pol2(double x,double* par);
  double pol3(double x,double* par);
  double pol4(double x,double* par);

 public:  // specific sub pid sys

  int useDedx() const {return USE_DEDX;}
  int useTof() const {return USE_TOF;}
  int useTof1() const {return USE_TOF1;}
  int useTof2() const {return USE_TOF2;}
  int useTofE() const {return USE_TOFE;}
  int useTofQ() const {return USE_TOFQ;}
  int useTofC() const {return USE_TOFC;}
  int useTofCorr() const {return USE_TOFCorr;}
  int useEmc()  const {return USE_EMC;}
  int useMuc()  const {return USE_MUC;}

 public: // identify which kind of particles
  int onlyPionKaon() const {return (IDENTIFY_PION | IDENTIFY_KAON);}
  int onlyPionKaonProton() const {return (IDENTIFY_PION | IDENTIFY_KAON | IDENTIFY_PROTON);}
  int onlyPionKaonElectron() const {return (IDENTIFY_PION | IDENTIFY_KAON | IDENTIFY_ELECTRON);}
  int all()  const {return (IDENTIFY_ELECTRON | IDENTIFY_PION | IDENTIFY_KAON | IDENTIFY_PROTON);}
  int onlyElectron() const {return IDENTIFY_ELECTRON;}
  int onlyMuon() const {return IDENTIFY_MUON;}
  int onlyPion() const {return IDENTIFY_PION;}
  int onlyKaon() const {return IDENTIFY_KAON;}
  int onlyProton() const {return IDENTIFY_PROTON;}

 public: // Particle ID method
  int methodLikelihood() const {return LIKELIHOOD_PID;}
  int methodProbability() const {return PROBABILITY_PID;}
  int methodNeuronNetwork() const {return NEURONNETWORK_PID;}
 public: //
  int dedxValid() const {return DEDX_VALID;}
  int tofValid() const {return TOF_VALID;}
  int tofeValid() const {return TOFE_VALID;}
  int tofqValid() const {return TOFQ_VALID;}
  int tofcValid() const {return TOFC_VALID;}
  int tofcorrValid() const {return TOFCorr_VALID;}
  int emcValid() const {return EMC_VALID;}
  int mucValid() const {return MUC_VALID;}

 protected:
  // path to dir with calibration/configuration/etc data
  static std::string path;
 public:
  void set_path(const char* s_path = 0);
  void set_path(std::string s_path) {path = s_path;}

 protected:
  // PID calculation routines(include PID correction)
  virtual int particleIDCalculation() = 0;
//  virtual int neuronPIDCalculation() = 0;
  //huangb add
  virtual int LikelihoodCalculation() =0;

 private:
  EvtRecTrack* m_trk;
  double m_runno;
  double m_chimin_cut, m_chimax_cut;
  double m_pdfsigmamin_cut; //the cut is multiple of the sigma. ie. if(m_pdfsigmamin_cut ==3), the cut is 3sigma
  static const int USE_DEDX;
  static const int USE_TOF1;
  static const int USE_TOF2;
  static const int USE_TOF;
  static const int USE_TOFE;
  static const int USE_TOFQ;
  static const int USE_TOFC;
  static const int USE_TOFCorr;
  static const int USE_EMC;
  static const int USE_MUC;

  static const int IDENTIFY_ELECTRON;
  static const int IDENTIFY_MUON;
  static const int IDENTIFY_PION;
  static const int IDENTIFY_KAON;
  static const int IDENTIFY_PROTON;

  static const int PROBABILITY_PID;
  static const int LIKELIHOOD_PID;
  static const int NEURONNETWORK_PID;

  static const int DEDX_VALID;
  static const int TOF_VALID;
  static const int TOF1_VALID;
  static const int TOF2_VALID;
  static const int TOFE_VALID;
  static const int TOFQ_VALID;
  static const int TOFC_VALID;
  static const int TOFCorr_VALID;
  static const int EMC_VALID;
  static const int MUC_VALID;
};

#endif
