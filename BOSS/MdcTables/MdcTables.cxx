#include "MdcTables/MdcTables.h"  

//
//*** raw data info. of MDC wire hits ***  
//

vector<MdcRaw_wirhit>* MdcRawWirhitCol::s_col=0;

vector<MdcRaw_wirhit>*
MdcRawWirhitCol::getMdcRawWirhitCol(void) {
    if (! s_col) s_col = new vector<MdcRaw_wirhit> ;
    return s_col;
}


//
//*** digitized info. of MDC wire hits ***
//
    
vector<MdcDat_wirhit>* MdcDatWirhitCol::s_col=0;

vector<MdcDat_wirhit>*
MdcDatWirhitCol::getMdcDatWirhitCol(void) {
    if (! s_col) s_col = new vector<MdcDat_wirhit> ;
    return s_col;
}
    

//
//*** digitized info. of MDC wire hits ***
//

vector<MdcDat_olhit>* MdcDatOlhitCol::s_col=0;

vector<MdcDat_olhit>*
MdcDatOlhitCol::getMdcDatOlhitCol(void) {
    if (! s_col) s_col = new vector<MdcDat_olhit> ;
    return s_col;
}

    
//
//*** const. param. to calculate drift distance ***
//

vector<MdcCal_tzero>* MdcCalTzeroCol::s_col=0;

vector<MdcCal_tzero>*
MdcCalTzeroCol::getMdcCalTzeroCol(void) {
    if (! s_col) s_col = new vector<MdcCal_tzero> ;
    return s_col;
}

    
//
//*** run by run t0 *** 
//

vector<MdcCal_tzero2>* MdcCalTzero2Col::s_col=0;

vector<MdcCal_tzero2>*
MdcCalTzero2Col::getMdcCalTzero2Col(void) {
    if (! s_col) s_col = new vector<MdcCal_tzero2> ;
    return s_col;
}

    
//
//*** Defifinition of bad wires  *** 
//

vector<MdcCal_badwire>* MdcCalBadwireCol::s_col=0;

vector<MdcCal_badwire>*
MdcCalBadwireCol::getMdcCalBadwireCol(void) {
    if (! s_col) s_col = new vector<MdcCal_badwire> ;
    return s_col;
}

    
//
//**** nition of tables for const. param. to calculate drift distance ***
//

vector<MdcCal_const>* MdcCalConstCol::s_col=0;

vector<MdcCal_const>*
MdcCalConstCol::getMdcCalConstCol(void) {
    if (! s_col) s_col = new vector<MdcCal_const> ;
    return s_col;
}

    
//
//*** const. param. for error of drift distance ***
//

vector<MdcCal_const2>* MdcCalConst2Col::s_col=0;

vector<MdcCal_const2>*
MdcCalConst2Col::getMdcCalConst2Col(void) {
    if (! s_col) s_col = new vector<MdcCal_const2> ;
    return s_col;
}

    
//
//*** const. param. for time walk *** 
//

vector<MdcCal_const3>* MdcCalConst3Col::s_col=0;

vector<MdcCal_const3>*
MdcCalConst3Col::getMdcCalConst3Col(void) {
    if (! s_col) s_col = new vector<MdcCal_const3> ;
    return s_col;
}

    
//
//*** fudge factor and tanL dep. ***
//

vector<MdcCal_const4>* MdcCalConst4Col::s_col=0;

vector<MdcCal_const4>*
MdcCalConst4Col::getMdcCalConst4Col(void) {
    if (! s_col) s_col = new vector<MdcCal_const4> ;
    return s_col;
}

    
//
//*** miscellaneous *** 
//

vector<MdcCal_misc>* MdcCalMiscCol::s_col=0;

vector<MdcCal_misc>*
MdcCalMiscCol::getMdcCalMiscCol(void) {
    if (! s_col) s_col = new vector<MdcCal_misc> ;
    return s_col;
}

    
//
//*** const. param. to calculate charge  *** 
//

vector<MdcCal_ped>* MdcCalPedCol::s_col=0;

vector<MdcCal_ped>*
MdcCalPedCol::getMdcCalPedCol(void) {
    if (! s_col) s_col = new vector<MdcCal_ped> ;
    return s_col;
}

    
//
//*** wire-hits of MDC *** 
//

MdcRec_wirhit::~MdcRec_wirhit( ) {
}

vector<MdcRec_wirhit>* MdcRecWirhitCol::s_col=0;

vector<MdcRec_wirhit>*
MdcRecWirhitCol::getMdcRecWirhitCol(void) {
    if (! s_col) s_col = new vector<MdcRec_wirhit> ;
    return s_col;
}

    
//
//*** track of MDC *** 
//
   
MdcRec_trk::~MdcRec_trk( ) {
}

vector<MdcRec_trk>* MdcRecTrkCol::s_col=0;

vector<MdcRec_trk>*
MdcRecTrkCol::getMdcRecTrkCol(void) {
    if (! s_col) s_col = new vector<MdcRec_trk> ;
    return s_col;
}

    
//
//*** dEdx of MDC ***  
//

vector<MdcRec_dedx>* MdcRecDedxCol::s_col=0;

vector<MdcRec_dedx>*
MdcRecDedxCol::getMdcRecDedxCol(void) {
    if (! s_col) s_col = new vector<MdcRec_dedx> ;
    return s_col;
}

    
//
//*** MDC timing *** 
//

