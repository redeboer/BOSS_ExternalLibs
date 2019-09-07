#include <fstream>
#include <cmath>
#include <cstdlib>

#include "ParticleID/DedxPID.h"
#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
#else
#include "RootEventDataVer.h"
#include "DstEvtRecTracks.h"
#endif

DedxPID * DedxPID::m_pointer = 0;

DedxPID * DedxPID::instance() {
   if(!m_pointer) m_pointer = new DedxPID();
   return m_pointer;
}

DedxPID::DedxPID():ParticleIDBase() {
   m_readstate=0;
}

void DedxPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
      m_offset[i] = 99.0;
      m_sigma[i] = 1.0;
   }
   m_chimin = 99.;
   m_pdfmin =99;
   m_ndof = 0;
   m_goodHits = -99;
   m_normPH = -99;
   m_probPH = -99;
   m_nhitcutdx=5;
}

void DedxPID::calculate() {
   //    int rundedx = getRunNo();
   if(!m_readstate) {
      inputpar();
      m_readstate=1;
   }
   if(particleIDCalculation() == 0) m_ndof=1;
}

int DedxPID::particleIDCalculation() {
   //    int rundedx2 = getRunNo();
   int nhitcutdedx=getNhitCutDx();
   int irc = -1;
   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();

   double ptrk = mdcTrk->p();
   int charge = mdcTrk->charge();
   if(ptrk>5) return irc;
   double cost = cos(mdcTrk->theta());
   //   double sig_the= sin(mdcTrk->theta());

   if(!(recTrk->isMdcDedxValid())) return irc;
   RecMdcDedx* dedxTrk = recTrk->mdcDedx();

   if((dedxTrk->normPH()>30)||(dedxTrk->normPH()<0)) return irc;
   m_goodHits = dedxTrk->numGoodHits();
   if(dedxTrk->numGoodHits()<nhitcutdedx) return irc;
   m_normPH = dedxTrk->normPH();
   m_probPH = dedxTrk->probPH();
   // calculate chi and min chi
   double chitemp = 99.;
   double pdftemp = 0;
   //  double testchi[5];
   //  double testptrk[5];
   //  double testcost[5];
   for(int i = 0; i < 5; i++) {
      double sep = dedxTrk->chi(i);

#ifndef BEAN
      string sftver = getenv("BES_RELEASE");
      string sft;
      sft.assign(sftver,0,5);
      if(sft=="6.6.0"||sft=="6.5.5") {
         m_chi[i] = CorrDedx(i,ptrk,cost,sep,charge);
      }
      else
         m_chi[i]=sep;
#else
// This is BEAN part:
#if (ROOTEVENTDATA_VERSION_NUMERIC == ROOTEVENTDATA_VER(6,5,5) ||\
     ROOTEVENTDATA_VERSION_NUMERIC == ROOTEVENTDATA_VER(6,6,0)   )
      m_chi[i] = CorrDedx(i,ptrk,cost,sep,charge);
#else
      m_chi[i]=sep;
#endif
#endif

      m_offset[i] = offsetDedx(i, ptrk, cost);
      m_sigma[i] = sigmaDedx(i, ptrk, cost);
      if(fabs(m_chi[i]) < chitemp) chitemp = fabs(m_chi[i]);
      double ppp = pdfCalculate(m_chi[i],1);
      if(fabs(ppp) > pdftemp) pdftemp = fabs(ppp);

   }
   m_chimin = chitemp;
   m_pdfmin = pdftemp;
   if(m_chimin > chiMinCut()) return irc;
   if(pdftemp < pdfCalculate(pdfMinSigmaCut(),1)) return irc;


   // calculate prob

   for(int i = 0; i < 5; i++)
      m_prob[i] = probCalculate(m_chi[i]*m_chi[i], 1);

   m_ndof = 1;
   irc = 0;
   return irc;
}

//
//  dE/dx Correction routines
//



double DedxPID::offsetDedx(int n, double ptrk, double cost) {
   return 0;
}

