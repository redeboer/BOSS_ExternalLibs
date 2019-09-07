#include <TString.h>
#include <TColor.h>
#include <iostream>

#include "math.h"
#include "TMath.h"
#include "BesVisLib/Muc2DStrip.h"
#include "BesVisLib/BesView.h"
#include "BesVisLib/BesCursor.h"

using namespace std;

#ifndef __CINT__
ClassImp(Muc2DStrip)
#endif

//_____________________________________________________

Muc2DStrip::Muc2DStrip() {

}

//_____________________________________________________

Muc2DStrip::Muc2DStrip(const char* name, const char* title, Int_t N, Double_t *P, Int_t part, Int_t seg, Int_t gap, Int_t strip)
{
    SetName(name);
    SetTitle(title);
    fTip = 0;
    fHighlighted = false;
    fFired = false;
    fPart  = part;
    fSeg   = seg;
    fGap   = gap;
    fStrip = strip;

    // ------XY------

    Double_t Pxy[3*4];
    for (Int_t i = 0; i < 4; i++) {
        if (part == 1) {
            Int_t iXYSeq[8] = {1,0,4,5, 2,3,7,6}; // center of 1+2, 0+3, 4+7, 5+6
            for (Int_t j = 0; j < 3; j++) {
                Pxy[3*i+j] = ( P[3*iXYSeq[i]+j] + P[3*(iXYSeq[i+4])+j] )/2.0;
            }
        }
        else {
            Int_t iXYSeq[8] = {0,1,2,3, 4,5,6,7}; // center of 0+4, 1+5, 2+6, 3+7
            for (Int_t j = 0; j < 3; j++) {
                Pxy[3*i+j] = ( P[3*iXYSeq[i]+j] + P[3*(iXYSeq[i+4])+j] )/2.0;
            }
        }
    }

    fStripXY      = new BesPolygon2D(name, title, 4, &Pxy[0]);

    // ------ZR------

    Double_t Pzr[3*6];
    Int_t nzrPoints = 6;
    if (part == 1) {
        Int_t iZRSeq[6] = {4,5,1,2,6,7}; //{1,2,6,5, 0,3,7,4}; // center of 0+4, 1+5, 2+6, 3+7
        for (Int_t i = 0; i < nzrPoints; i++) {
            for (Int_t j = 0; j < 3; j++) {
                Pzr[3*i+j] = P[3*iZRSeq[i]+j]; // ( P[3*iZRSeq[i]+j] + P[3*(iZRSeq[i+4])+j] )/2.0;
            }
        }
    }
    else {
        nzrPoints = 6;
        Int_t iZRSeq[6] = {1,2,3,7,6,5};
        for (Int_t i = 0; i < nzrPoints; i++) {
            for (Int_t j = 0; j < 3; j++) {
                Pzr[3*i+j] = P[3*iZRSeq[i]+j];
            }
        }
    }

    fStripZR      = new BesPolygon2D(name, title, nzrPoints, &Pzr[0]);

    // Init

    SetStyle();

    fTip = new BesPaveText(0, 0, 0.1, 0.1);
    CloseInfo();
}

//_____________________________________________________

Muc2DStrip::~Muc2DStrip() {
    //Long Peixun's update: remove "if"
    delete fStripXY;
    delete fStripZR;
}

void Muc2DStrip::SetStyle()
{
    lcStrip = 15;
    lwStrip = 1;
    lsStrip = 1;
    fcStrip = 1007; // lightBrown // 1005;  // lightBlue
    fsStrip = 1001;

    lcStripFired = 15; //kRed; //15
    lwStripFired = 1;
    lsStripFired = 1;
    fcStripFired = kRed;
    fsStripFired = 1001;

    lcStripHL = lcStrip;
    lwStripHL = 1;
    lsStripHL = 1;
    fcStripHL = kBlue;
    fsStripHL = 1001;

}

void Muc2DStrip::ClearInfo()
{
    fInfoCon.clear();
    fTip->DeleteText();
}

