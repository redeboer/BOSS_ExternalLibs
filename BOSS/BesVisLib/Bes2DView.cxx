//
// Bes2DView.cxx
//
// $Author: maqm $
// $Date: 2011/03/09 07:22:53 $
// $Revision: 1.7 $
//

#include "BesVisLib/Bes2DView.h"
#include <iostream>

#ifndef ROOT_TGeometry
#include <TGeometry.h>
#endif

#ifndef ROOT_TPad
#include <TPad.h>
#endif

// #ifndef ROOT_TVirtualPS
// #include <TVirtualPS.h>
// #endif

#ifndef ROOT_TPaveLabel
#include <TPaveLabel.h>
#endif

#ifndef ROOT_TROOT
#include <TROOT.h>
#endif

#ifndef ROOT_TMath
#include <TMath.h>
#endif
#include <TCanvas.h>

using namespace std;

#ifndef __CINT__
ClassImp(Bes2DView)
#endif

//_____________________________________________________
// Bes2DView
// View class designed for the Bes event display
//
Bes2DView::Bes2DView() : BesTView(1) {
    //
    // Bes2DView normal constructor
    if ( gDebug ) cout << "Bes2DView normal ctor called" << endl;

    // init Label
    fLabel = new TPaveLabel(0.02,0.17,0.02,0.09,"3D View");
    fLabel->SetFillColor(42);


    // init status of 3D view
    fStatus3D = new BesStatus();
    fStatus3D->Default(k3DView);

    // init status of XY view
    fStatusXY = new BesStatus();
    fStatusXY->Default(kXYView);

    // init status of ZR view
    fStatusZR = new BesStatus();
    fStatusZR->Default(kZRView);

    // init status of current view
    fStatusCurrent = new BesStatus();

    // Init
    f2DViewType = 0;  // XY:0, ZR:1;
    fStatusCurrent->Transfer(fStatus3D,1);
    SetRange(-200,-200,-200,200,200,200);
    fZoomFactor = 1.5;

    fMarkerSize = 0.1;
}

//_____________________________________________________

Bes2DView::~Bes2DView() {
    //
    // Bes2DView default destructor
    if ( gDebug ) cout << "Bes2DView default dtor called" << endl;

    delete fLabel;

    delete fStatus3D;
    delete fStatusXY;
    delete fStatusZR;
    delete fStatusCurrent;

}

//_____________________________________________________

void Bes2DView::ClearPad() {
    //
    // Clear current without deleting this view
    //
    // Probably this doesn't work for the postscript output
    // Look at TPad::Clear() for more details
    if ( gDebug ) cout << "Bes2DView::ClearPad called" << endl;
    if ( gPad->GetListOfPrimitives() ) gPad->GetListOfPrimitives()->Clear();
}

//_____________________________________________________

void Bes2DView::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
    //
    // Execute mouse events
    if ( gDebug ) cout << "Bes2DView::ExecuteEvent called" << endl;
    //cout << "f2DViewType " << f2DViewType << "  k3DView " << k3DView << endl;

    /*
    if ( fViewType & k3DView ) {
      ExecuteRotateView(event, px, py);
    } else {
    */

    // Editor mode
    if (gROOT->GetEditorMode()) {
        cout << "ROOT" << endl;
        gPad->ExecuteEvent(event,px,py);
        return;
    }

    static Double_t xmin, ymin, xmax, ymax, x0, y0;
    Double_t dx, dy, x, y;
    switch (event) {

    case kButton1Down:
        gPad->SetCursor(kMove);
        // Remember position of pointer
        xmin = gPad->GetX1();
        ymin = gPad->GetY1();
        xmax = gPad->GetX2();
        ymax = gPad->GetY2();
        x0   = gPad->AbsPixeltoX(px);
        y0   = gPad->AbsPixeltoY(py);
        break;

    case kButton1Motion:
        gPad->SetCursor(kMove);

    case kButton1Up:
        // Set new pad range
        x  = gPad->AbsPixeltoX(px);
        y  = gPad->AbsPixeltoY(py);
        dx = -(x - x0);
        dy = -(y - y0);
        //((TPad*)gPad)->SetNoAspect(kTRUE);
        gPad->Range(xmin+dx, ymin+dy, xmax+dx, ymax+dy);
        //((TPad*)gPad)->SetNoAspect(kFALSE);
        gPad->Modified();
        break;

    default:
        gPad->SetCursor(kCross);
    }
}

//_____________________________________________________

