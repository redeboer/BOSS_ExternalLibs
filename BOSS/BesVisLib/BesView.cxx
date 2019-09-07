//
// BesView.cxx
//

#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TPaveLabel.h>
#include <iostream>
#include "math.h"
#include "TMath.h"

#include "BesVisLib/BesView.h"
#include "BesVisLib/BesGeometry.h"
#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesCursor.h"

using namespace std;

#ifndef __CINT__
ClassImp(BesView)
#endif

//_____________________________________________________
// BesView
// View class designed for the BesVis display
//
BesView::BesView() : BesTView(1) {
    //
    // BesView normal constructor
    if ( gDebug ) cout << "BesView normal ctor called" << endl;

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
    fViewType = k3DView;
    fStatusCurrent->Transfer(fStatus3D,1);
    if ( !gBesGeometry ) {
        cout << "BesGeometry does not exist! " << endl;
        return;
    }

    m_BesR = gBesGeometry->GetBesR();
    m_BesZ = gBesGeometry->GetBesZ();
    DefaultRange();
    fExtentDefault = GetExtent();

    fZoomFactor = 1.2;
    //fMarkerSize = 0.1;    //Long Peixun's update: fMarkerSize has been initialized in DefaultRange()
    //Long Peixun's update: Initialize ViewCenter
    ViewCenter[0] = ViewCenter[1] = 0.0;
}

//_____________________________________________________

BesView::~BesView() {
    //
    // BesView default destructor
    if ( gDebug ) cout << "BesView default dtor called" << endl;

    delete fLabel;

    delete fStatus3D;
    delete fStatusXY;
    delete fStatusZR;
    delete fStatusCurrent;
}

//_____________________________________________________

void BesView::DefaultRange()
{
    SetRange(-1.0*m_BesR, -1.0*m_BesR, -0.5*m_BesZ, m_BesR, m_BesR, 0.5*m_BesZ);
    fMarkerSize = 0.1;      //Long Peixun's update: Reset marker size here
}

//_____________________________________________________

