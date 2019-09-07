#include <TString.h>
#include <TColor.h>
#include <iostream>

#include "math.h"
#include "TMath.h"
#include "BesVisLib/Tof2DScin.h"
#include "BesVisLib/BesView.h"
#include "BesVisLib/BesCursor.h"

using namespace std;

#ifndef __CINT__
ClassImp(Tof2DScin)
#endif

//_____________________________________________________

Tof2DScin::Tof2DScin() {

}

//_____________________________________________________

Tof2DScin::Tof2DScin(const char* name, const char* title, Int_t N, Double_t *P, Int_t part)
{
    SetName(name);
    SetTitle(title);
    fTip = 0;
    fHighlighted = false;
    fFired = false;
    fPart = part;

    fTime      = 0.0;
    fCharge    = 0.0;

    fZRSectionTolerance[1] = 0.071; // half of gap between two scin
    fZRSectionTolerance[2] = fZRSectionTolerance[0] = 0.1647;

    // ------XY------

    Double_t Pxy[3*4]; // N=8
    for (Int_t i = 0; i < 3*4; i++) {
        Pxy[i] = (P[i] + P[i+3*4])/2.0;
        //if (GetName() == TString("EastEcScin0")) cout << Pxy[i] << endl;
    }

    fScinXY      = new BesPolygon2D(name, title, 4, &Pxy[0]);
    fScinXYFired = new BesPolygon2D(name, title, 4, &Pxy[0]);

    // ------ZR------

    Double_t Pzr[3*4];
    Int_t iSeq[8] = {0,3,1,2,5,6,4,7}; // center of 0+3, 1+2, 5+6, 4+7
    for (Int_t i = 0; i < 4; i++) {
        for (Int_t j = 0; j < 3; j++) {
            Pzr[3*i+j] = (P[3*iSeq[2*i]+j] + P[3*iSeq[2*i+1]+j])/2.0;
        }
        //if (GetName() == TString("EastEcScin0")) cout << Pxy[i] << endl;
    }


    fScinZR      = new BesPolygon2D(TString(name)+TString("zr"), title, 4, &Pzr[0]);
    fScinZRFired = new BesPolygon2D(TString(name)+TString("zr"), title, 4, &Pzr[0]);

    fPhiMin = GetAngle( P[3*3], P[3*3+1] );
    fPhiMax = GetAngle( P[3*0], P[3*0+1] );
    //Huang Shuhui's update: Fix ZR view of Mrpc
    if (fPhiMin > fPhiMax && fPhiMin - fPhiMax < 180.0)
    { // not cross 0 degree
        Double_t phiTemp = fPhiMin;
        fPhiMin = fPhiMax;
        fPhiMax = phiTemp;
    }
    //Long Peixun's update: Consider crossing 0 degree
    if (fPhiMax > fPhiMin && fPhiMax - fPhiMin > 180.0)
    {
        Double_t phiTemp = fPhiMin;
        fPhiMin = fPhiMax;
        fPhiMax = phiTemp;
    }
    //cout << GetName() << " min "<< fPhiMin << " max " << fPhiMax << endl;

    SetStyle();

    fTip = new BesPaveText(0, 0, 0.1, 0.1);
    CloseInfo();
}

//_____________________________________________________

Tof2DScin::~Tof2DScin() {
    //Long Peixun's update: remove "if"
    delete fScinXY;
    delete fScinZR;
    delete fScinXYFired;
    delete fScinZRFired;
}

void Tof2DScin::SetStyle()
{
    lcScin = 15;
    lwScin = 1;
    fcScin = 1003;  // lightYellow
    fsScin = 1001;

    lcScinFired = kRed;
    lwScinFired = 1;
    fcScinFired = kRed;
    fsScinFired = 1001;

    lcScinHL = lcScin;
    lwScinHL = 1;
    fcScinHL = kBlue;
    fsScinHL = 1001;

    lcScinFiredHL = kRed;
    lwScinFiredHL = 2;
    fcScinFiredHL = kBlue;
    fsScinFiredHL = 4000;
}


void Tof2DScin::ClearInfo()
{
    fInfoCon.clear();
    fTip->DeleteText();
}

void Tof2DScin::CloseInfo()
{
    if (fTip) {
        fTip->Clear();

        if (fInfoCon.size() == 0) fInfoCon.push_back(GetTitle());
        fTip->SetText(fInfoCon);
    }
    else
        cout << "Tof2DScin::CloseInfo, not initialized" << endl;
}

//_____________________________________________________

Int_t Tof2DScin::DistancetoPrimitive(Int_t px, Int_t py) {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view->GetViewType() & kXYView && fScinXY) {
        return fScinXY->DistancetoPrimitive(px, py);
    }
    else if (view->GetViewType() & kZRView && fScinZR) {
        if (HasZRSection())
            return fScinZR->DistancetoPrimitive(px, py);
    }

    return 9999;
}

//_____________________________________________________

void Tof2DScin::ExecuteEvent(Int_t event, Int_t px, Int_t py) {

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

            //if (this->IsFired() || view->GetVisTofHitsGlobal()) {
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
                //if (view->GetVisTofHitsGlobal())
                Draw();

                view->UpdateView(0);
                gPad->Modified();
                gPad->Update();
                break;
            }
        }
    }
}

//_____________________________________________________

void Tof2DScin::Draw(Option_t *option) {
    //
    // Tof2DScin draw function
    TString opt = option;
    opt.ToUpper();

    AppendPad(option);
}

//_____________________________________________________

