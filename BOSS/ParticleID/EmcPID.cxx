#include <fstream>
#include <cmath>
#include <cstdlib>

#include "TTree.h"
#include "TMultiLayerPerceptron.h"

#include "ParticleID/EmcPID.h"

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "ExtEvent/RecExtTrack.h"
#endif

void CALG(double Px, double& e2); // see "calg.cxx"

EmcPID * EmcPID::m_pointer = 0;
TMultiLayerPerceptron * EmcPID::m_mlp_emc=0;
TTree *  EmcPID::m_trainTree_emconly=0;


EmcPID * EmcPID::instance() {
   if(!m_pointer) m_pointer = new EmcPID();
   return m_pointer;
}

EmcPID::EmcPID():ParticleIDBase() {
   m_mlp_emc = 0;
   std::string e_emc_file = path + "/share/elec_emc_hist.txt";
   //std::string e_emc_file = "$PARTICLEIDROOT/share/elechist.txt";
   ifstream input(e_emc_file.c_str(),std::ios_base::in);
   if ( !input ) {
      cout << " can not open: " << e_emc_file << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      for(int j=0; j<300; j++) {
         input>>m_e_h[i][j];
      }
   }
   //   std::string pi_emc_file = "/ihepbatch/bes/huangb/boss610/Analysis/ParticleID/ParticleID-00-02-03/share/pionhist.txt";
   std::string pi_emc_file =  path + "/share/pion_emc_hist.txt";
   //std::string pi_emc_file = "$PARTICLEIDROOT/share/pionhist.txt";
   ifstream input1(pi_emc_file.c_str(),std::ios_base::in);
   if ( !input1 ) {
      cout << " can not open: " << pi_emc_file << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      for(int j=0; j<300; j++) {
         input1>>m_p_h[i][j];
      }
   }
   std::string mu_emc_file =  path + "/share/muon_emc_hist.txt";
   //  std::string mu_emc_file = "/ihepbatch/bes/huangb/boss610/Analysis/ParticleID/ParticleID-00-02-03/share/muonhist.txt";
   //std::string mu_emc_file = "$PARTICLEIDROOT/share/muonhist.txt";
   ifstream input2(mu_emc_file.c_str(),std::ios_base::in);
   if ( !input2 ) {
      cout << " can not open: " << mu_emc_file << endl;
      exit(1);
   }
   for(int i=0; i<18; i++) {
      for(int j=0; j<300; j++) {
         input2>>m_m_h[i][j];

      }
   }

   if(!m_trainTree_emconly) {
      m_trainTree_emconly = new TTree("m_trainTree_emconly","m_trainTree_emconly");
      m_trainTree_emconly->Branch("ptrk",&m_ptrk,"ptrk/D");
      m_trainTree_emconly->Branch("pt",&m_pt,"pt/D");
      m_trainTree_emconly->Branch("type",&m_type,"type/D");
      m_trainTree_emconly->Branch("energy",&m_energy,"energy/D");
      m_trainTree_emconly->Branch("eseed",&m_eseed,"eseed/D");
      m_trainTree_emconly->Branch("e3x3",&m_e3x3,"e3x3/D");
      m_trainTree_emconly->Branch("e5x5",&m_e5x5,"e5x5/D");
      m_trainTree_emconly->Branch("latmoment",&m_latmoment,"latmoment/D");
      m_trainTree_emconly->Branch("a20moment",&m_a20moment,"a20moment/D");
      m_trainTree_emconly->Branch("a42moment",&m_a42moment,"a42moment/D");
      m_trainTree_emconly->Branch("secondmoment",&m_secondmoment,"secondmoment/D");
      m_trainTree_emconly->Branch("delta_phi",&m_delta_phi,"delta_phi/D");
      m_trainTree_emconly->Branch("delta_theta",&m_delta_theta,"delta_theta/D");
   }
   std::string emc =  path + "/share/emc.txt";
   if(!m_mlp_emc) {
      //  m_mlp_emc = new TMultiLayerPerceptron("ptrk,pt,energy,eseed,e3x3,e5x5,secondmoment,delta_theta,delta_phi:18:type",m_trainTree_emconly);
      m_mlp_emc = new TMultiLayerPerceptron("ptrk,pt,energy,eseed,e3x3,e5x5,secondmoment,latmoment,a20moment,a42moment,delta_theta,delta_phi:24:type",m_trainTree_emconly);

      m_mlp_emc->LoadWeights(emc.c_str());

   }
}
void EmcPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
   }
   m_chimin = 99.;
   m_ndof = 0;
   m_energy = -99;
   m_eseed = -99;
   m_e3x3 = -99;
   m_e5x5 = -99;
   m_delta_theta = -99;
   m_delta_phi = -99;
   m_secondmoment = -99;
   m_val_emc = -99;
   //   std::string emc =  path + "/share/emc_epimu.txt";
   /*   if(!m_trainTree_emconly){
     m_trainTree_emconly = new TTree("m_trainTree_emconly","m_trainTree_emconly");
     m_trainTree_emconly->Branch("ptrk",&m_ptrk,"ptrk/D");
     m_trainTree_emconly->Branch("pt",&m_pt,"pt/D");
     m_trainTree_emconly->Branch("type",&m_type,"type/D");
     m_trainTree_emconly->Branch("energy",&m_energy,"energy/D");
     m_trainTree_emconly->Branch("eseed",&m_eseed,"eseed/D");
     m_trainTree_emconly->Branch("e3x3",&m_e3x3,"e3x3/D");
     m_trainTree_emconly->Branch("e5x5",&m_e5x5,"e5x5/D");
     m_trainTree_emconly->Branch("secondmoment",&m_secondmoment,"secondmoment/D");
     m_trainTree_emconly->Branch("delta_phi",&m_delta_phi,"delta_phi/D");
     m_trainTree_emconly->Branch("delta_theta",&m_delta_theta,"delta_theta/D");
      }
     if(!m_mlp_emc){
    m_mlp_emc = new TMultiLayerPerceptron("ptrk,pt,energy,eseed,e3x3,e5x5,secondmoment,delta_theta,delta_phi:18:type",m_trainTree_emconly);
     m_mlp_emc->LoadWeights(emc.c_str());
   }
   */

   /*if(!m_trainTree_emconly){
   m_trainTree_emconly = new TTree("m_trainTree_emconly","m_trainTree_emconly");
   m_trainTree_emconly->Branch("ptrk",&m_ptrk,"ptrk/D");
   m_trainTree_emconly->Branch("pt",&m_pt,"pt/D");
   m_trainTree_emconly->Branch("type",&m_type,"type/D");
   m_trainTree_emconly->Branch("energy",&m_energy,"energy/D");
   m_trainTree_emconly->Branch("eseed",&m_eseed,"eseed/D");
   m_trainTree_emconly->Branch("e3x3",&m_e3x3,"e3x3/D");
   m_trainTree_emconly->Branch("e5x5",&m_e5x5,"e5x5/D");
   m_trainTree_emconly->Branch("secondmoment",&m_secondmoment,"secondmoment/D");
   m_trainTree_emconly->Branch("delta_phi",&m_delta_phi,"delta_phi/D");
   m_trainTree_emconly->Branch("delta_theta",&m_delta_theta,"delta_theta/D");
   }
   if(!m_mlp_emc){
   m_mlp_emc = new TMultiLayerPerceptron("ptrk,pt,energy,eseed,e3x3,e5x5,secondmoment,delta_theta,delta_phi:18:type",m_trainTree_emconly);
   m_mlp_emc->LoadWeights(emc.c_str());
   }
   */



}

