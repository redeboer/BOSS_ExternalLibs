#include <cmath>
#include "TMath.h"

#include "ParticleID/Tof1PID.h"

#ifndef BEAN
#include "MdcRecEvent/RecMdcTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#else
#include "TofHitStatus.h"
#endif

Tof1PID * Tof1PID::m_pointer = 0;
Tof1PID * Tof1PID::instance() {
   if(!m_pointer) m_pointer = new Tof1PID();
   return m_pointer;
}

Tof1PID::Tof1PID():ParticleIDBase() {
   m_pars[0]= -0.208289;
   m_pars[1]=  1.63092;
   m_pars[2]= -0.0733139;
   m_pars[3]= 1.02385;
   m_pars[4]= 0.00145052;
   m_pars[5]= -1.72471e-06;
   m_pars[6]= 5.92726e-10;
   m_pars[7]= -0.0645428;
   m_pars[8]= -0.00143637;
   m_pars[9]= -0.133817;
   m_pars[10]= 0.0101188;
   m_pars[11]= -5.07622;
   m_pars[12]= 5.31472;
   m_pars[13]= -0.443142;
   m_pars[14]= -12.1083;

}

void Tof1PID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
      m_sigma[i] = 1;
      m_offset[i] = 99.0;
   }
   m_chimin = 99.;
   m_pdfmin = 99.;
   m_ndof = 0;
   m_mass2 = -999;
   m_ph1 = -99;
   m_zhit1 = -99;
}

void Tof1PID::calculate() {
   if(particleIDCalculation() == 0) m_ndof=1;
}

int Tof1PID::particleIDCalculation() {
   int irc = -1;

   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();
   double ptrk = mdcTrk->p();
   double charge = mdcTrk->charge();
   //   double cost = cos(mdcTrk->theta());
   if(!(recTrk->isTofTrackValid())) return irc;

#ifndef BEAN
   SmartRefVector<RecTofTrack> tofTrk = recTrk->tofTrack();
   SmartRefVector<RecTofTrack>::iterator it;//=tofTrk.begin();
#else
   const std::vector<TTofTrack* >& tofTrk = recTrk->tofTrack();
   std::vector<TTofTrack* >::const_iterator it;//=tofTrk.begin();
#endif

   TofHitStatus *hitst = new TofHitStatus;
   std::vector<int> tof1count;
   int goodtof1trk=0;
   for(it = tofTrk.begin(); it!=tofTrk.end(); it++,goodtof1trk++) {
      unsigned int st = (*it)->status();
      hitst->setStatus(st);
      if( !(hitst->is_barrel()) ) continue;
      if( !(hitst->is_counter()) ) continue;
      if( hitst->layer()==1 )  tof1count.push_back(goodtof1trk);
   }
   delete hitst;
   if(tof1count.size()!=1) return irc;//not tof2 track or more than 1 tracks
   it = tofTrk.begin()+tof1count[0];
   double tof  = (*it)->tof();
   m_tof1 = tof;
   //   int qual = (*it)->quality();
   // if(qual != 1) return irc;
   int cntr = (*it)->tofID();
   double path  = ((*it)->path())*10.0;//the unit from mm to cm
   m_path1 = path;
   m_ph1  = (*it)->ph(); //no change
   m_zhit1 = ((*it)->zrhit())*10;//the unit from mm to cm
   double beta2 = path*path/velc()/velc()/tof/tof;
   m_mass2 = ptrk * ptrk * (1/beta2 -1);
   if ((m_mass2>20)||(m_mass2<-1)) return irc;
   if(tof <=0 ) return irc;
   double chitemp = 99.;
   double pdftemp = 0;
   double sigma_tmp= (*it)->sigma(0);
   for(int i = 0; i < 5; i++) {

      m_offset[i] = tof - (*it)->texp(i)-(*it)->toffset(i);//- offsetTof1(i, cntr, ptrk, m_zhit1, m_ph1,charge);
      if(sigma_tmp!=0) {
         m_sigma[i] = 1.2*sigma_tmp;
         if(i<2) m_sigma[i]=sigma_tmp;
      }
      else m_sigma[i]=sigmaTof1(i, cntr,ptrk,m_zhit1, m_ph1,charge);
      m_chi[i] = m_offset[i]/m_sigma[i];

      if(fabs(m_chi[i]) < chitemp) chitemp = fabs(m_chi[i]);
      double ppp = pdfCalculate(m_chi[i],1);
      if(fabs(ppp) > pdftemp) pdftemp = fabs(ppp);
   }
   m_chimin = chitemp;
   m_pdfmin = pdftemp;
   if(pdftemp < pdfCalculate(pdfMinSigmaCut(),1)) return irc;
   if(fabs(m_chimin) > chiMinCut()) return irc;
   for(int i = 0; i < 5; i++) {
      m_prob[i] = probCalculate(m_chi[i]*m_chi[i], 1);
   }
   irc = 0;
   return irc;
}


