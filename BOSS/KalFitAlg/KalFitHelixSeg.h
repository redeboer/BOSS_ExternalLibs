#ifndef _DEFINE_HELIXSEG_H_
#define _DEFINE_HELIXSEG_H_

//#include "TrackUtil/Helix.h"
#include "KalFitAlg/helix/Helix.h"
#include "KalFitAlg/KalFitHitMdc.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

using namespace CLHEP;

using namespace KalmanFit;


class KalFitHelixSeg: public Helix {

public:

//KalFitHelixSeg(void);

KalFitHelixSeg(KalFitHitMdc* hit,HepPoint3D pivot,HepVector a,HepSymMatrix Ea);

~KalFitHelixSeg(void) {};

KalFitHitMdc*  HitMdc(void) { return hitmdc_;}
double residual_exclude(void) { return residual_exclude_;}
double residual_include(void) { return residual_include_;}
double doca_exclude(void) { return doca_exclude_;} 
double doca_include(void) { return doca_include_;}
double tof(void) { return tof_;}
double dt(void)  { return dt_;} 
double dd(void)  { return dd_;}
int LR(void) {return lr_;}

void residual_exclude(double res) { residual_exclude_ = res;}
void residual_include(double res) { residual_include_ = res;}
void doca_exclude(double doca) { doca_exclude_ = doca;}
void doca_include(double doca) { doca_include_ = doca;}
void tof(double time) { tof_ = time;}
void dt(double drifttime)  { dt_ = drifttime; }
void dd(double driftdist) {dd_ = driftdist; }
void LR(int lr) {lr_ = lr;}

CLHEP::HepVector a_pre_fwd (void) { return a_pre_fwd_; }
CLHEP::HepVector a_pre_bwd (void) { return a_pre_bwd_; }
CLHEP::HepVector a_filt_fwd(void) { return a_filt_fwd_;}
CLHEP::HepVector a_filt_bwd(void) { return a_filt_bwd_;}
CLHEP::HepVector a_include (void) { return a_include_; }
CLHEP::HepVector a_exclude (void) { return a_exclude_; }


CLHEP::HepSymMatrix& Ea_pre_fwd(void)   { return Ea_pre_fwd_; }
CLHEP::HepSymMatrix Ea_filt_fwd(void)   { return Ea_filt_fwd_;}
CLHEP::HepSymMatrix Ea_pre_bwd (void)   { return Ea_pre_bwd_; }
CLHEP::HepSymMatrix Ea_filt_bwd(void)   { return Ea_filt_bwd_;}
CLHEP::HepSymMatrix Ea_include (void)   { return Ea_include_; }
CLHEP::HepSymMatrix Ea_exclude (void)   { return Ea_exclude_; }


void a_pre_fwd (CLHEP::HepVector a) { a_pre_fwd_  = a; }
void a_pre_bwd (CLHEP::HepVector a) { a_pre_bwd_  = a; }
void a_filt_fwd(CLHEP::HepVector a) { a_filt_fwd_ = a; }
void a_filt_bwd(CLHEP::HepVector a) { a_filt_bwd_ = a; }
void a_include (CLHEP::HepVector a) { a_include_  = a; }
void a_exclude (CLHEP::HepVector a) { a_exclude_  = a; }


void Ea_filt_fwd(CLHEP::HepSymMatrix Ea)   { Ea_filt_fwd_ = Ea; }
void Ea_filt_bwd(CLHEP::HepSymMatrix Ea)   { Ea_filt_bwd_ = Ea; }
void Ea_pre_bwd (CLHEP::HepSymMatrix Ea)   { Ea_pre_bwd_  = Ea; }
void Ea_pre_fwd (CLHEP::HepSymMatrix Ea)   { Ea_pre_fwd_  = Ea; }
void Ea_include (CLHEP::HepSymMatrix Ea)   { Ea_include_  = Ea; }
void Ea_exclude (CLHEP::HepSymMatrix Ea)   { Ea_exclude_  = Ea; }


int layer(void) {return (*hitmdc_).wire().layer().layerId();}



private:
KalFitHitMdc*  hitmdc_;
CLHEP::HepVector a_pre_fwd_;
CLHEP::HepVector a_pre_bwd_;
CLHEP::HepVector a_filt_fwd_;
CLHEP::HepVector a_filt_bwd_;
CLHEP::HepVector a_include_;
CLHEP::HepVector a_exclude_;

CLHEP::HepSymMatrix Ea_pre_fwd_;
CLHEP::HepSymMatrix Ea_filt_fwd_;
CLHEP::HepSymMatrix Ea_pre_bwd_;
CLHEP::HepSymMatrix Ea_filt_bwd_;
CLHEP::HepSymMatrix Ea_include_;
CLHEP::HepSymMatrix Ea_exclude_;

int lr_;
double residual_exclude_;
double residual_include_;
double doca_exclude_;
double doca_include_;
double  tof_;
double  dt_;
double  dd_;

};

#endif
