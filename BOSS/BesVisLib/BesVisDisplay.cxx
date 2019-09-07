

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// BesVisDisplay                                                          //
//                                                                      //
// Utility class to display ATLAS outline, tracks, clusters, jets,..    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <TROOT.h>
#include <TEnv.h>
#include <TButton.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TColor.h>
#include <TAttImage.h>
#include <TView.h>
#include <TArc.h>
#include <TText.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TList.h>
#include <TDiamond.h>
#include <TNode.h>
#include <TTUBE.h>
#include <TMath.h>
#include <X3DBuffer.h>
#include <TVirtualGeoPainter.h>
#include <TControlBar.h>
#include <TImage.h>
#include <TASImage.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TGeoBBox.h>
#include <TGeoTube.h>
#include <TGeoArb8.h>
#include <TViewerX3D.h>

#include "BesVisLib/BesPolygon2D.h"
#include "BesVisLib/BesCircle2D.h"
#include "BesVisLib/BesVisDisplay.h"
#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesTView.h"

BesVisDisplay *gDisplay = 0;

using namespace std;

#ifndef __CINT__
ClassImp(BesVisDisplay)
#endif

//_____________________________________________________________________________
BesVisDisplay::BesVisDisplay() : TQObject()
{

}

//_____________________________________________________________________________
BesVisDisplay::BesVisDisplay(const char *title) : TQObject(),
        fBesGeometry(0),
        fMdcROOTGeo(0),
        fTofROOTGeo(0),
        fEmcROOTGeo(0),
        fMucROOTGeo(0),
        fMdcCon(0),
        fTofCon(0),
        fEmcCon(0),
        fMucCon(0),
        fAllVis(0),
        fQuarterVis(0),
        fHalfVis(0),
        fNoEndVis(0)
{

}

//_____________________________________________________________________________
BesVisDisplay::~BesVisDisplay()
{

}

//_____________________________________________________________________________
void BesVisDisplay::SetCanvas(TCanvas *c)
{
    if (c != 0) {
        fCanvas = c;
    }
    else {
        fCanvas = new TCanvas("Canvas", " ",0,0,1024,768);
    }
}

void BesVisDisplay::Init()
{
    fDisplayMode = 0;
    //Long Peixun's update: Initialize flag of header visibility
    fVisHeader = 1;
    fCanvas->cd();
    //fCanvas->SetEditable(kFALSE);
    fCanvas->SetFillColor(10);

    // pad 3D
    fPad3D     = new TPad("Pad3D",     "Pad3D",     0.75, 0.75,  0.9, 0.9);
    //Long Peixun's update: Back color of 3D view -> White
    //fPad3D->SetFillColor(TColor::GetColor(204,204,204));
    //fPad3D->SetFillColor(16);
    fPad3D->Draw();

    // pad Header
    fPadHeader = new TPad("PadHeader", "PadHeader", 0.0, 0.68, 1.0, 1.0);
    fPadHeader->Draw();
    fPadHeader->cd();

    TString fp = gEnv->GetValue("Root.TTFontPath", "");
    TString bc = fp + "/BlackChancery.ttf";
    TString ar = fp + "/arial.ttf";

    string besvisPath = getenv("BESVISLIBROOT");
    TString filePath(besvisPath);
    filePath += "/icons/";

    fHeaderHImage = TImage::Open(filePath + "HeaderH.gif");
    fHeaderHImage->SetEditable(kTRUE);
    //fHeaderHImage->Draw("same");
    fHeaderVImage = TImage::Open(filePath + "HeaderV.gif");
    fHeaderVImage->SetEditable(kTRUE);
    // fHeaderVImage->Draw("same");
    fHeaderImage = TImage::Open(filePath + "Header.gif");
    fHeaderImage->DrawText(620, 15, "BesVis", 30,
                           gROOT->GetColor(2)->AsHexString(),
                           bc, TImage::kShadeBelow);
    fHeaderImage->SetConstRatio(false);
    fHeaderImage->Draw("same");

    fBesHeader = new BesHeader(0,0,1,1, "br");
    fBesHeader->SetTextSize(0.1);
    fBesHeader->SetFillColor(28);
    fBesHeader->SetFillStyle(0);
    fBesHeader->SetBorderSize(4);

    fLatticeWidth=0.185;
    fTextSizeTitle=0.08;
    fTextSizeTime=0.07;
    fTextSizeData=0.05;
    fTextSizeFlt=0.02;
    fCoordx=0.01;
    fCoordy=0.8;
    fDistanceOfRows=0.01;

    fMdcOn = true;
    fTofOn = true;
    fEmcOn = true;

    Connect(fBesHeader,"TransferMdcOn(Bool_t)","BesVisDisplay",this,"SetMdcOn(Bool_t)");
    Connect(fBesHeader,"TransferTofOn(Bool_t)","BesVisDisplay",this,"SetTofOn(Bool_t)");
    Connect(fBesHeader,"TransferEmcOn(Bool_t)","BesVisDisplay",this,"SetEmcOn(Bool_t)");

    DrawHeader();

    // pad XY
    fPadXY     = new TPad("PadXY",     "PadXY",     0.0,  0.0,   0.5, 0.68);
    fPadXY->Draw();

    // pad ZR
    fPadZR     = new TPad("PadZR",     "PadZR",     0.5,  0.0,   1.0, 0.68);
    fPadZR->Draw();

    /*
    fPad3D->SetBorderMode(0);
    fPad3D->SetBorderSize(3);
    fPadHeader->SetBorderMode(1);
    fPadHeader->SetBorderSize(3);
    fPadXY->SetBorderMode(2);
    fPadXY->SetBorderSize(3);
    fPadZR->SetBorderMode(3);
    fPadZR->SetBorderSize(3);
    */
}

