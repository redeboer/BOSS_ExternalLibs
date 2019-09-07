#include <TString.h>
#include <TColor.h>
#include <iostream>
#include "TMath.h"
#include "math.h"

#include "BesVisLib/Emc2DCrystal.h"
#include "BesVisLib/BesView.h"
#include "BesVisLib/BesCursor.h"

using namespace std;

#ifndef __CINT__
ClassImp(Emc2DCrystal)
#endif

//_____________________________________________________

Emc2DCrystal::Emc2DCrystal() {

}

//_____________________________________________________

Emc2DCrystal::Emc2DCrystal(const char* name, const char* title, Int_t N, Double_t *P, Int_t part, Int_t theta)
{
    SetName(name);
    SetTitle(title);
    fTip = 0;
    fHighlighted = false;
    fFired = false;
    fPart  = part;
    fTheta = theta;

    fTime      = 0.0;
    fCharge    = 0.0;

    fZRSectionTolerance[1] = 0.1;  // degree
    fZRSectionTolerance[2] = fZRSectionTolerance[0] = 1.2;   // degree

    // ------XY------

    Double_t Pxy[3*5]; // 4 side and 5 side shape in Emc End crystal
    if (N == 8) {  // 4 side
        Int_t iXYSeq[4] = {0,2,6,4};
        for (Int_t i = 0; i < 4; i++) {
            if (part == 1) {
                for (Int_t j = 0; j < 3; j++) {
                    Pxy[3*i+j] = ( P[3*iXYSeq[i]+j] + P[3*(iXYSeq[i]+1)+j] )/2.0;
                }
            }
            else if (part == 0) {
                SetXYPoint(&P[3*i], &P[3*(i+4)], &Pxy[3*i], ecXYPointZ);
                //for (Int_t j = 0; j < 3; j++) Pxy[3*i+j] = P[3*i+j];
            }
            else if (part == 2) {
                SetXYPoint(&P[3*i], &P[3*(i+4)], &Pxy[3*i], -ecXYPointZ);
                //for (Int_t j = 0; j < 3; j++) Pxy[3*i+j] = P[3*i+j];
            }
            //cout << GetName() << endl;
            //if (GetName() == TString("EastEcTheta5Phi0")) cout << Pxy[3*i] << " " << Pxy[3*i+1] << " " << Pxy[3*i+2] << endl;
        }

        fCrystalXY      = new BesPolygon2D(name, title, 4, &Pxy[0]);
        fCrystalXYFired = new BesPolygon2D(name, title, 4, &Pxy[0]);
    }
    if (N == 16) {  // 5 side
        Int_t iXYSeq[5] = {8,9,1,2,3};            //  9 10,1 2
        for (Int_t i = 0; i < 5; i++) {           //  8 11,0 3
            if (part == 0) {
                SetXYPoint(&P[3*iXYSeq[i]], &P[3*(iXYSeq[i]+4)], &Pxy[3*i], ecXYPointZ);
            }
            else if (part == 2) {
                SetXYPoint(&P[3*iXYSeq[i]], &P[3*(iXYSeq[i]+4)], &Pxy[3*i], -ecXYPointZ);
            }
            //for (Int_t j = 0; j < 3; j++) Pxy[3*i+j] = P[3*iXYSeq[i]+j];
        }

        fCrystalXY      = new BesPolygon2D(name, title, 5, &Pxy[0]);
        fCrystalXYFired = new BesPolygon2D(name, title, 5, &Pxy[0]);
    }

    // ------ZR------

    Double_t Pzr[3*4];
    Int_t iSeq[8] = {0,3,1,2,5,6,4,7}; // center of 0+3, 1+2, 5+6, 4+7
    for (Int_t i = 0; i < 4; i++) {
        for (Int_t j = 0; j < 3; j++) {
            Pzr[3*i+j] = (P[3*iSeq[2*i]+j] + P[3*iSeq[2*i+1]+j])/2.0;
        }
        //if (GetName() == TString("EastEcCrystal0")) cout << Pxy[i] << endl;
    }


    fCrystalZR      = new BesPolygon2D(TString(name)+TString("zr"), title, 4, &Pzr[0]);
    fCrystalZRFired = new BesPolygon2D(TString(name)+TString("zr"), title, 4, &Pzr[0]);

    if (N == 8) {
        if (GetPart() == 1) {
            fPhiMin = GetAngle( P[3*0], P[3*0+1] );
            fPhiMax = GetAngle( P[3*3], P[3*3+1] );
        }
        else {
            fPhiMin = GetAngle( P[3*3], P[3*3+1] );
            fPhiMax = GetAngle( P[3*0], P[3*0+1] );
        }
    }
    else if (N == 16) {
        fPhiMin = GetAngle( P[3*3], P[3*3+1] );
        fPhiMax = GetAngle( P[3*8], P[3*8+1] );
    }

    //Long Peixun's update: Remove fabs()
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

    // ------Side------

    Double_t Pside[3*5]; // 4 side and 5 side shape in Emc End crystal
    if (N == 8) {  // 4 side
        //Int_t iSideSeq[4] = {0,1,2,3};
        for (Int_t i = 0; i < 4; i++) {
            if (part == 1) {
                SetZRPoint(&P[3*i], &P[3*(i+4)], &Pside[3*i], brZRPointR);
            }
            else {
                ProjectECToSide(&Pxy[3*i], &Pside[3*i]);
            }
        }

        fCrystalSide      = new BesPolygon2D(name, title, 4, &Pside[0]);
        fCrystalSideFired = new BesPolygon2D(name, title, 4, &Pside[0]);
    }
    if (N == 16) {  // 5 side
        for (Int_t i = 0; i < 5; i++) {
            ProjectECToSide(&Pxy[3*i], &Pside[3*i]);
        }

        fCrystalSide      = new BesPolygon2D(name, title, 5, &Pside[0]);
        fCrystalSideFired = new BesPolygon2D(name, title, 5, &Pside[0]);
    }

    // Init

    SetStyle();

    fTip = new BesPaveText(0, 0, 0.45, 0.08);
    CloseInfo();
}