vector<MdcRec_timing>* MdcRecTimingCol::s_col=0;

vector<MdcRec_timing>*
MdcRecTimingCol::getMdcRecTimingCol(void) {
    if (! s_col) s_col = new vector<MdcRec_timing> ;
    return s_col;
}

    
//
//*** const. param. for dE/dx caliburation  *** 
//

vector<MdcRec_excalib>* MdcRecExcalibCol::s_col=0;

vector<MdcRec_excalib>*
MdcRecExcalibCol::getMdcRecExcalibCol(void) {
    if (! s_col) s_col = new vector<MdcRec_excalib> ;
    return s_col;
}

    
//
//*** const. param. to caliburate global gain ***   
//

vector<MdcRec_exglobal>* MdcRecExglobalCol::s_col=0;

vector<MdcRec_exglobal>*
MdcRecExglobalCol::getMdcRecExglobalCol(void) {
    if (! s_col) s_col = new vector<MdcRec_exglobal> ;
    return s_col;
}

    
//
//*** const. param. to caliburate dE/dx curve *** 
//

vector<MdcRec_excurve>* MdcRecExcurveCol::s_col=0;

vector<MdcRec_excurve>*
MdcRecExcurveCol::getMdcRecExcurveCol(void) {
    if (! s_col) s_col = new vector<MdcRec_excurve> ;
    return s_col;
}

    
//
//*** const. to calib. dE/dx curve for 5 partciles ***
//

vector<MdcRec_excurve_add>* MdcRecExcurveAddCol::s_col=0;

vector<MdcRec_excurve_add>*
MdcRecExcurveAddCol::getMdcRecExcurveAddCol(void) {
    if (! s_col) s_col = new vector<MdcRec_excurve_add> ;
    return s_col;
}

    
//
//*** re-reconst. dE/dx ( global calib )" 
//

vector<MdcRec_exnoggs>* MdcRecExnoggsCol::s_col=0;

vector<MdcRec_exnoggs>*
MdcRecExnoggsCol::getMdcRecExnoggsCol(void) {
    if (! s_col) s_col = new vector<MdcRec_exnoggs> ;
    return s_col;
}

    
//
//*** Definition of current calib. function parameter (2nd POLY.)" 
//

vector<MdcRec_excurrentfit>* MdcRecExcurrentfitCol::s_col=0;

vector<MdcRec_excurrentfit>*
MdcRecExcurrentfitCol::getMdcRecExcurrentfitCol(void) {
    if (! s_col) s_col = new vector<MdcRec_excurrentfit> ;
    return s_col;
}

    
//
//*** wire current from daqom" 
//

vector<MdcRec_daqom>* MdcRecDaqomCol::s_col=0;

vector<MdcRec_daqom>*
MdcRecDaqomCol::getMdcRecDaqomCol(void) {
    if (! s_col) s_col = new vector<MdcRec_daqom> ;
    return s_col;
}

    
//
//*** digitized info. of MDC wire hits *** 
//

vector<MdcDat_mcwirhit>* MdcDatMcwirhitCol::s_col=0;

vector<MdcDat_mcwirhit>*
MdcDatMcwirhitCol::getMdcDatMcwirhitCol(void) {
    if (! s_col) s_col = new vector<MdcDat_mcwirhit> ;
    return s_col;
}

    
//
//*** MC information of MDC track *** 
//

vector<MdcRec_mctrk>* MdcRecMctrkCol::s_col=0;

vector<MdcRec_mctrk>*
MdcRecMctrkCol::getMdcRecMctrkCol(void) {
    if (! s_col) s_col = new vector<MdcRec_mctrk> ;
    return s_col;
}

    
//
//*** MDC track and generated track matching *** 
//

vector<MdcRec_mctrk2hep>* MdcRecMctrk2hepCol::s_col=0;

vector<MdcRec_mctrk2hep>*
MdcRecMctrk2hepCol::getMdcRecMctrk2hepCol(void) {
    if (! s_col) s_col = new vector<MdcRec_mctrk2hep> ;
    return s_col;
}

    
//
//*** additional tables for track of MDC *** 
//
MdcRec_trk_add::
~MdcRec_trk_add( ) {
}
vector<MdcRec_trk_add>* MdcRecTrkAddCol::s_col=0;

vector<MdcRec_trk_add>*
MdcRecTrkAddCol::getMdcRecTrkAddCol(void) {
    if (! s_col) s_col = new vector<MdcRec_trk_add> ;
    return s_col;
}

    
//
//*** mdctime run-dependent offset  ***  
//

vector<MdcRec_timeofs>* MdcRecTimeofsCol::s_col=0;

vector<MdcRec_timeofs>*
MdcRecTimeofsCol::getMdcRecTimeofsCol(void) {
    if (! s_col) s_col = new vector<MdcRec_timeofs> ;
    return s_col;
}

    
//
//*** Tof_xtime2 information with tighter cuts ***
//*** from Tof_mdctime                         ***
//
 
vector<Mdc_xtime2>* MdcXtime2Col::s_col=0;

vector<Mdc_xtime2>*
MdcXtime2Col::getMdcXtime2Col(void) {
    if (! s_col) s_col = new vector<Mdc_xtime2> ;
    return s_col;
}