void Muc2DStrip::CloseInfo()
{
    if (fTip) {
        fTip->Clear();

        if (fInfoCon.size() == 0) fInfoCon.push_back(GetTitle());
        fTip->SetText(fInfoCon);
    }
    else
        cout << "Muc2DStrip::CloseInfo, not initialized" << endl;
}

//_____________________________________________________

Int_t Muc2DStrip::DistancetoPrimitive(Int_t px, Int_t py) {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if ( view->GetViewType() & kXYView && fStripXY &&
            ( view->GetVisMucStrips()  || (view->GetVisMucHitsGlobal() && this->IsFired()) )  ) {
        return fStripXY->DistancetoPrimitive(px, py);
    }
    else if ( view->GetViewType() & kZRView && fStripZR &&
              ( (view->GetVisMucStrips() && this->IsZRVisible()) ||
                (view->GetVisMucHitsGlobal() && this->IsFired() && this->IsZRVisible()) )  ) {
        return fStripZR->DistancetoPrimitive(px, py);
    }

    return 9999;
}

//_____________________________________________________

void Muc2DStrip::ExecuteEvent(Int_t event, Int_t px, Int_t py) {

    //cout << GetName() << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (gBesCursor->GetType() == kBesHand) {
        if (view) view->ExecuteEvent(event, px, py);
    }
    else if (gBesCursor->GetType() == kBesPick){

        if (gPad) gPad->SetCursor(kPointer);

        switch (event) {

        case kMouseEnter :
            this->SetHighlighted(true);

            Draw();

            fTip->SetPos(px, py);
            view->UpdateView(0);

            //Long Peixun's update: If ShowInfo is not checked, don't show tip text.
            if (gBesCursor->GetShowInfo()) fTip->Draw("BR,SAME"); // "BR,ARC,SAME"
            gPad->Modified();
            gPad->Update();

            break;

        case kMouseLeave:
            if (this->IsHighlighted()) {
                this->SetHighlighted(false);
                //if (view->GetVisMucHitsGlobal())
                Draw();

                view->UpdateView(0);
                gPad->Modified();
                gPad->Update();
                break;
            }
        }

        //if (view) view->ExecuteEvent(event, px, py);
    }
}

//_____________________________________________________

void Muc2DStrip::Draw(Option_t *option) {
    //
    // Muc2DStrip draw function
    TString opt = option;
    opt.ToUpper();

    AppendPad(option);
}

//_____________________________________________________

