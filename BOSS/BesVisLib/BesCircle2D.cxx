//
// BesCircle2D.cxx
//
// $Author: longpx $
// 2005/7/16
// Modified from zevis 2D shape
//

#include <TString.h>
#include <TPad.h>
#include <TMath.h>

#include "BesVisLib/BesCircle2D.h"
#include "BesVisLib/BesView.h"
#include <iostream>

using namespace std;

#ifndef __CINT__
ClassImp(BesCircle2D)
#endif

//_____________________________________________________
// BesCircle2D
// Circle in 2D
//
//
BesCircle2D::BesCircle2D() {
    //
    // BesCircle2D default constructor
    if ( gDebug ) cout << "BesCircle2D ctor called" << endl;
    fInnerRadius = 0;
    fOuterRadius = 0;
    fCenter = 0;
    fNSegment = 40;
    f_innerCircleX = NULL;
    f_innerCircleY = NULL;
    f_outerCircleX = NULL;
    f_outerCircleY = NULL;
    f_areaX = NULL;
    f_areaY = NULL;
}

//_____________________________________________________

BesCircle2D::BesCircle2D(const char* name, const char* title, Double_t innerRadius, Double_t outerRadius, Double_t *center) :
        TNamed(name, title), TAttLine(), TAttFill() {
    //
    // BesCircle2D normal constructor
    if ( gDebug ) cout << "BesCircle2D normal ctor called" << endl;

    fCenter = new Double_t[3];
    for ( Int_t i = 0; i < 3; i++ ) fCenter[i] = center[i];
    //Long Peixun's update: Update fInnerRadius and fOuterRadius initialization for single variables
    fInnerRadius = innerRadius;
    fOuterRadius = outerRadius;
    fNSegment = 40;
    f_innerCircleX = NULL;
    f_innerCircleY = NULL;
    f_outerCircleX = NULL;
    f_outerCircleY = NULL;
    f_areaX = NULL;
    f_areaY = NULL;
}

//_____________________________________________________

BesCircle2D::~BesCircle2D() {
    //
    // BesCircle2D default destructor
    if ( gDebug ) cout << "BesCircle2D default dtor called" << endl;

    delete[] fCenter;     //Long Peixun's update: delete -> delete[]
}

//_____________________________________________________

Int_t BesCircle2D::DistancetoPrimitive(Int_t px, Int_t py) {

    const Int_t inaxis = 7;
    Int_t dist = 9999;

    Int_t puxmin = gPad->XtoAbsPixel(gPad->GetUxmin());
    Int_t puymin = gPad->YtoAbsPixel(gPad->GetUymin());
    Int_t puxmax = gPad->XtoAbsPixel(gPad->GetUxmax());
    Int_t puymax = gPad->YtoAbsPixel(gPad->GetUymax());

    // return if point is not in the user area
    if (px < puxmin - inaxis) return dist;
    if (py > puymin + inaxis) return dist;
    if (px > puxmax + inaxis) return dist;
    if (py < puymax - inaxis) return dist;

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) return dist;

    Double_t x = gPad->PadtoX(gPad->AbsPixeltoX(px));
    Double_t y = gPad->PadtoY(gPad->AbsPixeltoY(py));
    Double_t xndc[3];
    //if (view->GetViewType() & kXYView)
    xndc[0] = x;
    xndc[1] = y;
    xndc[2] = 0;
    //cout << "NDC    X:" << xndc[0] << " Y:" << xndc[1] << endl;

    Double_t xwc[3];
    view->NDCtoWC(xndc, xwc);
    //cout << "WC     X:" << xwc[0] << " Y:" << xwc[1] << endl;
    //cout << "Center X:" << fCenter[0] << " Y:" << fCenter[1] << endl;

    Double_t distw = 0.0;
    for (Int_t i = 0; i < 2; i++) {
        distw += (xwc[i]-fCenter[i]) * (xwc[i]-fCenter[i]);
    }

    //Long Peixun's update: for single variables
    if (distw >= fInnerRadius * fInnerRadius &&
            distw <= fOuterRadius * fOuterRadius)
        return 0;
    else
        return dist;
}

//_____________________________________________________

void BesCircle2D::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
    //cout << "I am in " << GetName() << endl;

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) view->ExecuteEvent(event, px, py);
}

//_____________________________________________________

void BesCircle2D::Draw(Option_t *option) {
    //
    // BesCircle2D draw function
    TString opt = option;
    opt.ToUpper();

    AppendPad(option);
}

//_____________________________________________________

