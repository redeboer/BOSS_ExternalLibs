#include <TString.h>
#include <TColor.h>
#include <iostream>
#include "math.h"
#include "TMath.h"
#include "BesVisLib/Mdc2DWire.h"
#include "BesVisLib/BesView.h"
#include "BesVisLib/BesCursor.h"

using namespace std;

#ifndef __CINT__
ClassImp(Mdc2DWire)
#endif

//_____________________________________________________

Mdc2DWire::Mdc2DWire() {

}

//_____________________________________________________

Mdc2DWire::Mdc2DWire(const char* name,
                     const char* title,
                     Double_t rmin,
                     Double_t rmax,
                     Double_t dz,
                     Double_t phi,
                     Double_t *center )
{

    SetName(name);
    SetTitle(title);
    fTip = 0;
    fTube = 0;  //Long Peixun's update: Add fTube initialization
    fHighlighted = false;
    fFired = false;
    fColorfulWire = false;
    fWireType  = 1; // axial wire
    evTime = 0;

    Double_t r = 0.25*(rmax-rmin);
    fWireCircle = new BesCircle2D(name, title, 0.0, r, &center[0]);

    Double_t dx = fabs(r*cos(phi));
    Double_t dy = fabs(r*sin(phi));
    Double_t P[12] = { center[0] - dx, center[1] - dy, center[2] - dz,
                       center[0] + dx, center[1] + dy, center[2] - dz,
                       center[0] - dx, center[1] - dy, center[2] + dz,
                       center[0] + dx, center[1] + dy, center[2] + dz
                     };

    fWireCone = new BesPolygon2D(name, title, 4, P);

    Init();
}

//_____________________________________________________

Mdc2DWire::Mdc2DWire(const char* name, const char* title, Int_t N, Double_t *P)
{
    SetName(name);
    SetTitle(title);
    fTip = 0;
    fHighlighted = false;
    fFired = false;
    fColorfulWire = false;
    fWireType  = 2;  // stereo wire

    Double_t *tubeP = &P[3*N/2]; // east four points, P4~P7
    fTube = new BesPolygon2D(name, title, N/2, tubeP);
    fWireCircle = 0;
    fWireCone   = 0;

    for (Int_t i = 0; i < 3; i++) {
        fWestHole[i] = (P[i]    + P[3+i]  + P[6+i]  + P[9+i] )/4.0;  // (P0+P1+P2+P3)/4
        fEastHole[i] = (P[12+i] + P[15+i] + P[18+i] + P[21+i])/4.0;  // (P4+P5+P6+P7)/4
    }
    TVector3 westVec(fWestHole[0], fWestHole[1], fWestHole[2]);
    TVector3 eastVec(fEastHole[0], fEastHole[1], fEastHole[2]);
    fWestPhi = westVec.Phi() * TMath::RadToDeg();
    fEastPhi = eastVec.Phi() * TMath::RadToDeg();
    if (fWestPhi < 0.0) fWestPhi += 360.0;
    if (fEastPhi < 0.0) fEastPhi += 360.0;

    // ------XY------
    Double_t rMax = sqrt( (P[4*3]  -P[7*3])   * (P[4*3]-P[7*3])     +
                          (P[4*3+1]-P[7*3+1]) * (P[4*3+1]-P[7*3+1]) +
                          (P[4*3+2]-P[7*3+2]) * (P[4*3+2]-P[7*3+2]) ) / 2.0;

    Double_t rMinRatio = 0.0001; // min bubble radius = rMinRatio * rMax;
    Double_t rMaxRatio = 0.8;//0.6; // 0.4//yzhang

    Double_t newP[12];
    Double_t edge = rMaxRatio*0.0001; //*0.85
    for (Int_t i = 0; i < 3; i++) {
        newP[i]   = (0.5+edge*rMinRatio)*(P[i]+P[3+i])*0.5
                    + (0.5-edge*rMinRatio)*(P[6+i]+P[9+i])*0.5;   // near center of P0+P1
        newP[3+i] = (0.5-edge*rMinRatio)*(P[i]+P[3+i])*0.5
                    + (0.5+edge*rMinRatio)*(P[6+i]+P[9+i])*0.5;   // near center of P2+P3
        newP[6+i] = (0.5+edge*rMaxRatio)*(P[12+i]+P[15+i])*0.5
                    + (0.5-edge*rMaxRatio)*(P[18+i]+P[21+i])*0.5; // near center of P4+P5
        newP[9+i] = (0.5-edge*rMaxRatio)*(P[12+i]+P[15+i])*0.5
                    + (0.5+edge*rMaxRatio)*(P[18+i]+P[21+i])*0.5; // near center of P6+P7
    }
    fWireCone = new BesPolygon2D("WireCone", "WireCone", 4, &newP[0]);

    fWireCircle =
        new BesCircle2D("WireBubble", "WireBubble", 0.0, rMaxRatio*rMax, &fEastHole[0]);

    // ------ZR------
    fZRPosMarker2D =
        new BesMarker2D("MdcZRPos", "MdcZRPos", fEastHole[0], fEastHole[1], fEastHole[2]);

    Init();
}