//_____________________________________________________________________________
//Long Peixun's update: Consider NULL pointers
void BesVisDisplay::Reset()
{
    if (fPadXY) {
        fPadXY->cd();
        BesView* view = dynamic_cast<BesView*>(fPadXY->GetView());
        if (view) view->Reset();
    }
    if (fPadZR) {
        fPadZR->cd();
        BesView* view = dynamic_cast<BesView*>(fPadZR->GetView());
        if (view) view->Reset();
    }
    if (fPad3D) {
        fPad3D->cd();
        BesView* view = dynamic_cast<BesView*>(fPad3D->GetView());
        if (view) view->Reset();
    }

    SwitchDisplayMode(fDisplayMode);
}

//_____________________________________________________________________________
//Long Peixun's update: Refresh all pads
void BesVisDisplay::Refresh()
{
    if (fPadXY) {
        fPadXY->cd();
        BesView* view = dynamic_cast<BesView*>(fPadXY->GetView());
        if (view) view->Refresh();
    }
    if (fPadZR) {
        fPadZR->cd();
        BesView* view = dynamic_cast<BesView*>(fPadZR->GetView());
        if (view) view->Refresh();
    }
    if (fPad3D) {
        fPad3D->cd();
        BesView* view = dynamic_cast<BesView*>(fPad3D->GetView());
        if (view) view->Refresh();
    }

    SwitchDisplayMode(fDisplayMode);
}

//_____________________________________________________________________________

void BesVisDisplay::InitGeometryFromGDML(const TString fPath, bool mrpc, bool cgem)
{
    if ( gBesGeometry ) delete gBesGeometry;
    fBesGeometry = new BesGeometry();
    gBesGeometry = fBesGeometry;
    fBesGeometry->InitFromGDML(fPath, mrpc, cgem);
    fBesGeometry->SetDefaultVis();

    // should be deleted after BesView completed;
    fMdcROOTGeo = fBesGeometry->GetMdcROOTGeo();   //here liang
    fTofROOTGeo = fBesGeometry->GetTofROOTGeo();   //here liang
    fEmcROOTGeo = fBesGeometry->GetEmcROOTGeo();   //here liang
    fMucROOTGeo = fBesGeometry->GetMucROOTGeo();
}

//_____________________________________________________________________________

void BesVisDisplay::InitGeometryFromROOT(TGeoVolume *volBes)
{
    if ( gBesGeometry ) delete gBesGeometry;
    fBesGeometry = new BesGeometry();
    gBesGeometry = fBesGeometry;
    fBesGeometry->InitFromROOT(volBes);

    // should be deleted after BesView completed;
    fMdcROOTGeo = fBesGeometry->GetMdcROOTGeo();
    fTofROOTGeo = fBesGeometry->GetTofROOTGeo();
    fEmcROOTGeo = fBesGeometry->GetEmcROOTGeo();
    fMucROOTGeo = fBesGeometry->GetMucROOTGeo();
}

