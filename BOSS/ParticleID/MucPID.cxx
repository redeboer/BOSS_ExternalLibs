#include <fstream>
#include <cmath>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TMultiLayerPerceptron.h"
#include "TMLPAnalyzer.h"

#include "ParticleID/MucPID.h"

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MucRecEvent/RecMucTrack.h"
#endif

void CALG(double Px, double& e2); // see "calg.cxx"

MucPID * MucPID::m_pointer = 0;

MucPID * MucPID::instance() {

   if(!m_pointer) m_pointer = new MucPID();
   return m_pointer;
}

MucPID::MucPID():ParticleIDBase() {
   m_trainFile_muc = 0;
   m_trainTree_muc = 0;
   m_mlp_muc = 0;
   m_mlpa_muc = 0;
   //std::string pi_muc_file = "$PARTICLEIDROOT/share/pion_muc_hist.txt";
   //  std::string pi_muc_file = "/ihepbatch/bes/huangb/boss610/Analysis/ParticleID/ParticleID-00-02-03/share/pion_muc_hist.txt";
   std::string pi_muc_file = path + "/share/pion_muc_hist.txt";

   ifstream input1(pi_muc_file.c_str(),std::ios_base::in);
   if ( !input1 ) {
      cout << " can not open: " << pi_muc_file << endl;
      exit(1);
   }
   for(int i=0; i<13; i++) {
      for(int j=0; j<300; j++) {
         input1>>m_p_h[i][j];
      }
   }

   std::string mu_muc_file = path + "/share/muon_muc_hist.txt";
   //std::string mu_muc_file = "$PARTICLEIDROOT/share/muon_muc_hist.txt";
   //  std::string mu_muc_file = "/ihepbatch/bes/huangb/boss610/Analysis/ParticleID/ParticleID-00-02-03/share/muon_muc_hist.txt";
   ifstream input2(mu_muc_file.c_str(),std::ios_base::in);
   if ( !input2 ) {
      cout << " can not open: " << mu_muc_file << endl;
      exit(1);
   }
   for(int i=0; i<13; i++) {
      for(int j=0; j<300; j++) {
         input2>>m_m_h[i][j];
      }
   }




}

void MucPID::init() {
   for(int i = 0; i < 5; i++) {
      m_chi[i] = 99.0;
      m_prob[i] = -1.0;
   }
   m_chimin = 99.;
   m_ndof = 0;
   m_hits = -99;
   m_depth = -999;
   m_val_muc=-99;

   std::string neural = path + "/share/neural.root";
   std::string muc = path + "/share/muc.txt";
   double ptrk,phi,theta,depth,hits,chi2,distance,muc_delta_phi;
   int type;
   if(!m_trainTree_muc) {
      m_trainTree_muc = new TTree("m_trainTree_muc","m_trainTree_muc");
      m_trainTree_muc->Branch("ptrk",&ptrk,"ptrk/D");
      m_trainTree_muc->Branch("phi",&phi,"phi/D");
      m_trainTree_muc->Branch("theta",&theta,"theta/D");
      m_trainTree_muc->Branch("depth",&depth,"depth/D");
      m_trainTree_muc->Branch("hits",&hits,"hits/D");
      m_trainTree_muc->Branch("chi2",&chi2,"chi2/D");
      m_trainTree_muc->Branch("distance",&distance,"distance/D");
      m_trainTree_muc->Branch("muc_delta_phi",&muc_delta_phi,"muc_delta_phi/D");
      m_trainTree_muc->Branch("type",&type,"type/I");
   }
   if(!m_mlp_muc) {
      m_mlp_muc = new TMultiLayerPerceptron("@ptrk,@phi,@theta,@depth,@hits,@chi2,@distance,@muc_delta_phi:16:type",m_trainTree_muc);
      m_mlp_muc->LoadWeights(muc.c_str());
   }
}


void MucPID::calculate() {
   if(particleIDCalculation() == 0) m_ndof = 1;
}