void Tof2DScin::Paint(Option_t *option) {

    TString opt = option;
    opt.ToUpper();

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view->GetViewType() & kXYView) {

        if (this->IsFired()) {
            if (fScinXYFired) {
                fScinXYFired->SetLineColor(lcScinFired);
                fScinXYFired->SetLineWidth(lwScinFired);
                fScinXYFired->SetFillColor(fcScinFired);
                fScinXYFired->SetFillStyle(fsScinFired);
            }
        }

        if (fScinXY) {
            fScinXY->SetLineColor(lcScin);
            fScinXY->SetLineWidth(lwScin);
            fScinXY->SetFillColor(fcScin);
            fScinXY->SetFillStyle(fsScin);
        }

        if (this->IsHighlighted()) {
            if (fScinXY) {
                fScinXY->SetLineColor(lcScinHL);
                fScinXY->SetLineWidth(lwScinHL);
                fScinXY->SetFillColor(fcScinHL);
                fScinXY->SetFillStyle(fsScinHL);
            }
            if (fScinXYFired) {
                fScinXYFired->SetLineColor(lcScinFiredHL);
                fScinXYFired->SetLineWidth(lwScinFiredHL);
                fScinXYFired->SetFillColor(fcScinFiredHL);
                fScinXYFired->SetFillStyle(fsScinFiredHL);
            }
        }

        if (fScinXY) fScinXY->Paint();
        if (this->IsFired()) {
            if (view->GetVisTofHitsGlobal() &&
                    ( (fPart == 0 && view->GetVisTofHitsEast())   ||
                      (fPart == 1 && view->GetVisTofHitsBarrel()) ||
                      (fPart == 2 && view->GetVisTofHitsWest()) )) {
                if (fScinXYFired) {
                    //cout << "Tof fired Scin size " << fCharge/fChargeMax << endl;
                    fScinXYFired->Restore(); // restore default size
                    fScinXYFired->SetSize(fCharge/fChargeMax);
                    fScinXYFired->Paint();
                }
            }
        }
    }

    if (view->GetViewType() & kZRView) {
        if (HasZRSection()) {
            if (this->IsFired()) {
                if (fScinZRFired) {
                    fScinZRFired->SetLineColor(lcScinFired);
                    fScinZRFired->SetLineWidth(lwScinFired);
                    fScinZRFired->SetFillColor(fcScinFired);
                    fScinZRFired->SetFillStyle(fsScinFired);
                }
            }

            if (fScinZR) {
                fScinZR->SetLineColor(lcScin);
                fScinZR->SetLineWidth(lwScin);
                fScinZR->SetFillColor(fcScin);
                fScinZR->SetFillStyle(fsScin);
            }

            if (this->IsHighlighted()) {
                if (fScinZR) {
                    fScinZR->SetLineColor(lcScinHL);
                    fScinZR->SetLineWidth(lwScinHL);
                    fScinZR->SetFillColor(fcScinHL);
                    fScinZR->SetFillStyle(fsScinHL);
                }
                if (fScinZRFired) {
                    fScinZRFired->SetLineColor(lcScinFiredHL);
                    fScinZRFired->SetLineWidth(lwScinFiredHL);
                    fScinZRFired->SetFillColor(fcScinFiredHL);
                    fScinZRFired->SetFillStyle(fsScinFiredHL);
                }
            }

            if (fScinZR) fScinZR->Paint();
            if (this->IsFired()) {
                if (view->GetVisTofHitsGlobal() &&
                        ( (fPart == 0 && view->GetVisTofHitsEast())   ||
                          (fPart == 1 && view->GetVisTofHitsBarrel()) ||
                          (fPart == 2 && view->GetVisTofHitsWest()) )) {
                    if (fScinZRFired) {
                        //cout << "Tof fired Scin size " << fCharge/fChargeMax << endl;
                        fScinZRFired->Restore(); // restore default size
                        fScinZRFired->SetSize(fCharge/fChargeMax);
                        fScinZRFired->Paint();
                    }
                }
            }
        }
    }

}

//_____________________________________________________

char *Tof2DScin::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

//_____________________________________________________

bool Tof2DScin::HasZRSection()
{
    bool flag = false;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    Double_t viewPhi = view->GetLongitude();
    viewPhi = Range360(viewPhi);
    //if (GetName() == TString("Layer0Wire0")) cout << "view " << viewPhi << endl;

    Double_t zrPhi = viewPhi-90.0;
    zrPhi = Range360(zrPhi);
    //if (GetName() == TString("Layer0Wire0")) cout << zrPhi << endl;

    for (Int_t i = 0; i < 2; i++) {
        if (i==1) {
            zrPhi += 180.0;
            zrPhi = Range360(zrPhi);
        }

        if (zrPhi >= fPhiMin-fZRSectionTolerance[GetPart()] &&
                zrPhi <= fPhiMax+fZRSectionTolerance[GetPart()]) {
            flag = true;
            break;
        }
        else if (fPhiMin > fPhiMax) { // cross 0 degree
            if (zrPhi >= fPhiMin-fZRSectionTolerance[GetPart()] ||
                    zrPhi <= fPhiMax+fZRSectionTolerance[GetPart()]) {
                flag = true;
                break;
            }
        }
    }

    return flag;
}

//_____________________________________________________

Double_t Tof2DScin::GetAngle(Double_t x, Double_t y)  //degree
{
    Double_t angle = TMath::ACos( x/TMath::Sqrt(x*x+y*y) ) * TMath::RadToDeg();
    if ( y<0.0 ) angle *= -1;
    angle = Range360(angle);

    return angle;
}

//_____________________________________________________

Double_t Tof2DScin::Range360(Double_t input)
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
