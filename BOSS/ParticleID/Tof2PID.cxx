#include <cmath>
#include "TMath.h"

#include "ParticleID/Tof2PID.h"

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "DstEvent/TofHitStatus.h"
#else
#include "TofHitStatus.h"
#endif

Tof2PID * Tof2PID::m_pointer = 0;

Tof2PID * Tof2PID::instance() {
   if(!m_pointer) m_pointer = new Tof2PID();
   return m_pointer;
}


Tof2PID::Tof2PID():ParticleIDBase() {
   m_pars[0]=-0.237207;
   m_pars[1]= 1.90436;
   m_pars[2]= -0.210625;
   m_pars[3]= 0.664667;
   m_pars[4]= 0.00165226;
   m_pars[5]= -1.86503e-06;
   m_pars[6]= 6.07045e-10;
   m_pars[7]=-0.0882228;
   m_pars[8]= 0.0125708;
   m_pars[9]= -0.117157;
   m_pars[10]=   0.00252878;
   m_pars[11]= 0.254343;
   m_pars[12]= -5.74886;
   m_pars[13]=   5.20507;
   m_pars[14]= 1.86515;

}

void Tof2PID::init() {

   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
      m_sigma[i] = 1.0;
      m_offset[i] =99.0;
   }
   m_chimin = 99.;
   m_pdfmin =99.;
   m_ndof = 0;
   m_mass2 = -999;
   m_ph2 = -99;
   m_zhit2 = -99;
}

void Tof2PID::calculate() {
   if(particleIDCalculation() == 0) m_ndof=1;
}

int Tof2PID::particleIDCalculation() {
   int irc = -1;
   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();

   double ptrk = mdcTrk->p();
   //   double cost = cos(mdcTrk->theta());
   double charge = mdcTrk->charge();

   if(!(recTrk->isTofTrackValid())) return irc;

#ifndef BEAN
   SmartRefVector<RecTofTrack> tofTrk = recTrk->tofTrack();
   SmartRefVector<RecTofTrack>::iterator it;//=tofTrk.begin();
#else
   const std::vector<TTofTrack* >& tofTrk = recTrk->tofTrack();
   std::vector<TTofTrack* >::const_iterator it;//=tofTrk.begin();
#endif

   TofHitStatus *hitst = new TofHitStatus;
   std::vector<int> tof2count;
   int goodtof2trk=0;
   for(it = tofTrk.begin(); it!=tofTrk.end(); it++,goodtof2trk++) {
      unsigned int st = (*it)->status();
      hitst->setStatus(st);

      if( !(hitst->is_barrel()) ) continue;
      if( !(hitst->is_counter()) ) continue;
      if( hitst->layer()==2 ) tof2count.push_back(goodtof2trk);
   }
   delete hitst;

   if(tof2count.size()!=1) return irc;//not tof2 track or more than 1 tracks
   it = tofTrk.begin()+tof2count[0];
   double tof  = (*it)->tof();
   m_tof2 = tof;

   if(tof <=0 ) return irc;
   //   int qual = (*it)->quality();
   //  if(qual != 1) return irc;
   int cntr = (*it)->tofID();
   double path = ((*it)->path())*10.0;//the unit from mm to cm

   m_path2 = path;
   //   m_path2 = path;
   // fix the bugs the 6.0.0, He K.L.
   // double path = tofTrk->getPath1();
   //
   m_ph2   = (*it)->ph();
   m_zhit2 = ((*it)->zrhit())*10; //the unit from mm to cm

   double beta2 = path*path/velc()/velc()/tof/tof;
   m_mass2 = ptrk * ptrk * (1/beta2 -1);
   if ((m_mass2>20)||(m_mass2<-1)) return irc;

   double chitemp = 99.;
   double pdftemp = 0;
   double sig_tmp = (*it)->sigma(0);
   for(int i = 0; i < 5; i++) {
      m_offset[i] = tof - (*it)->texp(i)-(*it)->toffset(i);//- offsetTof1(i, cntr, ptrk, m_zhit1, m_ph1,charge);
      if(sig_tmp!=0)  {
         m_sigma[i] = 1.2*sig_tmp;
         if(i<2) m_sigma[i]=sig_tmp;
      }
      else m_sigma[i]= sigmaTof2(i, cntr,ptrk,m_zhit2, m_ph2,charge);
      m_chi[i] = m_offset[i]/m_sigma[i];
      if(fabs(m_chi[i]) < chitemp) chitemp = fabs(m_chi[i]);
      double ppp = pdfCalculate(m_chi[i],1);
      if(fabs(ppp) > pdftemp) pdftemp = fabs(ppp);
   }
   //  if(ptrk<0.65) m_chi[2]=m_chi[3];
   m_chimin = chitemp;
   m_pdfmin = pdftemp;
   if(pdftemp < pdfCalculate(pdfMinSigmaCut(),1)) return irc;
   if(fabs(m_chimin) > chiMinCut()) return irc;

   // calculate prob

   for(int i = 0; i < 5; i++) {
      m_prob[i] = probCalculate(m_chi[i]*m_chi[i], 1);
   }
   irc = 0;
   return irc;
}


//
//  TOF endcap: Correction routines
//

double Tof2PID::offsetTof2(int n, int cntr, double ptrk, double z, double ph,double charge) {
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
   z=z/1000.0;
   double Q = ph;
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
   func[7]=1./(0.89*0.89+z*z);
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

double Tof2PID::sigmaTof2(int n,  int cntr, double ptrk, double ztof, double ph,double charge) {
   double sigma;
   //   double gb = ptrk/xmass(n);
   double cosz = cos(ztof/1000.0);
   //  double phtemp=ph;
   switch(n) {

   case 0: { // Electron
      sigma = 0.115816 -0.0933215*cosz+ 0.0788252*cosz*cosz;
      break;
   }

   case 1: { // Muon
      sigma = 0.121536 -0.0935898*cosz+0.0748771*cosz*cosz;
      break;
   }

   case 2: { // Pion
      sigma =  0.106882-0.0147375*cosz+0.027491*cosz*cosz;
      break;
   }
   case 3: { // Kaon
      sigma =0.106882 -0.0147375*cosz +0.027491 *cosz*cosz;
      break;
   }

   case 4: { // Proton
      sigma = 0.168489 -0.131762*cosz+ 0.105708*cosz*cosz;
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


double Tof2PID::sampleQ0(double betagamma,double beta) {
   double p1 = 0.261;
   double p2 = 5.43;
   double p3 = 1.12;
   double p4 = 1.96;
   double p5 = 0.386;
   double val = p1 / TMath::Power(beta, p4) *
                ( p2- TMath::Power(beta, p4) -
                  TMath::Log(p3 + TMath::Power(1/betagamma, p5)));

   return val*100.;
}

