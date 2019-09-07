#include <TPad.h>
#include <TVector3.h>
#include <TGeoManager.h>
#include "math.h"
#include <TMath.h>

#include <iostream>

#include "BesVisLib/BesGeoTrack.h"
#include "BesVisLib/BesView.h"
#include "BesVisLib/BesMarker2D.h"
#include "BesVisLib/Mdc2DWire.h"
#include "BesVisLib/Muc2DStrip.h"
#include "BesVisLib/BesCursor.h"

using namespace std;

#ifndef __CINT__
ClassImp(BesGeoTrack)
#endif
//int BesGeoTrack::count = 0;
BesGeoTrack::BesGeoTrack()
        : TGeoTrack()
{
//  std::cout << "co BesGeoTrack: " << ++count << std::endl;
    Init();
}

BesGeoTrack::BesGeoTrack(Int_t id, Int_t pdgcode, TVirtualGeoTrack *parent, TObject *particle)
        : TGeoTrack(id, pdgcode, parent, particle)
{
//  std::cout << "co BesGeoTrack: " << ++count << std::endl;
    Init();
}

BesGeoTrack::~BesGeoTrack()
{
//  std::cout << "de BesGeoTrack: " << --count << std::endl;
    //Long Peixun's update: remove "if"
    delete fTip;
    delete fPolyLine3D;
    delete fMarker;
}

void BesGeoTrack::Init()
{
    fTip = new BesPaveText(0, 0, 0.1, 0.1, "TL");
    fPolyLine3D = 0;    //Long Peixun's update
    fMarker = 0;

    //fHits.Clear("C");
}

void BesGeoTrack::CloseInfo()
{
    if (fTip) {
        fTip->SetText(fInfoCon);
    }
    else
        cout << "BesGeoTrack::CloseInfo, not initialized" << endl;
}

void BesGeoTrack::AddPoint(Double_t x, Double_t y, Double_t z, Double_t t)
{
    TGeoTrack::AddPoint(x,y,z,t);
}

void BesGeoTrack::SetMarker(Double_t x, Double_t y, Double_t z)
{
    fMarker = new BesMarker2D("RecMarker", "RecMarker", x, y, z);

    if (fType == 1) { // tof
        fMarker->SetMarkerSizeMultiple(15);     //Long Peixun's update: 20 -> 15
        fMarker->SetMarkerStyle(5);
        fMarker->SetMarkerColor(kBlack);
    }
    else if (fType == 2) { // emc
        fMarker->SetMarkerSizeMultiple(15);     //Long Peixun's update: 20 -> 15
        fMarker->SetMarkerStyle(2);
        fMarker->SetMarkerColor(kBlack);
    }
    else if (fType == 6)    //Long Peixun's update: cluster in CGEM view
    {
        fMarker->SetMarkerSizeMultiple(1.1);
        fMarker->SetMarkerStyle(4);
        fMarker->SetMarkerColor(kBlack);
    }
    else if (fType == 7)    //Long Peixun's update: cluster in CGEM UF view
    {
        fMarker->SetMarkerSizeMultiple(1.2);
        fMarker->SetMarkerStyle(4);
        fMarker->SetMarkerColor(kBlack);
    }
}

Int_t BesGeoTrack::DistancetoPrimitive(Int_t px, Int_t py)
{
    Int_t dist = 9999;
    Int_t cx, cy;
    Double_t edge = 4.0; // edge width of track
    Double_t markSizeRatio = 4.0;  // 4.0 is approximate size.

    if (fType == 1 || fType == 2 || fType == 6 || fType == 7) { // tof or emc or cgem
        if (!fMarker) return dist;
        fMarker->GetCenterPixel(cx, cy);
        if (fType == 1) { // tof
            Double_t r = sqrt( Double_t((px-cx)*(px-cx) + (py-cy)*(py-cy)) );
            Double_t markerR = markSizeRatio*fMarker->GetMarkerSize();
            if (markerR > 50) markerR = 50;  
	    // if the size of a marker with type of circle is too large > 50 pixel, it could not be enlarged.
            if ( fabs(r-markerR) < edge ) dist = 0;
        }
        //Long Peixun's update: consider cluster in CGEM view
        else if (fType == 2 || fType == 6 || fType == 7) { // emc
            Double_t dx = abs(px-cx), dy = abs(py-cy);
            if ( fabs(dx) < edge && fabs(dy) < markSizeRatio*fMarker->GetMarkerSize() ||
                    fabs(dy) < edge && fabs(dx) < markSizeRatio*fMarker->GetMarkerSize() )
                dist = 0;
        }

        return dist;
    }
    else return TGeoTrack::DistancetoPrimitive(px, py);
}