void BesVisDisplay::DrawHeader()
{
    fPadHeader->cd();
    //fPadHeader->Clear();

    //TASImage asImage("HeaderH2.gif");
    //Pixmap_t pic = asImage.GetPixmap();

    //cout << "mode " << fDisplayMode << endl;
    // draw text over image with funny font
    if (!IsVHeader())   //Long Peixun's update: replace by IsVHeader()
    {
        fHeaderImage->Zoom(161, 649, 635, 174);  // (x Offset, y Offset, width, height)
        //fHeaderImage->Scale(800, 200);
    }
    else {
        fHeaderImage->Zoom(600, 324, 164, 473);
        //fHeaderImage->Scale(200, 600);
    }

    // draw text over image with foreground specified by pixmap
    //img->DrawText(250, 350, "goodbye cruel world ...", 24, 0, ar, TImage::kPlain, "fore.xpm");

    fBesHeader->Clear();
    //fBesHeader->AddText("Commented");

    if (!gEvent) {
        fBesHeader->AddText("No Event Info");
    }
    else {
        // old coordinates
        Double_t coordx=fCoordx;
        Double_t coordy=fCoordy;

        // Get data
        Long64_t runno   = gEvent->GetRun();
        Long64_t eventno = gEvent->GetEvent();
        Int_t year    = gEvent->GetYear();
        Int_t month   = gEvent->GetMonth();
        Int_t day     = gEvent->GetDay();
        Int_t hour    = gEvent->GetHour();
        Int_t min     = gEvent->GetMin();
        Int_t sec     = gEvent->GetSec();

        // Add run no.
        if (IsVHeader()) {
          coordx = 0.1;
          coordy = 0.88;        //Long Peixun's update: 0.90 -> 0.88
        }
        else             {
            coordx = 0.02;
            coordy = 0.88;      //Long Peixun's update: 0.90 -> 0.88
        }
        char title1[20];
        sprintf(title1, "Run %ld ",  runno);
        TText* ttitle1 = fBesHeader->AddText(coordx,coordy,title1);
        ttitle1->SetTextSize(fTextSizeTitle);
        ttitle1->SetTextColor(1);
        ttitle1->SetTextAlign(12);

        // Add event no.
        if (IsVHeader()) {
            coordx = 0.1;
            coordy = 0.81;      //Long Peixun's update: 0.86 -> 0.81
        }
        else             {
            coordx = 0.02;
            coordy = 0.76;      //Long Peixun's update: 0.82 -> 0.76
        }
        char title2[20];
        sprintf(title2, "Event %ld ",  eventno);
        TText* ttitle2 = fBesHeader->AddText(coordx,coordy,title2);
        ttitle2->SetTextSize(fTextSizeTitle);
        ttitle2->SetTextColor(1);
        ttitle2->SetTextAlign(12);

        // Add estime
        if (IsVHeader()) {
            coordx = 0.1;
            coordy = 0.74;      //Long Peixun's update: 0.83 -> 0.74
        }
        else {
            coordx = 0.02;
            coordy = 0.63;      //Long Peixun's update: 0.72 -> 0.63      
        }
        char titleEvTime[30];

        //Long Peixun's update: if this event is not a rec event, don't show estime
        if (gEvent->GetHeader().GetEventRec())
        {
            if (fabs(gEvent->GetHeader().GetEvTime())>0.0001)
            {
                double esTimeConst = 230; // sub in T0 of each layer in MDC calibration
                sprintf(titleEvTime, "Estime:%5.1fns", gEvent->GetHeader().GetEvTime() - esTimeConst);
            }
            else sprintf(titleEvTime, "Estime:0 ns");
        }
        else sprintf(titleEvTime, "Estime: -");

        TText* ttitleEvTime = fBesHeader->AddText(coordx,coordy,titleEvTime);
        ttitleEvTime->SetTextSize(fTextSizeTime);
        ttitleEvTime->SetTextFont(52);
        ttitleEvTime->SetTextColor(1);
        ttitleEvTime->SetTextAlign(12);

        // Add stat
        if (IsVHeader()) {
            coordx = 0.1;
            coordy = 0.69;      //Long Peixun's update: 0.81 -> 0.69
        }
        else {
            coordx = 0.2;
            coordy = 0.63;      //Long Peixun's update: 0.72 -> 0.63
	    }
        char titleEvTimeStat[30];

        //Long Peixun's update: if this event is not a rec event, don't show status
        if (gEvent->GetHeader().GetEventRec())
        {
            sprintf(titleEvTimeStat, "    stat:%3d", gEvent->GetHeader().GetEvTimeStatus());
        }
        else sprintf(titleEvTimeStat, "    stat: -");

        TText* ttitleEvTimeStat = fBesHeader->AddText(coordx,coordy,titleEvTimeStat);
        ttitleEvTimeStat->SetTextSize(fTextSizeTime);
        ttitleEvTimeStat->SetTextFont(52);
        ttitleEvTimeStat->SetTextColor(1);
        ttitleEvTimeStat->SetTextAlign(12);

        // Add quality
        if (IsVHeader()) {
            coordx = 0.1;
            coordy = 0.64;      //Long Peixun's update: 0.79 -> 0.64
        }
        else {
            coordx = 0.4;
            coordy = 0.63;      //Long Peixun's update: 0.72 -> 0.63
        }
        char titleEvTimeQ[30];

        //Long Peixun's update: if this event is not a rec event, don't show quality
        if (gEvent->GetHeader().GetEventRec())
        {
            sprintf(titleEvTimeQ, "  quality:%3.1f", gEvent->GetHeader().GetEvTimeQuality());
        }
        else sprintf(titleEvTimeQ, "  quality: -");

        TText* ttitleEvTimeQ = fBesHeader->AddText(coordx,coordy,titleEvTimeQ);
        ttitleEvTimeQ->SetTextSize(fTextSizeTime);
        ttitleEvTimeQ->SetTextFont(52);
        ttitleEvTimeQ->SetTextColor(1);
        ttitleEvTimeQ->SetTextAlign(12);

        // Add date
        if (IsVHeader()) {
            coordx = 0.1;
            coordy = 0.57;      //Long Peixun's update: 0.76 -> 0.57
        }
        else {
            coordx = 0.02;
            coordy = 0.51;      //Long Peixun's update: 0.60 -> 0.51
        }
        char date[70];
        sprintf(date,"date: %4d-%02d-%02d ", year,month,day);
        TText* tdate = fBesHeader->AddText(coordx,coordy,date);
        tdate->SetTextSize(fTextSizeTime);
        tdate->SetTextFont(52);
        tdate->SetTextColor(1);
        tdate->SetTextAlign(12);

        // Add time
        if (IsVHeader()) {
            coordx = 0.1;
            coordy = 0.52;      //Long Peixun's update: 0.69 -> 0.52
        }
        else {
            coordx = 0.25;
            coordy = 0.51;      //Long Peixun's update: 0.60 -> 0.51
        }
        char time[70];
        sprintf(time,"time: %02d:%02d:%02d", hour,min,sec);
        TText* ttime=fBesHeader->AddText(coordx,coordy,time);
        ttime->SetTextSize(fTextSizeTime);
        ttime->SetTextFont(52);
        ttime->SetTextColor(1);
        ttime->SetTextAlign(12);

        // string for contents
        TString info;
        vector<TString> statusCon;

        char data[25];
        if (gEvent->GetMC()) sprintf(data, "MC=Yes");
        else                 sprintf(data, "MC=No");
        statusCon.push_back(TString(data));

        vector<Int_t> trigChannelVector = gEvent->GetHeader().GetTrigChannelVector();
        vector<Int_t> trigConditionVector = gEvent->GetHeader().GetTrigConditionVector();
	    Int_t timeType = gEvent->GetHeader().GetTimeType();
        //************************************
	    sprintf(data, "Time Type: %d", timeType);
	    statusCon.push_back(TString(data));
        if (IsVHeader()) {
            coordx = 0.10;
            coordy = 0.45;      //Long Peixun's update: 0.71 -> 0.45
        }
        else             {
            coordx = 0.02;
            coordy = 0.38;      //Long Peixun's update: 0.50 -> 0.38
        }
        for (Int_t i = 0; i < (Int_t)statusCon.size(); i++) {
            TText* ttitle=fBesHeader->AddText(coordx,coordy,statusCon[i].Data());
            if (IsVHeader()) coordy -= 0.04;    //Long Peixun's update: 0.02 -> 0.04
            else coordx += 0.2;
                ttitle->SetTextSize(fTextSizeData);
                ttitle->SetTextFont(62);
                ttitle->SetTextColor(1);
                ttitle->SetTextAlign(12);
        }
        vector<TString> infoCon;
        //cout << "BesVisDisplay::time Type: " << timeType << endl;
        vector<Int_t>::iterator pTrigVector;
        //Long Peixun's update: Annotate "for"
        // for (pTrigVector = trigConditionVector.begin(); 
        //     pTrigVector != trigConditionVector.end(); 
        //     pTrigVector++){
        //   //cout << "BesVisDisplay::trigCondition: " << *pTrigVector << endl;
        // }
        //for (pTrigVector = trigChannelVector.begin(); 
        //    pTrigVector != trigChannelVector.end(); 
        //    pTrigVector++){
	  //sprintf(data, "Trig Channel_00");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_01");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_02");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_03");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_04");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_05");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_06");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_07");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_08");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_09");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_10");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_11");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_12");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_13");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_14");
	  //infoCon.push_back(TString(data));
	  //sprintf(data, "Trig Channel_15");
	  //infoCon.push_back(TString(data));
          //cout << "BesVisDisplay::trigChannel: " << *pTrigVector << endl;
        //}
        //************************************

        //if (GetMdcOn()) {
        //    //Float_t p  = gEvent->GetHeader().GetP();
        //    //Float_t pt = gEvent->GetHeader().GetPt();
        //    //Float_t px = gEvent->GetHeader().GetPx();
        //    //Float_t py = gEvent->GetHeader().GetPy();
        //    //Float_t pz = gEvent->GetHeader().GetPz();

        //    //sprintf(data, "P= %-.3fGeV", p);
        //    //infoCon.push_back(TString(data));
        //    //sprintf(data, "Pt= %-.3fGeV", pt);
        //    //infoCon.push_back(TString(data));
        //    //sprintf(data, "Px= %-.3fGeV", px);
        //    //infoCon.push_back(TString(data));
        //    //sprintf(data, "Py= %-.3fGeV", py);
        //    //infoCon.push_back(TString(data));
        //    //sprintf(data, "Pz= %-.3fGeV", pz);
        //    //infoCon.push_back(TString(data));
        //}

        //if (GetTofOn()) {
        //    Float_t t  = gEvent->GetHeader().GetT();

        //    sprintf(data, "tofMin= %-.3fns", t);
        //    infoCon.push_back(TString(data));
        //}

        //if (GetEmcOn()) {
        //    Float_t e  = gEvent->GetHeader().GetE();

        //    sprintf(data, "ECal= %-.3fGeV", e);
        //    infoCon.push_back(TString(data));
        //}

        ////sprintf(data, "MDC Track(GeV): ");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "P1=0.945");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "P2=0.702");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "P3=0.421");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "P4=1.048");
        ////infoCon.push_back(TString(data));

        ////sprintf(data, "EMC Cluster(MeV):");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "E1=151.91");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "E2=226.00");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "E3=295.91");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "E4=165.27");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "E5=48.68");
        ////infoCon.push_back(TString(data));
        ////sprintf(data, "E6=193.98");
        ////infoCon.push_back(TString(data));
        Int_t npr;
        Float_t rowH;
        Float_t textSize = fTextSizeData;
        if (IsVHeader()) {
            fCoordx = 0.10;
            fCoordy = 0.35;     //Long Peixun's update: 0.70 -> 0.35
            npr = 1;
            rowH = 0.04;
            textSize *= 1.2;
        }
        else             {
            fCoordx = 0.02;
            fCoordy = 0.38;     //Long Peixun's update: 0.50 -> 0.38
            npr = 4;
            rowH = 0.1;
        }

        pTrigVector = trigChannelVector.begin(); 
        for (Int_t i = 0; i < (Int_t)infoCon.size(); i++) {
            coordx = fCoordx + (i%npr)*(1.0/npr);
            coordy = fCoordy - (i/npr)*rowH;

            if (*pTrigVector == i){
            TString temp = infoCon[i] + " ON";
                TText* tdata=fBesHeader->AddText(coordx,coordy,temp.Data());
                tdata->SetTextSize(textSize);
                tdata->SetTextFont(62);
                tdata->SetTextColor(2);
                tdata->SetTextAlign(12);
            pTrigVector++;
            }
            else {
            TString temp = infoCon[i] + " OFF";
                TText* tdata=fBesHeader->AddText(coordx,coordy,temp.Data());
                tdata->SetTextSize(textSize);
                tdata->SetTextFont(62);
                tdata->SetTextColor(1);
                tdata->SetTextAlign(12);
            }
        }

    }

    fBesHeader->Draw("same");

    fPadHeader->SetEditable(kFALSE);
    fPadHeader->Modified();
    fPadHeader->Update();
    fCanvas->cd();
}