double DedxPID::CorrDedx(int n, double ptrk, double cost,double chi,int charge) {
   int rundedx2 = getRunNo();
   double offset = 0.0;
   double offsetp = 0.0;
   double offsetc = 0.0;
   double sigcos = 1;
   double sigp = 1;
   double chicor=chi;
   //   double gb = ptrk/xmass(n);

   switch(n) {
   case 0: { // Electron
      break;
   }

   case 1: {// Muon
      break;
   }

   case 2: {// Pion
      //     double  ptemp = ptrk;
      double  costm = cost;
      if(ptrk<0.1||ptrk>1) break;
      int index = int((ptrk-0.1)/0.05);
      if(index<=0) index=1;
      if(index>=17) index=16;

      if(fabs(costm)>=0.8) break;
      int index1 = int((costm+0.8)/0.1);
      if(index1<=0) index1=1;
      if(index1>=15) index1=14;

      //psipp data
      if(rundedx2>=11414&&rundedx2<=14604) {
         offsetp = cal_par(index,m_psipp_pi_ptrk_offset,ptrk,0.125,0.05);
         sigp = cal_par(index,m_psipp_pi_ptrk_sigma,ptrk,0.125,0.05);
         offsetc = cal_par(index1,m_psipp_pi_theta_offset,costm,-0.75,0.1);
         sigcos = cal_par(index1,m_psipp_pi_theta_sigma,costm,-0.75,0.1);
      }
      //psipp mc
      if(rundedx2<=-11414&&rundedx2>=-14604) {
         offsetp = cal_par(index,m_psipp_mc_pi_ptrk_offset,ptrk,0.125,0.05);
         sigp = cal_par(index,m_psipp_mc_pi_ptrk_sigma,ptrk,0.125,0.05);
         offsetc = cal_par(index1,m_psipp_mc_pi_theta_offset,costm,-0.75,0.1);
         sigcos = cal_par(index1,m_psipp_mc_pi_theta_sigma,costm,-0.75,0.1);
      }

      offset=offsetp+sigp*offsetc;
      chicor=(chicor-offset)/(sigcos*sigp);
      break;
   }

   case 3: {// Kaon
      //     double  ptemp = ptrk;
      double  costm = cost;
      if(ptrk<0.3||ptrk>0.8) break;
      offset=0;
      int index = int((ptrk-0.3)/0.1);
      if(index<=0) index=1;
      if(index>=4) index=3;

      int index1 = int((costm+0.9)/0.1);
      if(index1<=0) index1=1;
      if(index1>=17) index1=16;
      //data Jpsi
      if(rundedx2>=9947&&rundedx2<=10878) {
         if(charge>0) {
            offsetp = cal_par(index,m_jpsi_kap_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_kap_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_kap_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_kap_theta_sigma,costm,-0.85,0.1);
            }
         }
         if(charge<0) {
            offsetp = cal_par(index,m_jpsi_kam_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_kam_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_kam_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_kam_theta_sigma,costm,-0.85,0.1);
            }
         }
      }

      //mc Jpsi
      if(rundedx2<=-9947&&rundedx2>=-10878) {
         if(charge>0) {
            offsetp = cal_par(index,m_jpsi_mc_kap_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_mc_kap_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_mc_kap_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_mc_kap_theta_sigma,costm,-0.85,0.1);
            }
         }
         if(charge<0) {
            offsetp = cal_par(index,m_jpsi_mc_kam_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_mc_kam_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_mc_kam_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_mc_kam_theta_sigma,costm,-0.85,0.1);
            }
         }
      }

      //data Psip
      if(rundedx2>=8093&&rundedx2<=9025) {
         if(ptrk<0.3||ptrk>1.2) break;
         index = int((ptrk-0.3)/0.1);
         if(index<=0) index=1;
         if(index>=8) index=7;
         if(charge>0) {
            offsetp = cal_par(index,m_psip_kap_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_kap_ptrk_sigma,ptrk,0.35,0.1);
         }
         if(charge<0) {
            offsetp = cal_par(index,m_psip_kam_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_kam_ptrk_sigma,ptrk,0.35,0.1);
         }
      }

      //mc Psip
      if(rundedx2<=-8093&&rundedx2>=-9025) {
         //    if(ptrk < 0.4) ptrk = 0.4;
         if(ptrk<0.3||ptrk>1.2) break;
         index = int((ptrk-0.3)/0.1);
         if(index<=0) index=1;
         if(index>=8) index=7;
         if(charge>0) {
            offsetp = cal_par(index,m_psip_mc_kap_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_mc_kap_ptrk_sigma,ptrk,0.35,0.1);
         }
         if(charge<0) {
            offsetp = cal_par(index,m_psip_mc_kam_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_mc_kam_ptrk_sigma,ptrk,0.35,0.1);
         }
      }


      //psipp kaon data
      if(rundedx2>=11414&&rundedx2<=14604) {
         if(ptrk<0.15||ptrk>1) break;
         index = int((ptrk-0.15)/0.05);
         if(index<=0) index=1;
         if(index>=16) index=15;
         if(fabs(costm)>=0.8) break;
         index1 = int((costm+0.8)/0.1);
         if(index1<=0) index1=1;
         if(index1>=15) index1=14;

         offsetp = cal_par(index,m_psipp_ka_ptrk_offset,ptrk,0.175,0.05);
         sigp = cal_par(index,m_psipp_ka_ptrk_sigma,ptrk,0.175,0.05);
         offsetc = cal_par(index1,m_psipp_ka_theta_offset,costm,-0.75,0.1);
         sigcos = cal_par(index1,m_psipp_ka_theta_sigma,costm,-0.75,0.1);
      }
      //psipp kaon mc
      if(rundedx2<=-11414&&rundedx2>=-14604) {
         if(ptrk<0.15||ptrk>1) break;
         index = int((ptrk-0.15)/0.05);
         if(index<=0) index=1;
         if(index>=16) index=15;
         if(fabs(costm)>=0.8) break;
         index1 = int((costm+0.8)/0.1);
         if(index1<=0) index1=1;
         if(index1>=15) index1=14;
         offsetp = cal_par(index,m_psipp_mc_ka_ptrk_offset,ptrk,0.175,0.05);
         sigp = cal_par(index,m_psipp_mc_ka_ptrk_sigma,ptrk,0.175,0.05);
         offsetc = cal_par(index1,m_psipp_mc_ka_theta_offset,costm,-0.75,0.1);
         sigcos = cal_par(index1,m_psipp_mc_ka_theta_sigma,costm,-0.75,0.1);
      }

      offset=offsetp+sigp*offsetc;
      chicor=(chicor-offset)/(sigcos*sigp);
      break;
   }

   case 4 : { // Proton
      //     double  ptemp = ptrk;
      double  costm = cost;
      if(ptrk<0.3||ptrk>1.1) break;
      int index = int((ptrk-0.3)/0.1);
      if(index<=0) index=1;
      if(index>=7) index=6;

      int index1 = int((costm+0.9)/0.1);
      if(index1<=0) index1=1;
      if(index1>=17) index1=16;

      //    double plog = log(ptemp);
      offset=0;
      if(rundedx2>=9947&&rundedx2<=10878) {
         if(charge>0) {
            offsetp = cal_par(index,m_jpsi_protonp_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_protonp_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_protonp_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_protonp_theta_sigma,costm,-0.85,0.1);
            }
         }
         if(charge<0) {
            offsetp = cal_par(index,m_jpsi_protonm_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_protonm_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_protonm_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_protonm_theta_sigma,costm,-0.85,0.1);
            }
         }
      }

      //mc JPsi
      if(rundedx2<=-9947&&rundedx2>=-10878) {
         if(charge>0) {
            offsetp = cal_par(index,m_jpsi_mc_protonp_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_mc_protonp_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_mc_protonp_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_mc_protonp_theta_sigma,costm,-0.85,0.1);
            }
         }
         if(charge<0) {
            offsetp = cal_par(index,m_jpsi_mc_protonm_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_jpsi_mc_protonm_ptrk_sigma,ptrk,0.35,0.1);
            if(fabs(costm)<=0.83) {
               offsetc = cal_par(index1,m_jpsi_mc_protonm_theta_offset,costm,-0.85,0.1);
               sigcos = cal_par(index1,m_jpsi_mc_protonm_theta_sigma,costm,-0.85,0.1);
            }
         }
      }

      //data Psip
      if(rundedx2>=8093&&rundedx2<=9025) {
         if(charge>0) {
            offsetp = cal_par(index,m_psip_protonp_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_protonp_ptrk_sigma,ptrk,0.35,0.1);
         }
         if(charge<0) {
            offsetp = cal_par(index,m_psip_protonm_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_protonm_ptrk_sigma,ptrk,0.35,0.1);
         }
      }

      //mc Psip
      if(rundedx2<=-8093&&rundedx2>=-9025) {
         if(charge>0) {
            offsetp = cal_par(index,m_psip_mc_protonp_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_mc_protonp_ptrk_sigma,ptrk,0.35,0.1);
         }
         if(charge<0) {
            offsetp = cal_par(index,m_psip_mc_protonm_ptrk_offset,ptrk,0.35,0.1);
            sigp = cal_par(index,m_psip_mc_protonm_ptrk_sigma,ptrk,0.35,0.1);
         }
      }

      //psipp proton data
      if(rundedx2>=11414&&rundedx2<=14604) {
         if(ptrk<0.2||ptrk>1.1) break;
         index = int((ptrk-0.2)/0.05);
         if(index<=0) index=1;
         if(index>=17) index=16;
         if(fabs(costm)>=0.83) break;
         index1 = int((costm+0.9)/0.1);
         if(index1<=0) index1=1;
         if(index1>=17) index1=16;

         offsetp = cal_par(index,m_psipp_proton_ptrk_offset,ptrk,0.225,0.05);
         sigp = cal_par(index,m_psipp_proton_ptrk_sigma,ptrk,0.225,0.05);
         offsetc = cal_par(index1,m_psipp_proton_theta_offset,costm,-0.85,0.1);
         sigcos = cal_par(index1,m_psipp_proton_theta_sigma,costm,-0.85,0.1);
      }
      //psipp proton mc
      if(rundedx2<=-11414&&rundedx2>=-14604) {
         if(ptrk<0.2||ptrk>1.1) break;
         index = int((ptrk-0.2)/0.1);
         if(index<=0) index=1;
         if(index>=8) index=7;
         if(fabs(costm)>=0.83) break;
         index1 = int((costm+0.9)/0.1);
         if(index1<=0) index1=1;
         if(index1>=17) index1=16;
         offsetp = cal_par(index,m_psipp_mc_proton_ptrk_offset,ptrk,0.25,0.1);
         sigp = cal_par(index,m_psipp_mc_proton_ptrk_sigma,ptrk,0.25,0.1);
         offsetc = cal_par(index1,m_psipp_mc_proton_theta_offset,costm,-0.85,0.1);
         sigcos = cal_par(index1,m_psipp_mc_proton_theta_sigma,costm,-0.85,0.1);
      }
      offset=offsetp+sigp*offsetc;
      chicor=(chicor-offset)/(sigcos*sigp);
      break;
   }

   default:
      offset = 0.0;
      break;
   }
   //  offset = 0.0;
   return chicor;
}