void BesGeoTrack::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (gBesCursor->GetType() == kBesHand) {
        if (view) view->ExecuteEvent(event, px, py);
    }
    else if (gBesCursor->GetType() == kBesPick){

        if (gPad) gPad->SetCursor(kPointer);

        Int_t width, color;
        Int_t normalWidth = 1, hlWidth = 2;

        switch (event) {
        case kMouseEnter:
            if (fType == 0) {
                if (view->GetVisMdcHitsGlobal()) {
                    for (Int_t i = 0; i < fHits.GetEntries(); i++) {
                        Mdc2DWire *aHit = (Mdc2DWire*)fHits.At(i);
                        if (aHit != 0) {
                            if (view->GetVisMdcHits()) {
                                aHit->SetHighlighted(true);
                            }
                        }
                    }
                }
            }
            else if (fType == 1) {
                if (view->GetVisTofHitsGlobal()) {
                    for (Int_t i = 0; i < fHits.GetEntries(); i++) {
                        Tof2DScin *aHit = (Tof2DScin*)fHits.At(i);
                        if (aHit != 0) {
                            if ( aHit->GetPart() == 0 && view->GetVisTofHitsEast() ||
                                    aHit->GetPart() == 1 && view->GetVisTofHitsBarrel() ||
                                    aHit->GetPart() == 2 && view->GetVisTofHitsWest() ) {
                                aHit->SetHighlighted(true);
                            }
                        }
                    }
                }
            }
            else if (fType == 2) {
                if (view->GetVisEmcHitsGlobal()) {
                    for (Int_t i = 0; i < fHits.GetEntries(); i++) {
                        Emc2DCrystal *aHit = (Emc2DCrystal*)fHits.At(i);
                        if (aHit != 0) {
                            if ( aHit->GetPart() == 0 && view->GetVisEmcHitsEast() ||
                                    aHit->GetPart() == 1 && view->GetVisEmcHitsBarrel() ||
                                    aHit->GetPart() == 2 && view->GetVisEmcHitsWest() ) {
                                aHit->SetHighlighted(true);
                            }
                        }
                    }
                }
            }
            else if (fType == 3) {
                if (view->GetVisMucHitsGlobal()) {
                    for (Int_t i = 0; i < fHits.GetEntries(); i++) {
                        Muc2DStrip *aHit = (Muc2DStrip*)fHits.At(i);
                        if (aHit != 0) {
                            if (view->GetVisMucHitsGlobal()) {
                                aHit->SetHighlighted(true);
                            }
                        }
                    }
                }
            }


            if (fType == 0) {
                width = this->GetLineWidth();
                color = this->GetLineColor();
                this->SetLineWidth(hlWidth);
                this->SetLineColor(kRed);
            }
            else if (fType == 1) {
                fMarker->SetMarkerColor(kMagenta);
            }
            else if (fType == 2) {
                fMarker->SetMarkerColor(kMagenta);
            }
            else if (fType == 3) {
                width = this->GetLineWidth();
                color = this->GetLineColor();
                this->SetLineWidth(hlWidth);
                this->SetLineColor(kRed);
            }
            else if (fType == 4) {
                this->SetLineWidth(hlWidth);
                this->SetLineColor(kBlue);
            }
            else if (fType == 6 || fType == 7)  //Long Peixun's update: for clusters in CGEM
            {
                fMarker->SetMarkerColor(kViolet);
                fMarker->SetMarkerStyle(8);
            }

            fTip->SetPos(px, py);
            view->UpdateView(0);

	    //yzhang 2011-11-15 TEMP
	    //drawHeader(fTip->GetText);
            if (gBesCursor->GetShowInfo()) fTip->Draw("TL,SAME"); // "BR,ARC,SAME"
            gPad->Modified();
            gPad->Update();
            break;

        case kMouseLeave:
            if (fType == 0) {
                for (Int_t i = 0; i < (Int_t)fHits.GetEntries(); i++) {
                    Mdc2DWire *aHit = (Mdc2DWire*)fHits.At(i);
                    if (aHit != 0 && aHit->IsHighlighted()) {
                        aHit->SetHighlighted(false);
                    }
                }
            }
            else if (fType == 1) {
                for (Int_t i = 0; i < fHits.GetEntries(); i++) {
                    Tof2DScin *aHit = (Tof2DScin*)fHits.At(i);
                    if (aHit != 0 && aHit->IsHighlighted()) {
                        aHit->SetHighlighted(false);
                    }
                }
            }
            else if (fType == 2) {
                for (Int_t i = 0; i < fHits.GetEntries(); i++) {
                    Emc2DCrystal *aHit = (Emc2DCrystal*)fHits.At(i);
                    if (aHit != 0 && aHit->IsHighlighted()) {
                        aHit->SetHighlighted(false);
                    }
                }
            }
            else if (fType == 3) {
                for (Int_t i = 0; i < (Int_t)fHits.GetEntries(); i++) {
                    Muc2DStrip *aHit = (Muc2DStrip*)fHits.At(i);
                    if (aHit != 0 && aHit->IsHighlighted()) {
                        aHit->SetHighlighted(false);
                    }
                }
            }


            if (fType == 0) {
                this->SetLineWidth(normalWidth);
                this->SetLineColor(kBlack);
            }
            else if (fType == 1) {
                fMarker->SetMarkerColor(kBlack);
            }
            else if (fType == 2) {
                fMarker->SetMarkerColor(kBlack);
            }
            else if (fType == 3) {
                this->SetLineWidth(normalWidth);
                this->SetLineColor(kBlack);
            }
            else if (fType == 4) {
                this->SetLineWidth(normalWidth);
                this->SetLineColor(kBlack);
            }
            else if (fType == 6 || fType == 7)  //Long Peixun's update: for clusters in CGEM
            {
                fMarker->SetMarkerColor(kBlack);
                fMarker->SetMarkerStyle(4);
            }

            view->UpdateView(0);
            gPad->Modified();
            gPad->Update();
            break;
        }

        TGeoTrack::ExecuteEvent(event, px, py);
    }
}

