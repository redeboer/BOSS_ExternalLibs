//
// BesPolygon2D.cxx
//
// $Author: longpx $
// 2005/7/16
// Modified from zevis 2D shape
//

#include "BesVisLib/BesView.h"
#include "BesVisLib/BesPolygon2D.h"
#include <iostream>

#include <math.h>
#include <TMath.h>
#ifndef ROOT_TPad
#include <TPad.h>
#endif

#ifndef ROOT_TString
#include <TString.h>
#endif

#ifndef ROOT_TView
#include <TView.h>
#endif

#ifndef ROOT_TGeometry
#include <TGeometry.h>
#endif

#ifndef ROOT_TPaveText
#include <TPaveText.h>
#endif

using namespace std;

#ifndef __CINT__
ClassImp(BesPolygon2D)
#endif

//_____________________________________________________
// BesPolygon2D
// 2-dimensional polygon
//
//

int BesPolygon2D::num = 0;
BesPolygon2D::BesPolygon2D() {
    //
    // BesPolygon2D default constructor
    //cout << "####################" << endl;
    //cout << "BesPolygon2D ctor called " << ++num << endl;
    //cout << "####################" << endl;

    fInfoBox = 0;
    fN = 0;
    fP = NULL;
    fPBackUp = NULL;
    fRotatable = false;
    f_xx = NULL;
    f_yy = NULL;

}

//_____________________________________________________

BesPolygon2D::BesPolygon2D(const char* name, const char* title,
                           Int_t N, Double_t *P) :
        TNamed(name, title), TAttLine(), TAttFill() {
    //
    // BesPolygon2D normal constructor
    //cout << "####################" << endl;
    //cout << "BesPolygon2D ctor called " << ++num << endl;
    //cout << "####################" << endl;

    fN = N;
    fP = new Double_t[fN*3];
    fPBackUp = new Double_t[fN*3];
    f_xx = NULL;
    f_yy = NULL;

    fInfoBox = 0;
    if (P!=NULL){
        SetPoints(P);
    }

    for ( Int_t i = 0; i < fN*3; i++ ) {
        fPBackUp[i] = fP[i];
    }

    for (Int_t j = 0; j < 3; j++) {
        fCenter[j] = 0.0;
        for ( Int_t i = 0; i < fN; i++) {
            fCenter[j] += fP[3*i+j];
        }
        fCenter[j] /= fN;
    }

    for ( Int_t i = 0; i < fN; i++ ) {
    }

    fRotatable = false;
}

//_____________________________________________________

BesPolygon2D::~BesPolygon2D() {
    //
    // BesPolygon2D default destructor
    //cout << "####################" << endl;
    //cout << "BesPolygon2D dtor called " << --num << endl;
    //cout << "####################" << endl;

    //Long Peixun's update: remove "if"
    delete [] fP;
    delete [] fPBackUp;
}

//_____________________________________________________

void BesPolygon2D::Draw(Option_t *option) {
    //
    // BesPolygon2D draw function
    TString opt = option;
    opt.ToUpper();

    AppendPad(option);
}

//_____________________________________________________

void BesPolygon2D::Paint(Option_t *option) {
    //
    // BesPolygon2D paint function
    TString opt = option;
    opt.ToUpper();

    // Transform to normalised desktop coordinates
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view == 0) cout << "no view found" << endl;
    Double_t viewPhi = view->GetLongitude();
    if (IsRotatable()) RotatePhi(viewPhi-180.0);

    if (f_xx) {
        delete [] f_xx;
        f_xx = NULL;
    }
    if (f_yy) {
        delete [] f_yy;
        f_yy = NULL;
    }

    f_xx = new Double_t[fN+1];
    f_yy = new Double_t[fN+1];
    Double_t Pndc[3];

    for ( Int_t i = 0; i < fN; i++ ) {
        view->WCtoNDC(&fP[i*3], Pndc);
        f_xx[i] = Pndc[0];
        f_yy[i] = Pndc[1];
    }

    // Close surface
    f_xx[fN] = f_xx[0];
    f_yy[fN] = f_yy[0];

    TAttLine::Modify();  //Change line attributes only if necessary
    TAttFill::Modify();  //Change fill attributes only if necessary

    gPad->PaintFillArea(fN, f_xx, f_yy);
    gPad->PaintPolyLine(fN+1, f_xx, f_yy);
    if (IsRotatable()) Restore();
}