double DedxPID::sigmaDedx(int n, double ptrk, double cost) {

   /*  int rundedx3 = getRunNo();
     double sigma = 1.0;
    double sigp = 1.0;
    double sigmac = 1.0;
    double gb = ptrk/xmass(n);
    switch(n) {

    case 0: {// Electron
      double  ptemp = ptrk;
      double  costm = cost;
   break;
    }

    case 1: {// Muon
      double  ptemp = ptrk;
      double  costm = cost;
   break;
    }

    case 2: {// Pion
      double  ptemp = ptrk;
      double  costm = cost;
   break;
    }

    case 3: { // Kaon
      double  ptemp = ptrk;
      double  costm = cost;
   break;
    }


    case 4: {// Proton
      double  ptemp = ptrk;
      double  costm = cost;
   break;
    }

    default:
      sigma = 1.0;
      break;
    }
   */
   //  sigma = 1.2;
   //  sigma =1.0;
   return 1;
   //  return sigma;
}

double DedxPID::mypol3(double x, double par0, double par1, double par2, double par3)
{
   double y = x;
   return par0 + (par1 * y) +(par2 * y * y) + (par3 * y * y * y);

}

double DedxPID::mypol5(double x, double par0, double par1, double par2, double par3, double par4, double par5)
{
   double y = x;
   return par0 + (par1 * y) +(par2 * y * y) + (par3 * y * y * y) + (par4 * y * y * y *y)+ (par5 * y * y * y * y * y);

}