void BesView::SetViewType(EBESViewType ViewType, Bool_t resetview) {
    //
    // Set type of view

    // check the state of the component before change and copy current status to respective status
    if (fViewType & k3DView)           { fStatus3D->Transfer(fStatusCurrent, 0); }
    else if (fViewType & kXYView)      { fStatusXY->Transfer(fStatusCurrent, 0); }
    else if (fViewType & kZRView)      { fStatusZR->Transfer(fStatusCurrent, 0); }

    fViewType = ViewType;
    ClearPad();

    // Set view direction properly
    Int_t iret;
    if ( fViewType & k3DView ) {
        //cout << "SetViewType 3D" << endl;
        //
        // 3D view

        // remember my previous state, set parallel first anyway, then choose perspective or not
        Int_t iPerspective = 0;

        if (IsPerspective()) iPerspective = 1;

        // copy 3D status to current status
        fStatusCurrent->Transfer(fStatus3D,1);

        // Geometry exist ?
        if ( !gBesGeometry ) return;

        // set view
        if ( resetview ) {
            //SetView(0, 90, 270, iret);
            this->Front();
            this->Center();

            DefaultRange();
            //for (int i = 0; i < 2; i++) this->ZoomView(gPad, fZoomFactor);
            //Long Peixun's update: Using SetZoomRatio instead of ZoomView
            SetZoomRatio(fStatus3D->fZoom);

            //Long Peixun's update: don't reset marker size
            //fMarkerSize = 0.1;
        }

        // Draw geometry

        //cout << "is there any problem after update from root's version 4.03 to 4.04" << endl; //Long Peixun remove
        gBesGeometry->Draw("3D,SAME");
        //this->SetParralel();       //comment out 2007.5.1 try to use ROOT-5.10.00
        this->SetParallel();
        //old version 4.0.3 have a spell mistake BesTView::SetParralel()=>SetParallel()
        //changed by tianhl at Mon Aug 20 2007
        if (iPerspective == 1) this->SetPerspective();

        // Draw tracks
        if ( gEvent )
        {
            gEvent->DrawTracks("");
        }
        //else cout << "3D gEvent does not exist " << endl;

    } else if ( fViewType & kXYView ) {
        //cout << "SetViewType XY" << endl;
        //
        // XY view
        // =======

        // copy XY status to current status
        fStatusCurrent->Transfer(fStatusXY,1);

        // Geometry exist ?
        if ( !gBesGeometry ) return;

        // set view
        if ( resetview ) {
            SetView(0, 0, 270, iret);
            this->Center();

            DefaultRange();
            //DefaultRange();   //Long Peixun's update: Remove redundant DefaultRange()
            //for (int i = 0; i < 2; i++) this->ZoomView(gPad, fZoomFactor);
            //Long Peixun's update: Using SetZoomRatio instead of ZoomView
            SetZoomRatio(fStatusXY->fZoom);

            //Long Peixun's update: don't reset marker size
            //fMarkerSize = 0.1;

        }

        /*
        // Draw event
        if ( gEvent ) gEvent->SetHits("XY");
        else cout << "XY : gEvent does not exist " << endl;
        */

        // Draw geometry
        gBesGeometry->Draw("XY,SAME");

        // Draw 2D hits and tracks
        if ( gEvent ) {
            gEvent->DrawHits("");
            gEvent->DrawTracks("");
        }
        //else cout << "XY : gEvent does not exist " << endl;

    } else if ( fViewType & kZRView ) {
        //cout << "SetViewType ZR" << endl;
        //
        // ZR view
        // =======

        // copy ZR status to current status
        fStatusCurrent->Transfer(fStatusZR,1);

        // Geometry exist ?
        if ( !gBesGeometry ) return;

        // set view
        if ( resetview ) {
            SetView(180,90,90,iret);
            this->Center();

            DefaultRange();
            //DefaultRange();   //Long Peixun's update: Remove redundant DefaultRange()
            //for (int i = 0; i < 2; i++) this->ZoomView(gPad, fZoomFactor);
            //Long Peixun's update: Using SetZoomRatio instead of ZoomView
            SetZoomRatio(fStatusZR->fZoom);

            //Long Peixun's update: don't reset marker size
            //fMarkerSize = 0.1;

        }

        /*
        // Draw event
        if ( gEvent ) gEvent->SetHits("ZR");
        else cout << "ZR : gEvent does not exist " << endl;
        */

        // Draw geometry
        gBesGeometry->Draw("ZR,SAME");

        // Draw hits and tracks
        if ( gEvent ) {
            gEvent->DrawHits("");
            gEvent->DrawTracks("");
        }
        //else cout << "ZR : gEvent does not exist " << endl;

    }

    // Draw label
    DrawLabel();
}

void BesView::SetViewTypeBare(EBESViewType ViewType, Bool_t resetview) {
    /*
    //
    // Set type of view

    cout << "setviewtypeBare: start..." << endl;
    // check the state of the component before change and copy current status to respective status
    if ( fViewType & k3DView ){

      fStatus3D->Transfer(fStatusCurrent,0);

    } else if ( fViewType & kXYView ) {

      fStatusXY->Transfer(fStatusCurrent,0);

    } else if ( fViewType & kZRView ) {


      fStatusZR->Transfer(fStatusCurrent,0);

    }

    fViewType = ViewType;
    ClearPad();

    // Set view direction properly
    Int_t iret;
    cout << "fViewType=" << fViewType << endl;
    if ( fViewType & k3DView ) {
      //
      // 3D view

      // copy 3D status to current status
      fStatusCurrent->Transfer(fStatus3D,1);
      // Geometry exist ?
      if ( !gGeometry ) return;

      // set view
      if ( resetview ) {
        SetView(0, 90, 270, iret);
        SetRange(-600,-600,-400,600,600,800);

        // reset marker size
        fMarkerSize = 0.1;

      }

    } else if ( fViewType & kXYView ) {
      //
      // XY view
      // =======

      // copy XY status to current status
      fStatusCurrent->Transfer(fStatusXY,1);

      // Geometry exist ?
      if ( !gGeometry ) return;

      // set view
      if ( resetview ) {
        SetView(270, 0, 0, iret);
        // SetRange(-200,-200,-200,200,200,200);
        SetRange(-310,-310,-310,310,310,310);
        // reset marker size
        fMarkerSize = 0.1;

      }

    } else if ( fViewType & kZRView ) {

      //
      // ZR view
      // =======

      // copy ZR status to current status
      cout << "fViewType & kZRView: before transfer..." << endl;
      fStatusCurrent->Transfer(fStatusZR,1);
      cout << "fViewType & kZRView: after transfer..." << endl;

      // Geometry exist ?
      if ( !gGeometry ) return;

      // set view
      if ( resetview ) {
        SetView(90, 180, 0, iret);
        //      SetRange(-130,-200,-200,270,200,200);
        SetRange(-160,-400,-400,740,400,400);
        // reset marker size
        fMarkerSize = 0.1;

      }

    }

    // Draw label
    cout << "SetViewTypeBare: no drawlabel" << endl;
    //DrawLabel();
    */
}

