#include <cmath>

#include "ParticleID/TofQPID.h"

#ifndef BEAN
#include "MdcRecEvent/RecMdcTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EvtRecEvent/EvtRecTrack.h"
#endif

TofQPID * TofQPID::m_pointer = 0;

TofQPID * TofQPID::instance() {
   if(!m_pointer) m_pointer = new TofQPID();
   return m_pointer;
}

TofQPID::TofQPID():ParticleIDBase() {
   ;
}

void TofQPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
   }
   m_chimin = 99.;
   m_ndof = 0;
   m_hitstatus = -2;
   //  m_q1 = -1;
   //  m_q2 = -1;
}

void TofQPID::calculate() {
   if(particleIDCalculation() == 0) m_ndof=1;
}
int TofQPID::particleIDCalculation() {
   int irc = -1;
   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   //   RecMdcTrack* mdcTrk = recTrk->mdcTrack();

   //   double ptrk = mdcTrk->p();
   //   double cost = cos(mdcTrk->theta());

   if(!(recTrk->isTofTrackValid())) return irc;
   // RecTofTrack* tofTrk = recTrk->tofTrack();

   //
   // two layer hit in barrel TOF are required
   //
   //  if(tofTrk->getPart() != 1) return irc;
   // double ph1 = tofTrk->getPh1();
   // double ph2 = tofTrk->getPh2();
   // if(ph1 <= 0 || ph2 <= 0) return irc;
   //
   //  Tof Q calibration is needed
   //
   //  m_hitstatus = tofTrk->getHitStatus();
   //m_hitstatus = 1;
   //if(m_hitstatus == 0 || m_hitstatus == 1) m_q1 = tofTrk->getQ1();
   //if(m_hitstatus == 0 || m_hitstatus == 2) m_q2 = tofTrk->getQ2();

   //m_q1 = 1.0;
   //m_q2 = 1.0;
   m_ndof = 1;
   irc = 0;
   return irc;
}