//_____________________________________________________________________________

void BesVisDisplay::SetMdcOn(Bool_t MdcOn)
{
    fMdcOn = MdcOn;
    //fBesHeader->SetMdcOn(MdcOn);
    DrawHeader();
}

//_____________________________________________________________________________

void BesVisDisplay::SetTofOn(Bool_t TofOn)
{
    fTofOn = TofOn;
    //fBesHeader->SetTofOn(TofOn);
    DrawHeader();
}

//_____________________________________________________________________________

void BesVisDisplay::SetEmcOn(Bool_t EmcOn)
{
    fEmcOn = EmcOn;
    //fBesHeader->SetEmcOn(EmcOn);
    DrawHeader();
}

//_____________________________________________________________________________

void BesVisDisplay::SwitchDisplayMode(Int_t mode)
{
    SetDisplayMode(mode);
    ResizePad();
    fCanvas->Update();
}

//_____________________________________________________________________________
void BesVisDisplay::ResizePad()
{
    fCanvas->cd();
    Double_t xlowH = -0.11, xupH = -0.1, ylowH = -0.11, yupH = -0.1;
    Int_t width  = fCanvas->GetWw();
    Int_t height = fCanvas->GetWh();
    Double_t hVSw = Double_t(height)/width;
    Double_t x2D = 1.0-hVSw;
    //cout << "Canvas width " << width << " height " << height << " x2D " << x2D << endl;
    Double_t y2D = 0.5/hVSw;
    Double_t x3D = (1.0/3)/hVSw;

    switch ( fDisplayMode ) {
    case 0 : // 2D XY+ZR
        fPadHeader->SetPad(0.0,  y2D,  1.0,  1.0);
        fPadXY    ->SetPad(0.0,  0.0,  0.5,  y2D);
        fPadZR    ->SetPad(0.5,  0.0,  1.0,  y2D);
        fPad3D    ->SetPad(xlowH, ylowH, xupH, yupH);
        break;
    case 1 : // 2D XY
        fPadHeader->SetPad(0.0, 0.0,  x2D, 1.0);
        fPadXY    ->SetPad(x2D, 0.0,  1.0, 1.0);
        fPadZR    ->SetPad(xlowH, ylowH, xupH, yupH);
        fPad3D    ->SetPad(xlowH, ylowH, xupH, yupH);
        break;
    case 2 : // 2D ZR
        fPadHeader->SetPad(0.0, 0.0,  x2D, 1.0);
        fPadXY    ->SetPad(xlowH, ylowH, xupH, yupH);
        fPadZR    ->SetPad(x2D, 0.0,  1.0,  1.0);
        fPad3D    ->SetPad(xlowH, ylowH, xupH, yupH);
        break;
    case 3 : // 3D
        fPadHeader->SetPad(0.0, 0.0,  x2D, 1.0);
        fPadXY    ->SetPad(xlowH, ylowH, xupH, yupH);
        fPadZR    ->SetPad(xlowH, ylowH, xupH, yupH);
        fPad3D    ->SetPad(x2D, 0.0,  1.0,  1.0);
        break;
    case 4 : // 2D+3D
        //fPadHeader->SetPad(xlowH, ylowH, xupH, yupH);
        fPadHeader->SetPad(0.0,   2*x3D, 1.0,  1.0);
        fPadXY    ->SetPad(0.0,   x3D,   1.0/3, 2*x3D);
        fPadZR    ->SetPad(0.0,   0.0,   1.0/3, x3D);
        fPad3D    ->SetPad(1.0/3, 0.0,   1.0,   2*x3D);
        break;
    default :
        return;
    }

    //Long Peixun's update: Add Header visibility control
    if (!fVisHeader) fPadHeader->SetPad(xlowH * 10, ylowH * 10, xupH * 10, yupH * 10);

    //fPadHeader->Update();
    DrawHeader();

    switch ( fDisplayMode ) {
    case 0 : // 2D XY+ZR
        fPadXY->cd();
        fPadXY->Update();
        fPadZR->Update();
        break;
    case 1 : // 2D XY
        fPadXY->cd();
        fPadXY->Update();
        break;
    case 2 : // 2D ZR
        fPadZR->cd();
        fPadZR->Update();
        break;
    case 3 : // 3D
        fPad3D->cd();
        fPad3D->Update();
        break;
    case 4 : // 2D+3D
        fPad3D->cd();
        fPad3D->Update();
        break;
    //Long Peixun's update: remove useless code
    }
}

