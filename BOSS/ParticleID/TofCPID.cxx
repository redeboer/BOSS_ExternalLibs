#include <fstream>
#include <cmath>
#include <cstdlib>

#include "ParticleID/TofCPID.h"

#ifndef BEAN
#include "MdcRecEvent/RecMdcTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#else
#include "TofHitStatus.h"
#endif


TofCPID * TofCPID::m_pointer = 0;

TofCPID * TofCPID::instance() {
   if(!m_pointer) m_pointer = new TofCPID();
   return m_pointer;
}

TofCPID::TofCPID():ParticleIDBase() {
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
   m_readstate=0;
}

void TofCPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
      m_offset[i] = 99.0;
      m_sigma[i] = 1.0;
   }
   m_chimin = 99.;
   m_pdfmin =99;
   m_ndof = 0;
}

void TofCPID::calculate() {
   int runtof = getRunNo();
   if(!m_readstate) {
      std::cout<<"read tofC"<<std::endl;
      std::string tofdata_mom_file = path + "/share/pidparatof/tofpdata.txt";
      ifstream inputmomdata(tofdata_mom_file.c_str(),std::ios_base::in);
      if ( !inputmomdata ) {
         cout << " can not open: " << tofdata_mom_file << endl;
         exit(1);
      }

      std::string tofdata_theta_file = path + "/share/pidparatof/tofthetadata.txt";
      ifstream inputthetadata(tofdata_theta_file.c_str(),std::ios_base::in);
      if ( !inputthetadata ) {
         cout << " can not open: " << tofdata_theta_file << endl;
         exit(1);
      }

      std::string tofdata_endcap_file = path + "/share/pidparatof/tofendcapdata.txt";
      ifstream inputendcapdata(tofdata_endcap_file.c_str(),std::ios_base::in);
      if ( !inputendcapdata ) {
         cout << " can not open: " << tofdata_endcap_file << endl;
         exit(1);
      }


      std::string tofmc_mom_file = path + "/share/pidparatof/tofpmc.txt";
      ifstream inputmommc(tofmc_mom_file.c_str(),std::ios_base::in);
      if ( !inputmommc ) {
         cout << " can not open: " << tofmc_mom_file << endl;
         exit(1);
      }

      std::string tofmc_theta_file = path + "/share/pidparatof/tofthetamc.txt";
      ifstream inputthetamc(tofmc_theta_file.c_str(),std::ios_base::in);
      if ( !inputthetamc ) {
         cout << " can not open: " << tofmc_theta_file << endl;
         exit(1);
      }

      std::string tofmc_endcap_file = path + "/share/pidparatof/tofendcapmc.txt";
      ifstream inputendcapmc(tofmc_endcap_file.c_str(),std::ios_base::in);
      if ( !inputendcapmc ) {
         cout << " can not open: " << tofmc_endcap_file << endl;
         exit(1);
      }


      if(runtof>0)
      {
         for(int i=0; i<5; i++)
         {
            for(int j=0; j<8; j++)
            {
               inputthetadata>>m_thetapara[i][j];
            }
         }

         for(int i=0; i<5; i++)
         {
            for(int j=0; j<12; j++)
            {
               inputmomdata>>m_momentpara[i][j];
            }
         }

         for(int i=0; i<5; i++)
         {
            for(int j=0; j<4; j++)
            {
               inputendcapdata>>m_endcappara[i][j];
            }
         }

      } else
      {
         for(int i=0; i<5; i++)
         {
            for(int j=0; j<8; j++)
            {
               inputthetamc>>m_thetapara[i][j];
            }
         }

         for(int i=0; i<5; i++)
         {
            for(int j=0; j<12; j++)
            {
               inputmommc>>m_momentpara[i][j];
            }
         }

         for(int i=0; i<5; i++)
         {
            for(int j=0; j<4; j++)
            {
               inputendcapmc>>m_endcappara[i][j];
            }
         }

      }
      m_readstate=1;
   }
   if(particleIDCalculation() == 0) m_ndof=1;
}