//_____________________________________________________

Mdc2DWire::~Mdc2DWire() {

    //Long Peixun's update: remove "if"
    delete fTube;
    delete fWireCircle;
    delete fWireCone;
    delete fZRPosMarker2D;
    delete fTip;
}

//_____________________________________________________

void Mdc2DWire::Init() {

    SetStyle();

    fTip = new BesPaveText(0, 0, 0.1, 0.1);
    CloseInfo();
}

//_____________________________________________________

void Mdc2DWire::SetStyle()
{
    lcTube = kBlack; // grey;
    lwTube = 1;
    fcTube = kBlack; // grey 15;
    fsTube = 4000;

    lcWire = 15;
    if (fWireType == 1) lcWire = kGreen;
    if (fWireType == 2) lcWire = kBlue;
    lwCircle = 1;
    lwCone   = 1;
    fcCircle = kWhite;
    fcCone   = 0;
    fsCircle = 1001;
    fsCone   = 0;

    lcWireFired    = kRed;
    lwCircleFired  = 2;//yzhang 2;
    lwConeFired    = 2;
    fcCircleFired  = 1004; //kBlack;
    fcConeFired    = kBlack;
    fsCircleFired  = 1001;//solid
    fsConeFired    = 3001;

    lcWireHL   = kMagenta;//yzhang lcWire;
    lwCircleHL = 2;
    lwConeHL   = 2;
    fcCircleHL = kMagenta;//yzhang kBlack;
    fcConeHL   = kRed;
    fsCircleHL = 1001;
    fsConeHL   = 4000;

    sizeMarker   = 4;
    cMarker      = kBlue;
    cMarkerFired = kRed;    // color of fired marker
    cMarkerHL    = cMarker; // cMarkerFired;
    sMarker      = 24;
    sMarkerFired = 20;   // style of fired marker
    sMarkerHL    = 25;   // style of highlighted marker
}

void Mdc2DWire::ClearInfo()
{
    fInfoCon.clear();
    fTip->DeleteText();
}

void Mdc2DWire::CloseInfo()
{
    if (fTip) {
        fTip->Clear();

        if (fInfoCon.size() == 0) fInfoCon.push_back(GetTitle());
        fTip->SetText(fInfoCon);
    }
    else
        cout << "Mdc2DWire::CloseInfo, not initialized" << endl;
}

//_____________________________________________________