void BesVisDisplay::SwitchPad()
{
    if (fDisplayMode == 0) {
        if ((TPad*)gPad == fPadXY) fPadZR->cd();
        else fPadXY->cd();
    }

    if (fDisplayMode == 4) {
        if ((TPad*)gPad == fPadXY) fPadZR->cd();
        else if ((TPad*)gPad == fPadZR) fPad3D->cd();
        else fPadXY->cd();
    }

    fCanvas->Update();
}


//_____________________________________________________________________________
void BesVisDisplay::DisplayTrigger(Int_t trig)
{
//    Display trigger parameters for current event
    trig = 1;

    if (trig) fEM1->SetFillColor(kGreen);
    else fEM1->SetFillColor(kRed);
    if (trig) fPH1->SetFillColor(kGreen);
    else fPH1->SetFillColor(kRed);
    if (trig) fEM2->SetFillColor(kGreen);
    else fEM2->SetFillColor(kRed);
    if (trig) fMU1->SetFillColor(kGreen);
    else fMU1->SetFillColor(kRed);
    if (trig) fMU2->SetFillColor(kGreen);
    else fMU2->SetFillColor(kRed);
    if (trig) fEMU->SetFillColor(kGreen);
    else fEMU->SetFillColor(kRed);
    if (trig) fJT1->SetFillColor(kGreen);
    else fJT1->SetFillColor(kRed);
    if (trig) fJT3->SetFillColor(kGreen);
    else fJT3->SetFillColor(kRed);
    if (trig) fJT4->SetFillColor(kGreen);
    else fJT4->SetFillColor(kRed);
    if (trig) fALL->SetFillColor(kGreen);
    else fALL->SetFillColor(kRed);
    fTrigPad->Modified();
}

//______________________________________________________________________________
Int_t BesVisDisplay::DistancetoPrimitive(Int_t px, Int_t py)
{
// Compute distance from point px,py to objects in event

    if (fBesHeader) return fBesHeader->DistancetoPrimitive(px, py)-10;
    else return 9999;

    if (gPad == fTrigPad) {
        gPad->SetCursor(kCross);
        return 0;
    }

    const Int_t big = 9999;
    Int_t dist = big;
    Float_t xmin = gPad->GetX1();
    Float_t xmax = gPad->GetX2();
    Float_t dx   = 0.05*(xmax - xmin);
    Float_t x    = gPad->AbsPixeltoX(px);
    if (x < xmin+dx || x > xmax-dx) return dist;

    // scan list of particles
    //dist = fParticle->DistancetoPrimitive(px, py);
    //if (dist <= 0) return 0;

    // scan list of fruits
    /*
      TIter nextf(fFruits);
    BesVisFruit *fruit;
    while((fruit=(BesVisFruit*)nextf())) {
       dist = fruit->DistancetoPrimitive(px, py);
       if (dist < 5) {
          gPad->SetSelected(fruit->Fruit());
          gPad->SetCursor(kCross);
          return 0;
       }
    }
    */

    /*
    // scan list of detectors (currently only one tube)
    dist = fNodin->DistancetoPrimitive(px, py);
    if (gPad->GetCanvas()->GetSelected() == gPad->GetView()) {
       gPad->SetSelected(this);
    }
    */

    return 0;
}

//_____________________________________________________________________________
void BesVisDisplay::Draw(Option_t *)
{
//    Insert current event in graphics pad list

    //fPad->cd();

    //fCanvas->Range(0,0,1,1);
    Draw2D();
    Draw3D();
    SwitchDisplayMode(0);
    //DrawView(fTheta, fPhi);
    //fPad->x3d();
    /*
    TViewerX3D *x3dPad = new TViewerX3D(fPad);
    x3dPad->CreateScene("x3d");
    //x3dPad->SetForegroundColor(x3dPad->GetWhitePixel());
    //x3dPad->SetBackgroundColor(x3dPad->GetWhitePixel());
    x3dPad->Show();
    */

    // Display the event number and title
    //fPad->cd();
    //DrawTitle();
}