//_____________________________________________________

void BesView::ClearPad() {
    //
    // Clear current without deleting this view
    //
    // Probably this doesn't work for the postscript output
    // Look at TPad::Clear() for more details
    if ( gDebug ) cout << "BesView::ClearPad called" << endl;
    if ( gPad->GetListOfPrimitives() ) gPad->GetListOfPrimitives()->Clear();
}

//_____________________________________________________

void BesView::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
    //
    // Execute mouse events
    if ( gDebug ) cout << "BesView::ExecuteEvent called" << endl;

    // Editor mode
    if (gROOT->GetEditorMode()) {
        gPad->ExecuteEvent(event,px,py);
        return;
    }

    if (gBesCursor->GetType() == kBesHand) {

        gPad->SetCursor(kMove);
        static Double_t xmin, ymin, xmax, ymax, x0, y0;
        Double_t dx, dy, x, y;
        switch (event) {

        case kButton1Down:
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
            //((ZPad*)gPad)->SetNoAspect(kTRUE);
            gPad->Range(xmin+dx, ymin+dy, xmax+dx, ymax+dy);
            //((ZPad*)gPad)->SetNoAspect(kFALSE);
            gPad->Modified();
            //Long Peixun's update: Set view center
            ViewCenter[0] = (xmin + xmax) / 2 + dx;
            ViewCenter[1] = (ymin + ymax) / 2 + dy;
            break;

        default:
            gPad->SetCursor(kMove);
        }
    }
    else if (gBesCursor->GetType() == kBesPick){

        gPad->SetCursor(kPointer);

        if ( fViewType & k3DView ) {
            ExecuteRotateView(event, px, py);
        }
    }
}

//_____________________________________________________

void BesView::Move(Int_t px, Int_t py) {

    /*
    Double_t p[3], pw[3];
    for (Int_t i = 0; i < 3; i++) {
      pw[i] = 0.5*(fRmin[i]+fRmax[i]);
    }
    cout << "pw " << pw[0] << " " << pw[1] << " " << pw[2] << endl;
    WCtoNDC(pw, p);
    cout << "p " << p[0] << " " << p[1] << " " << p[2] << endl;

    p[0] += (gPad->PixeltoX(0) - gPad->PixeltoX(px));
    p[1] += (gPad->PixeltoY(0) - gPad->PixeltoY(py));
    cout << "p " << p[0] << " " << p[1] << " " << p[2] << endl;
    NDCtoWC(p, pw);
    cout << "pw " << pw[0] << " " << pw[1] << " " << pw[2] << endl;

    MoveFocus(&pw[0], fRmax[0]-fRmin[0], fRmax[1]-fRmin[1], fRmax[2]-fRmin[2]);
    gPad->Modified();
    gPad->Update();
    */
   if (!gPad) return;  //Long Peixun's update: check pad pointer before move

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

    gPad->Range(x1+dx, y1+dy, x2+dx, y2+dy);
    gPad->Modified();

    //Long Peixun's update: Set view center
    ViewCenter[0] = (x1 + x2) / 2 + dx;
    ViewCenter[1] = (y1 + y2) / 2 + dy;
}

