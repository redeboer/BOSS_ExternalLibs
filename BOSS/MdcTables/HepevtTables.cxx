#include "MdcTables/HepevtTables.h"
 
//
//*** generated particle information in LUND7 format
//

vector<Gen_hepevt>* GenHepevtCol::s_col=0;
 
vector<Gen_hepevt>*
GenHepevtCol::getGenHepevtCol(void) {
    if (! s_col) s_col = new vector<Gen_hepevt> ;
    return s_col;
}


//
//*** Packed Gen_HEPEVT (main part)
//

vector<Gen_pakevt>* GenPakevtCol::s_col=0;
 
vector<Gen_pakevt>*
GenPakevtCol::getGenPakevtCol(void) {
    if (! s_col) s_col = new vector<Gen_pakevt> ;
    return s_col;
}


//
//*** Packed Gen_HEPEVT (vertex part)
//

vector<Gen_pakvtx>* GenPakvtxCol::s_col=0;
 
vector<Gen_pakvtx>*
GenPakvtxCol::getGenPakvtxCol(void) {
    if (! s_col) s_col = new vector<Gen_pakvtx> ;
    return s_col;
}