//Long Peixun's update: Construct 3D line before draw it
void BesGeoTrack::Construct3DLine()
{
    Double_t x,y,z,t;
    fPolyLine3D = new TPolyLine3D(GetNpoints());
    fPolyLine3D->SetBit(kCanDelete);
    for (Int_t i = 0; i < GetNpoints(); i++)
    {
        GetPoint(i,x,y,z,t);
        fPolyLine3D->SetPoint(i,x,y,z);
    }
    fPolyLine3D->SetLineWidth(5);
    fPolyLine3D->SetLineColor(897);    //Long Peixun's update: Red -> Red with a bit purple
}

void BesGeoTrack::Draw(Option_t *option)
{
    // Draw Polyline3D for OpenGL and X3d
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view && view->GetViewType() & k3DView) {
        //Long Peixun's update: Move these code to Construct3DLine()
        Construct3DLine();
        fPolyLine3D->Draw(option);
    }

    //Long Peixun's update: The color of components in the track should be Red
    for (Int_t i = 0; i < f3DHits.GetEntries(); ++i)
    {
        TGeoPhysicalNode* pHit = (TGeoPhysicalNode*)f3DHits.At(i);
        if (pHit) pHit->SetLineColor(kRed);
        
    }

    if (!gPad) gGeoManager->GetMasterVolume()->Draw();
    char *opt1 = Compress(option); // we will have to delete this ?
    TString opt(opt1);
    Bool_t is_default = kTRUE;
    Bool_t is_onelevel = kFALSE;
    Bool_t is_all = kFALSE;
    Bool_t is_type = kFALSE;
    if (opt.Contains("/D")) {
        is_onelevel = kTRUE;
        is_default = kFALSE;
    }
    if (opt.Contains("/*")) {
        is_all = kTRUE;
        is_default = kFALSE;
    }
    if (opt.Contains("/N")) {
        is_type = kTRUE;
        Int_t ist = opt.Index("/N")+2;
        Int_t ilast = opt.Index("/",ist);
        if (ilast<0) ilast=opt.Length();
        TString type = opt(ist, ilast-ist);
        gGeoManager->SetParticleName(type.Data());
    }
    SetBits(is_default, is_onelevel, is_all, is_type);
    AppendPad("SAME");

    delete [] opt1;
    return;
}

void BesGeoTrack::Paint(Option_t *option)
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    Double_t viewPhi = view->GetLongitude();
    TVector3 viewVec(cos(viewPhi*TMath::DegToRad()), sin(viewPhi*TMath::DegToRad()), 0);
    Double_t x, y, z, t;
    Double_t x1=0.0, y1=0.0, z1=0.0;
    Double_t dot, dotMin = 1e+20, dotTolerance = 0.01;
    for (Int_t i = 1; i < GetNpoints(); i++) {
        GetPoint(i, x, y, z, t);
        TVector3 vec(x,y,0);
        if (vec.Mag() > 5.0) {
            vec.SetMag(1.0);
            dot = fabs(vec.Dot(viewVec));
            if ( dot < dotTolerance && dot < dotMin ) {
                x1 = x;
                y1 = y;
                z1 = z;
                dotMin = dot;
            }
        }
    }

    if (dotMin < 1e+20) {
        BesMarker2D sectionPhi("trackPhi", "trackPhi", x1, y1, z1);
        sectionPhi.SetMarkerStyle(28);
        sectionPhi.SetMarkerColor(1);
        sectionPhi.SetMarkerSize(2);
        sectionPhi.Paint();  
	//sectionPhi paint the marker in origin point of MdcTrack!!!
    }

    if (fMarker) fMarker->Paint(option);
    TGeoTrack::Paint(option);
}

//_____________________________________________________

char *BesGeoTrack::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

void BesGeoTrack::SetCharge(Int_t charge)
{
    fCharge = charge;
    if (fCharge > 0)      this->SetLineColor(kBlack); // +
    else if (fCharge < 0) this->SetLineColor(kBlack); // -
    else                   this->SetLineColor(kGreen); // 0
}

