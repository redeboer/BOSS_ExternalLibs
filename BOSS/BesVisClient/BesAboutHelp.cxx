//
// BesAboutHelp.cxx
//

static char* rcsname="$Name: BesVisClient-00-04-10 $";
static char* rcsdate="$Date: 2018/11/06 00:17:31 $";

#include "BesVisClient/BesAboutHelp.h"
#include <iostream>

#include <TGLabel.h>
#include <TList.h>
#include <TEnv.h>
#include <TVirtualX.h>
#include <TGButton.h>
#include <TGClient.h>
#include <TGTab.h>
#include <TGLayout.h>
#include <TGListBox.h>
#include <TString.h>
#include <TGIcon.h>
#include <TGPicture.h>
#include <TASImage.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


//#ifndef __CINT__
ClassImp(BesAboutHelp)
//#endif


//_________________________________________________________________
// BesAboutHelp
// Information about Bes Vis Info
//
//
BesAboutHelp::BesAboutHelp(const TGWindow *p, const TGWindow *main) :
        TGTransientFrame(p, main, 360, 750) {
    //
    // BesAboutHelp normal constructor

    fWidgets = new TList();

    this->SetBackgroundPixmap(GetPic("AboutBckgnd.gif"));

    // Create title frame
    fTitleFrame = new TGHorizontalFrame(this, 10, 10);
    fTitleFrame->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    // Add icon
    //const TGPicture *icon = fClient->GetPicture("mb_asterisk_s.xpm");
    //fIcon = new TGIcon(fTitleFrame, icon, icon->GetWidth(), icon->GetHeight());
    //fLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5);
    //fWidgets->Add(fLayout);
    //fTitleFrame->AddFrame(fIcon, fLayout);

    // Create title
    FontStruct_t titlefont;
    titlefont = gClient->GetFontByName(gEnv->GetValue("Gui.BoldFont",
                                       "-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));

    GCValues_t gval;
    gval.fMask = kGCFont;
    gval.fFont = gVirtualX->GetFontHandle(titlefont);
    GContext_t titlegc = gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval);

    TString title("BesVis ");
    title.Append(TString(TString(rcsname).Strip(TString::kBoth, '$')).Remove(0,5).Data());
    fTitle = new TGLabel(fTitleFrame, title.Data(), titlegc);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 5, 5, 5);
    fWidgets->Add(fLayout);
    //fTitleFrame->AddFrame(fTitle, fLayout);

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 75, 0);
    fWidgets->Add(fLayout);
    AddFrame(fTitleFrame, fLayout);

    // Create tabs
    fMain = new TGTab(this, 300, 650);
    fMain->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    TGCompositeFrame *tf;

    // About
    tf = fMain->AddTab("About");
    tf->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    fAbout = new TGVerticalFrame(tf, 200, 750);
    fAbout->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY);
    fWidgets->Add(fLayout);
    tf->AddFrame(fAbout, fLayout);

    fDescription1 = new TGLabel(fAbout, "BESIII  Visualizaion Software");
    fDescription1->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY);
    fWidgets->Add(fLayout);
    fAbout->AddFrame(fDescription1, fLayout);

    TString date((TString(TString(rcsdate).Strip(TString::kBoth, '$')).Remove(0,5)).Data());
    fDescription2 = new TGLabel(fAbout, ""/*date.Data()*/);
    fDescription2->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY);
    fWidgets->Add(fLayout);

    fAbout->AddFrame(fDescription2, fLayout);
    fDescription3 = new TGLabel(fAbout, "Developed by Zhengyun YOU in Peking University");
    fDescription3->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY);
    fWidgets->Add(fLayout);
    fAbout->AddFrame(fDescription3, fLayout);

    fDescription4 = new TGLabel(fAbout, "Emai: youzy@hep.pku.cn");
    fDescription4->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY);
    fWidgets->Add(fLayout);
    fAbout->AddFrame(fDescription4, fLayout);

    // Version
    tf = fMain->AddTab("Version");
    tf->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    fVersion = new TGListBox(tf);
    fVersion->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    tf->AddFrame(fVersion, fLayout);


    fVersion->AddEntry("2005-12-23", 0);
    fVersion->AddEntry("    V 1.0.0", 0);
    //Long Peixun's update: Add about information
    fVersion->AddEntry("2018-4-28", 0);
    fVersion->AddEntry("    V 2.0.0.B, contains MRPC", 0);
    fVersion->AddEntry("", 0);
    //for (Int_t iEntry = 0; iEntry < fVersion->GetNumberOfEntries(); iEntry++) {
    //fVersion->GetEntry(iEntry)->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    //}
    fVersion->Resize(80, 80);

    for (Int_t iTab = 0; iTab < fMain->GetNumberOfTabs(); iTab++) {
        fMain->GetTabTab(iTab)->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    }

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
    fWidgets->Add(fLayout);
    AddFrame(fMain, fLayout);

    // Create bottom frame
    fBottomFrame = new TGHorizontalFrame(this, 10, 10);
    fBottomFrame->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    // Close button
    fCloseBtn = new TGTextButton(fBottomFrame, "    &Close    ", 1);
    fCloseBtn->Associate(this);
    fCloseBtn->SetBackgroundPixmap(GetPic("AboutGrey.gif"));
    fLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    fBottomFrame->AddFrame(fCloseBtn, fLayout);

    fLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5, 5, 0, 5);
    fWidgets->Add(fLayout);
    AddFrame(fBottomFrame, fLayout);

    // Palette
    tf = fMain->AddTab("Palette");
    tf->SetBackgroundPixmap(GetPic("AboutGrey.gif"));

    fMdcPaletteFrame= new TGVerticalFrame(tf, 200, 700);
    fMdcPaletteFrame->SetBackgroundPixmap(GetPic("MdcPalette.gif"));
    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY);
    fWidgets->Add(fLayout);
    //// Create Mdc Palette frame
    //fMdcPaletteFrame = new TGHorizontalFrame(this, 10, 10);
    //fMdcPaletteFrame->SetBackgroundPixmap(GetPic("MdcPalette.gif"));
    //fLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 2, 2, 2, 2);
    //fWidgets->Add(fLayout);
    //fBottomFrame->AddFrame(fMdcPaletteFrame, fLayout);

    //fLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5, 5, 0, 5);

    // Widget layout
    SetWindowName("About BesVis");
    MapSubwindows();

    // position relative to the parent's window
    Int_t  ax, ay;
    Window_t wdum;
    gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                                    (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                                    (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                                    ax, ay, wdum);
    Move(ax, ay);
    SetWMPosition(ax, ay);

    Resize(GetDefaultSize());
    Resize(360,750);
    MapWindow();
}

//_________________________________________________________________

BesAboutHelp::~BesAboutHelp() {
    //
    // BesAboutHelp default destructor

    fWidgets->Delete();
    delete fWidgets;

    delete fCloseBtn;
    delete fTitle;
    //delete fIcon;
    delete fMain;
    delete fAbout;
    delete fVersion;
    delete fDescription1;
    delete fDescription2;
    delete fDescription3;
    delete fDescription4;
    delete fTitleFrame;
    delete fBottomFrame;

}

//_________________________________________________________________

Bool_t BesAboutHelp::ProcessMessage(Long_t msg, Long_t parm, Long_t) {
    //
    // Handle message send to the BesAboutHelp object
    switch ( GET_MSG(msg) ) {
    case kC_COMMAND:
        switch ( GET_SUBMSG(msg) ) {
        case kCM_BUTTON:
            delete this;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return kTRUE;
}

Pixmap_t BesAboutHelp::GetPic(const char *file)
{
    TString filePath = TString(getenv("BESVISLIBROOT"));
    filePath += "/icons/";
    filePath += file;

    TASImage asImage(filePath);
    Pixmap_t pic = asImage.GetPixmap();
    //asImage->Draw();
    return pic;
}
