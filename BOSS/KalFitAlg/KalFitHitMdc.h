//--------------------------------------------------------------------
// File from KalFit module
//
// Filename : KalFitHitMdc.cc
//--------------------------------------------------------------------
// Description : Hit in Mdc 
//--------------------------------------------------------------------
// Modif :
//--------------------------------------------------------------------
#ifndef _DEFINE_HITMdc_H_
#define _DEFINE_HITMdc_H_

#include "KalFitAlg/KalFitWire.h"
#include "MdcRecEvent/RecMdcHit.h"

/// Description of a Hit in Mdc
class KalFitHitMdc{

public:
  /// constructor
  KalFitHitMdc(int id, int LR, double tdc, double dist[2],
	     double erdist[2], KalFitWire * wire, RecMdcHit* rechitptr);
  KalFitHitMdc(int id, int LR, double tdc, double dist[2],
             double erdist[2], KalFitWire * wire );
  /// destructor
  ~KalFitHitMdc(void);
  /// Extractor :
  /// Convention for LR (-1 : L, 1 : R)
  int LR(void) const { return LR_;}
  double chi2(void) const { return chi2_;}
  double chi2_back(void) const { return chi2_back_;}
  double tdc(void) const { return tdc_; }
  const double* dist(void) const { return dist_; }
  const double* erdist(void) const { return erdist_; }
  const KalFitWire & wire(void) const { return *wire_; }
  RecMdcHit* rechitptr(void) { return rechitptr_;}
  int id(void) const { return id_;}

  /// Modifier :
  void LR(int LR) { LR_ = LR;}
  void chi2(double chi2) { chi2_ = chi2;}
  void chi2_back(double chi2) { chi2_back_ = chi2;}

  int isolated(void);

 private:
  int  LR_;
  double tdc_;
  double dist_[2];
  double erdist_[2];
  KalFitWire* wire_;
  RecMdcHit* rechitptr_;
  double chi2_, chi2_back_;
  int id_;
};

#endif