int TofCPID::particleIDCalculation() {
   /*
   cout<<"m_momentpara[2][2]="<<m_momentpara[2][2]<<endl;
   cout<<"m_momentpara[2][3]="<<m_momentpara[2][3]<<endl;
   cout<<"m_momentpara[3][2]="<<m_momentpara[3][2]<<endl;
   cout<<"m_momentpara[3][3]="<<m_momentpara[3][3]<<endl;
   cout<<"m_thetapara[2][2]="<<m_thetapara[2][2]<<endl;
   cout<<"m_thetapara[2][3]="<<m_thetapara[2][3]<<endl;
   cout<<"m_thetapara[3][2]="<<m_thetapara[3][2]<<endl;
   cout<<"m_thetapara[3][3]="<<m_thetapara[3][3]<<endl;
   cout<<"m_endcappara[2][2]="<<m_endcappara[2][2]<<endl;
   cout<<"m_endcappara[2][3]="<<m_endcappara[2][3]<<endl;
   cout<<"m_endcappara[3][2]="<<m_endcappara[3][2]<<endl;
   cout<<"m_endcappara[3][3]="<<m_endcappara[3][3]<<endl;
   */
   int irc = -1;

   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();
   double ptrk = mdcTrk->p();
   //    double charge = mdcTrk->charge();
   double cost = cos(mdcTrk->theta());
   if(!(recTrk->isTofTrackValid())) return irc;

#ifndef BEAN
   SmartRefVector<RecTofTrack> tofTrk = recTrk->tofTrack();
   SmartRefVector<RecTofTrack>::iterator it;//=tofTrk.begin();
#else
   const std::vector<TTofTrack* >& tofTrk = recTrk->tofTrack();
   std::vector<TTofTrack* >::const_iterator it;//=tofTrk.begin();
#endif

   TofHitStatus *hitst = new TofHitStatus;
   std::vector<int> tofccount;
   int goodtofctrk=0;
   for(it = tofTrk.begin(); it!=tofTrk.end(); it++,goodtofctrk++) {
      unsigned int st = (*it)->status();
      hitst->setStatus(st);
      //     if( !(hitst->is_barrel()) ) continue;
      //     if( !(hitst->is_counter()) ) continue;
      //     if( hitst->layer()==1 )  tofccount.push_back(goodtofctrk);
      if(hitst->is_cluster())  tofccount.push_back(goodtofctrk);
   }
   delete hitst;
   if(tofccount.size()!=1) return irc;//not tof2 track or more than 1 tracks
   it = tofTrk.begin()+tofccount[0];
   double tof  = (*it)->tof();
   m_tofc = tof;
   //    int qual = (*it)->quality();
   //    int cntr = (*it)->tofID();
   double path  = ((*it)->path())*10.0;//the unit from mm to cm
   m_pathc = path;
   m_phc  = (*it)->ph(); //no change
   m_zhitc = ((*it)->zrhit())*10;//the unit from mm to cm
   double beta2 = path*path/velc()/velc()/tof/tof;
   m_mass2 = ptrk * ptrk * (1/beta2 -1);
   if ((m_mass2>20)||(m_mass2<-1)) return irc;
   if(tof <=0 ) return irc;
   double chitemp = 99.;
   double pdftemp = 0;
   //    double sigma_tmp= (*it)->sigma(0);
   double testchi[5];
   double testpdf[5];
   for(int i = 0; i < 5; i++) {
      /*
            m_offset[i] = tof - (*it)->texp(i);//- offsetTofC(i, cntr, ptrk, m_zhit1, m_ph1,charge);
            if(sigma_tmp!=0) m_sigma[i] = 1.1*sigma_tmp/1000.;
            else
            m_sigma[i]=sigmaTofC(i, cntr,ptrk,m_zhitc, m_phc,charge);
            m_chi[i] = m_offset[i]/m_sigma[i];
      */
      double sep = tof - (*it)->texp(i)-(*it)->toffset(i);
      m_chi[i] = (sep - offsetTofC(i, ptrk, cost))/sigmaTofC(i, ptrk, cost);
      m_offset[i] = offsetTofC(i, ptrk, cost);
      m_sigma[i] = sigmaTofC(i, ptrk, cost);
      testchi[i]=sep;
      if(fabs(m_chi[i]) < chitemp) chitemp = fabs(m_chi[i]);
      double ppp = pdfCalculate(m_chi[i],1);
      testpdf[i]=ppp;
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
//  dE/dx Correction routines
//

double TofCPID::offsetTofC(int n, double ptrk, double cost) {
   int rundedx2 = getRunNo();
   double offset = 0.0;
   double offsetp = 0.0;
   double offsetc = 0.0;
   double sigcos = 0.0;
   double sigp = 0.0;
   //    double gb = ptrk/xmass(n);

   switch(n) {
   case 0: { // Electron
      double  ptemp = ptrk;
      double  costm = cost;

      if(rundedx2>0)
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }

      double plog = log(ptemp);
      double costcos = cos(costm);
      offsetp= mypol5(plog,m_momentpara[0][0],m_momentpara[0][1],m_momentpara[0][2],m_momentpara[0][3],m_momentpara[0][4],m_momentpara[0][5]);
      sigp=mypol5(plog,m_momentpara[0][6],m_momentpara[0][7],m_momentpara[0][8],m_momentpara[0][9],m_momentpara[0][10],m_momentpara[0][11]);

      if(costm<-0.83) {
         offsetc=m_endcappara[0][0];
         sigcos=m_endcappara[0][2];
      }
      if(costm>0.83) {
         offsetc=m_endcappara[0][1];
         sigcos=m_endcappara[0][3];
      }
      if(fabs(costm)<=0.83)
      {
         offsetc=mypol3(costcos,m_thetapara[0][0],m_thetapara[0][1],m_thetapara[0][2],m_thetapara[0][3]);
         sigcos=mypol3(costcos,m_thetapara[0][4],m_thetapara[0][5],m_thetapara[0][6],m_thetapara[0][7]);
      }


      offset=offsetc+sigcos*offsetp;
      //offset=offsetc;
      offset=offsetp+sigp*offsetc;
      break;
   }

   case 1: {// Muon
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx2>0)
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }

      double plog = log(ptemp);
      double costcos = cos(costm);
      offsetp= mypol5(plog,m_momentpara[1][0],m_momentpara[1][1],m_momentpara[1][2],m_momentpara[1][3],m_momentpara[1][4],m_momentpara[1][5]);
      sigp=mypol5(plog,m_momentpara[1][6],m_momentpara[1][7],m_momentpara[1][8],m_momentpara[1][9],m_momentpara[1][10],m_momentpara[1][11]);

      if(costm<-0.83) {
         offsetc=m_endcappara[1][0];
         sigcos=m_endcappara[1][2];
      }
      if(costm>0.83) {
         offsetc=m_endcappara[1][1];
         sigcos=m_endcappara[1][3];
      }
      if(fabs(costm)<=0.83)
      {
         offsetc=mypol3(costcos,m_thetapara[1][0],m_thetapara[1][1],m_thetapara[1][2],m_thetapara[1][3]);
         sigcos=mypol3(costcos,m_thetapara[1][4],m_thetapara[1][5],m_thetapara[1][6],m_thetapara[1][7]);
      }


      offset=offsetc+sigcos*offsetp;
      //offset=offsetc;
      offset=offsetp+sigp*offsetc;
      break;
   }

   case 2: {// Pion
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx2>0)
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.6) ptemp = 1.6;
      }
      else
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.6) ptemp = 1.6;
      }

      double plog = log(ptemp);
      double costcos = cos(costm);
      offsetp= mypol5(plog,m_momentpara[2][0],m_momentpara[2][1],m_momentpara[2][2],m_momentpara[2][3],m_momentpara[2][4],m_momentpara[2][5]);
      sigp=mypol5(plog,m_momentpara[2][6],m_momentpara[2][7],m_momentpara[2][8],m_momentpara[2][9],m_momentpara[2][10],m_momentpara[2][11]);

      if(costm<-0.83) {
         offsetc=m_endcappara[2][0];
         sigcos=m_endcappara[2][2];
      }
      if(costm>0.83) {
         offsetc=m_endcappara[2][1];
         sigcos=m_endcappara[2][3];
      }
      if(fabs(costm)<=0.83)
      {
         offsetc=mypol3(costcos,m_thetapara[2][0],m_thetapara[2][1],m_thetapara[2][2],m_thetapara[2][3]);
         sigcos=mypol3(costcos,m_thetapara[2][4],m_thetapara[2][5],m_thetapara[2][6],m_thetapara[2][7]);
      }


      offset=offsetc+sigcos*offsetp;
      //offset=offsetc;
      offset=offsetp+sigp*offsetc;
      break;
   }

   case 3: {// Kaon
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx2>0)
      {  if(ptrk < 0.4) ptemp = 0.4;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.4) ptemp = 0.4;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      double plog = log(ptemp);
      double costcos = cos(costm);
      offsetp= mypol5(plog,m_momentpara[3][0],m_momentpara[3][1],m_momentpara[3][2],m_momentpara[3][3],m_momentpara[3][4],m_momentpara[3][5]);
      sigp=mypol5(plog,m_momentpara[3][6],m_momentpara[3][7],m_momentpara[3][8],m_momentpara[3][9],m_momentpara[3][10],m_momentpara[3][11]);

      if(costm<-0.83) {
         offsetc=m_endcappara[3][0];
         sigcos=m_endcappara[3][2];
      }
      if(costm>0.83) {
         offsetc=m_endcappara[3][1];
         sigcos=m_endcappara[3][3];
      }
      if(fabs(costm)<=0.83)
      {
         offsetc=mypol3(costcos,m_thetapara[3][0],m_thetapara[3][1],m_thetapara[3][2],m_thetapara[3][3]);
         sigcos=mypol3(costcos,m_thetapara[3][4],m_thetapara[3][5],m_thetapara[3][6],m_thetapara[3][7]);
      }


      offset=offsetc+sigcos*offsetp;
      //offset=offsetc;
      offset=offsetp+sigp*offsetc;
      break;
   }

   case 4 : { // Proton
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx2>0)
      {  if(ptrk < 0.5) ptemp = 0.5;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.5) ptemp = 0.5;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      double plog = log(ptemp);
      double costcos = cos(costm);
      offsetp= mypol5(plog,m_momentpara[4][0],m_momentpara[4][1],m_momentpara[4][2],m_momentpara[4][3],m_momentpara[4][4],m_momentpara[4][5]);
      sigp=mypol5(plog,m_momentpara[4][6],m_momentpara[4][7],m_momentpara[4][8],m_momentpara[4][9],m_momentpara[4][10],m_momentpara[4][11]);

      if(costm<-0.83) {
         offsetc=m_endcappara[4][0];
         sigcos=m_endcappara[4][2];
      }
      if(costm>0.83) {
         offsetc=m_endcappara[4][1];
         sigcos=m_endcappara[4][3];
      }
      if(fabs(costm)<=0.83)
      {
         offsetc=mypol3(costcos,m_thetapara[4][0],m_thetapara[4][1],m_thetapara[4][2],m_thetapara[4][3]);
         sigcos=mypol3(costcos,m_thetapara[4][4],m_thetapara[4][5],m_thetapara[4][6],m_thetapara[4][7]);
      }

      offset=offsetc+sigcos*offsetp;
      //offset=offsetc;
      offset=offsetp+sigp*offsetc;
      break;
   }

   default:
      offset = 0.0;
      break;
   }
   //  offset = 0.0;
   return offset;
}