//_____________________________________________________

Int_t BesPolygon2D::DistancetoPrimitive(Int_t px, Int_t py) {
    //
    // Compute the closest distance of approach from point px,py to the
    // center of this polygon
    // The distance is computed in pixels units.

    const Int_t inaxis = 7;
    Int_t  dist = 9999;

    if (this->IsRotatable()) return dist;

    Int_t puxmin = gPad->XtoAbsPixel(gPad->GetUxmin());
    Int_t puymin = gPad->YtoAbsPixel(gPad->GetUymin());
    Int_t puxmax = gPad->XtoAbsPixel(gPad->GetUxmax());
    Int_t puymax = gPad->YtoAbsPixel(gPad->GetUymax());

    // return if point is not in the user area
    if (px < puxmin - inaxis) return dist;
    if (py > puymin + inaxis) return dist;
    if (px > puxmax + inaxis) return dist;
    if (py < puymax - inaxis) return dist;

    // judge the mouse point and center are always on the same side of any line of ploygon
    // Transform to normalised desktop coordinates

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) return dist;

    Bool_t inPolygon = true;
    Int_t x1, y1, x2, y2, cx, cy;
    Double_t Pndc[3], k, b, pb, cb;

    view->WCtoNDC(&fCenter[0], Pndc);
    cx = gPad->XtoAbsPixel(Pndc[0]);
    cy = gPad->YtoAbsPixel(Pndc[1]);

    //cout << "px " << px << " py " << py << endl;
    //cout << "center " << cx << " " << cy << endl;

    for (Int_t i = 0; i < fN; i++) {
        view->WCtoNDC(&fP[3*i], Pndc);
        x1 = gPad->XtoAbsPixel(Pndc[0]);
        y1 = gPad->YtoAbsPixel(Pndc[1]);

        if (i != fN-1) {
            view->WCtoNDC(&fP[3*(i+1)], Pndc);
        }
        else
            view->WCtoNDC(&fP[0], Pndc);

        x2 = gPad->XtoAbsPixel(Pndc[0]);
        y2 = gPad->YtoAbsPixel(Pndc[1]);

        //cout << "x1 " << x1 << " y1 " << y1 << endl;
        //cout << "x2 " << x2 << " y2 " << y2 << endl;
        if (x1 == x2) {
            if ((px-x1)*(cx-x1) <= 0) {
                inPolygon = false;
                break;
            }
        }
        else {
            k = Double_t(y2-y1)/(x2-x1);
            b  = y1-k*x1;
            pb = py-k*px;
            cb = cy-k*cx;
            if ((pb-b)*(cb-b) <= 0) {
                inPolygon = false;
                break;
            }
        }
    }

    if (inPolygon == true) {
        //gPad->SetSelected(this);
        //gPad->SetCursor(kHand);
        return 0;
    }
    else return 9999;

    //cout << GetName() << dist << endl;
    //if (dist < 100) dist = 0;
}

//_____________________________________________________

void BesPolygon2D::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
    //cout << "I am in " << GetName() << endl;

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) view->ExecuteEvent(event, px, py);
}

//_____________________________________________________

