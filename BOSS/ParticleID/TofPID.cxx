#include <cmath>

#include "ParticleID/TofPID.h"
#include "ParticleID/Tof1PID.h"
#include "ParticleID/Tof2PID.h"

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#endif


TofPID * TofPID::m_pointer = 0;
TofPID * TofPID::instance() {
   if(!m_pointer) m_pointer = new TofPID();
   return m_pointer;
}

TofPID::TofPID():ParticleIDBase() {
   ;
}

void TofPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
      m_sigma[i] = 1;
      m_offset[i] = 99.0;
   }
   m_chimin = 99.;
   m_pdfmin = 99.;
   m_ndof = 0;
}

double TofPID :: chilayer1(int n) const {
   double chi = 99;
   if(val_tof1) chi = m_tof1pid->chi(n);
   return chi;
}

double TofPID :: chilayer2(int n) const {
   double chi = 99;
   if(val_tof2) chi = m_tof2pid->chi(n);
   return chi;
}

void TofPID::calculate() {
   if(particleIDCalculation() == 0) m_ndof=1;
}



int TofPID::particleIDCalculation() {
   int irc=-1;
   EvtRecTrack* recTrk = PidTrk();
   m_tof1pid = Tof1PID::instance();
   m_tof1pid->init();
   m_tof1pid->setRecTrack(recTrk);
   m_tof1pid->setChiMinCut(chiMinCut());
   m_tof1pid->setPdfMinSigmaCut(pdfMinSigmaCut());
   m_tof1pid->calculate();
   val_tof1 = m_tof1pid->IsPidInfoValid();
   m_tof2pid = Tof2PID::instance();
   m_tof2pid->init();
   m_tof2pid->setRecTrack(recTrk);
   m_tof2pid->setChiMinCut(chiMinCut());
   m_tof2pid->setPdfMinSigmaCut(pdfMinSigmaCut());
   m_tof2pid->calculate();
   val_tof2 = m_tof2pid->IsPidInfoValid();
   double chitemp = 99.;
   double pdftemp = 0;
   for(int i=0; i<5; i++) {
      double t_tof1 = m_tof1pid->offset(i);
      double t_tof2 = m_tof2pid->offset(i);
      double m_sigma1 = m_tof1pid->sigma(i);
      double m_sigma2 = m_tof2pid->sigma(i);
      //  double weight1,weight2;
      if(val_tof1==1&&val_tof2==1) {
         weight1 =  (m_sigma2*m_sigma2-0.041*0.041)/(m_sigma1*m_sigma1+m_sigma2*m_sigma2-2*0.041*0.041);
         weight2 = (m_sigma1*m_sigma1-0.041*0.041)/(m_sigma1*m_sigma1+m_sigma2*m_sigma2-2*0.041*0.041);
         m_sigma[i] = sqrt((m_sigma1*m_sigma1*m_sigma2*m_sigma2 - 0.041*0.041*0.041*0.041)/(m_sigma1*m_sigma1+m_sigma2*m_sigma2-2*0.041*0.041));
         m_offset[i] = weight1*t_tof1+weight2*t_tof2;
         m_chi[i] = m_offset[i]/m_sigma[i];
      }
      if(val_tof1==1&&val_tof2==0) {
         weight1=1;
         weight2=0;
         m_sigma[i] =  m_tof1pid->sigma(i);
         m_offset[i] = t_tof1;
         m_chi[i] = m_tof1pid->chi(i);
      }

      if(val_tof1==0&&val_tof2==1) {
         weight1=0;
         weight2=1;
         m_sigma[i] =  m_tof2pid->sigma(i);
         m_offset[i] = t_tof2;
         m_chi[i] = m_tof2pid->chi(i);
      }

      if(val_tof1==0&&val_tof2==0) return irc;
      if(fabs(m_chi[i]) < chitemp) chitemp = fabs(m_chi[i]);
      double ppp = pdfCalculate(m_chi[i],1);
      if(fabs(ppp) > pdftemp) pdftemp = fabs(ppp);
   }

   m_chimin = chitemp;
   m_pdfmin = pdftemp;
   if(pdftemp < pdfCalculate(pdfMinSigmaCut(),1)) return irc;
   if(m_chimin > chiMinCut()) return irc;
   for(int i = 0; i < 5; i++)
      m_prob[i] = probCalculate(m_chi[i]*m_chi[i], 1);
   irc = 0;
   return irc;
}



