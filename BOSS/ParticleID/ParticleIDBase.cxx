#include <iostream>
#include <cmath>
#include <cstdlib>

#include "ParticleID/ParticleIDBase.h"
#include "TMath.h"

const int ParticleIDBase::USE_DEDX = 1;
const int ParticleIDBase::USE_TOF1 = 2;
const int ParticleIDBase::USE_TOF2 = 4;
const int ParticleIDBase::USE_TOFE = 8;
const int ParticleIDBase::USE_TOFQ = 16;
const int ParticleIDBase::USE_EMC  = 32;
const int ParticleIDBase::USE_MUC  = 64;
const int ParticleIDBase::USE_TOF  = 128;
const int ParticleIDBase::USE_TOFC = 256;
const int ParticleIDBase::USE_TOFCorr = 512;

const int ParticleIDBase::IDENTIFY_ELECTRON = 1;
const int ParticleIDBase::IDENTIFY_MUON = 2;
const int ParticleIDBase::IDENTIFY_PION = 4;
const int ParticleIDBase::IDENTIFY_KAON = 8;
const int ParticleIDBase::IDENTIFY_PROTON  =  16;

const int ParticleIDBase::PROBABILITY_PID = 1;
const int ParticleIDBase::LIKELIHOOD_PID = 2;
const int ParticleIDBase::NEURONNETWORK_PID = 4;

const int ParticleIDBase::DEDX_VALID = 1;
const int ParticleIDBase::TOF1_VALID = 2;
const int ParticleIDBase::TOF2_VALID = 4;
const int ParticleIDBase::TOFE_VALID = 8;
const int ParticleIDBase::TOFQ_VALID = 16;
const int ParticleIDBase::EMC_VALID = 32;
const int ParticleIDBase::MUC_VALID = 64;
const int ParticleIDBase::TOF_VALID = 128;
const int ParticleIDBase::TOFC_VALID = 256;
const int ParticleIDBase::TOFCorr_VALID = 512;


std::string ParticleIDBase::path = "";

ParticleIDBase::ParticleIDBase()
{
   m_trk = 0;
   m_chimin_cut = 4;
   m_chimax_cut = 6;
   m_pdfsigmamin_cut=99;

#ifndef BEAN
   if( path.empty() ) set_path(0);
#endif
}

void ParticleIDBase::set_path(const char* s_path)
{
   if ( s_path ) {
      path = string(s_path);
   } else {
      char* env_path = getenv("PARTICLEIDROOT");
      if ( !env_path ) {
         cout << "  ParticleIDBase::set_path ERROR:"
              " the environment PARTICLEIDROOT not defined " << endl;
         exit(1);
      }
      path = string(env_path);
   }
}


double ParticleIDBase::xmass(int n) {
   double mass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
   if(n < 0 || n >=5) return 0.0;
   return mass[n];
}

double ParticleIDBase::velc() {
   //  double vel = 29.9792458;  // tof_path unit in cm.
   double vel = 299.792458;   // tof path unit in mm
   return vel;
}

double ParticleIDBase::probCalculate(double chi2, int ndof) {
   double p = -1.0;
   if(chi2 < 0) return p;
   p = TMath::Prob(chi2, ndof);
   return p;
}

//huangb add -----------------------------------------
double ParticleIDBase::pdfCalculate(double offset,double sigma) {
   //  const double pi =  3.141592653589793238;
   const double pi =  M_PI;
   const double twoPi = 2*pi;
   double chi2 = -0.5*offset*offset/(sigma*sigma);
   double pdf = exp(chi2)/(sigma*sqrt(twoPi));
   return pdf;
}

double ParticleIDBase::p() {
   double val = 999;
   if(!m_trk) return val;
   if(!m_trk->isMdcTrackValid()) return val;
   RecMdcTrack *mdcTrk = m_trk->mdcTrack();
   val = mdcTrk->p();
   return val;
}
double ParticleIDBase::pt() {
   double val = 999;
   if(!m_trk) return val;
   if(!m_trk->isMdcTrackValid()) return val;
   RecMdcTrack *mdcTrk = m_trk->mdcTrack();
   val = mdcTrk->pxy();
   return val;
}
double ParticleIDBase::charge() {
   double val = 999;
   if(!m_trk) return val;
   if(!m_trk->isMdcTrackValid()) return val;
   RecMdcTrack *mdcTrk = m_trk->mdcTrack();
   val = mdcTrk->charge() + 0.0;
   return val;
}

double ParticleIDBase::interpolation(double* x, double* y,double x1) {
   double c1 = (y[0]-y[1])*(x[1]-x[2])-(x[0]-x[1])*(y[1]-y[2]);
   double c2 = (x[0]*x[0]-x[1]*x[1])*(x[1]-x[2])-(x[1]*x[1]-x[2]*x[2])*(x[0]-x[1]);
   double c = c1/c2;
   double b1 = (y[0]-y[1])*(x[1]*x[1]-x[2]*x[2])-(x[0]*x[0]-x[1]*x[1])*(y[1]-y[2]);
   double b2 = (x[0]-x[1])*(x[1]*x[1]-x[2]*x[2])-(x[1]-x[2])*(x[0]*x[0]-x[1]*x[1]);
   double b = b1/b2;
   double a = y[0] - b*x[0]-c*x[0]*x[0];
   double y1 = a + b*x1 +c*x1*x1;
   return y1;
}

double ParticleIDBase::pol2(double x, double* par) {
   double y=x;
   //    return par[0] + (par[1] * y) +(par[2] * y * y);
   return par[0] + y*(par[1] + y*(par[2]));
}

double ParticleIDBase::pol3(double x, double* par) {
   double y=x;
   //    return par[0] + (par[1] * y) +(par[2] * y * y)+(par[3] * y * y*y);
   return par[0] + y*(par[1] + y*(par[2] + y*(par[3])));
}

double ParticleIDBase::pol4(double x, double* par) {
   double y=x;
   //    return par[0] + (par[1] * y) +(par[2] * y * y)+(par[3] * y * y*y) + (par[4] * y * y*y*y);
   return par[0] + y*(par[1] + y*(par[2] + y*(par[3] + y*(par[4]))));
}