//_____________________________________________________

void BesView::Center() {
    //
    // Center view
    //if ( fViewType & k3DView ){
    //this->Centered();
    //} else {
    Double_t xrange, yrange;
    xrange = gPad->GetX2() - gPad->GetX1();
    yrange = gPad->GetY2() - gPad->GetY1();
    gPad->Range(-xrange/2, -yrange/2, xrange/2, yrange/2);
    gPad->Modified();

    //Long Peixun's update: Set view center
    ViewCenter[0] = ViewCenter[1] = 0.0;
    //}
}

//_____________________________________________________

void BesView::SetZoomRatio(Double_t ratio)
{
    //Long Peixun's update: Move pad and let Pad center be ViewCenter
    Double_t dx = (ratio / GetZoomRatio() - 1) * ViewCenter[0];
    Double_t dy = (ratio / GetZoomRatio() - 1) * ViewCenter[1];

    DefaultRange();
    fMarkerSize *= ratio;       //Long Peixun's update: Should set marker size
    ZoomView(gPad, ratio);

    //Long Peixun's update: Move pad and let Pad center be ViewCenter
    Double_t newx1 = gPad->GetX1() + dx;
    Double_t newy1 = gPad->GetY1() + dy;
    Double_t newx2 = gPad->GetX2() + dx;
    Double_t newy2 = gPad->GetY2() + dy;
    gPad->Range(newx1, newy1, newx2, newy2);
    gPad->Modified();
    ViewCenter[0] = (newx1 + newx2) / 2;
    ViewCenter[1] = (newy1 + newy2) / 2;
    
    //yzhang TEMP FIXME
    fStatusCurrent->fZoom = ratio;
    fStatusXY->fZoom  = ratio;
    fStatusZR->fZoom  = ratio;
    fStatus3D->fZoom  = ratio;
}

//_____________________________________________________

void BesView::ZoomIn() {
    //
    // Magnify
    //Long Peixun's update: Move pad and let Pad center be ViewCenter
    Double_t dx = (fZoomFactor - 1) * ViewCenter[0];
    Double_t dy = (fZoomFactor - 1) * ViewCenter[1];

    fMarkerSize = fMarkerSize * fZoomFactor;
    ZoomView(gPad, fZoomFactor); // dont use ZoomView as it is not base on center

    fStatusCurrent->fZoom = GetZoomRatio();
    fStatusXY->fZoom  = GetZoomRatio();
    fStatusZR->fZoom  = GetZoomRatio();
    fStatus3D->fZoom  = GetZoomRatio();

    //Long Peixun's update: Move pad and let Pad center be ViewCenter
    Double_t newx1 = gPad->GetX1() + dx;
    Double_t newy1 = gPad->GetY1() + dy;
    Double_t newx2 = gPad->GetX2() + dx;
    Double_t newy2 = gPad->GetY2() + dy;
    gPad->Range(newx1, newy1, newx2, newy2);
    gPad->Modified();
    ViewCenter[0] = (newx1 + newx2) / 2;
    ViewCenter[1] = (newy1 + newy2) / 2;

    //Double_t scale[3], center[3];
    //Int_t irep;
    //FindScope(&scale[0], &center[0], irep);
    //cout << "scale  " << scale[0]  << "  " << scale[1]  << " " << scale[2]  << endl;
    //cout << "center " << center[0] << "  " << center[1] << " " << center[2] << endl;

    //Double_t extent = GetExtent();
    //cout << extent << endl;
    //cout << GetZoomRatio() << endl;

    /*
    Double_t x1, y1, x2, y2;
    x1 = gPad->GetX1();
    y1 = gPad->GetY1();
    x2 = gPad->GetX2();
    y2 = gPad->GetY2();

    Double_t dx = (gPad->GetX2() - gPad->GetX1())*(1-1/fZoomFactor);
    Double_t dy = (gPad->GetY2() - gPad->GetY1())*(1-1/fZoomFactor);

    gPad->Range(x1+dx/2, y1+dy/2, x2-dx/2, y2-dy/2);
    gPad->Modified();
    */
}