void Muc2DStrip::Paint(Option_t *option) {

    TString opt = option;
    opt.ToUpper();

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    Double_t min[3], max[3];
    view->GetRange(&min[0], &max[0]);
    Double_t maxRange = 0.0;
    Double_t range[3];
    for (Int_t i = 0; i < 3; i++) {
        range[i] = fabs(max[i] - min[i]);
        if (range[i] > maxRange) maxRange = range[i];
    }

    if (view->GetViewType() & kXYView) {

        if (fStripXY) {
            fStripXY->SetLineColor(lcStrip);
            fStripXY->SetLineWidth(lwStrip);
            fStripXY->SetLineStyle(lsStrip);
            fStripXY->SetFillColor(fcStrip);
            fStripXY->SetFillStyle(fsStrip);
        }

        if (this->IsFired()) {
            if (fStripXY) {
                fStripXY->SetLineColor(lcStripFired);
                if (maxRange > m_kDrawLineRange && fPart == 1)
                    fStripXY->SetLineColor(fcStripFired); // too small makes it hard to see
                fStripXY->SetLineWidth(lwStripFired);
                fStripXY->SetLineStyle(lsStripFired);
                fStripXY->SetFillColor(fcStripFired);
                fStripXY->SetFillStyle(fsStripFired);
            }
        }

        if (this->IsHighlighted()) {
            if (fStripXY) {
                fStripXY->SetLineColor(lcStripHL);
                if (maxRange > m_kDrawLineRange && fPart == 1)
                    fStripXY->SetLineColor(fcStripHL); // too small makes it hard to see
                fStripXY->SetLineWidth(lwStripHL);
                fStripXY->SetLineStyle(lsStripHL);
                fStripXY->SetFillColor(fcStripHL);
                fStripXY->SetFillStyle(fsStripHL);
            }
        }

        if ( fStripXY && view->GetVisMucStrips() ) fStripXY->Paint();
        if (this->IsFired()) {
            if (view->GetVisMucHitsGlobal() &&
                    ( (fPart == 0 && view->GetVisMucHitsEast())   ||
                      (fPart == 1 && view->GetVisMucHitsBarrel()) ||
                      (fPart == 2 && view->GetVisMucHitsWest()) )) {
                fStripXY->Paint();
            }
        }
    }

    if (view->GetViewType() & kZRView) {

        if (fStripZR) {
            fStripZR->SetLineColor(lcStrip);
            fStripZR->SetLineWidth(lwStrip);
            fStripZR->SetLineStyle(lsStrip);
            fStripZR->SetFillColor(fcStrip);
            fStripZR->SetFillStyle(fsStrip);
        }

        if (this->IsFired()) {
            if (fStripZR) {
                fStripZR->SetLineColor(lcStripFired);
                if (maxRange > m_kDrawLineRange)
                    fStripZR->SetLineColor(fcStripFired); // too small makes it hard to see
                fStripZR->SetLineWidth(lwStripFired);
                fStripZR->SetLineStyle(lsStripFired);
                fStripZR->SetFillColor(fcStripFired);
                fStripZR->SetFillStyle(fsStripFired);
            }
        }

        if (this->IsHighlighted()) {
            if (fStripZR) {
                fStripZR->SetLineColor(lcStripHL);
                if (maxRange > m_kDrawLineRange)
                    fStripZR->SetLineColor(fcStripHL); // too small makes it hard to see
                fStripZR->SetLineWidth(lwStripHL);
                fStripZR->SetLineStyle(lsStripHL);
                fStripZR->SetFillColor(fcStripHL);
                fStripZR->SetFillStyle(fsStripHL);
            }
        }

        if ( fStripZR && view->GetVisMucStrips() && IsZRVisible() ) fStripZR->Paint();
        if (this->IsFired() && IsZRVisible() ) {
            if (view->GetVisMucHitsGlobal() &&
                    ( (fPart == 0 && view->GetVisMucHitsEast())   ||
                      (fPart == 1 && view->GetVisMucHitsBarrel()) ||
                      (fPart == 2 && view->GetVisMucHitsWest()) )) {
                fStripZR->Paint();
            }
        }
    }
}

//_____________________________________________________

char *Muc2DStrip::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

//_____________________________________________________

Bool_t Muc2DStrip::IsZRVisible()
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    Double_t viewPhi = view->GetLongitude();
    viewPhi = Range360(viewPhi);

    if (fPart != 1) {
        if ( (Int_t)viewPhi % 90 == 0 ) return true;
        else return false;
    }
    else {
        Int_t viewSeg = Int_t(viewPhi/45.0+0.5);
        //cout << "viewSeg " << viewSeg << endl;
        Int_t upSeg   = viewSeg-2;
        Int_t downSeg = viewSeg+2;
        if (upSeg   < 0) upSeg   += 8;
        else if (upSeg   >= 8) upSeg   -= 8;
        if (downSeg < 0) downSeg += 8;
        else if (downSeg >= 8) downSeg -= 8;
        //if (fStrip == 0) cout << GetName() << " upSeg " << upSeg << " downSeg " << downSeg << endl;

        if ( (Int_t)viewPhi % 45 == 0 && (fSeg == upSeg || fSeg == downSeg) ) return true;
        else return false;
    }

    return false;
}

//_____________________________________________________

Double_t Muc2DStrip::GetAngle(Double_t x, Double_t y)  //degree
{
    Double_t angle = TMath::ACos( x/TMath::Sqrt(x*x+y*y) ) * TMath::RadToDeg();
    if ( y<0.0 ) angle *= -1;
    angle = Range360(angle);

    return angle;
}

//_____________________________________________________

Double_t Muc2DStrip::Range360(Double_t input)
{
    if (input >= 360.0) {
        do {
            input -= 360.0;
        }
        while (input >= 360.0);
    }
    else if (input < 0.0) {
        do {
            input += 360.0;
        }
        while (input < 0.0);
    }

    return input;
}