//_____________________________________________________

Emc2DCrystal::~Emc2DCrystal() {
    //Long Peixun's update: remove "if"
    delete fCrystalXY;
    delete fCrystalZR;
    delete fCrystalXYFired;
    delete fCrystalZRFired;
}

void Emc2DCrystal::SetStyle()
{
    lcCrystal = 15;
    lwCrystal = 1;
    fcCrystal = 1005;  // lightBlue
    fsCrystal = 1001;
    fsCrystalTrans = 4000;

    lcCrystalFired = kRed;
    lwCrystalFired = 1;
    fcCrystalFired = kRed;
    fsCrystalFired = 1001;

    lcCrystalHL = lcCrystal;
    lwCrystalHL = 1;
    fcCrystalHL = kBlue;
    fsCrystalHL = 1001;

    lcCrystalFiredHL = kRed;
    lwCrystalFiredHL = 2;
    fcCrystalFiredHL = kWhite;
    fsCrystalFiredHL = 4000;
}

void Emc2DCrystal::ClearInfo()
{
    fInfoCon.clear();
    fTip->DeleteText();
}

void Emc2DCrystal::CloseInfo()
{
    if (fTip) {
        fTip->Clear();

        if (fInfoCon.size() == 0) fInfoCon.push_back(GetTitle());
        fTip->SetText(fInfoCon);
    }
    else
        cout << "Emc2DCrystal::CloseInfo, not initialized" << endl;
}

//_____________________________________________________