Int_t Mdc2DWire::DistancetoPrimitive(Int_t px, Int_t py) {
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view->GetViewType() & kXYView) {
        return fWireCircle->DistancetoPrimitive(px, py);
    }
    else if (view->GetViewType() & kZRView) {
        if (fZRPosMarker2D) {
            //Long Peixun's update: Remove duplicate code

            //We need to research what damage fZRPoseMarker2D pointers
            //This bug can be avoid by changing the initialize order of subdetectors
            // if ((unsigned long long)(fZRPosMarker2D) & 0xFFFFFFFF00000000)
            // {
            //     cout << this << "  " << fZRPosMarker2D << endl;
            // }

            return fZRPosMarker2D->DistancetoPrimitive(px, py);
        }
    }

    return 999;
}

//_____________________________________________________

void Mdc2DWire::ExecuteEvent(Int_t event, Int_t px, Int_t py) {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (gBesCursor->GetType() == kBesHand) {
        if (view) view->ExecuteEvent(event, px, py);
    }
    else if (gBesCursor->GetType() == kBesPick){

        if (gPad) gPad->SetCursor(kPointer);

        Int_t    origColor = 1;

        switch (event) {

        case kMouseEnter :

            if ( fWireCircle )    origColor = fWireCircle->GetFillColor();
            if ( fZRPosMarker2D ) origColor = fZRPosMarker2D->GetMarkerColor();
            this->SetHighlighted(true);

            if (this->IsFired() || view->GetVisMdcWires()) {
                Draw("WIRE,SAME");

                fTip->SetPos(px, py);
                view->UpdateView(0);

                //Long Peixun's update: If ShowInfo is not checked, don't show tip text.
                if (gBesCursor->GetShowInfo()) fTip->Draw("BR,SAME"); // "BR,ARC,SAME"
                gPad->Modified();
                gPad->Update();
            }
            break;

        case kMouseLeave:

            if (this->IsHighlighted()) {
                this->SetHighlighted(false);
                if (view->GetVisMdcWires()) Draw("WIRE,SAME");

                view->UpdateView(0);
                gPad->Modified();
                gPad->Update();
            }
            break;
        }

    }
}

//_____________________________________________________

void Mdc2DWire::Draw(Option_t *option) {
    //
    // Mdc2DWire draw function
    TString opt = option;
    opt.ToUpper();

    AppendPad(option);
}

//_____________________________________________________