//
//  TOF endcap: Correction routines
//

double Tof1PID::offsetTof1(int n, int cntr, double ptrk, double z, double ph1,double charge) {
   double offset;
   //   double gb = ptrk/xmass(n);
   double betagamma;
   switch(n) {

   case 0: {  // Electron
      offset = 0.0;
      return offset;
   }

   case 1: { // Muon
      offset = 0.0;
      return offset;
   }

   case 2: { // Pion
      betagamma=ptrk/139.57;
      break;
   }
   case 3: { // Kaon
      betagamma=ptrk/493.68;
      break;
   }

   case 4: { // Proton
      betagamma=ptrk/938.27;
      break;
   }

   default: {
      offset = 0.0;
      return offset;
   }
   break;
   }
   double Q = ph1;
   z=z/1000.0;
   double beta = betagamma / TMath::Sqrt(1 + betagamma*betagamma);
   double Q0 = sampleQ0(betagamma,beta);
   double func[15]= {0.};
   func[0]=1.;
   func[1]=1./sqrt(Q);
   func[2]=z/sqrt(Q);
   func[3]=z*z/sqrt(Q);
   func[4]=Q;
   func[5]=Q*Q;
   func[6]=Q*Q*Q;
   func[7]=1./(0.84*0.84+z*z);
   func[8]=z;
   func[9]=z*z;
   func[10]=z*z*z;
   func[11]=1./sqrt(Q0);
   func[12]=z/sqrt(Q0);
   func[13]=z*z/sqrt(Q0);
   func[14]=z*z*z/sqrt(Q0);
   offset=0.;
   for(int i=0; i<15; i++) {
      offset+= m_pars[i]*func[i];
   }

   return offset;
}

double Tof1PID::sigmaTof1(int n,  int cntr, double ptrk, double ztof, double ph,double charge) {
   double sigma;
   //   double gb = ptrk/xmass(n);
   double cosz = cos(ztof/1000.0);

   switch(n) {

   case 0: { // Electron
      sigma = 0.132405-0.135638*cosz+0.105528*cosz*cosz;
      break;
   }

   case 1: { // Muon
      sigma = 0.164057 -0.199648*cosz+ 0.139481*cosz*cosz;
      break;
   }

   case 2: { // Pion
      sigma = 0.132943-0.0996678*cosz+0.0785578*cosz*cosz;
      break;
   }
   case 3: { // Kaon
      sigma = 0.146572 -0.124672*cosz+0.0920656*cosz*cosz;
      break;
   }

   case 4: { // Proton
      sigma = 0.1513 -0.0806081*cosz+0.0607409*cosz*cosz;
      break;
   }

   default:
      sigma = 0.100;
      break;
   }
   sigma = 0.110;
   //sigma = 1.0;
   return sigma;
}


double Tof1PID::sampleQ0(double betagamma,double beta) {
   double val = 0.261/ TMath::Power(beta, 1.96) *( 5.43- TMath::Power(beta, 1.96) -TMath::Log(1.12 + TMath::Power(1/betagamma, 0.386)));
   return val*100.;
}