Int_t Emc2DCrystal::DistancetoPrimitive(Int_t px, Int_t py) {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if ( view->GetViewType() & kXYView ) {
        if ( fCrystalXY && IsXYVisible() ) {
            return fCrystalXY->DistancetoPrimitive(px, py);
        }
    }
    else if ( view->GetViewType() & kZRView && fCrystalZR && fCrystalSide ) {
        if (HasZRSection())
            return fCrystalZR->DistancetoPrimitive(px, py);
        else if ( IsSideVisible() &&
                  (view->GetVisEmcSide() || (view->GetVisEmcHitsSide() && IsFired())) )
            return fCrystalSide->DistancetoPrimitive(px, py);
    }

    return 9999;
}

//_____________________________________________________

void Emc2DCrystal::ExecuteEvent(Int_t event, Int_t px, Int_t py) {

    //if (fWireType == 2 && fWireBubble[0] ) {
    //if (fWireCone->GetLineWidth() > 1) // FIRED
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

            //if (this->IsFired() || view->GetVisEmcHitsGlobal()) {
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
                //if (view->GetVisEmcHitsGlobal())
                Draw();

                /*
                  if (fTip) {
                  delete fTip;
                  fTip = 0;
                }
                */

                view->UpdateView(0);
                gPad->Modified();
                gPad->Update();
                break;
            }
        }
    }
}

//_____________________________________________________

void Emc2DCrystal::Draw(Option_t *option) {
    //
    // Emc2DCrystal draw function
    TString opt = option;
    opt.ToUpper();

    AppendPad(option);
}

//_____________________________________________________

