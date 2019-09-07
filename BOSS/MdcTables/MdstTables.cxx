#include "MdcTables/MdstTables.h"
 
//
//*** MDST v.2.30. Run header
//

vector<Mdst_run_head>* MdstRunHeadCol::s_col=0;
 
vector<Mdst_run_head>*
MdstRunHeadCol::getMdstRunHeadCol(void) {
    if (! s_col) s_col = new vector<Mdst_run_head> ;
    return s_col;
}


//
//*** Run trailer
//

vector<Mdst_run_end>* MdstRunEndCol::s_col=0;
 
vector<Mdst_run_end>*
MdstRunEndCol::getMdstRunEndCol(void) {
    if (! s_col) s_col = new vector<Mdst_run_end> ;
    return s_col;
}


//
//*** Event header
//

vector<Mdst_event>* MdstEventCol::s_col=0;
 
vector<Mdst_event>*
MdstEventCol::getMdstEventCol(void) {
    if (! s_col) s_col = new vector<Mdst_event> ;
    return s_col;
}


//
//***Additional event header 
//

vector<Mdst_event_add>* MdstEventAddCol::s_col=0;
 
vector<Mdst_event_add>*
MdstEventAddCol::getMdstEventAddCol(void) {
    if (! s_col) s_col = new vector<Mdst_event_add> ;
    return s_col;
}


//
//*** Data quality in the current event
//

vector<Mdst_quality>* MdstQualityCol::s_col=0;
 
vector<Mdst_quality>*
MdstQualityCol::getMdstQualityCol(void) {
    if (! s_col) s_col = new vector<Mdst_quality> ;
    return s_col;
}


//
//*** Reconstructed charged tracks.
//

vector<Mdst_charged>* MdstChargedCol::s_col=0;
 
vector<Mdst_charged>*
MdstChargedCol::getMdstChargedCol(void) {
    if (! s_col) s_col = new vector<Mdst_charged> ;
    return s_col;
}


//
//*** Reconstructed V0-particles, i.e. Ks, Lambda, 
//*** Lambda-bar, converted-gamma.
//

vector<Mdst_vee2>* MdstVee2Col::s_col=0;
 
vector<Mdst_vee2>*
MdstVee2Col::getMdstVee2Col(void) {
    if (! s_col) s_col = new vector<Mdst_vee2> ;
    return s_col;
}


//
//*** Track params. and error matrix of V0 daughters 
//*** before the vtx-constraint fit.
//

vector<Mdst_vee_daughters>* MdstVeeDaughtersCol::s_col=0;
 
vector<Mdst_vee_daughters>*
MdstVeeDaughtersCol::getMdstVeeDaughtersCol(void) {
    if (! s_col) s_col = new vector<Mdst_vee_daughters> ;
    return s_col;
}


//
//*** Reconstructed gamma's.
//

vector<Mdst_gamma>* MdstGammaCol::s_col=0;
 
vector<Mdst_gamma>*
MdstGammaCol::getMdstGammaCol(void) {
    if (! s_col) s_col = new vector<Mdst_gamma> ;
    return s_col;
}


//
//*** Reconstructed pi0's.
//

vector<Mdst_pi0>* MdstPi0Col::s_col=0;
 
vector<Mdst_pi0>*
MdstPi0Col::getMdstPi0Col(void) {
    if (! s_col) s_col = new vector<Mdst_pi0> ;
    return s_col;
}


//
//*** Reconstructed K-long's.  
//

vector<Mdst_klong>* MdstKlongCol::s_col=0;
 
vector<Mdst_klong>*
MdstKlongCol::getMdstKlongCol(void) {
    if (! s_col) s_col = new vector<Mdst_klong> ;
    return s_col;
}


//
//*** Muon ID information. 
//

vector<Mdst_muid>* MdstMuidCol::s_col=0;
 
vector<Mdst_muid>*
MdstMuidCol::getMdstMuidCol(void) {
    if (! s_col) s_col = new vector<Mdst_muid> ;
    return s_col;
}


//
//*** TRK(=CDC plus SVD) information.
//

vector<Mdst_trk>* MdstTrkCol::s_col=0;
 
vector<Mdst_trk>*
MdstTrkCol::getMdstTrkCol(void) {
    if (! s_col) s_col = new vector<Mdst_trk> ;
    return s_col;
}


//
//*** 
//

vector<Mdst_trk_add>* MdstTrkAddCol::s_col=0;
 
vector<Mdst_trk_add>*
MdstTrkAddCol::getMdstTrkAddCol(void) {
    if (! s_col) s_col = new vector<Mdst_trk_add> ;
    return s_col;
}


//
//*** Track fit results with multiple mass hypotheses. 
//

vector<Mdst_trk_fit>* MdstTrkFitCol::s_col=0;
 