void Bes2DView::Move(Int_t px, Int_t py) {
    //
    // Move (px, py in pixel coordinates)
    Double_t x1, y1, x2, y2, dx, dy, x, y, x0, y0;
    x1 = gPad->GetX1();
    y1 = gPad->GetY1();
    x2 = gPad->GetX2();
    y2 = gPad->GetY2();
    x0 = gPad->PixeltoX(0);
    y0 = gPad->PixeltoY(0);
    x = gPad->PixeltoX(px);
    y = gPad->PixeltoY(py);
    dx = -(x - x0);
    dy = -(y - y0);
    //((TPad*)gPad)->SetNoAspect(kTRUE);
    gPad->Range(x1+dx, y1+dy, x2+dx, y2+dy);
    //((TPad*)gPad)->SetNoAspect(kFALSE);
    gPad->Modified();
}

//_____________________________________________________

void Bes2DView::Center() {
    //
    // Center view
    /*
    if ( fViewType & k3DView ){
      this->Centered();
    } else {
    */
    Double_t xrange, yrange;
    xrange = gPad->GetX2() - gPad->GetX1();
    yrange = gPad->GetY2() - gPad->GetY1();
    gPad->Range(-xrange/2, -yrange/2, xrange/2, yrange/2);
    gPad->Modified();
}

//_____________________________________________________

void Bes2DView::Zoom() {
    //
    // Magnify
    fMarkerSize = fMarkerSize * fZoomFactor;
    ZoomView(gPad, fZoomFactor);
}

//_____________________________________________________

void Bes2DView::UnZoom() {
    //
    // Demagnify
    fMarkerSize = fMarkerSize / fZoomFactor;
    UnzoomView(gPad, fZoomFactor);
}

//_____________________________________________________

Int_t Bes2DView::GetRSign(Float_t Phi) {
    //
    // Get sign of the R coordinate (ZR view)
    // for a given phi (in radians)
    Int_t Sign;

    // Convert to deg, because phi range is given in degrees
    Float_t Phi2 = (Phi - 2*TMath::Pi()*Int_t(Phi/(2*TMath::Pi())))
                   * 180./TMath::Pi();
    if ( Phi2 < 0 ) Phi2 += 360;
    if ( Phi2 >= fStatusCurrent->fPhiRangeMin && Phi2 < fStatusCurrent->fPhiRangeMax ) {
        Sign = 1;
    } else {
        Sign = -1;
    }
    return Sign;
}


//_____________________________________________________

void Bes2DView::UpdateView(Bool_t resetview) {
    //
    // Update current view
    if ( !gGeometry ) return;

    // Redraw
    //SetViewType(fViewType,resetview);
}




void Bes2DView::DrawLabel() {
    //
    // Draw label for current view

    fLabel->SetX1NDC(0.05);
    fLabel->SetX2NDC(0.20);
    fLabel->SetY1NDC(0.05);
    fLabel->SetY2NDC(0.12);

    switch (f2DViewType) {
        //case k3DView:
        //fLabel->SetLabel("3D View");
        //break;
    case 0:
        fLabel->SetLabel("XY View");
        break;
    case 1:
        fLabel->SetLabel("ZR View");
        break;
    }
    fLabel->Draw("BR,NDC,SAME");
}

//_____________________________________________________

void Bes2DView::WCtoNDC(const Float_t *pw, Float_t *pn) {
    //
    // Transform world coordinates to
    // normalised device coordinates.
    // This method was taken from BesTView
    // and extended by FishEye view
    BesTView::WCtoNDC(pw, pn);
    //cout << "fFishEye " << fFishEye << endl;

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Float_t r, s;
        if ( f2DViewType == 1) {
            Float_t pvWC[3], pvNDC[3];
            pvWC[0] = +4;
            pvWC[1] = 0;
            pvWC[2] = 0;
            BesTView::WCtoNDC(pvWC, pvNDC);
            pn[0] = TMath::Sign(Double_t(TMath::Sqrt(TMath::Abs(pn[0] - pvNDC[0]))),
                                Double_t(pn[0] - pvNDC[0])) + pvNDC[0];
            pn[1] = TMath::Sign(Double_t(TMath::Sqrt(TMath::Abs(pn[1] - pvNDC[1]))),
                                Double_t(pn[1] - pvNDC[1])) + pvNDC[1];
        } else {
            r = TMath::Sqrt(TMath::Power(pn[0],2)
                            +TMath::Power(pn[1],2));
            s = TMath::Sqrt(r)/r;
            pn[0] *= s;
            pn[1] *= s;
        }
    }
}

//_____________________________________________________

