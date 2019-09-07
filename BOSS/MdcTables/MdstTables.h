#ifndef _MdstTables_h_
#define _MdstTables_h_

#include <string>
#include <iostream>   
#include <vector>           

#include "MdcTables/HepevtTables.h"

using namespace std;

//
//*** MDST v.2.30. Run header
//
class Mdst_run_head {
public:
  Mdst_run_head() {};
public:
  int id;
  int version;
  int data_type;
  int exp_no;
  int run_no;
};

class MdstRunHeadCol {
public:
   static vector<Mdst_run_head>* getMdstRunHeadCol(void);
private:
   static vector<Mdst_run_head>* s_col;
};


//
//*** Run trailer
//
class Mdst_run_end {
public:
  Mdst_run_end() {};
public:  
  int id;
  int exp_no;
  int run_no;
};

class MdstRunEndCol {
public:
   static vector<Mdst_run_end>* getMdstRunEndCol(void);
private:
   static vector<Mdst_run_end>* s_col;
};


//
//*** Event header
//
class Mdst_event {
public:
  Mdst_event() {};
public:  
  int id;
  int exp_no;
  int run_no;
  int event_no;
  int trigger;
  int date;
  int time;
};

class MdstEventCol {
public:
   static vector<Mdst_event>* getMdstEventCol(void);
private:
   static vector<Mdst_event>* s_col;
};


//
//***Additional event header 
//
class Mdst_event_add {
public:
  Mdst_event_add() {};
public: 
  int id;
  float scale;
  int flag[5];
};

class MdstEventAddCol {
public:
   static vector<Mdst_event_add>* getMdstEventAddCol(void);
private:
   static vector<Mdst_event_add>* s_col;
};


//
//*** Data quality in the current event
//
class Mdst_quality {
public:
  Mdst_quality() {};
public:  
  int id;
  char name[4];
  int quality;
};

class MdstQualityCol {
public:
   static vector<Mdst_quality>* getMdstQualityCol(void);
private:
   static vector<Mdst_quality>* s_col;
};


//
//*** Reconstructed charged tracks.
//
class Mdst_trk;
class Mdst_acc;
class Mdst_tof;
class Mdst_muid;
class Mdst_charged {
public:
  Mdst_charged() {};
public:  
  int id;
  float charge;
  float px;
  float py;
  float pz;
  float mass;
  int used_pid;
  float pid_e;
  float pid_mu;
  float pid_pi;
  float pid_K;
  float pid_p;
  //  int elid;
  Mdst_trk * trk;
  Mdst_acc * acc;
  Mdst_tof *  tof;
  //  Mdst_klm *  klm;
  Mdst_muid * muid;
};

class MdstChargedCol {
public:
   static vector<Mdst_charged>* getMdstChargedCol(void);
private:
   static vector<Mdst_charged>* s_col;
};


//
//*** Reconstructed V0-particles, i.e. Ks, Lambda, 
//*** Lambda-bar, converted-gamma.
//
class Mdst_vee_daughters;
class Mdst_vee2 {
public:
  Mdst_vee2() {};
public:  
  int id;
  int kind;
  Mdst_charged * chgd[2];
  float p[4];
  float v[3];
  float z_dist;
  float chisq;
  int type;
  Mdst_vee_daughters * daut;
};

class MdstVee2Col {
public:
   static vector<Mdst_vee2>* getMdstVee2Col(void);
private:
   static vector<Mdst_vee2>* s_col;
};


//
//*** Track params. and error matrix of V0 daughters 
//*** before the vtx-constraint fit.
//
class Mdst_vee_daughters {
public:
  Mdst_vee_daughters() {};
public:  
  int id;
  float helix_p[5];
  float helix_m[5];
  float error_p[15];
  float error_m[15];
};

class MdstVeeDaughtersCol {
public:
   static vector<Mdst_vee_daughters>* getMdstVeeDaughtersCol(void);
private:
   static vector<Mdst_vee_daughters>* s_col;
};


//
//*** Reconstructed gamma's.
//
class Mdst_ecl;
class Mdst_efc;
class Mdst_gamma {
public:
  Mdst_gamma() {};
public:  
  int id;
  float px;
  float py;
  float pz;
  Mdst_ecl * ecl;
  Mdst_efc * efc;
};

class MdstGammaCol {
public:
   static vector<Mdst_gamma>* getMdstGammaCol(void);
private:
   static vector<Mdst_gamma>* s_col;
};


//
//*** Reconstructed pi0's.
//
class Mdst_pi0 {
public:
  Mdst_pi0() {};
public:  
  int id;
  Mdst_gamma * gamma[2];
  float px;
  float py;
  float pz;
  float energy;
  float mass;
  float chisq;
};

class MdstPi0Col {
public:
   static vector<Mdst_pi0>* getMdstPi0Col(void);
private:
   static vector<Mdst_pi0>* s_col;
};