void BesCircle2D::Paint(Option_t *option) {
    // BesCircle2D paint function
    TString opt = option;
    opt.ToUpper();

    // Transform to normalised desktop coordinates
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view == 0) cout << "no view found" << endl;

    // Draw Painted area between circles as PaintFilledArea
    Int_t np = fNSegment; //40;

    if (f_innerCircleX){
        delete [] f_innerCircleX;
        f_innerCircleX = NULL;
    }
    if (f_innerCircleY){
        delete [] f_innerCircleY;
        f_innerCircleY = NULL;
    }
    if (f_outerCircleX){
        delete [] f_outerCircleX;
        f_outerCircleX = NULL;
    }
    if (f_outerCircleY){
        delete [] f_outerCircleY;
        f_outerCircleY = NULL;
    }
    if (f_areaX) {
        delete [] f_areaX;
        f_areaX = NULL;
    }
    if (f_areaY) {
        delete [] f_areaY;
        f_areaY = NULL;
    }

    f_innerCircleX = new Double_t[np+1];
    f_innerCircleY = new Double_t[np+1];
    f_outerCircleX = new Double_t[np+1];
    f_outerCircleY = new Double_t[np+1];
    f_areaX = new Double_t[4*np];
    f_areaY = new Double_t[4*np];

    TAttLine::Modify();  //Change line attributes only if necessary
    TAttFill::Modify();  //Change fill attributes only if necessary

    Double_t angle;
    Double_t dphi = 2*TMath::Pi()/(np);
    Double_t pointWC[3],pointNDC[3];

    for (Int_t i=0; i< np ; i++) {
        angle = Double_t(i)*dphi;

        // inner circle
        //Long Peixun's update: fInnerRadius has been changed to single variable
        pointWC[0] = fCenter[0] + fInnerRadius * TMath::Cos(angle);
        pointWC[1] = fCenter[1] + fInnerRadius * TMath::Sin(angle);
        pointWC[2] = fCenter[2];
        view->WCtoNDC(pointWC,pointNDC);
        f_innerCircleX[i] = pointNDC[0];
        f_innerCircleY[i] = pointNDC[1];
        f_areaX[4*i] = pointNDC[0];
        f_areaY[4*i] = pointNDC[1];
        if ( i == 0 ) {
            f_areaX[4*np - 3] = pointNDC[0];
            f_areaY[4*np - 3] = pointNDC[1];
        } else {
            f_areaX[4*i - 3] = pointNDC[0];
            f_areaY[4*i - 3] = pointNDC[1];
        }

        // outer circle
        //Long Peixun's update: fOuterRadius has been changed to single variable
        pointWC[0] = fCenter[0] + fOuterRadius * TMath::Cos(angle);
        pointWC[1] = fCenter[1] + fOuterRadius * TMath::Sin(angle);
        pointWC[2] = fCenter[2];
        view->WCtoNDC(pointWC,pointNDC);
        f_outerCircleX[i] = pointNDC[0];
        f_outerCircleY[i] = pointNDC[1];
        f_areaX[4*i + 3] = pointNDC[0];
        f_areaY[4*i + 3] = pointNDC[1];
        if ( i == 0 ) {
            f_areaX[4*np - 2] = pointNDC[0];
            f_areaY[4*np - 2] = pointNDC[1];
        } else {
            f_areaX[4*i - 2] = pointNDC[0];
            f_areaY[4*i - 2] = pointNDC[1];
        }

    }

    // last point for circles
    f_innerCircleX[np] = f_innerCircleX[0];
    f_innerCircleY[np] = f_innerCircleY[0];
    f_outerCircleX[np] = f_outerCircleX[0];
    f_outerCircleY[np] = f_outerCircleY[0];

    // paint filled areas
    for (Int_t i = 0; i < np; i++ ) {
        gPad->PaintFillArea(4,&f_areaX[4*i],&f_areaY[4*i]);
    }

    // paint circles
    // yzhang
    gPad->PaintPolyLine(np+1,f_innerCircleX,f_innerCircleY);
    gPad->PaintPolyLine(np+1,f_outerCircleX,f_outerCircleY);

}
//_____________________________________________________

char *BesCircle2D::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

//_____________________________________________________

void BesCircle2D::SetCenter(Double_t x, Double_t y, Double_t z)
{
    fCenter[0] = x;
    fCenter[1] = y;
    fCenter[2] = z;
}

//_____________________________________________________

void BesCircle2D::GetCenter(Double_t *center)
{
    for (Int_t i = 0; i < 3; i++) center[i] = fCenter[i];
}