vector<Mdst_trk_fit>*
MdstTrkFitCol::getMdstTrkFitCol(void) {
    if (! s_col) s_col = new vector<Mdst_trk_fit> ;
    return s_col;
}


//
//*** ACC (Aerogel Cherenkov Counter) information. 
//

vector<Mdst_acc>* MdstAccCol::s_col=0;
 
vector<Mdst_acc>*
MdstAccCol::getMdstAccCol(void) {
    if (! s_col) s_col = new vector<Mdst_acc> ;
    return s_col;
}


//
//*** TOF (Time Of Flight counter) information. 
//

vector<Mdst_tof>* MdstTofCol::s_col=0;
 
vector<Mdst_tof>*
MdstTofCol::getMdstTofCol(void) {
    if (! s_col) s_col = new vector<Mdst_tof> ;
    return s_col;
}


//
//*** ECL (Electromagnetic CaLorimeter) information. 
//

vector<Mdst_ecl>* MdstEclCol::s_col=0;
 
vector<Mdst_ecl>*
MdstEclCol::getMdstEclCol(void) {
    if (! s_col) s_col = new vector<Mdst_ecl> ;
    return s_col;
}


//
//*** ECL (Electromagnetic Calorimeter) information add on.
//

vector<Mdst_ecl_aux>* MdstEclAuxCol::s_col=0;
 
vector<Mdst_ecl_aux>*
MdstEclAuxCol::getMdstEclAuxCol(void) {
    if (! s_col) s_col = new vector<Mdst_ecl_aux> ;
    return s_col;
}


//
//*** ECL (Electromagnetic Calorimeter) information- Connected 
//*** Region(super-cluster): not calibrated yet. 
//

vector<Mdst_ecl_cr>* MdstEclCrCol::s_col=0;
 
vector<Mdst_ecl_cr>*
MdstEclCrCol::getMdstEclCrCol(void) {
    if (! s_col) s_col = new vector<Mdst_ecl_cr> ;
    return s_col;
}


//
//*** TRK to ECL match information.
//

vector<Mdst_ecl_trk>* MdstEclTrkCol::s_col=0;
 
vector<Mdst_ecl_trk>*
MdstEclTrkCol::getMdstEclTrkCol(void) {
    if (! s_col) s_col = new vector<Mdst_ecl_trk> ;
    return s_col;
}


//
//*** Cluster information of KLM.
//

vector<Mdst_klm_cluster>* MdstKlmClusterCol::s_col=0;
 
vector<Mdst_klm_cluster>*
MdstKlmClusterCol::getMdstKlmClusterCol(void) {
    if (! s_col) s_col = new vector<Mdst_klm_cluster> ;
    return s_col;
}


//
//*** Extended muon ID information related to KLM. 
//

vector<Mdst_klm_mu>* MdstKlmMuCol::s_col=0;
 
vector<Mdst_klm_mu>*
MdstKlmMuCol::getMdstKlmMuCol(void) {
    if (! s_col) s_col = new vector<Mdst_klm_mu> ;
    return s_col;
}


//
//*** Muon ID Extended Information.
//

vector<Mdst_klm_mu_ex>* MdstKlmMuExCol::s_col=0;
 
vector<Mdst_klm_mu_ex>*
MdstKlmMuExCol::getMdstKlmMuExCol(void) {
    if (! s_col) s_col = new vector<Mdst_klm_mu_ex> ;
    return s_col;
}


//
//*** EFC (Extreme Forward Calorimeter) information.
//

vector<Mdst_efc>* MdstEfcCol::s_col=0;
 
vector<Mdst_efc>*
MdstEfcCol::getMdstEfcCol(void) {
    if (! s_col) s_col = new vector<Mdst_efc> ;
    return s_col;
}

//
//*** Cross reference between track and hepevt
//

vector<Mdst_sim_trk>* MdstSimTrkCol::s_col=0;
 
vector<Mdst_sim_trk>*
MdstSimTrkCol::getMdstSimTrkCol(void) {
    if (! s_col) s_col = new vector<Mdst_sim_trk> ;
    return s_col;
}


//
//*** Cross reference between shower and hepevt
//

vector<Mdst_sim_ecl>* MdstSimEclCol::s_col=0;
 
vector<Mdst_sim_ecl>*
MdstSimEclCol::getMdstSimEclCol(void) {
    if (! s_col) s_col = new vector<Mdst_sim_ecl> ;
    return s_col;
}


//
//*** Random seeds 
//

vector<Mdst_sim_rand>* MdstSimRandCol::s_col=0;
 
vector<Mdst_sim_rand>*
MdstSimRandCol::getMdstSimRandCol(void) {
    if (! s_col) s_col = new vector<Mdst_sim_rand> ;
    return s_col;
}