//
//*** Reconstructed K-long's.  
//
class Mdst_ecl;
class Mdst_klm_cluster;
class Mdst_klong {
public:
  Mdst_klong() {};
public:  
  int id;
  float cos_x;
  float cos_y;
  float cos_z;
  Mdst_ecl * ecl;
  //  Mdst_klm * klm;
  Mdst_klm_cluster * klmc;
};

class MdstKlongCol {
public:
   static vector<Mdst_klong>* getMdstKlongCol(void);
private:
   static vector<Mdst_klong>* s_col;
};


//
//*** Muon ID information. 
//
class Mdst_klm_mu;
class Mdst_muid {
public:
  Mdst_muid() {};
public:  
  int id;
  int quality;
  int muon;
  Mdst_klm_mu * klm;
};

class MdstMuidCol {
public:
   static vector<Mdst_muid>* getMdstMuidCol(void);
private:
   static vector<Mdst_muid>* s_col;
};


//
//*** TRK(=CDC plus SVD) information.
//
class Mdst_trk_fit;
class Mdst_trk {
public:
  Mdst_trk() {};
public:  
  int id;
  Mdst_trk_fit * mhyp[5];
  int quality;
  int quality_dedx;
  float dEdx;
  float dEdx_exp[5];
  float sigma_dEdx[5];
  float pid_e;
  float pid_mu;
  float pid_pi;
  float pid_K;
  float pid_p;
};

class MdstTrkCol {
public:
   static vector<Mdst_trk>* getMdstTrkCol(void);
private:
   static vector<Mdst_trk>* s_col;
};


//
//*** 
//
class Mdst_trk_add {
public:
  Mdst_trk_add() {};
public:  
  int id;
  int stat[10];
  float buff[10];
};

class MdstTrkAddCol {
public:
   static vector<Mdst_trk_add>* getMdstTrkAddCol(void);
private:
   static vector<Mdst_trk_add>* s_col;
};


//
//*** Track fit results with multiple mass hypotheses. 
//
class Mdst_trk_fit {
public:
  Mdst_trk_fit() {};
public:  
  int id;
  int quality;
  int hit_cdc[2];
  int hit_svd;
  int nhits[5];
  float mass;
  float pivot_x;
  float pivot_y;
  float pivot_z;
  float helix[5];
  float error[15];
  float chisq;
  int ndf;
  float first_x;
  float first_y;
  float first_z;
  float last_x;
  float last_y;
  float last_z;
};

class MdstTrkFitCol {
public:
   static vector<Mdst_trk_fit>* getMdstTrkFitCol(void);
private:
   static vector<Mdst_trk_fit>* s_col;
};


//
//*** ACC (Aerogel Cherenkov Counter) information. 
//
class Mdst_acc {
public:
  Mdst_acc() {};
public: 
  int id;
  int quality;
  int fire;
  float photo_electron;
  float pid_e;
  float pid_mu;
  float pid_pi;
  float pid_K;
  float pid_p;
};

class MdstAccCol {
public:
   static vector<Mdst_acc>* getMdstAccCol(void);
private:
   static vector<Mdst_acc>* s_col;
};


//
//*** TOF (Time Of Flight counter) information. 
//
class Mdst_tof {
public:
  Mdst_tof() {};
public:  
  int id;
  int quality;
  float path_length;
  float tof;
  float tof_exp[5];
  float sigma_tof;
  float ndf[5];
  float pid_e;
  float pid_mu;
  float pid_pi;
  float pid_K;
  float pid_p;
  float cl_e;
  float cl_mu;
  float cl_pi;
  float cl_K;
  float cl_p;
};

class MdstTofCol {
public:
   static vector<Mdst_tof>* getMdstTofCol(void);
private:
   static vector<Mdst_tof>* s_col;
};


//
//*** ECL (Electromagnetic CaLorimeter) information. 
//
class Mdst_ecl {
public:
  Mdst_ecl() {};
public:  
  int id;
  int match;
  int quality;
  float energy;
  float phi;
  float theta;
  float r;
  float error[6];
  int electron;
};

class MdstEclCol {
public:
   static vector<Mdst_ecl>* getMdstEclCol(void);
private:
   static vector<Mdst_ecl>* s_col;
};


//
//*** ECL (Electromagnetic Calorimeter) information add on.
//
class Mdst_ecl_cr;
class Mdst_ecl_aux {
public:
  Mdst_ecl_aux() {};
public:  
  int id;
  float mass;
  float width;
  float e9oe25;
  float e9oe25unf;
  float seed;
  int cId;
  float property[10];
  int nhits;
  Mdst_ecl_cr * cr;
  Mdst_trk * trk;
};

class MdstEclAuxCol {
public:
   static vector<Mdst_ecl_aux>* getMdstEclAuxCol(void);
private:
   static vector<Mdst_ecl_aux>* s_col;
};