void EmcPID::calculate() {
   if(particleIDCalculation() == 0) m_ndof = 1;
}

int EmcPID::particleIDCalculation() {
   int irc = -1;
   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();
   if (!(recTrk)->isMdcKalTrackValid()) return irc;
   RecMdcKalTrack* mdcKalTrk = (recTrk)->mdcKalTrack();
   mdcKalTrk->setPidType(RecMdcKalTrack::electron);
   double ptrk=mdcKalTrk->p();
   // double ptrk = mdcTrk->p();
   m_ptrk = ptrk;

   m_pt=mdcKalTrk->pxy();
   m_pt = m_pt*mdcTrk->charge();
   //   double cost = cos(mdcTrk->theta());

   if(!(recTrk->isExtTrackValid())) return irc;
   RecExtTrack* extTrk = recTrk->extTrack();
   if(extTrk->emcVolumeNumber() == -1) return irc;
   if (!(recTrk->isEmcShowerValid())) return irc;
   RecEmcShower* emcTrk = recTrk->emcShower();

   m_energy = emcTrk->energy();
   m_eseed = emcTrk->eSeed();
   m_e3x3 = emcTrk->e3x3();
   m_e5x5 = emcTrk->e5x5();

   double m_emc_theta = emcTrk->theta();
   double m_emc_phi = emcTrk->phi();

   Hep3Vector mc = extTrk->emcPosition();
   double m_ext_theta = mc.theta();
   double m_ext_phi = mc.phi();


   m_delta_theta = m_emc_theta - m_ext_theta;
   m_delta_phi = m_emc_phi - m_ext_phi;
   if(m_delta_phi>1)   m_delta_phi=m_delta_phi-6.283;
   if(m_delta_phi<-1)  m_delta_phi=m_delta_phi+6.283;


   m_secondmoment = emcTrk->secondMoment()/1000.;
   m_a20moment=emcTrk->a20Moment();
   m_latmoment=emcTrk->latMoment();
   m_a42moment=emcTrk->a42Moment();

   if(emcTrk->energy() <= 0) return irc;
   //if(emcTrk->energy() > ptrk) return irc;

   /*  params_emc1[0] = m_ptrk;
     params_emc1[1] = m_pt;
     params_emc1[2] = m_energy;
     params_emc1[3] = m_eseed;
     params_emc1[4] = m_e3x3;
     params_emc1[5] = m_e5x5;
     params_emc1[6] = m_secondmoment;
     params_emc1[7] = m_delta_theta;
     params_emc1[8] = m_delta_phi;*/
   params_emc1[0] =m_ptrk;
   params_emc1[1] =m_pt;
   params_emc1[2] =m_energy;
   params_emc1[3] =m_eseed;
   params_emc1[4] =m_e3x3;
   params_emc1[5] =m_e5x5;
   params_emc1[6] =m_secondmoment;
   params_emc1[7] =m_latmoment;
   params_emc1[8] =m_a20moment;
   params_emc1[9] =m_a42moment;
   params_emc1[10] =m_delta_theta;
   params_emc1[11] =m_delta_phi;

   m_val_emc = m_mlp_emc->Evaluate(0,params_emc1);
   int pindex = int((m_ptrk-0.2)/0.1);
   int bindex = int((m_val_emc-0.5)/0.01);
   if(bindex>300||bindex<0) return irc;
   if(pindex>17) pindex=17;
   if(pindex<0) pindex=0;
   //   double bin_pos[3];
   m_prob[0] = m_e_h[pindex][bindex];
   m_prob[1] = m_m_h[pindex][bindex];
   m_prob[2] = m_p_h[pindex][bindex];
   m_prob[3] = m_p_h[pindex][bindex];
   m_prob[4] = m_p_h[pindex][bindex];
   for(int i =0; i<5; i++) {
      if(m_prob[i]==0) m_prob[i] = 0.001;
   }
   //calculate the chisq value using GAUSIN
   float ppp[5];
   for(int i=0; i<5; i++) {
      ppp[i]=0;
   }
   for(int j=0; j<=bindex; j++) {
      ppp[0]+= m_e_h[pindex][j];
      ppp[1]+= m_m_h[pindex][j];
      ppp[2]+= m_p_h[pindex][j];
   }
   for(int i=0; i<3; i++) {
      ppp[i]=ppp[i]*0.01;
      if(ppp[i]>0&&ppp[i]<1) {
         CALG(ppp[i],m_chi[i]);
      }
      if(ppp[i]<=0||ppp[i]>=1) m_chi[i]=-99;
   }
   // if(fabs(m_chi[2])==-99)
   m_chi[3]=m_chi[2];
   m_chi[4]=m_chi[2];

   m_ndof = 1;
   irc = 0;
   return irc;
}