void Mdc2DWire::Paint(Option_t *option) {

    TString opt = option;
    opt.ToUpper();

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    //gStyle->SetPalette(1);
    if (view->GetViewType() & kXYView) {

        Double_t wcCenter[3], ncCenter[3];
        fWireCircle->GetCenter(&wcCenter[0]);
        view->WCtoNDC(wcCenter, &ncCenter[0]);
        Double_t x1, y1, x2, y2;
        Double_t d = 0.05;
        gPad->GetRange(x1, y1, x2, y2);

        // Only draw wire in pad range
        if (ncCenter[0] > x1-d && ncCenter[0] < x2+d &&
                ncCenter[1] > y1-d && ncCenter[1] < y2+d) {
            if (opt.Contains("TUBE") && fTube ) {
                fTube->SetLineColor(lcTube);
                fTube->SetLineWidth(lwTube);
                fTube->SetFillColor(fcTube);
                fTube->SetFillStyle(fsTube);

                fTube->Paint();
            }

            if (this->IsFired()) {
                if (fWireCircle) {
                    fWireCircle->SetLineColor(lcWireFired);
                    fWireCircle->SetLineWidth(lwCircleFired);
                    fWireCircle->SetFillColor(fcCircleFired);
                    fWireCircle->SetFillStyle(fsCircleFired);
                }
                if (fWireCone) {
                    fWireCone->SetLineColor(lcWireFired);
                    fWireCone->SetLineWidth(lwConeFired);
                    fWireCone->SetFillColor(fcConeFired);
                    fWireCone->SetFillStyle(fsConeFired);
                }
            }
            else {
                if (fWireCone) {
                    fWireCone->SetLineColor(lcWire);
                    fWireCone->SetLineWidth(lwCone);
                    fWireCone->SetFillColor(fcCone);
                    fWireCone->SetFillStyle(fsCone);
                }
                if (fWireCircle) {
                    fWireCircle->SetLineColor(lcWire);
                    fWireCircle->SetLineWidth(lwCircle);
                    fWireCircle->SetFillColor(fcCircle);
                    fWireCircle->SetFillStyle(fsCircle);
                }
            }

            if (this->IsHighlighted()) {
                if (fWireCircle) {
                    fWireCircle->SetLineColor(lcWireHL);
                    fWireCircle->SetLineWidth(lwCircleHL);
                    fWireCircle->SetFillColor(fcCircleHL);
                    fWireCircle->SetFillStyle(fsCircleHL);
                }
                if (fWireCone) {
                    fWireCone->SetLineColor(lcWireHL);
                    fWireCone->SetLineWidth(lwConeHL);
                    fWireCone->SetFillColor(fcConeHL);
                    fWireCone->SetFillStyle(fsConeHL);
                }
            }
            if (opt.Contains("WIRE")) {
                //Long Peixun's update: Only draw fired wires' cone
                if (IsFired()) fWireCone->Paint();  //Long Peixun's update: Draw colorful wires' cone
                if (fWireCircle) fWireCircle->Paint();
            }
        }
    }

    if (view->GetViewType() & kZRView) {
        if (HasZRSection()) {
            fZRPosMarker2D->SetWCX(fZRSectionPos[0]);
            fZRPosMarker2D->SetWCY(fZRSectionPos[1]);
            fZRPosMarker2D->SetWCZ(fZRSectionPos[2]);
            fZRPosMarker2D->SetMarkerColor(cMarker);
            fZRPosMarker2D->SetMarkerStyle(sMarker);
            fZRPosMarker2D->SetMarkerSizeMultiple(sizeMarker);

            if (this->IsFired()) {
                if (fZRPosMarker2D) {
                    fZRPosMarker2D->SetMarkerColor(cMarkerFired);
                    fZRPosMarker2D->SetMarkerStyle(sMarkerFired);
                }
            }
            else {
                if (fZRPosMarker2D) {
                    fZRPosMarker2D->SetMarkerColor(cMarker);
                    fZRPosMarker2D->SetMarkerStyle(sMarker);
                }
            }

            if (this->IsHighlighted()) {
                if (fZRPosMarker2D) {
                    fZRPosMarker2D->SetMarkerColor(cMarkerHL);
                    fZRPosMarker2D->SetMarkerStyle(sMarkerHL);
                }
            }

            if (opt.Contains("WIRE")) {
                if (fZRPosMarker2D) fZRPosMarker2D->Paint();
            }
        }
    }
}

//_____________________________________________________

char *Mdc2DWire::GetObjectInfo(Int_t px, Int_t py) const {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) return view->GetObjectInfo(px, py);
    else return TObject::GetObjectInfo(px, py);
}

//_____________________________________________________

bool Mdc2DWire::HasZRSection()
{
    if (fWireType != 2) return false;

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

        if ( fabs(fEastPhi-fWestPhi) < 180.0 ) {  // no cross phi=0.0
            if ( (fEastPhi-zrPhi)*(fWestPhi-zrPhi) > 0.0 ) flag = false;
            else {
                Double_t factor = fabs( (fEastPhi-zrPhi)/(fEastPhi-fWestPhi) );
                for (Int_t i = 0; i < 3; i++) {
                    fZRSectionPos[i] =  factor*fWestHole[i] + (1.0-factor)*fEastHole[i];
                }
                flag = true;
                break;
            }
        }
        else { // cross phi=0.0
            if ( (fEastPhi-zrPhi)*(fWestPhi-zrPhi) < 0.0 ) flag = false;
            else { // range 0~360 -> -180~180
                Double_t tempEastPhi = fEastPhi;
                if (tempEastPhi >= 180.0) tempEastPhi -= 360.0;
                Double_t tempWestPhi = fWestPhi;
                if (tempWestPhi >= 180.0) tempWestPhi -= 360.0;
                Double_t tempZRPhi   = zrPhi;
                if (tempZRPhi   >= 180.0) tempZRPhi   -= 360.0;

                Double_t factor = fabs( (tempEastPhi-tempZRPhi)/(tempEastPhi-tempWestPhi) );
                for (Int_t i = 0; i < 3; i++) {
                    fZRSectionPos[i] =  factor*fWestHole[i] + (1.0-factor)*fEastHole[i];
                }
                flag = true;
                break;
            }
        }
    }

    return flag;
}