//_____________________________________________________________________________
void BesVisDisplay::Draw2D(Option_t *)
{
    cout << "Create 2D View " << endl;
    fPadXY->cd();
    fPadXY->Clear();
    Draw2DXY();
    fPadXY->Update();

    fPadZR->cd();
    fPadZR->Clear();
    Draw2DZR();
    fPadZR->Update();

    //DrawTitle();

    //fPad->SetEditable(kFALSE);
}

//_____________________________________________________________________________
void BesVisDisplay::Draw3D(Option_t *)
{
    cout << "enter BesVisDisplay::Draw3D()" << endl; // added by tianhl to debug shutdown

    fPad3D->cd();
    fPad3D->Clear();

    f3DView = new BesView();
    f3DView->View3D();
}

//_____________________________________________________________________________
void BesVisDisplay::Draw2DXY(Option_t *)
{
    fxyView = new BesView();
    fxyView->ViewXY();
}


//_____________________________________________________________________________
void BesVisDisplay::Draw2DZR(Option_t *)
{
    fzrView = new BesView();
    fzrView->ViewZR();
}

//_____________________________________________________________________________
void BesVisDisplay::DrawAllViews()
{
//    Draw front,top,side and 30 deg views

    fDrawAllViews = kTRUE;
    fPad->cd();
    fPad->SetFillColor(15);
    fPad->Clear();
    fPad->Divide(2,2);

    // draw 30 deg view
    fPad->cd(1);
    DrawView(30, 30);
    DrawTitle();

    // draw front view
    fPad->cd(2);
    DrawView(0, 180); //0,-90, 0
    DrawTitle("Front");

    // draw top view
    fPad->cd(3);
    DrawView(90, 90);
    DrawTitle("Top");

    // draw side view
    fPad->cd(4);
    DrawView(90, 0);
    DrawTitle("Side");

    fPad->cd(2);
}

//_____________________________________________________________________________
void BesVisDisplay::DrawTitle(Option_t *option)
{
//    Draw the event title

    Float_t xmin = gPad->GetX1();
    Float_t xmax = gPad->GetX2();
    Float_t ymin = gPad->GetY1();
    Float_t ymax = gPad->GetY2();
    Float_t dx   = xmax-xmin;
    Float_t dy   = ymax-ymin;
    if (strlen(option) == 0) {
        TPaveText *title = new TPaveText(xmin +0.01*dx, ymax-0.09*dy, xmin +0.5*dx, ymax-0.01*dy);
        title->SetBit(kCanDelete);
        title->SetFillColor(42);
        title->Draw();
        char ptitle[100];
        //sprintf(ptitle,"Pythia event: %d, Run:%d",gBesVisast->Event(), gBesVisast->Run());
        //sprintf(ptitle,"Monte Carlo event: %d",gBesVisast->Event());
        sprintf(ptitle,"Monte Carlo event");
        title->AddText(ptitle);
        //sprintf(ptitle,"Pythia Mode: %s",gBesVisast->MCMaker()->GetTitle());
        //title->AddText(ptitle);
    } else {
        TPaveLabel *label = new TPaveLabel(xmin +0.01*dx, ymax-0.07*dy, xmin +0.2*dx, ymax-0.01*dy,option);
        label->SetBit(kCanDelete);
        label->SetFillColor(42);
        label->Draw();
    }
}

//_____________________________________________________________________________
void BesVisDisplay::DrawView(Float_t theta, Float_t phi)
{
//    Draw a view of ATLAS

    //fPad3D->SetFillColor(10);
    // Display ATLAS outline
    gPad->Clear();

    //TImage *i1 = TImage::Open("rose512.jpg");
    //i1->SetConstRatio(kFALSE);
    //gPad->cd();
    //i1->Draw();

    Int_t iret;
    BesTView *view = new BesTView(1);
    view->SetRange(-0.5*fBesR, -0.5*fBesR, -0.5*fBesZ, 0.5*fBesR, 0.5*fBesR, 0.5*fBesZ);
    view->ZoomView(gPad, 1.2);
    view->AdjustPad();      //Long Peixun's update: Adjust Pad

    //fNodin->Draw("same");
    //fBes->Draw("same");
    fBesGeometry->Draw("3D");

    // add itself to the list
    //AppendPad();

    //Loop on all makers to add their products to the pad
    /*
      TIter next(gBesVisast->Makers());
    BesVisMaker *maker;
    while ((maker = (BesVisMaker*)next())) {
       maker->Draw();
    }
    */

    //view->SetView(phi, theta, 90, iret);  //previous side view necessary
    view->SetView(phi, theta, 270, iret);
    //view->SetParralel();  //-----------liangyt
    view->SetParallel();
    //use SetParallel() instead of depreciated function TView::SetParralel()
    //changed by tianhl at Mon Aug 20 2007
    //view->SetPerspective();
    //view->ShowAxis();

    //DrawViewRange(0, 0, 0, 2500);
}

//_____________________________________________________________________________
void BesVisDisplay::DrawViewRange(Double_t x0, Double_t y0, Double_t z0,
                                  Double_t zoomSize)
{
//    Draw a view of some place

    gPad->GetView()->SetRange(x0 - 0.5*zoomSize, y0 - 0.5*zoomSize, z0 - 0.5*zoomSize,
                              x0 + 0.5*zoomSize, y0 + 0.5*zoomSize, z0 + 0.5*zoomSize);
}

//_____________________________________________________________________________
void BesVisDisplay::DrawViewGL()
{
//    Draw current view using OPENGL

    TPad *pad = (TPad*)gPad->GetPadSave();
    pad->cd();
    TView *view = pad->GetView();
    if (!view) return;
    pad->x3d("OPENGL");//-----------liangyt
}

