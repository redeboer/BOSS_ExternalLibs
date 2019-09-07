//
// BesHeader.cxx
//
// $Author: tianhl $
// $Date: 2007/12/10 07:50:42 $
// $Revision: 1.2 $
//


#include "BesVisLib/BesHeader.h"

using namespace std;

#ifndef __CINT__
ClassImp(BesHeader)
#endif

//______________________________________________________________-
//BesHeader
//BesVis Header
//
//
BesHeader::BesHeader() : TPaveText(), TQObject() {

    fMdcOn   = kTRUE;
    fTofOn   = kTRUE;
    fEmcOn   = kTRUE;
}


//________________________________________________________________

BesHeader::BesHeader(Double_t x1,Double_t y1,Double_t x2,Double_t y2,Option_t *option) : TPaveText(x1,y1,x2,y2,option), TQObject(){

    fMdcOn   = kTRUE;
    fTofOn   = kTRUE;
    fEmcOn   = kTRUE;
}

//________________________________________________________________

Int_t BesHeader::DistancetoPrimitive(Int_t px, Int_t py)
{
    //return TPaveText::DistancetoPrimitive(px, py);

    return 0;  // so that mouse right button will not call TImage Menu.
}

//_____________________________________________________________

BesHeader::~BesHeader(){
}