//
//*** ECL (Electromagnetic Calorimeter) information- Connected 
//*** Region(super-cluster): not calibrated yet. 
//
class Mdst_ecl_cr {
public:
  Mdst_ecl_cr() {};
public:  
  int id;
  int match;
  int quality;
  float energy;
  float phi;
  float theta;
  float r;
  float mass;
  float width;
  float property[10];
  int nhits;
  int nshowers;
};

class MdstEclCrCol {
public:
   static vector<Mdst_ecl_cr>* getMdstEclCrCol(void);
private:
   static vector<Mdst_ecl_cr>* s_col;
};


//
//*** TRK to ECL match information.
//
class Mdst_ecl_trk {
public:
  Mdst_ecl_trk() {};
public:  
  int id;
  Mdst_ecl * ecl;
  Mdst_trk * trk;
  int type;
  int cId;
  float x[3];
  float p[3];
  float error[21];
};

class MdstEclTrkCol {
public:
   static vector<Mdst_ecl_trk>* getMdstEclTrkCol(void);
private:
   static vector<Mdst_ecl_trk>* s_col;
};


//
//*** Cluster information of KLM.
//
class Mdst_klm_cluster {
public:
  Mdst_klm_cluster() {};
public:  
  int id;
  Mdst_trk * trk;
  Mdst_ecl * ecl;
  int quality;
  int layers;
  int first_layer;
  float phi;
  float theta;
  float error[3];
};

class MdstKlmClusterCol {
public:
   static vector<Mdst_klm_cluster>* getMdstKlmClusterCol(void);
private:
   static vector<Mdst_klm_cluster>* s_col;
};


//
//*** Extended muon ID information related to KLM. 
//
class Mdst_klm_mu {
public:
  Mdst_klm_mu() {};
public:  
  int id;
  float absp_len_pas;
  float absp_len_hit;
  int layer_pas;
  int layer_hit;
};

class MdstKlmMuCol {
public:
   static vector<Mdst_klm_mu>* getMdstKlmMuCol(void);
private:
   static vector<Mdst_klm_mu>* s_col;
};


//
//*** Muon ID Extended Information.
//
class Mdst_klm_mu_ex {
public:
  Mdst_klm_mu_ex() {};
public:  
  int id;
  Mdst_charged * pMDST_Charged;
  float Muon_likelihood;
  float Pion_likelihood;
  float Kaon_likelihood;
  float Miss_likelihood;
  float Junk_likelihood;
  int Outcome;
  float Chi_2;
  int N_hits;
  int N_strips;
  int Layer_trk_brl;
  int Layer_trk_end;
  int Layer_hit_brl;
  int Layer_hit_end;
  int N_layer_trk_brl;
  int N_layer_trk_end;
  int N_layer_hit_brl;
  int N_layer_hit_end;
  int Trk_pattern;
  int Hit_pattern;
};

class MdstKlmMuExCol {
public:
   static vector<Mdst_klm_mu_ex>* getMdstKlmMuExCol(void);
private:
   static vector<Mdst_klm_mu_ex>* s_col;
};


//
//*** EFC (Extreme Forward Calorimeter) information.
//
class Mdst_efc {
public:
  Mdst_efc() {};
public:  
  int id;
  int quality[2];
  float energy;
  float phi;
  float theta;
  float error[6];
};

class MdstEfcCol {
public:
   static vector<Mdst_efc>* getMdstEfcCol(void);
private:
   static vector<Mdst_efc>* s_col;
};


//
//*** Cross reference between track and hepevt
//
class Mdst_sim_trk {
public:
  Mdst_sim_trk() {};
public:  
  int id;
  Mdst_trk * trk;
  Gen_hepevt * hepevt;
  float relation[3];
};

class MdstSimTrkCol {
public:
   static vector<Mdst_sim_trk>* getMdstSimTrkCol(void);
private:
   static vector<Mdst_sim_trk>* s_col;
};


//
//*** Cross reference between shower and hepevt
//
class Mdst_sim_ecl {
public:
  Mdst_sim_ecl() {};
public:  
  int id;
  Mdst_ecl * ecl;
  Gen_hepevt * hepevt;
  float relation[3];
};

class MdstSimEclCol {
public:
   static vector<Mdst_sim_ecl>* getMdstSimEclCol(void);
private:
   static vector<Mdst_sim_ecl>* s_col;
};


//
//*** Random seeds 
//
class Mdst_sim_rand {
public:
  Mdst_sim_rand() {};
public:  
  int id;
  int seed1;
  int seed2;
  int iseq;
};

class MdstSimRandCol {
public:
   static vector<Mdst_sim_rand>* getMdstSimRandCol(void);
private:
   static vector<Mdst_sim_rand>* s_col;
};


#endif // _MdstTables_h_