//_____________________________________________________________________________
void BesVisDisplay::DrawViewX3D()
{
//    Draw current view using X3D

    TPad *pad = (TPad*)gPad->GetPadSave();
    pad->cd();
    TView *view = pad->GetView();
    if (!view) return;
    pad->x3d();//-----------liangyt
}

//_____________________________________________________________________________
void BesVisDisplay::DrawViewRaytracer(Float_t theta, Float_t phi, Float_t psi)
{
    //TCanvas *rayCanvas = new TCanvas("rayCanvas", "Raytracer Canvas", 10, 10, 800, 800);
    TCanvas *rayCanvas = new TCanvas("rayCanvas", "Raytracer Canvas", 10, 10, 800, 800);
    rayCanvas->cd();


    //TPad *rayPad = new TPad("rayPad", "Raytracer display",0.,0.,1,1);
    //rayPad->Draw();
    //rayPad->Modified();
    gPad->SetFillColor(1);

    //rayPad->SetBorderSize(2);
    //rayPad->cd();
    //fMucGeo->GetVolMuc()->Draw();
    //fBes->Draw();

    TView *view = gPad->GetView();
    //view->SetPerspective();
    int iret;
    view->ZoomView(gPad, 1.5);
    view->SetView(phi, theta, psi, iret);
    //   |
    //  \|/

    //gGeoManager->GetGeomPainter()->SetBombFactors(2, 2, 2, 2);
    //gGeoManager->GetGeomPainter()->SetExplodedView(3);

    gGeoManager->GetGeomPainter()->SetRaytracing(kTRUE);

    rayCanvas->Modified();
    rayCanvas->Update();
    //rayPad->Modified();
    //rayPad->Update();

    stringstream sPicCount;
    sPicCount << fRayPicCount << ".gif";
    string str = sPicCount.str();

    gPad->SaveAs(str.c_str());
    fRayPicCount++;
}

void BesVisDisplay::DrawViewX3D(char option)
{
    TCanvas *x3dCanvas = new TCanvas("x3dCanvas", "X3dtracer Canvas", 10, 10, 800, 800);
    x3dCanvas->cd();

    gPad->SetFillColor(1);
    //x3dPad->SetBorderSize(2);
    //x3dPad->cd();

    //fBes->Draw("x3d");

    //TView *view = gPad->GetView();
    //view->SetPerspective();
    //int iret;
    //view->ZoomView(gPad, 1.2);
    //view->SetView(30, 60, 270, iret);


    //gPad->x3d();//-----------liangyt
    gPad->GetViewer3D();
    //use TPad::GetViewer3D() instead of depreciated function TPad::x3d()
    //changed by tianhl at Mon Aug 20 2007
    //TViewerX3D *x3d = (TViewerX3D*)gPad->GetViewer3D();

    //TViewerX3D *x3d = new TViewerX3D(x3dCanvas, "");
    //x3d->ExecCommand(0,0,option);

    //for (Int_t i=0;i<500;i++) {
    //Int_t px = i%500;
    //Int_t py = (2*i)%200;
    //x3d->ExecCommand(px,py,0);  //rotate
    //if (i%20 >10) x3d->ExecCommand(px,py,'j'); //zoom
    //if (i%20 <10) x3d->ExecCommand(px,py,'k'); //unzoom
    //}


    stringstream sPicCount;
    fRayPicCount = 0;
    sPicCount << fRayPicCount << ".gif";
    string str = sPicCount.str();

    gPad->SaveAs(str.c_str());
    fRayPicCount++;
}

void BesVisDisplay::DrawImageSequence(Int_t thetaStart, Int_t thetaStep)
{
    /*
     fRayPicCount = 0;
     //for(int i = thetaStart; i < thetaStart + 360; i += thetaStep) {
     //DrawViewRaytracer(i, 30, 270);
     //}
     for (int i = 0; i < fTofROOTGeo->GetScinNb(0); i++) {
       for (int j=0; j < 20; j++) {
         fTofROOTGeo->GetPhysicalScin(1, i*20+j)->SetVisibility(1);
       }
       DrawViewRaytracer(30, 30, 270);
     }
    */
}

//______________________________________________________________________________
void BesVisDisplay::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================

    if (gPad->GetView()) {
        gPad->GetView()->ExecuteRotateView(event, px, py);
    }
}

//_____________________________________________________________________________
void BesVisDisplay::GetEvent(Long64_t event)
{
//    Read event in memory

    //   gBesVisast->GetEvent(event);

    //Draw();

    //gGeoManager->ClearAttributes();
    SetMdcFiredCell();
    SetTofFiredCell();
    SetEmcFiredCell();
    SetMucFiredCell();
}

//_____________________________________________________________________________
void BesVisDisplay::Paint(Option_t *)
{
//    Paint miscellaneous items

}

//_____________________________________________________________________________
void BesVisDisplay::PaintFruit(TObject *obj, Float_t eta, Float_t phi, Float_t pt, Int_t type, Option_t *option)
{
    //    Display fruit from obj
    /*
      if (fDrawClusters) {
      if (!fFruits) fFruits = new TList();
      BesVisFruit *fruit = new BesVisFruit(obj, eta, phi, pt, type);
      fFruits->Add(fruit);
      fruit->Paint(option);
    }
    */
}

//_____________________________________________________________________________
void BesVisDisplay::PaintParticles(Option_t *option)
{
    //if (fDrawParticles) fParticle->Paint(option);
}

//_____________________________________________________________________________
void BesVisDisplay::SetPTcut(Float_t ptcut)
{
    fPTcut = ptcut;

    if (fDrawAllViews) {
        fPad->cd(1);
        gPad->Modified();
        fPad->cd(2);
        gPad->Modified();
        fPad->cd(3);
        gPad->Modified();
        fPad->cd(4);
        gPad->Modified();
        fPad->cd();
    }
}