int MucPID::particleIDCalculation() {
   int irc = -1;
   EvtRecTrack* recTrk = PidTrk();
   if(!(recTrk->isMdcTrackValid())) return irc;
   RecMdcTrack* mdcTrk = recTrk->mdcTrack();

   m_depth = -99;
   m_hits =  -99;
   m_chi2 =-99;
   m_distance =-99;
   m_muc_delta_phi =-99;

   double ptrk = mdcTrk->p();
   double m_ptrk = ptrk;
   double m_pt = mdcTrk->pxy();
   double phi = mdcTrk->phi();
   double theta = mdcTrk->theta();
   double cost = cos(mdcTrk->theta());
   if(ptrk<0.5) return irc;
   if(fabs(cost)>0.83) return irc;
   if(!(recTrk->isExtTrackValid())) return irc;
   RecExtTrack* extTrk = recTrk->extTrack();
   if(extTrk->mucVolumeNumber() == -1) return irc;
   if (!(recTrk->isMucTrackValid())) return irc;
   RecMucTrack* mucTrk = recTrk->mucTrack();

   //  if(mucTrk->maxHitsInLayer()< 0) return irc;
   if(mucTrk->depth()>100000) return irc;

   m_hits = mucTrk->maxHitsInLayer();
   m_depth = mucTrk->depth();
   m_distance = mucTrk->distance();
   m_chi2 = mucTrk->chi2();
   /* Hep3Vector phi_muc;
    phi_muc.set(mucTrk->xPos(),mucTrk->yPos(),0);
    Hep3Vector phi_mdc;
    phi_mdc.set(mdcTrk->px(),mdcTrk->py(),0);
    m_muc_delta_phi = phi_muc.angle(phi_mdc);
    if(m_muc_delta_phi<0) m_muc_delta_phi = -m_muc_delta_phi;        */
   m_muc_delta_phi= mucTrk->deltaPhi();
   m_muc_delta_phi=3.14159-m_muc_delta_phi;
   theta = cos(theta);
   params_muc1[0] = m_ptrk;
   params_muc1[1] = phi;
   params_muc1[2] = theta;
   params_muc1[3] = m_depth;
   params_muc1[4] = m_hits;
   params_muc1[5] = m_chi2;
   params_muc1[6] = m_distance;
   params_muc1[7] = m_muc_delta_phi;

   m_val_muc = m_mlp_muc->Evaluate(0,params_muc1);
   if(m_pt<0) m_pt = -m_pt;
   int pindex = int((m_ptrk-0.5)/0.1);
   int bindex = int((m_val_muc-0.5)/0.01);
   if(bindex>300||bindex<0) return irc;
   if(pindex>11) pindex=11;
   if(pindex<0) pindex=0;
   m_prob[0] = m_p_h[pindex][bindex];;
   m_prob[1] = m_m_h[pindex][bindex];
   m_prob[2] = m_p_h[pindex][bindex];
   m_prob[3] = m_p_h[pindex][bindex];
   m_prob[4] = m_p_h[pindex][bindex];
   for(int i =0; i<5; i++) {
      if(m_prob[i]==0) m_prob[i] = 0.001;
   }
   float ppp[5];
   for(int i =0; i<5; i++) {
      ppp[i] = 0.0;
   }

   for(int j=0; j<bindex; j++) {
      ppp[1]+= m_m_h[pindex][j]*0.01;
      ppp[2]+= m_p_h[pindex][j]*0.01;
   }
   if(ppp[1]>0&&ppp[1]<1)
      CALG(ppp[1],m_chi[1]);
   if(ppp[2]>0&&ppp[2]<1)
      CALG(ppp[2],m_chi[2]);
   if(ppp[1]<=0||ppp[1]>=1)
      m_chi[1]=-99;
   if(ppp[2]<=0||ppp[2]>=1)
      m_chi[2]=-99;

   m_chi[3]=m_chi[2];
   m_chi[4]=m_chi[2];
   m_chi[0] =m_chi[2];
   m_ndof = 1;
   irc = 0;
   return irc;
}