void DedxPID::inputpar() {

   //Jpsi ka+ momentum correction
   std::string jpsi_kap_mom = path + "/share/JPsi/kaon/dedx_kap.txt";
   std::string jpsi_kap_mom_mc = path + "/share/JPsi/kaon/dedx_kap_mc.txt";
   ifstream inputmomdata6(jpsi_kap_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata6 ) {
      cout << " can not open: " << jpsi_kap_mom << endl;
      exit(1);
   }
   ifstream inputmomdata6mc(jpsi_kap_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata6mc ) {
      cout << " can not open: " << jpsi_kap_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<12; i++) {
      inputmomdata6>>m_jpsi_kap_ptrk_offset[i];
      inputmomdata6>>m_jpsi_kap_ptrk_sigma[i];
      inputmomdata6mc>>m_jpsi_mc_kap_ptrk_offset[i];
      inputmomdata6mc>>m_jpsi_mc_kap_ptrk_sigma[i];
   }

   //Jpsi ka- momentum correction
   std::string jpsi_kam_mom = path + "/share/JPsi/kaon/dedx_kam.txt";
   std::string jpsi_kam_mom_mc =  path + "/share/JPsi/kaon/dedx_kam_mc.txt";
   ifstream inputmomdata7(jpsi_kam_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata7 ) {
      cout << " can not open: " << jpsi_kam_mom << endl;
      exit(1);
   }
   ifstream inputmomdata7mc(jpsi_kam_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata7mc ) {
      cout << " can not open: " << jpsi_kam_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<12; i++) {
      inputmomdata7>>m_jpsi_kam_ptrk_offset[i];
      inputmomdata7>>m_jpsi_kam_ptrk_sigma[i];
      inputmomdata7mc>>m_jpsi_mc_kam_ptrk_offset[i];
      inputmomdata7mc>>m_jpsi_mc_kam_ptrk_sigma[i];

   }


   //Jpsi ka+ theta correction
   std::string jpsi_kap_the =  path + "/share/JPsi/kaon/dedx_kap_theta.txt";
   std::string jpsi_kap_the_mc =  path + "/share/JPsi/kaon/dedx_kap_theta_mc.txt";
   ifstream inputmomdata8(jpsi_kap_the.c_str(),std::ios_base::in);
   if ( !inputmomdata8 ) {
      cout << " can not open: " << jpsi_kap_the << endl;
      exit(1);
   }
   ifstream inputmomdata8mc(jpsi_kap_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata8mc ) {
      cout << " can not open: " << jpsi_kap_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata8>>m_jpsi_kap_theta_offset[i];
      inputmomdata8>>m_jpsi_kap_theta_sigma[i];
      inputmomdata8mc>>m_jpsi_mc_kap_theta_offset[i];
      inputmomdata8mc>>m_jpsi_mc_kap_theta_sigma[i];
   }

   //Jpsi ka- theta correction
   std::string jpsi_kam_the =  path + "/share/JPsi/kaon/dedx_kam_theta.txt";
   std::string jpsi_kam_the_mc =  path + "/share/JPsi/kaon/dedx_kam_theta_mc.txt";
   ifstream inputmomdata9(jpsi_kam_the.c_str(),std::ios_base::in);
   if ( !inputmomdata9 ) {
      cout << " can not open: " << jpsi_kam_the << endl;
      exit(1);
   }
   ifstream inputmomdata9mc(jpsi_kam_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata9mc ) {
      cout << " can not open: " << jpsi_kam_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata9>>m_jpsi_kam_theta_offset[i];
      inputmomdata9>>m_jpsi_kam_theta_sigma[i];
      inputmomdata9mc>>m_jpsi_mc_kam_theta_offset[i];
      inputmomdata9mc>>m_jpsi_mc_kam_theta_sigma[i];
   }

   //Jpsi proton+ momentum correction
   std::string jpsi_protonp_mom =  path + "/share/JPsi/proton/dedx_protonp.txt";
   std::string jpsi_protonp_mom_mc =  path + "/share/JPsi/proton/dedx_protonp_mc.txt";
   ifstream inputmomdata12(jpsi_protonp_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata12 ) {
      cout << " can not open: " << jpsi_protonp_mom << endl;
      exit(1);
   }
   ifstream inputmomdata12mc(jpsi_protonp_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata12mc ) {
      cout << " can not open: " << jpsi_protonp_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<8; i++) {
      inputmomdata12>>m_jpsi_protonp_ptrk_offset[i];
      inputmomdata12>>m_jpsi_protonp_ptrk_sigma[i];
      inputmomdata12mc>>m_jpsi_mc_protonp_ptrk_offset[i];
      inputmomdata12mc>>m_jpsi_mc_protonp_ptrk_sigma[i];
   }

   //Jpsi proton- momentum correction
   std::string jpsi_protonm_mom =  path + "/share/JPsi/proton/dedx_protonm.txt";
   std::string jpsi_protonm_mom_mc =  path + "/share/JPsi/proton/dedx_protonm_mc.txt";
   ifstream inputmomdata13(jpsi_protonm_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata13 ) {
      cout << " can not open: " << jpsi_protonm_mom << endl;
      exit(1);
   }
   ifstream inputmomdata13mc(jpsi_protonm_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata13mc ) {
      cout << " can not open: " << jpsi_protonm_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<8; i++) {
      inputmomdata13>>m_jpsi_protonm_ptrk_offset[i];
      inputmomdata13>>m_jpsi_protonm_ptrk_sigma[i];
      inputmomdata13mc>>m_jpsi_mc_protonm_ptrk_offset[i];
      inputmomdata13mc>>m_jpsi_mc_protonm_ptrk_sigma[i];
   }

   //Jpsi proton+ theta correction
   std::string jpsi_protonp_the = path + "/share/JPsi/proton/dedx_protonp_theta.txt";
   std::string jpsi_protonp_the_mc = path + "/share/JPsi/proton/dedx_protonp_theta_mc.txt";

   ifstream inputmomdata14(jpsi_protonp_the.c_str(),std::ios_base::in);
   if ( !inputmomdata14 ) {
      cout << " can not open: " << jpsi_protonp_the << endl;
      exit(1);
   }
   ifstream inputmomdata14mc(jpsi_protonp_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata14mc ) {
      cout << " can not open: " << jpsi_protonp_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata14>>m_jpsi_protonp_theta_offset[i];
      inputmomdata14>>m_jpsi_protonp_theta_sigma[i];
      inputmomdata14mc>>m_jpsi_mc_protonp_theta_offset[i];
      inputmomdata14mc>>m_jpsi_mc_protonp_theta_sigma[i];
   }

   //Jpsi proton- theta correction
   std::string jpsi_protonm_the = path + "/share/JPsi/proton/dedx_protonm_theta.txt";
   std::string jpsi_protonm_the_mc = path + "/share/JPsi/proton/dedx_protonm_theta_mc.txt";
   ifstream inputmomdata15(jpsi_protonm_the.c_str(),std::ios_base::in);
   if ( !inputmomdata15 ) {
      cout << " can not open: " << jpsi_protonm_the << endl;
      exit(1);
   }
   ifstream inputmomdata15mc(jpsi_protonm_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata15mc ) {
      cout << " can not open: " << jpsi_protonm_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata15>>m_jpsi_protonm_theta_offset[i];
      inputmomdata15>>m_jpsi_protonm_theta_sigma[i];
      inputmomdata15mc>>m_jpsi_mc_protonm_theta_offset[i];
      inputmomdata15mc>>m_jpsi_mc_protonm_theta_sigma[i];
   }




   // Psip ka+ momentum correction
   std::string psip_kap_mom = path + "/share/Psip/kaon/dedx_kap.txt";
   std::string psip_kap_mom_mc = path + "/share/Psip/kaon/dedx_kap_mc.txt";
   ifstream inputmomdata24(psip_kap_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata24 ) {
      cout << " can not open: " << psip_kap_mom << endl;
      exit(1);
   }
   ifstream inputmomdata24mc(psip_kap_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata24mc ) {
      cout << " can not open: " << psip_kap_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<9; i++) {
      inputmomdata24>>m_psip_kap_ptrk_offset[i];
      inputmomdata24>>m_psip_kap_ptrk_sigma[i];
      inputmomdata24mc>>m_psip_mc_kap_ptrk_offset[i];
      inputmomdata24mc>>m_psip_mc_kap_ptrk_sigma[i];
   }

   //Psip ka- momentum correction
   std::string psip_kam_mom = path + "/share/Psip/kaon/dedx_kam.txt";
   std::string psip_kam_mom_mc = path + "/share/Psip/kaon/dedx_kam_mc.txt";
   ifstream inputmomdata25(psip_kam_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata25 ) {
      cout << " can not open: " << psip_kam_mom << endl;
      exit(1);
   }
   ifstream inputmomdata25mc(psip_kam_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata25mc ) {
      cout << " can not open: " << psip_kam_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<9; i++) {
      inputmomdata25>>m_psip_kam_ptrk_offset[i];
      inputmomdata25>>m_psip_kam_ptrk_sigma[i];
      inputmomdata25mc>>m_psip_mc_kam_ptrk_offset[i];
      inputmomdata25mc>>m_psip_mc_kam_ptrk_sigma[i];
   }


   // Psip proton+ momentum correction
   std::string psip_protonp_mom = path + "/share/Psip/proton/dedx_protonp.txt";
   std::string psip_protonp_mom_mc = path + "/share/Psip/proton/dedx_protonp_mc.txt";
   ifstream inputmomdata26(psip_protonp_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata26 ) {
      cout << " can not open: " << psip_protonp_mom << endl;
      exit(1);
   }
   ifstream inputmomdata26mc(psip_protonp_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata26mc ) {
      cout << " can not open: " << psip_protonp_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<9; i++) {
      inputmomdata26>>m_psip_protonp_ptrk_offset[i];
      inputmomdata26>>m_psip_protonp_ptrk_sigma[i];
      inputmomdata26mc>>m_psip_mc_protonp_ptrk_offset[i];
      inputmomdata26mc>>m_psip_mc_protonp_ptrk_sigma[i];
   }

   //Psip proton- momentum correction
   std::string psip_protonm_mom = path + "/share/Psip/proton/dedx_protonm.txt";
   std::string psip_protonm_mom_mc = path + "/share/Psip/proton/dedx_protonm_mc.txt";
   ifstream inputmomdata27(psip_protonm_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata27 ) {
      cout << " can not open: " << psip_protonm_mom << endl;
      exit(1);
   }
   ifstream inputmomdata27mc(psip_protonm_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata27mc ) {
      cout << " can not open: " << psip_protonm_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<9; i++) {
      inputmomdata27>>m_psip_protonm_ptrk_offset[i];
      inputmomdata27>>m_psip_protonm_ptrk_sigma[i];
      inputmomdata27mc>>m_psip_mc_protonm_ptrk_offset[i];
      inputmomdata27mc>>m_psip_mc_protonm_ptrk_sigma[i];
   }

   //Psipp pi momentum correction
   std::string psipp_pi_mom = path + "/share/Psipp/pion/dedx_pi.txt";
   std::string psipp_pi_mom_mc = path + "/share/Psipp/pion/dedx_pi_mc.txt";
   ifstream inputmomdata28(psipp_pi_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata28 ) {
      cout << " can not open: " << psipp_pi_mom << endl;
      exit(1);
   }
   ifstream inputmomdata28mc(psipp_pi_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata28mc ) {
      cout << " can not open: " << psipp_pi_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata28>>m_psipp_pi_ptrk_offset[i];
      inputmomdata28>>m_psipp_pi_ptrk_sigma[i];
      inputmomdata28mc>>m_psipp_mc_pi_ptrk_offset[i];
      inputmomdata28mc>>m_psipp_mc_pi_ptrk_sigma[i];
   }

   //Psipp pi theta correction
   std::string psipp_pi_the = path + "/share/Psipp/pion/dedx_pi_theta.txt";
   std::string psipp_pi_the_mc = path + "/share/Psipp/pion/dedx_pi_theta_mc.txt";
   ifstream inputmomdata29(psipp_pi_the.c_str(),std::ios_base::in);
   if ( !inputmomdata29 ) {
      cout << " can not open: " << psipp_pi_the << endl;
      exit(1);
   }
   ifstream inputmomdata29mc(psipp_pi_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata29mc ) {
      cout << " can not open: " << psipp_pi_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<16; i++) {
      inputmomdata29>>m_psipp_pi_theta_offset[i];
      inputmomdata29>>m_psipp_pi_theta_sigma[i];
      inputmomdata29mc>>m_psipp_mc_pi_theta_offset[i];
      inputmomdata29mc>>m_psipp_mc_pi_theta_sigma[i];
   }

   //Psipp ka momentum correction
   std::string psipp_ka_mom = path + "/share/Psipp/kaon/dedx_ka.txt";
   std::string psipp_ka_mom_mc = path + "/share/Psipp/kaon/dedx_ka_mc.txt";
   ifstream inputmomdata30(psipp_ka_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata30 ) {
      cout << " can not open: " << psipp_ka_mom << endl;
      exit(1);
   }
   ifstream inputmomdata30mc(psipp_ka_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata30mc ) {
      cout << " can not open: " << psipp_ka_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<17; i++) {
      inputmomdata30>>m_psipp_ka_ptrk_offset[i];
      inputmomdata30>>m_psipp_ka_ptrk_sigma[i];
      inputmomdata30mc>>m_psipp_mc_ka_ptrk_offset[i];
      inputmomdata30mc>>m_psipp_mc_ka_ptrk_sigma[i];
   }

   //Psipp ka theta correction
   std::string psipp_ka_the = path + "/share/Psipp/kaon/dedx_ka_theta.txt";
   std::string psipp_ka_the_mc = path + "/share/Psipp/kaon/dedx_ka_theta_mc.txt";
   ifstream inputmomdata31(psipp_ka_the.c_str(),std::ios_base::in);
   if ( !inputmomdata31 ) {
      cout << " can not open: " << psipp_ka_the << endl;
      exit(1);
   }
   ifstream inputmomdata31mc(psipp_ka_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata31mc ) {
      cout << " can not open: " << psipp_ka_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<16; i++) {
      inputmomdata31>>m_psipp_ka_theta_offset[i];
      inputmomdata31>>m_psipp_ka_theta_sigma[i];
      inputmomdata31mc>>m_psipp_mc_ka_theta_offset[i];
      inputmomdata31mc>>m_psipp_mc_ka_theta_sigma[i];
   }


   //Psipp proton momentum correction
   std::string psipp_proton_mom = path + "/share/Psipp/proton/dedx_proton.txt";
   std::string psipp_proton_mom_mc = path + "/share/Psipp/proton/dedx_proton_mc.txt";
   ifstream inputmomdata32(psipp_proton_mom.c_str(),std::ios_base::in);
   if ( !inputmomdata32 ) {
      cout << " can not open: " << psipp_proton_mom << endl;
      exit(1);
   }
   ifstream inputmomdata32mc(psipp_proton_mom_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata32mc ) {
      cout << " can not open: " << psipp_proton_mom_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata32>>m_psipp_proton_ptrk_offset[i];
      inputmomdata32>>m_psipp_proton_ptrk_sigma[i];
   }
   for(int i=0; i<9; i++) {
      inputmomdata32mc>>m_psipp_mc_proton_ptrk_offset[i];
      inputmomdata32mc>>m_psipp_mc_proton_ptrk_sigma[i];
   }

   //Psipp proton theta correction
   std::string psipp_proton_the = path + "/share/Psipp/proton/dedx_proton_theta.txt";
   std::string psipp_proton_the_mc = path + "/share/Psipp/proton/dedx_proton_theta_mc.txt";
   ifstream inputmomdata33(psipp_proton_the.c_str(),std::ios_base::in);
   if ( !inputmomdata33 ) {
      cout << " can not open: " << psipp_proton_the << endl;
      exit(1);
   }
   ifstream inputmomdata33mc(psipp_proton_the_mc.c_str(),std::ios_base::in);
   if ( !inputmomdata33mc ) {
      cout << " can not open: " << psipp_proton_the_mc << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      inputmomdata33>>m_psipp_proton_theta_offset[i];
      inputmomdata33>>m_psipp_proton_theta_sigma[i];
      inputmomdata33mc>>m_psipp_mc_proton_theta_offset[i];
      inputmomdata33mc>>m_psipp_mc_proton_theta_sigma[i];
   }

}