//_____________________________________________________________________________
void BesVisDisplay::SetPTcutEGMUNU(Float_t ptcut)
{
    fPTcutEGMUNU = ptcut;

    if (fDrawAllViews) {
        fPad->cd(1);
        gPad->Modified();
        fPad->cd(2);
        gPad->Modified();
        fPad->cd(3);
        gPad->Modified();
        fPad->cd(4);
        gPad->Modified();
        fPad->cd();
    }
}

//_____________________________________________________________________________
void BesVisDisplay::SetView(Float_t theta, Float_t phi)
{
//  change viewing angles for current event

    fPad->cd();
    fDrawAllViews = kFALSE;
    fPhi   = phi;
    fTheta = theta;
    Int_t iret;

    TView *view = gPad->GetView();
    if (view) view->SetView(fPhi, fTheta, 0, iret);
    else      Draw();

    gPad->Modified();
}

//_____________________________________________________________________________
void BesVisDisplay::ShowNextEvent(Int_t delta)
{
//  Display (current event_number+delta)
//    delta =  1  shown next event
//    delta = -1 show previous event

    //fTheta += 10;
    //fPhi += 10;
    if (delta) {
        //gBesVisast->Clear();
        //Int_t current_event = gBesVisast->Event();
        //Int_t new_event     = current_event + delta;
        //gBesVisast->GetEvent(new_event);
        //GetEvent(new_event);
    }
    fPad->cd();
    Draw();
}

//_____________________________________________________________________________
void BesVisDisplay::SetMdcFiredCell()
{
    int layer[135] = {0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,15,16,17,18,19,19,20,21,22,22,23,24,25,26,27,28,29,29,30,31,32,33,34,35,36,37,38,39,40,41,41,42,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,37,38,39,40,41,42,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
    int replica[135] = {12 ,13 ,14 ,17 ,19 ,23 ,25 ,25 ,25 ,26 ,29 ,30 ,33 ,34 ,38 ,38 ,44 ,44 ,48 ,48 ,49 ,51 ,52 ,51 ,52 ,52 ,62 ,63 ,63 ,63 ,68 ,68 ,69 ,68 ,69 ,79 ,80 ,79 ,80 ,91 ,91 ,91 ,92 ,103,103,104,103,4  ,7  ,9  ,4  ,9  ,4  ,9  ,9  ,8  ,8  ,55 ,56 ,63 ,64 ,71 ,71 ,82 ,82 ,89 ,90 ,99 ,100,99 ,100,120,120,120,120,130,131,130,131,151,151,151,151,166,166,167,166,167,187,188,188,7  ,9  ,9  ,12 ,13 ,15 ,16 ,17 ,17 ,17 ,20 ,20 ,22 ,23 ,25 ,26 ,29 ,30 ,32 ,33 ,34 ,35 ,34 ,35 ,42 ,42 ,42 ,42 ,45 ,46 ,46 ,46 ,46 ,53 ,54 ,53 ,54 ,61 ,62 ,61 ,62 ,69 ,70 ,70};

    for (int i = 0; i < 135; i++) {
        fMdcROOTGeo->GetPhysicalReplica(layer[i], replica[i])->SetVisibility(1);
        if (layer[i] > 35) fMdcROOTGeo->GetPhysicalReplica(layer[i]+7, replica[i])->SetVisibility(1);
        std::cout << layer[i] << "  " << replica[i] << std::endl;
    }
}

//_____________________________________________________________________________
void BesVisDisplay::SetTofFiredCell()
{
    int part[7] = {1,1,1,2,2,1,1,};
    int scin[7] = {31,32,57,31,32,59,21};

    std::cout << "set tof fired cell" << std::endl;
    for (int i = 0; i < 7; i++) {
        fTofROOTGeo->GetPhysicalScin(part[i], 0, scin[i])->SetVisibility(1);
        std::cout << part[i] << "  " << scin[i] << std::endl;
    }
}

//_____________________________________________________________________________
void BesVisDisplay::SetEmcFiredCell()
{
    int theta[26] = {3,3,2,9,9,8,5,7,6,10,10,7,8,9,9,9,37,2,7,7,4,4,5,5,15,15};
    int phi[26] = {45,46,46,80,81,80,83,81,81,81,80,80,81,82,83,84,17,84,77,78,79,80,80,79,31,32};

    for (int i = 0; i < 26; i++) {
        fEmcROOTGeo->GetPhysicalCrystal(1, phi[i], theta[i])->SetVisibility(1);
        std::cout << "1" << "  " << phi[i] << " " << theta[i] << std::endl;
    }
}

//_____________________________________________________________________________
void BesVisDisplay::SetMucFiredCell()
{
    vector<int> cellCon;

    int part[11] = {1,1,1,1,1,1,1,1,1,1,1};
    int seg[11]  = {6,6,2,2,2,2,2,2,2,2,2};
    int gap[11]  = {0, 1, 0, 1, 2, 2, 3, 4, 5, 6, 7};
    int strip[11]= {2,20,20,29,21,22,27,25,25,30,22};

    for (int i = 0; i < 11; i++) {
        fMucROOTGeo->GetPhysicalStrip(part[i], seg[i], gap[i], strip[i])->SetVisibility(1);
        std::cout << " " << part[i] << " " << seg[i] << " " << gap[i] << " " << strip[i] << std::endl;
    }
}

//______________________________________________________________________________
void BesVisDisplay::SizeFruit() const
{
    const Int_t npoints = 2;
    gSize3D.numPoints += npoints;
    gSize3D.numSegs   += (npoints-1);
    gSize3D.numPolys  += 0;
}

//______________________________________________________________________________
void BesVisDisplay::SizeParticles() const
{
    //if (fDrawParticles)  fParticle->SizeParticles();
    //printf("numPoints=%d, numSegs=%d, numPolys=%d\n",gSize3D.numPoints,gSize3D.numSegs,gSize3D.numPolys);
}


//_____________________________________________________________________________
void BesVisDisplay::Clear(Option_t *)
{
//    Delete graphics temporary objects

}

//_____________________________________________________________________________
//Long Peixun's update: Set visibility of header
void BesVisDisplay::SetVisHeader(Bool_t val)
{
    fVisHeader = val;
    ResizePad();
}