void BesPolygon2D::SetInfoBox() {
    //
    // Set tooltip textbox with some information
    TView *view = dynamic_cast<TView*>(gPad->GetView());
    Double_t Pndc[3];
    if (view) view->WCtoNDC(&fP[0], Pndc);

    if (fInfoBox){
        delete fInfoBox;
        fInfoBox = 0;
    }
    fInfoBox  = new TPaveText(Pndc[0], Pndc[1],
                              Pndc[0]+0.4, Pndc[1]+0.1);
    fInfoBox->SetBorderSize(1);
    fInfoBox->SetFillColor(191);
    fInfoBox->AddText(GetTitle());
    fInfoBox->AddText(GetObjectInfo(0,0));
    fInfoBox->Draw();
}

//_____________________________________________________

char *BesPolygon2D::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

//_____________________________________________________

void BesPolygon2D::SetZRSign(Int_t sign) {
    //
    // set sign of points for ZR view

    for ( Int_t i = 0; i < fN; i++ ) {
        // clear sign
        fP[(i*3)+1] = TMath::Sign(1.,Double_t(fP[(i*3)+1])) * fP[(i*3)+1];

        // set sign
        fP[(i*3)+1] = TMath::Sign(1,sign) * fP[(i*3)+1];
    }

}

//_____________________________________________________

void BesPolygon2D::Resize(Double_t ScaleFactor) {
    //
    // Resize the polygon by ScaleFactor

    // Compute geometric center of the polygon
    Double_t C[3];
    GetCenter(C);

    // Rescale distances from the center
    for ( Int_t i = 0; i < 3; i++ ) {
        for ( Int_t j = 0; j < fN; j++ ) {
            fP[3*j+i] = C[i] + ScaleFactor*(fP[3*j+i]-C[i]);
        }
    }
}

//_____________________________________________________

void BesPolygon2D::GetCenter(Double_t *Center) const {
    //
    // Compute geometric center of this polygon
    for ( Int_t i = 0; i < 3; i++ ) {
        Center[i] = 0;
        for ( Int_t j = 0; j < fN; j++ ) Center[i] += fP[3*j+i];
        Center[i] /= fN;
    }
}

//_____________________________________________________

void BesPolygon2D::RotatePhi(Double_t phi) {

    //cout << "phi " << phi << endl;
    for (Int_t i = 0; i < fN; i++) {
        TVector3 vec(fP[i*3], fP[i*3+1], fP[i*3+2]);
        Double_t r = vec.Pt();
        Double_t newPhi = vec.Phi() + phi*TMath::DegToRad();
        fP[i*3]   = r * cos(newPhi);
        fP[i*3+1] = r * sin(newPhi);
    }
}

//_____________________________________________________

void BesPolygon2D::Restore() {

    for (Int_t i = 0; i < fN*3; i++) {
        fP[i] = fPBackUp[i];
    }
}

//_____________________________________________________

void BesPolygon2D::SetSize(Double_t size) {

    if (size > 0.95) size = 0.98; // too big could not see border
    if (size < 0.15) size = 0.2; // too small could not be seen

    for (Int_t i = 0; i < 3; i++) {
        for (Int_t j = 0; j < fN; j++) {
            fP[3*j+i] = size * fP[3*j+i] + (1.0-size) * fCenter[i];
        }
    }
}

//_____________________________________________________
//Long Peixun's update: Stretch polygon along (sx, sy, sz)
void BesPolygon2D::Stretch(Double_t sx, Double_t sy, Double_t sz, Double_t factor)
{
    // Compute geometric center of the polygon
    Double_t C[3], V[3];
    GetCenter(C);
    Double_t s = TMath::Sqrt(sx * sx + sy * sy + sz * sz);
    V[0] = sx / s;
    V[1] = sy / s;
    V[2] = sz / s;

    // Rescale distances from the center
    for (Int_t i = 0; i < fN; ++i)
    {
        Double_t dot = (fP[3*i] - C[0]) * V[0] + (fP[3*i+1] - C[1]) * V[1] + (fP[3*i+2] - C[2]) * V[2];
        for (Int_t j = 0; j < 3; ++j)
        {
            fP[3*i+j] += dot * (factor - 1) * V[j];
        }
    }
}