void Emc2DCrystal::Paint(Option_t *option) {

    TString opt = option;
    opt.ToUpper();

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view->GetViewType() & kXYView) {

        if (fCrystalXY) {
            fCrystalXY->SetLineColor(lcCrystal);
            fCrystalXY->SetLineWidth(lwCrystal);
            fCrystalXY->SetFillColor(fcCrystal);
            fCrystalXY->SetFillStyle(fsCrystal);
        }

        if (this->IsFired()) {
            if (fCrystalXY) {
                fCrystalXY->SetFillStyle(fsCrystalTrans);
            }
            if (fCrystalXYFired) {
                fCrystalXYFired->SetLineColor(lcCrystalFired);
                fCrystalXYFired->SetLineWidth(lwCrystalFired);
                fCrystalXYFired->SetFillColor(fcCrystalFired);
                fCrystalXYFired->SetFillStyle(fsCrystalFired);
            }
        }

        if (this->IsHighlighted()) {
            if (fCrystalXY) {
                fCrystalXY->SetLineColor(lcCrystalHL);
                fCrystalXY->SetLineWidth(lwCrystalHL);
                fCrystalXY->SetFillColor(fcCrystalHL);
                fCrystalXY->SetFillStyle(fsCrystalHL);
            }
            if (fCrystalXYFired) {
                fCrystalXYFired->SetLineColor(lcCrystalFiredHL);
                fCrystalXYFired->SetLineWidth(lwCrystalFiredHL);
                fCrystalXYFired->SetFillColor(fcCrystalFiredHL);
                fCrystalXYFired->SetFillStyle(fsCrystalFiredHL);
            }
        }

        if ( fCrystalXY && IsXYVisible() ) fCrystalXY->Paint();
        if (this->IsFired()) {
            if (view->GetVisEmcHitsGlobal() &&
                    ( (fPart == 0 && view->GetVisEmcHitsEast())   ||
                      (fPart == 1 && view->GetVisEmcHitsBarrel()) ||
                      (fPart == 2 && view->GetVisEmcHitsWest()) )) {
                if (fCrystalXYFired) {
                    fCrystalXY->Paint();
                    //if (this->IsHighlighted()) cout << GetName() << " is highlighted " << endl;
                    //cout << "Emc fired Crystal size " << fCharge/fChargeMax << endl;
                    fCrystalXYFired->Restore(); // restore default size
                    fCrystalXYFired->SetSize(fCharge/fChargeMax);
                    fCrystalXYFired->Paint();
                }
            }
        }
    }

    if (view->GetViewType() & kZRView) {
        if (HasZRSection()) {
            if (this->IsFired()) {
                if (fCrystalZRFired) {
                    fCrystalZRFired->SetLineColor(lcCrystalFired);
                    fCrystalZRFired->SetLineWidth(lwCrystalFired);
                    fCrystalZRFired->SetFillColor(fcCrystalFired);
                    fCrystalZRFired->SetFillStyle(fsCrystalFired);
                }
            }

            if (fCrystalZR) {
                fCrystalZR->SetLineColor(lcCrystal);
                fCrystalZR->SetLineWidth(lwCrystal);
                fCrystalZR->SetFillColor(fcCrystal);
                fCrystalZR->SetFillStyle(fsCrystal);
            }

            if (this->IsHighlighted()) {
                if (fCrystalZR) {
                    fCrystalZR->SetLineColor(lcCrystalHL);
                    fCrystalZR->SetLineWidth(lwCrystalHL);
                    fCrystalZR->SetFillColor(fcCrystalHL);
                    fCrystalZR->SetFillStyle(fsCrystalHL);
                }
                if (fCrystalZRFired) {
                    fCrystalZRFired->SetLineColor(lcCrystalFiredHL);
                    fCrystalZRFired->SetLineWidth(lwCrystalFiredHL);
                    fCrystalZRFired->SetFillColor(fcCrystalFiredHL);
                    fCrystalZRFired->SetFillStyle(fsCrystalFiredHL);
                }
            }

            if ( fCrystalZR ) fCrystalZR->Paint();
            if (this->IsFired()) {
                if (view->GetVisEmcHitsGlobal() &&
                        ( (fPart == 0 && view->GetVisEmcHitsEast())   ||
                          (fPart == 1 && view->GetVisEmcHitsBarrel()) ||
                          (fPart == 2 && view->GetVisEmcHitsWest()) )) {
                    if (fCrystalZRFired) {
                        //cout << "Emc fired Crystal size " << fCharge/fChargeMax << endl;
                        fCrystalZRFired->Restore(); // restore default size
                        fCrystalZRFired->SetSize(fCharge/fChargeMax);
                        fCrystalZRFired->Paint();
                    }
                }
            }
        }

        // side
        if (this->IsFired()) {
            if (fCrystalSideFired) {
                fCrystalSideFired->SetLineColor(lcCrystalFired);
                fCrystalSideFired->SetLineWidth(lwCrystalFired);
                fCrystalSideFired->SetFillColor(fcCrystalFired);
                fCrystalSideFired->SetFillStyle(fsCrystalFired);
            }
        }

        if (fCrystalSide) {
            fCrystalSide->SetLineColor(lcCrystal);
            fCrystalSide->SetLineWidth(lwCrystal);
            fCrystalSide->SetFillColor(fcCrystal);
            fCrystalSide->SetFillStyle(fsCrystal);
        }

        if (this->IsHighlighted()) {
            if (fCrystalSide) {
                fCrystalSide->SetLineColor(lcCrystalHL);
                fCrystalSide->SetLineWidth(lwCrystalHL);
                fCrystalSide->SetFillColor(fcCrystalHL);
                fCrystalSide->SetFillStyle(fsCrystalHL);
            }
            if (fCrystalSideFired) {
                fCrystalSideFired->SetLineColor(lcCrystalFiredHL);
                fCrystalSideFired->SetLineWidth(lwCrystalFiredHL);
                fCrystalSideFired->SetFillColor(fcCrystalFiredHL);
                fCrystalSideFired->SetFillStyle(fsCrystalFiredHL);
            }
        }

        if ( IsSideVisible() ) {
            if ( (view->GetVisEmcSide() || view->GetVisEmcHitsSide() && this->IsFired()) && fCrystalSide ) fCrystalSide->Paint();
            if ( this->IsFired() ) {
                if (view->GetVisEmcHitsGlobal() && view->GetVisEmcHitsSide()) {
                    if (fCrystalSideFired) {
                        //cout << "Emc fired Crystal size " << fCharge/fChargeMax << endl;
                        fCrystalSideFired->Restore(); // restore default size
                        fCrystalSideFired->SetSize(fCharge/fChargeMax);
                        fCrystalSideFired->Paint();
                    }
                }
            }
        }
    }
}

