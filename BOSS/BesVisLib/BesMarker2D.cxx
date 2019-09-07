
#include <iostream>
#include <TString.h>
#include <TPad.h>

#include "BesVisLib/BesMarker2D.h"
#include "BesVisLib/BesView.h"

using namespace std;

#ifndef __CINT__
ClassImp(BesMarker2D)
#endif

//_____________________________________________________
// BesMarker2D
// 2-dimensional marker
//
//
BesMarker2D::BesMarker2D() {
    //
    // BesMarker2D default constructor
    if ( gDebug ) cout << "BesMarker2D ctor called" << endl;
}

//_____________________________________________________

BesMarker2D::BesMarker2D(const char* name, const char* title, Float_t x, Float_t y, Float_t z) : TMarker(0,0,3) {
    //
    // BesMarker2D normal constructor
    if ( gDebug ) cout << "BesMarker2D normal ctor called" << endl;
    fName = name;
    fTitle = title;
    fWCX = x;
    fWCY = y;
    fWCZ = z;
    this->SetMarkerColor(kGreen);
    this->SetMarkerSize(1);
    this->SetMarkerSizeMultiple(4);
    TAttMarker::Modify();

}

//_____________________________________________________

BesMarker2D::BesMarker2D(const char* name, const char* title, Double_t x, Double_t y, Double_t z) : TMarker(0,0,3) {
    //
    // BesMarker2D normal constructor
    if ( gDebug ) cout << "BesMarker2D normal ctor called" << endl;
    fName = name;
    fTitle = title;
    fWCX = (Float_t)x;
    fWCY = (Float_t)y;
    fWCZ = (Float_t)z;
    this->SetMarkerColor(kGreen);
    this->SetMarkerSize(1);
    this->SetMarkerSizeMultiple(4); // default
    TAttMarker::Modify();

}
/*
//______________________________________________________________________________

Int_t BesMarker2D::DistancetoPrimitive(Int_t px, Int_t py) {

  Int_t pxm, pym;
  if (TestBit(kMarkerNDC)) {
    pxm = gPad->UtoPixel(fX);
    pym = gPad->VtoPixel(fY);
  } else {
    pxm  = gPad->XtoAbsPixel(gPad->XtoPad(fX));
    pym  = gPad->YtoAbsPixel(gPad->YtoPad(fY));
  }
  Int_t dist = sqrt( Double_t((px-pxm)*(px-pxm) + (py-pym)*(py-pym)) );
  if (dist < 20) dist = 2;

  return dist;
}
*/

void BesMarker2D::GetCenterPixel(Int_t &px, Int_t &py) {

    if (TestBit(kMarkerNDC)) {
        px = gPad->UtoPixel(fX);
        py = gPad->VtoPixel(fY);
    } else {
        px  = gPad->XtoAbsPixel(gPad->XtoPad(fX));
        py  = gPad->YtoAbsPixel(gPad->YtoPad(fY));
    }
}

//________________________________________________________

void BesMarker2D::Draw(Option_t *option) {
    //
    // ZMarker3D draw function
    AppendPad(option);
}

//_____________________________________________________

void BesMarker2D::Paint(Option_t *option) {
    //
    // BesMarker2D paint function

    // get View
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    // get Marker Size from ZView
    this->SetMarkerSize(view->GetMarkerSize() * this->GetMarkerSizeMultiple());
    //TAttMarker::Modify();

    // transform to NDC
    Float_t WC[3],NDC[3];
    WC[0]=fWCX;
    WC[1]=fWCY;
    WC[2]=fWCZ;
    view->WCtoNDC(WC,NDC);
    this->SetX(NDC[0]);
    this->SetY(NDC[1]);

    // call TMarker::Paint

    TMarker::Paint(option);

}

//_____________________________________________________

BesMarker2D::~BesMarker2D() {
    //
    // BesMarker2D default destructor
    if ( gDebug ) cout << "BesMarker2D default dtor called" << endl;
}

//_____________________________________________________

char *BesMarker2D::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);

    /*  //
    // Returns string containing info about the object at position (px,py).
    TString info("Info: x=");
    info += px;
    info += (", y=");
    info += py;
    return (char*)(info.Data());
    */
}

//_____________________________________________________

void BesMarker2D::SetParameter(const char* name, const char* title, Float_t x, Float_t y, Float_t z) {
    //
    // set parameters
    fName = name;
    fTitle = title;
    fWCX = x;
    fWCY = y;
    fWCZ = z;
}

