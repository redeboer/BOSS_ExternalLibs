#include "MdcTables/TrkTables.h"

//
//*** Info. on tracks reconstructed in KalFitter. For each track 5 slots 
//*** are allocated to handle different mass  hypotheses.
//

vector<MdcTrk>* MdcTrkCol::s_col=0;
 
vector<MdcTrk>*
MdcTrkCol::getMdcTrkCol(void) {
    if (! s_col) s_col = new vector<MdcTrk> ;
    return s_col;
}


//
//*** Track fit results indep. of position. ***
//

vector<MdcTrk_global>* MdcTrkGlobalCol::s_col=0;
 
vector<MdcTrk_global>*
MdcTrkGlobalCol::getMdcTrkGlobalCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_global> ;
    return s_col;
}


//
//*** Track fit results at near the coord. origin. ***
//

vector<MdcTrk_localz>* MdcTrkLocalzCol::s_col=0;
 
vector<MdcTrk_localz>*
MdcTrkLocalzCol::getMdcTrkLocalzCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_localz> ;
    return s_col;
}


//
//*** Track fit results at the first hit point. ***
//

vector<MdcTrk_localf>* MdcTrkLocalfCol::s_col=0;
 
vector<MdcTrk_localf>*
MdcTrkLocalfCol::getMdcTrkLocalfCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_localf> ;
    return s_col;
}


//
//*** Track fit results at the last hit point. ***
//

vector<MdcTrk_locall>* MdcTrkLocallCol::s_col=0;
 
vector<MdcTrk_locall>*
MdcTrkLocallCol::getMdcTrkLocallCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_locall> ;
    return s_col;
}


//
//*** V0 candidates. ***
//

vector<MdcTrk_v0>* MdcTrkV0Col::s_col=0;
 
vector<MdcTrk_v0>*
MdcTrkV0Col::getMdcTrkV0Col(void) {
    if (! s_col) s_col = new vector<MdcTrk_v0> ;
    return s_col;
}


//
//*** V0 candidates; V0 info. after vtx-constrained fit***
//

vector<MdcTrk_v02>* MdcTrkV02Col::s_col=0;
 
vector<MdcTrk_v02>*
MdcTrkV02Col::getMdcTrkV02Col(void) {
    if (! s_col) s_col = new vector<MdcTrk_v02> ;
    return s_col;
}


//
//*** Track params. and error matrix of V0 daughters before the    ***
//*** vtx-constraint fit. They are defined at the V0 vtx-position. ***
//

vector<MdcTrk_v0_daughters>* MdcTrkV0DaughtersCol::s_col=0;
 
vector<MdcTrk_v0_daughters>*
MdcTrkV0DaughtersCol::getMdcTrkV0DaughtersCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_v0_daughters> ;
    return s_col;
}


//
//*** Path length in MDC for dEdx pid. ***
//

vector<MdcTrk_pathl>* MdcTrkPathlCol::s_col=0;
 
vector<MdcTrk_pathl>*
MdcTrkPathlCol::getMdcTrkPathlCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_pathl> ;
    return s_col;
}


//
//*** Info. on tracks found either in MDC+SVD combined system or in SVD only. ***
//

vector<MdcTrk_svd>* MdcTrkSvdCol::s_col=0;
 
vector<MdcTrk_svd>*
MdcTrkSvdCol::getMdcTrkSvdCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_svd> ;
    return s_col;
}


//
//*** Tof results for diff mass assumptions ***
//

vector<MdcTrk_tof>* MdcTrkTofCol::s_col=0;
 
vector<MdcTrk_tof>*
MdcTrkTofCol::getMdcTrkTofCol(void) {
    if (! s_col) s_col = new vector<MdcTrk_tof> ;
    return s_col;
}

