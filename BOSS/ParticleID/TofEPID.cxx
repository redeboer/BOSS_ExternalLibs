#include <cmath>

#include "ParticleID/TofEPID.h"

#ifndef BEAN
#include "MdcRecEvent/RecMdcTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#else
#include "TofHitStatus.h"
#endif

TofEPID * TofEPID::m_pointer = 0;

TofEPID * TofEPID::instance() {
   if(!m_pointer) m_pointer = new TofEPID();
   return m_pointer;
}

TofEPID::TofEPID():ParticleIDBase() {
   //readSigPar();
}

void TofEPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
      m_offset[i] = 99.0;
      m_sigma[i] = 1.0;
   }
   m_chimin = 99.;
   m_pdfmin =99.;
   m_ndof = 0;
   m_mass2 = -999;
   m_rhit = -99;
}

void TofEPID::calculate() {
   if(particleIDCalculation()==0) m_ndof=1;
}

int TofEPID::particleIDCalculation() {
   int irc = -1;
   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   if(!(recTrk->isTofTrackValid())) return irc;

#ifndef BEAN
   SmartRefVector<RecTofTrack> tofTrackCol = recTrk->tofTrack();
   SmartRefVector<RecTofTrack>::iterator it;
#else
   const std::vector<TTofTrack* >& tofTrackCol = recTrk->tofTrack();
   std::vector<TTofTrack* >::const_iterator it;
#endif

   bool isMRPC = false;
   if( tofTrackCol.size()!=1 && tofTrackCol.size()!=3 ) { return irc; }
   else {
     TofHitStatus *hitst = new TofHitStatus;
     if( tofTrackCol.size()==1 ) {
       it = tofTrackCol.begin();
       hitst->setStatus( (*it)->status() );
       bool isRaw = hitst->is_raw();
       if( isRaw ) { return irc; }
       isMRPC = hitst->is_mrpc();
       bool isReadout = hitst->is_readout();
       bool isCounter = hitst->is_counter();
       bool isCluster = hitst->is_cluster();
       if( !isReadout || !isCounter || !isCluster ) { return irc; }
     }
     else if( tofTrackCol.size()==3 ) {
       unsigned int icounter = -1;
       unsigned int inumber  = 0;
       for( it = tofTrackCol.begin(); it != tofTrackCol.end(); it++, inumber++ ) {
	 hitst->setStatus( (*it)->status() );
	 bool isRaw = hitst->is_raw();
	 if( isRaw ) continue;
	 isMRPC = hitst->is_mrpc();
	 if( !isMRPC ) { return irc; } // MRPC is possible to have 3 record.
	 bool isReadout = hitst->is_readout();
	 bool isCounter = hitst->is_counter();
	 bool isCluster = hitst->is_cluster();
	 if( !isReadout && isCounter && isCluster ) { icounter = inumber; }
       }
       if( icounter == -1 ) { return irc; }
       it = tofTrackCol.begin() + icounter;
     }
   }

   double tof = (*it)->tof();
   if( tof <= 0 ) { return irc; }
   double path = (*it)->path();
   m_rhit = (*it)->zrhit();
   double beta = path/velc()/tof;
   double beta2 = beta*beta;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();
   double ptrk = mdcTrk->p();
   m_mass2 = ptrk*ptrk*(1.0/beta2 -1.0);

   double chitemp = 99.;
   double pdftemp = 0;
   for( unsigned int i=0; i<5; i++ ) {
     double texp = (*it)->texp(i);
     m_offset[i] = tof - texp - (*it)->toffset(i);
     double sigma_tmp = (*it)->sigma(0);
     if( !isMRPC ) {
       if( sigma_tmp>0 ) {
	 if( i<2 ) { m_sigma[i] = sigma_tmp; }
	 else { m_sigma[i] = 1.2*sigma_tmp; }
       }
       else { m_sigma[i] = 0.12; }
     }
     else {
       //       m_sigma[i] = 0.065;
       int strip = (*it)->strip();
       if( strip<0 || strip>11 ) { m_sigma[i] = 0.065; }
       else {
	 double p0, p1;
	 if( strip==0 )       { p0=0.16;     p1=0.0;      }
	 else if( strip==1 )  { p0=0.051094; p1=0.019467; }
	 else if( strip==2 )  { p0=0.056019; p1=0.012964; }
	 else if( strip==3 )  { p0=0.043901; p1=0.015933; }
	 else if( strip==4 )  { p0=0.049750; p1=0.010473; }
	 else if( strip==5 )  { p0=0.048345; p1=0.008545; }
	 else if( strip==6 )  { p0=0.046518; p1=0.009038; }
	 else if( strip==7 )  { p0=0.048803; p1=0.007251; }
	 else if( strip==8 )  { p0=0.047523; p1=0.008434; }
	 else if( strip==9 )  { p0=0.042187; p1=0.010307; }
	 else if( strip==10 ) { p0=0.050337; p1=0.005951; }
	 else if( strip==11 ) { p0=0.054740; p1=0.005629; }
	 if( ptrk<0.05 ) { m_sigma[i] = p0+p1/0.05; }
	 else { m_sigma[i] = p0+p1/ptrk; }
       }
     }
     m_chi[i] = m_offset[i]/m_sigma[i];

     if( fabs(m_chi[i]) < chitemp ) { chitemp = fabs(m_chi[i]); }
     double ppp = pdfCalculate(m_chi[i],1);
     if( fabs(ppp) > pdftemp ) { pdftemp = fabs(ppp); }
   }
   m_chimin = chitemp;
   //   if( pdftemp < pdfCalculate(pdfMinSigmaCut(),1) ) { return irc; }
   //   if( fabs(m_chimin) > chiMinCut() ) { return irc; }

   for( unsigned int i=0; i<5; i++ ) {
     m_prob[i] = probCalculate( m_chi[i]*m_chi[i], 1 );
   }

   irc = 0;
   return irc;
}