//_____________________________________________________

void BesView::ZoomOut() {
    //
    // Demagnify
    //Long Peixun's update: Move pad and let Pad center be ViewCenter
    Double_t dx = (1 / fZoomFactor - 1) * ViewCenter[0];
    Double_t dy = (1 / fZoomFactor - 1) * ViewCenter[1];

    fMarkerSize = fMarkerSize / fZoomFactor;
    UnzoomView(gPad, fZoomFactor); // dont use ZoomView as it is not base on center

    fStatusCurrent->fZoom = GetZoomRatio();
    fStatusXY->fZoom  = GetZoomRatio();
    fStatusZR->fZoom  = GetZoomRatio();
    fStatus3D->fZoom  = GetZoomRatio();

    //Long Peixun's update: Move pad and let Pad center be ViewCenter
    Double_t newx1 = gPad->GetX1() + dx;
    Double_t newy1 = gPad->GetY1() + dy;
    Double_t newx2 = gPad->GetX2() + dx;
    Double_t newy2 = gPad->GetY2() + dy;
    gPad->Range(newx1, newy1, newx2, newy2);
    gPad->Modified();
    ViewCenter[0] = (newx1 + newx2) / 2;
    ViewCenter[1] = (newy1 + newy2) / 2;

    //Double_t extent = GetExtent();
    //cout << extent << endl;
    //cout << GetZoomRatio() << endl;

    /*
    Double_t x1, y1, x2, y2;
    x1 = gPad->GetX1();
    y1 = gPad->GetY1();
    x2 = gPad->GetX2();
    y2 = gPad->GetY2();

    Double_t dx = (gPad->GetX2() - gPad->GetX1())*(fZoomFactor-1);
    Double_t dy = (gPad->GetY2() - gPad->GetY1())*(fZoomFactor-1);

    gPad->Range(x1-dx/2, y1-dy/2, x2+dx/2, y2+dy/2);
    gPad->Modified();
    */
}

//_____________________________________________________

