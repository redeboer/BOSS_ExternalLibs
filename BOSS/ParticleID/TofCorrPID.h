#ifndef ParticleID_TofCorrPID_H
#define ParticleID_TofCorrPID_H
//
// TofCorrPID package: particle Identification with TOF
//
// The offset for hadrons are corrected.
//
// Author: SUN Shengsen, LV Meng     date: 01/16/2013, created
//


#include "ParticleID/ParticleIDBase.h"

class TofCorrPID : public ParticleIDBase {

 public:
  
  static TofCorrPID * instance();
  ~TofCorrPID(){;} 
  
  void   init();
  void   calculate();
  bool   IsPidInfoValid()             const { return (m_ndof > 0);           }
  double chi(int n)                   const { return m_chi[n];               }
  double prob(int n)                  const { return m_prob[n];              }
  double offset(int n)                const { return m_offset[n];            }
  double sigma(int n)                 const { return m_sigma[n];             }
  int    ndof()                       const { return m_ndof;                 }

  int    ipmt()                       const { return m_ipmt;                 }
  double dt(int ipar, int ipmt)       const { return m_dt[ipar][ipmt];       }
  double dtCorr(int ipar, int ipmt)   const { return m_dtCorr[ipar][ipmt];   }
  double sigCorr(int ipar, int ipmt)  const { return m_sigCorr[ipar][ipmt];  }
  double chiCorr(int ipar, int ipmt)  const { return m_chiCorr[ipar][ipmt];  }

 protected:

  int particleIDCalculation();
  int neuronPIDCalculation() { return -1;}
  int LikelihoodCalculation() {return -1;}

  void   inputParameter( int run );
  double offsetTof( unsigned int ispecies, bool barrel, unsigned int ipmt, double betaGamma, int charge, double zrhit, double dt );
  double offsetTof( unsigned int ispecies, int tofid, double zrhit, double betaGamma, int charge, double dt );
  double offsetTof( unsigned int ispecies, int tofid1, int tofid2, double zrhit1, double zrhit2, double betaGamma, int charge, double dt );
  double sigmaTof( unsigned int ispecies, int charge, bool barrel, unsigned int ipmt, int tofid[2], double zrhit[2], double betaGamma );
  double sigmaTof( unsigned int ispecies, int charge, bool barrel, unsigned int ipmt, double zrhit, double betaGamma );

  double qCurveFunc( unsigned int layer, double betaGamma );
  double bSigma( unsigned int end, int tofid, double zrhit );
  double bSigma( int tofid[2], double zrhit[2] );
  double eSigma( int tofid, double zrhit );

  bool correlationCheck( unsigned int ipmt );

 private:
  double  m_chi[5];
  double  m_offset[5], m_sigma[5];
  double  m_prob[5];
  double  m_chimin, m_chimax;
  double  m_pdfmin;
  int     m_ndof;

 private:
  int     m_ipmt;
  double  m_dt[5][7], m_dtCorr[5][7], m_sigCorr[5][7], m_chiCorr[5][7]; 

 private:
  int     m_runBegin, m_runEnd;
  double  m_p_weight[176][3][5], m_p_common, m_ec_sigma[96][3];
  double  m_q0_bg[3][5], m_par_ab[5][2][5], m_par_pbar_ab[5][2][5];
  double  m_par_sigma[4][8][12], m_par_sig_mom[3][7][3];
  double  m_p_offset[2][4][10][20], m_p_sigma[2][7][10][20];
  double  m_p_offset_12[2][7][20][46], m_p_sigma_12[2][7][20][46];
  double  m_p_offset_ec12[2][20][7], m_p_sigma_ec12[2][20][7];

  double  m_par_ab_12[2][5][2][2][11];

 private:
  TofCorrPID();
  static TofCorrPID *m_pointer;
};

#endif