//_____________________________________________________

char *Emc2DCrystal::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

//_____________________________________________________

bool Emc2DCrystal::HasZRSection()
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
            //cout << GetName() << " zrPhi " << zrPhi << " min " << fPhiMin << " max " << fPhiMax << endl;
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

Double_t Emc2DCrystal::GetAngle(Double_t x, Double_t y)  //degree
{
    Double_t angle = TMath::ACos( x/TMath::Sqrt(x*x+y*y) ) * TMath::RadToDeg();
    if ( y<0.0 ) angle *= -1;
    angle = Range360(angle);

    return angle;
}

//_____________________________________________________

Double_t Emc2DCrystal::Range360(Double_t input)
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

bool Emc2DCrystal::IsXYVisible()
{
    if ( GetPart() != 1 || GetTheta() == 0 || IsFired() )
        return true;
    else
        return false;
}

bool Emc2DCrystal::IsSideVisible()
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    Double_t viewPhi = view->GetLongitude();
    Double_t viewPhiMin = viewPhi - sideDeltaPhi;
    Double_t viewPhiMax = viewPhi + sideDeltaPhi;

    viewPhi    = Range360(viewPhi);
    viewPhiMin = Range360(viewPhiMin);
    viewPhiMax = Range360(viewPhiMax);
    //if (GetName() == TString("Layer0Wire0")) cout << "view " << viewPhi << endl;

    if (viewPhiMin < viewPhiMax) { // not cross 0 degree
        if (fPhiMax > viewPhiMin && fPhiMax < viewPhiMax) return true;
    }
    else { // cross 0 degree
        if (fPhiMax > viewPhiMin || fPhiMax < viewPhiMax) return true;
    }

    return false;
}

// the point newP is on the line connecting p1 and p2, cut point by the plane Z=z

void Emc2DCrystal::SetXYPoint(Double_t *p1, Double_t *p2, Double_t *newP, Double_t z)
{
    Double_t ratio = fabs( (z-p1[2]) / (p2[2]-p1[2]) );
    for (Int_t i = 0; i < 3; i++) {
        newP[i] = p1[i] * (1.0-ratio) + p2[i] * ratio;
    }
}

// the point newP is on the line connecting p1 and p2, cut point by the plane R=r

void Emc2DCrystal::SetZRPoint(Double_t *p1, Double_t *p2, Double_t *newP, Double_t r)
{
    Double_t r1 = sqrt(p1[0]*p1[0] + p1[1]*p1[1]);
    Double_t r2 = sqrt(p2[0]*p2[0] + p2[1]*p2[1]);

    Double_t ratio = fabs( (r-r1) / (r2-r1) );
    for (Int_t i = 0; i < 3; i++) {
        newP[i] = p1[i] * (1.0-ratio) + p2[i] * ratio;
    }
}

void Emc2DCrystal::ProjectECToSide(Double_t *p1, Double_t *newP)
{
    //  ------ R         ---------
    //       |     -->        |
    //       | r              |

    Double_t r1 = sqrt(p1[0]*p1[0] + p1[1]*p1[1]);
    newP[0] = p1[0] * (brZRPointR/r1);
    newP[1] = p1[1] * (brZRPointR/r1);

    Double_t drToR = brZRPointR - r1;
    newP[2] = fabs(ecXYPointZ + drToR) * (p1[2]/fabs(p1[2]));


    //if (GetName() == TString("EastEcTheta0Phi0")) {
    //cout << p1[0] << " " << p1[1] << " " << p1[2] << endl;
    //cout << newP[0] << " " << newP[1] << " " << newP[2] << endl;
    //}

}