Int_t BesView::GetRSign(Float_t Phi) {
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

void BesView::SetPhiRange(Float_t PhiMin, Float_t PhiMax) {
    //
    // Set phi range for ZR view
    if ( gDebug ) cout << "BesView::SetPhiRange called" << endl;

    fStatusCurrent->fPhiRangeMin = PhiMin;
    fStatusCurrent->fPhiRangeMax = PhiMax;

    // Update view
    if ( !gBesGeometry ) return;
    //gEvent->AddBACPadTowerZR();
    UpdateView(0);
}

//_____________________________________________________

void BesView::UpdateView(Bool_t resetview) {
    //
    // Update current view
    if ( !gBesGeometry ) return;

    // Redraw
    SetViewType(fViewType,resetview);
}


// void BesView::Plot_Et_Eta_Phi() {
//   //
//   // View Et_eta_phi histogram
//   static TCanvas *c_hist = new TCanvas("c_eta_phi", "Eta Phi Histogram",
// 				       400, 300);
//   c_hist->cd();
//   gEvent->GetHistEt_eta_phi()->Draw("Lego2");
// }


void BesView::DrawLabel() {
    //
    // Draw label for current view

    fLabel->SetX1(-0.9);
    fLabel->SetX2(-0.6);
    fLabel->SetY1(-0.9);
    fLabel->SetY2(-0.78);

    switch (fViewType) {
    case k3DView:
        fLabel->SetLabel("3D View");
        break;
    case kXYView:
        fLabel->SetLabel("XY View");
        break;
    case kZRView:
        fLabel->SetLabel("ZR View");
        break;
    //Long Peixun's update: CGEM Unfolded display for each layer
    case kCgemUFView0:
        fLabel->SetLabel("CGEM Layer0");
        break;
    case kCgemUFView1:
        fLabel->SetLabel("CGEM Layer1");
        break;
    case kCgemUFView2:
        fLabel->SetLabel("CGEM Layer2");
        break;
    }
    fLabel->Draw("BR,NDC,SAME");


    /*
    switch (fViewType) {
    case k3DView:
      fLabel->DrawPaveLabel(0.02,0.02,0.17,0.09, "3D View", "BR,NDC,SAME");
      break;
    case kXYView:
      //fLabel->SetLabel("XY View");
      fLabel->DrawPaveLabel(0.02,0.02,0.17,0.09, "XY View", "BR,NDC,SAME");
      break;
    case kZRView:
      fLabel->DrawPaveLabel(0.02,0.02,0.17,0.09, "ZR View", "BR,NDC,SAME");
      break;
    }
    */
}

//_____________________________________________________

void BesView::WCtoNDC(const Float_t *pw, Float_t *pn) {
    //
    // Transform world coordinates to
    // normalised device coordinates.
    // This method was taken from BesTView
    // and extended by FishEye view
    BesTView::WCtoNDC(pw, pn);
    //cout << "fFishEye " << fFishEye << endl;

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Float_t r, s;
        if ( fViewType & kZRView ) {
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

void BesView::WCtoNDC(const Double_t *pw, Double_t *pn) {
    //
    // Transform world coordinates to
    // normalised device coordinates.
    // This method was taken from BesTView
    // and extended by FishEye view
    BesTView::WCtoNDC(pw, pn);

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Double_t r, s;
        if ( fViewType & kZRView ) {
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

void BesView::NDCtoWC(const Float_t* pn, Float_t* pw) {
    //
    // Transform back from NDCs
    Float_t p[3];
    p[0] = pn[0];
    p[1] = pn[1];
    p[2] = pn[2];

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Float_t r;
        if ( fViewType & kZRView ) {
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

void BesView::NDCtoWC(const Double_t* pn, Double_t* pw) {
    //
    // Transform back from NDCs
    Double_t p[3];
    p[0] = pn[0];
    p[1] = pn[1];
    p[2] = pn[2];

    if ( fStatusCurrent->fFishEye == kTRUE ) {
        Double_t r;
        if ( fViewType & kZRView ) {
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

void BesView::Side3D() {
    //
    // set side view for 3D

    this->SetViewType(k3DView,1);
    Int_t iret;
    this->SetView(0,90,270,iret);
}

//_____________________________________________________

char *BesView::GetObjectInfo(Int_t px, Int_t py) const {
    //
    // Returns string containing info about the object at position (px,py).
    Double_t pw[3];
    Double_t Pndc[3] = { gPad->AbsPixeltoX(px), gPad->AbsPixeltoY(py), 0};
    BesView* view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) view->NDCtoWC(Pndc, pw);      //Long Peixun's update: check view pointer before call function
    //cout << gPad->GetName() << endl;

    //
    // Converts from pixel coordinates to world
    // coordinates of according view

    static char info[64];
    if ( fViewType & kZRView ) {
        sprintf(info, "z=%-.3fmm, r=%-.3fmm", pw[2], pw[1]);
        return info;
    } else {
        sprintf(info, "x=%-.3fmm, y=%-.3fmm", pw[0], pw[1]);
        return info;
    }
}

//_____________________________________________________

void BesView::Reset() {
    //
    // reset view to default values

    // reset status instances of views
    fStatus3D->Default(k3DView);
    fStatusXY->Default(kXYView);
    fStatusZR->Default(kZRView);
    fZoomFactor = 1.2;

    // reset current status instance tp fViewType
    fStatusCurrent->Default(fViewType);

    // reset phirange
    //  this->SetPhiRange(0,180);
    this->UpdateView(1);
    gPad->Update();
}

//______________________________________________________
//Long Peixun's update: refresh view's values
void BesView::Refresh()
{
    //Reset status instances of views
    fStatus3D->Default(k3DView);
    fStatusXY->Default(kXYView);
    fStatusZR->Default(kZRView);

    //Refresh status instances of views
    switch (fViewType)
    {
    case k3DView:
        fStatus3D->Transfer(fStatusCurrent, 1);
        break;
    case kXYView:
        fStatusXY->Transfer(fStatusCurrent, 1);
        break;
    case kZRView:
        fStatusZR->Transfer(fStatusCurrent, 1);
        break;
    }

    this->UpdateView(1);
    gPad->Update();
}