//_____________________________________________________

Double_t Mdc2DWire::Range360(Double_t input)
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

//_____________________________________________________

void Mdc2DWire::SetTime(Double_t time)
{ 
    mdcTime = time;
}

//_____________________________________________________

void Mdc2DWire::SetCharge(Double_t charge)
{ 
    mdcCharge = charge;
}

//_____________________________________________________

void Mdc2DWire::SetQOverflow(Bool_t qOvfl)
{ 
    qOverflow = qOvfl;
}

//_____________________________________________________

void Mdc2DWire::SetEvTime(Double_t time)
{ 
    evTime = time;
}

//_____________________________________________________
//Long Peixun's update: raw channel data for mdc hits display
void Mdc2DWire::SetTimeChannel(UInt_t tc)
{
    timeChannel = tc;
}
void Mdc2DWire::SetChargeChannel(UInt_t cc)
{
    chargeChannel = cc;
}

//_____________________________________________________

void Mdc2DWire::SetColorfulWire(Bool_t colorfulWire, Bool_t subEvTime)
{ 
    fColorfulWire = colorfulWire;
    if(fColorfulWire){
	//yzhang change LINE COLOR of fired mdc wires 
	//n color bin = 50, time of max bin is 2000 
	//time max value = 1943.8125 ~ 2000 ns 
	//no time info color is kGray 
	//time > 2000 && time <=10000, color is kBlack
	int tempColor;
	if(subEvTime){ 
	    // raw time - event start time
	    double tempMdcTime = mdcTime - evTime; 
	    if(fabs(evTime)>0.0001) tempMdcTime += 230;
	    if(tempMdcTime>10000) { 
		tempColor = kGray;
	    }else if(tempMdcTime>2000&&tempMdcTime<=10000){
		tempColor = kRed;
	    }else if(tempMdcTime<-10){
		tempColor = kWhite;
	    }else{
		tempColor = 51 + (int) (tempMdcTime/40);
	    }
	}else{
	    //raw time
	    if(mdcTime>10000) { 
		tempColor= kGray;
	    }else if(mdcTime>2000&&mdcTime<=10000){
		tempColor = kRed;
	    }else{
		tempColor = 51 + (int) (mdcTime/40);
	    }
	}
	lcWireFired = tempColor;
	//std::cout<< " mdcTime  "<<mdcTime <<" evTime "<< evTime << " t-t0 "<< mdcTime - evTime +230<< " lcWireFired " << lcWireFired <<std::endl;

	//yzhang change FILL COLOR of fired mdc wires
	//n color bin = 50,  charge of max bin is 2000
	//charge max value = 29783 ~ 30000
	//adc> 2000&&<=30000 color is kBlack
	//no adc color is kGray
	if(mdcCharge>30000){
	    tempColor = kGray;
	}else if(mdcCharge>2000&&mdcCharge<=30000){
	    tempColor = kRed;
	}else {
	    tempColor = 51 + (int) (mdcCharge/40);
	}
	if(qOverflow) { tempColor = kBlack; }
	fcCircleFired  = tempColor;
	//std::cout<< " mdcCharge "<<mdcCharge<<" ovfl "<<qOverflow<<" fcCircleFired " << fcCircleFired<< std::endl;
    }else{
	lcWireFired = kRed;
	fcCircleFired  = 1004;
    } 
}