double DedxPID::iterate(double ptrk,double *mean,double *p) {
   double p1,p2,p3;
   p2=((mean[0]-mean[1])*(p[1]*p[1]-p[2]*p[2])-(mean[1]-mean[2])*(p[0]*p[0]-p[1]*p[1]))/((p[0]-p[1])*(p[1]*p[1]-p[2]*p[2])-(p[1]-p[2])*(p[0]*p[0]-p[1]*p[1]));
   p3=((p[0]-p[1])*(mean[1]-mean[2])-(p[1]-p[2])*(mean[0]-mean[1]))/((p[0]-p[1])*(p[1]*p[1]-p[2]*p[2])-(p[1]-p[2])*(p[0]*p[0]-p[1]*p[1]));
   p1=mean[0]-p2*p[0]-p3*p[0]*p[0];
   double mean1 = p1+p2*ptrk+p3*ptrk*ptrk;
   return mean1;
}

double DedxPID::cal_par(int index1,double *m_jpsi_pip_ptrk_offset,double ptrk,double begin,double bin) {
   double mean1[3],p[3];
   p[0]=begin+(index1-1)*bin;
   p[1]=begin+index1*bin;
   p[2]=begin+(index1+1)*bin;
   mean1[0]=m_jpsi_pip_ptrk_offset[index1-1];
   mean1[1]=m_jpsi_pip_ptrk_offset[index1];
   mean1[2]=m_jpsi_pip_ptrk_offset[index1+1];
   double res=iterate(ptrk,mean1,p);
   return res;
}