double TofCPID::sigmaTofC(int n, double ptrk, double cost) {
   int rundedx3 = getRunNo();
   double sigma = 1.0;
   double sigmap = 1.0;
   double sigmac = 1.0;
   //    double gb = ptrk/xmass(n);
   switch(n) {

   case 0: {// Electron
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx3>0)
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }

      double plog = log(ptemp);
      double costcos = cos(costm);

      sigmap=mypol5(plog,m_momentpara[0][6],m_momentpara[0][7],m_momentpara[0][8],m_momentpara[0][9],m_momentpara[0][10],m_momentpara[0][11]);

      if(costm<-0.83) {
         sigmac=m_endcappara[0][2];
      }
      if(costm>0.83) {
         sigmac=m_endcappara[0][3];
      }
      if(fabs(costm)<0.83)
      {
         sigmac=mypol3(costcos,m_thetapara[0][4],m_thetapara[0][5],m_thetapara[0][6],m_thetapara[0][7]);
      }

      sigma=sigmap*sigmac;
      //sigma=sigmac;
      break;
   }

   case 1: {// Muon
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx3>0)
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.3) ptemp = 1.3;
      }

      double plog = log(ptemp);
      double costcos = cos(costm);

      sigmap=mypol5(plog,m_momentpara[1][6],m_momentpara[1][7],m_momentpara[1][8],m_momentpara[1][9],m_momentpara[1][10],m_momentpara[1][11]);

      if(costm<-0.83) {
         sigmac=m_endcappara[1][2];
      }
      if(costm>0.83) {
         sigmac=m_endcappara[1][3];
      }
      if(fabs(costm)<0.83)
      {
         sigmac=mypol3(costcos,m_thetapara[1][4],m_thetapara[1][5],m_thetapara[1][6],m_thetapara[1][7]);
      }


      sigma=sigmap*sigmac;
      //sigma=sigmac;
      break;
   }

   case 2: {// Pion
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx3>0)
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.6) ptemp = 1.6;
      }
      else
      {  if(ptrk < 0.3) ptemp = 0.3;
         if(ptrk > 1.6) ptemp = 1.6;
      }

      double plog = log(ptemp);
      double costcos = cos(costm);
      sigmap=mypol5(plog,m_momentpara[2][6],m_momentpara[2][7],m_momentpara[2][8],m_momentpara[2][9],m_momentpara[2][10],m_momentpara[2][11]);

      if(costm<-0.83) {
         sigmac=m_endcappara[2][2];
      }
      if(costm>0.83) {
         sigmac=m_endcappara[2][3];
      }
      if(fabs(costm)<0.83)
      {
         sigmac=mypol3(costcos,m_thetapara[2][4],m_thetapara[2][5],m_thetapara[2][6],m_thetapara[2][7]);
      }

      sigma=sigmap*sigmac;
      //sigma=sigmac;

      break;
   }

   case 3: { // Kaon
      double  ptemp = ptrk;
      double  costm = cost;

      if(rundedx3>0)
      {  if(ptrk < 0.4) ptemp = 0.4;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.4) ptemp = 0.4;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      double plog = log(ptemp);
      double costcos = cos(costm);
      sigmap=mypol5(plog,m_momentpara[3][6],m_momentpara[3][7],m_momentpara[3][8],m_momentpara[3][9],m_momentpara[3][10],m_momentpara[3][11]);

      if(costm<-0.83) {
         sigmac=m_endcappara[3][2];
      }
      if(costm>0.83) {
         sigmac=m_endcappara[3][3];
      }
      if(fabs(costm)<0.83)
      {
         sigmac=mypol3(costcos,m_thetapara[3][4],m_thetapara[3][5],m_thetapara[3][6],m_thetapara[3][7]);
      }

      sigma=sigmap*sigmac;
      //sigma=sigmac;
      break;
   }


   case 4: {// Proton
      double  ptemp = ptrk;
      double  costm = cost;
      if(rundedx3>0)
      {  if(ptrk < 0.5) ptemp = 0.5;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      else
      {  if(ptrk < 0.5) ptemp = 0.5;
         if(ptrk > 1.3) ptemp = 1.3;
      }
      double plog = log(ptemp);
      double costcos = cos(costm);
      sigmap=mypol5(plog,m_momentpara[4][6],m_momentpara[4][7],m_momentpara[4][8],m_momentpara[4][9],m_momentpara[4][10],m_momentpara[4][11]);

      if(costm<-0.83) {
         sigmac=m_endcappara[4][2];
      }
      if(costm>0.83) {
         sigmac=m_endcappara[4][3];
      }
      if(fabs(costm)<0.83)
      {
         sigmac=mypol3(costcos,m_thetapara[4][4],m_thetapara[4][5],m_thetapara[4][6],m_thetapara[4][7]);
      }

      sigma=sigmap*sigmac;
      //sigma=sigmac;
      break;
   }

   default:
      sigma = 1.0;
      break;
   }

   //  sigma =1.0;
   return sigma;
}

double TofCPID::mypol3(double x, double par0, double par1, double par2, double par3)
{
   double y = x;
   return par0 + (par1 * y) +(par2 * y * y) + (par3 * y * y * y);

}

double TofCPID::mypol5(double x, double par0, double par1, double par2, double par3, double par4, double par5)
{
   double y = x;
   return par0 + (par1 * y) +(par2 * y * y) + (par3 * y * y * y) + (par4 * y * y * y *y)+ (par5 * y * y * y * y * y);

}