//
//  TOF endcap: Correction routines
//

double TofEPID::offsetTofE(int n, int cntr, double ptrk, double rtof, double ph,double charge) {
   double offset;
   //   double gb = ptrk/xmass(n);
   switch(n) {
   case 0: {  // Electron
      double ptemp = ptrk;
      if(ptrk<0.2) ptemp = 0.2;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      offset = 0.001*(-28.8481+138.159*plog-249.334*plog*plog);
      break;
   }

   case 1: { // Muon
      double ptemp = ptrk;
      if(ptrk<0.2) ptemp = 0.2;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      offset = 0.001*(-33.6966+1.91915*plog-0.592320*plog*plog);
      break;
   }
   case 2: { // Pion
      double ptemp = ptrk;
      if(ptrk<0.2) ptemp = 0.2;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      offset = 0.001*(-27.9965 + 1.213 * plog - 2.02740 * plog * plog);
      break;
   }
   case 3: { // Kaon
      double ptemp = ptrk;
      if(ptrk<0.3) ptemp = 0.3;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      offset = 0.001*(-23.4842 -28.7569 * plog + 78.21* plog *plog);
      break;
   }

   case 4: { // Proton
      double ptemp = ptrk;
      if(ptrk<0.4) ptemp = 0.4;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      if(charge>0)
         offset = 0.001*(-4.854-110.540*plog+99.8732*plog*plog);
      if(charge<0)
         offset = 0.001*(27.047-145.120*plog+167.014*plog*plog);
      break;
   }

   default:
      offset = 0.0;
      break;
   }
   //  offset = 0.0;
   return offset;
}

double TofEPID::sigmaTofE(int n,  int cntr, double ptrk, double rtof, double ph,double charge) {

   double sigma;
   //   double gb = ptrk/xmass(n);
   switch(n) {

   case 0: { // Electron
      double ptemp = ptrk;
      if(ptrk < 0.2) ptemp = 0.2;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      sigma = 0.001 * (109.974 +15.2457 * plog + 36.8139 * plog * plog);

      break;
   }

   case 1: { // Muon
      double ptemp = ptrk;
      if(ptrk < 0.2) ptemp = 0.2;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      sigma = 0.001 * (96.5077 -2.96232 * plog + 3.12910 * plog * plog);
      break;
   }

   case 2: { // pion
      double ptemp = ptrk;
      if(ptrk < 0.2) ptemp = 0.2;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      sigma = 0.001 * (105.447 - 2.08044 * plog + 3.44846 * plog * plog);
      break;
   }

   case 3: { // Kaon
      double ptemp = ptrk;
      if(ptrk < 0.3) ptemp = 0.3;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      sigma = 0.001*(88.8806 - 26.8464 * plog + 113.672 * plog * plog);
      break;
   }
   case 4: { // Proton
      double ptemp = ptrk;
      if(ptrk < 0.5) ptemp = 0.5;
      if(ptrk > 2.1) ptemp = 2.1;
      double plog = log(ptemp);
      if(charge>0)
         sigma = 0.001 * (96.3534 -44.1139 * plog + 53.9805 * plog * plog);
      if(charge<0)
         sigma = 0.001 * (157.345 -98.7357 * plog + 55.1145 * plog * plog);
      break;
   }

   default:
      sigma = 0.100;

      break;
   }
   // sigma =1;
   return sigma;
}