void Bes2DView::WCtoNDC(const Double_t *pw, Double_t *pn) {
    //
    // Transform world coordinates to
    // normalised device coordinates.
    // This method was taken from BesTView
    // and extended by FishEye view
    BesTView::WCtoNDC(pw, pn);

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Double_t r, s;
        if ( f2DViewType == 1 ) {
            Double_t pvWC[3], pvNDC[3];
            pvWC[0] = +4;
            pvWC[1] = 0;
            pvWC[2] = 0;
            BesTView::WCtoNDC(pvWC, pvNDC);
            pn[0] = TMath::Sign(TMath::Sqrt(TMath::Abs(pn[0] - pvNDC[0])),
                                pn[0] - pvNDC[0]) + pvNDC[0];
            pn[1] = TMath::Sign(TMath::Sqrt(TMath::Abs(pn[1] - pvNDC[1])),
                                pn[1] - pvNDC[1]) + pvNDC[1];
        } else {

            r = TMath::Sqrt(TMath::Power(pn[0],2)
                            +TMath::Power(pn[1],2));
            s = TMath::Sqrt(r)/r;
            pn[0] *= s;
            pn[1] *= s;
        }
    }
}

//_____________________________________________________

void Bes2DView::NDCtoWC(const Float_t* pn, Float_t* pw) {
    //
    // Transform back from NDCs
    Float_t p[3];
    if (f2DViewType == 0) { // XYView
        p[0] = pn[0];
        p[1] = pn[1];
        p[2] = pn[2];
    }
    else {                // ZRView
        p[0] = pn[2];
        p[1] = pn[1];
        p[2] = pn[0];
    }

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Float_t r;
        if ( f2DViewType == 1 ) {
            Float_t pvWC[3], pvNDC[3];
            pvWC[0] = +4;
            pvWC[1] = 0;
            pvWC[2] = 0;
            BesTView::WCtoNDC(pvWC, pvNDC);
            p[0] = TMath::Sign((pn[0] - pvNDC[0])*(pn[0] - pvNDC[0]),
                               pn[0] - pvNDC[0]) + pvNDC[0];
            p[1] = TMath::Sign((pn[1] - pvNDC[1])*(pn[1] - pvNDC[1]),
                               pn[1] - pvNDC[1]) + pvNDC[1];
        } else {
            r = TMath::Sqrt(TMath::Power(p[0],2)
                            +TMath::Power(p[1],2));
            p[0] *= r;
            p[1] *= r;
        }
    }
    BesTView::NDCtoWC(p, pw);
}

//_____________________________________________________

void Bes2DView::NDCtoWC(const Double_t* pn, Double_t* pw) {
    //
    // Transform back from NDCs
    Double_t p[3];
    if (f2DViewType == 0) { // XYView
        p[0] = pn[0];
        p[1] = pn[1];
        p[2] = pn[2];
    }
    else {                // ZRView
        p[0] = pn[2];
        p[1] = pn[1];
        p[2] = pn[0];
    }

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Double_t r;
        if ( f2DViewType == 1 ) {
            Double_t pvWC[3], pvNDC[3];
            pvWC[0] = +4;
            pvWC[1] = 0;
            pvWC[2] = 0;
            BesTView::WCtoNDC(pvWC, pvNDC);
            p[0] = TMath::Sign((pn[0] - pvNDC[0])*(pn[0] - pvNDC[0]),
                               pn[0] - pvNDC[0]) + pvNDC[0];
            p[1] = TMath::Sign((pn[1] - pvNDC[1])*(pn[1] - pvNDC[1]),
                               pn[1] - pvNDC[1]) + pvNDC[1];
        } else {
            r = TMath::Sqrt(TMath::Power(p[0],2)
                            +TMath::Power(p[1],2));
            p[0] *= r;
            p[1] *= r;
        }
    }
    BesTView::NDCtoWC(p, pw);
}

//_____________________________________________________

char *Bes2DView::GetObjectInfo(Int_t px, Int_t py) const {
    //
    // Returns string containing info about the object at position (px,py).
    Double_t pw[3];
    //((TPad*)gPad)->PixeltoWC(px, py, pw);

    static char info[64];
    if ( f2DViewType == 1 ) {
        sprintf(info, "z=%-.3fcm, r=%-.3fcm", pw[0], pw[1]);
        return info;
    } else {
        sprintf(info, "x=%-.3fcm, y=%-.3fcm", pw[0], pw[1]);
        return info;
    }
}

//_____________________________________________________

void Bes2DView::Reset() {
    //
    // reset view to default values

    // reset status instances of views
    //fStatus3D->Default(k3DView);
    //fStatusXY->Default(kXYView);
    //fStatusZR->Default(kZRView);
    fZoomFactor = 1.5;

    // reset current status instance tp fViewType
    //fStatusCurrent->Default(fViewType);

    // center the view
    this->Center();
    this->SetRange(-2635,-2635,-2875,2635,2635,2875);
    //int iret;
    //this->SetView(0,0,270,iret);
    this->ZoomView(gPad, 1.5);

    // reset phirange
    //  this->SetPhiRange(0,180);
    //this->UpdateView();
    gPad->Update();
}
