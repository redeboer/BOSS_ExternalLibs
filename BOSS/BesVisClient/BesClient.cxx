//
// BesClient.cxx
//


#include "BesVisClient/BesClient.h"
#include "BesVisClient/BesAboutHelp.h"
#include "BesVisClient/BesMdcPalette.h"
#include "BesVisLib/BesVisDisplay.h"
#include "BesVisLib/BesCursor.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include <TList.h>
#include <TRootEmbeddedCanvas.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TInterpreter.h>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TGeoManager.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TGLayout.h>
#include <TGStatusBar.h>
#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGProgressBar.h>
#include <TGTab.h>
#include <TGSplitter.h>
#include <TGCanvas.h>
#include <TSystem.h>
#include <TGListTree.h>
#include <TGLabel.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TG3DLine.h>
#include <TGSlider.h>
#include <TCanvas.h>
#include <TView.h>
#include <TGeometry.h>
#include <TASImage.h>
#include <TColor.h>
#include <TViewerX3D.h>
// tianhl, TViewerOpneGL has been removed; waiting for new Class
//#include <TViewerOpenGL.h>
#include <TVirtualViewer3D.h>
#include <TGLViewer.h>
//#include <TGLEditor.h>
#include <TVirtualGL.h>
#include <TVirtualGeoPainter.h>
#include <TRootHelpDialog.h>
#include <TRootDialog.h>

#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TDisTrack.h"
#include "RootEventData/TRecEvTime.h"
#include "BesVisLib/DstConvert.h"       //Long Peixun's update: Convert Dst to Rec
using namespace std;

//#ifndef __CINT__
ClassImp(BesClient)
//#endif

//Long Peixun's update: Remove help text in program
/* const char gHelpBesVis[] = "\
                           PRESS \n\
                           \tg\t--- save graph\n\
                           \tn\t--- next event\n\
                           \tp\t--- previous event\n\n\
                           \tw\t--- move up\n\
                           \ta\t--- move left\n\
                           \ts\t--- move down\n\
                           \td\t--- move right\n\n\
                           \tq\t--- zoom out\n\
                           \te\t--- zoom in\n\n" //hehe, too long string literal :)))
                           "\n\
                           Add more..."; */

const char *OpenGeoTypes[] = {"ROOT files", "*.root",
                              "GDML files", "*.gdml",
                              0, 0
                             };

//Long Peixun's update: adjust the order of file types
const char *SaveGeoTypes[] = {
                              "ROOT files", "*.root",
                              "REC files", "*.rec",
                              0, 0
                             };

//Long Peixun's update: adjust the order of file types
const char *OpenEventTypes[] = {
                                "rec files", "*.rec",
                                "raw files", "*.rtraw",
                                "dst files", "*.dst",   //Long Peixun's update: Allow dst files
                                "ROOT files", "*.root",
//#ifdef CF__ZSMSM
                                //"Generic Adamo files", "*.cz",    //Long Peixun's update: remove unsupported file
//#endif
                                0, 0
                               };

const char *SavePicTypes[] = {"GIF",  "*.gif",
                              "JPG",  "*.jpg",
                              "PS",   "*.ps",
                              "EPS",  "*.eps",
                              "XPM",  "*.xpm",
                              "PNG",  "*.png",
                              "TIFF", "*.tiff",
                              0, 0
                             };

const char *SavePicPS[] = {"PS",  "*.ps",
                           0, 0
                          };

//Long Peixun's update: for BesVis style file
const char* StyleTypes[] = {
                            "Text file", "*.txt",
                            0, 0
                           };

extern char	*optarg;
extern int	optint, opterr;

struct sembuf acquire = {
    0, -1, SEM_UNDO
};
struct sembuf release = {
    0,  1, SEM_UNDO
};


//__________________________________________________________________
// BesClient
// Bes event display client
//
//
BesClient::BesClient(const TGWindow *p, const char* title,
                     UInt_t width, UInt_t height, Option_t *option, Int_t argc, char **argv) :
        TGMainFrame(p, width, height) {

    //
    // BesClient default contructor
    if ( gDebug ) cout << "BesClient ctor called" << endl;

    //// acquire semaphore, decrease resource supply
    //acquire.sem_num = 0;
    //acquire.sem_op = -1;
    //acquire.sem_flg = SEM_UNDO;

    //// release semaphore, increase resource supply
    //release.sem_num = 0;
    //release.sem_op = 1;
    //release.sem_flg = SEM_UNDO;
    //recdis = NULL;
    recTrack1 = NULL;
    mdchit = NULL;
    //kalTrack = NULL;
    tofTrack = NULL;
    muctrk = NULL;
    emcshower = NULL;
    //  fRecEvent_1 =0;

    fDstEvent = NULL;   //Long Peixun's update: Initialize dst tracks
    fEvent = NULL;      //Long Peixun's update: Initialize fEvent to NULL
    fEventTree = NULL;
    fAutoGeom = false;  //Long Peixun's update: Initialize fAutoGeom
    fCurGeom = TString("");     //Long Peixun's update: Initialize fCurGeom
    isRecEvent = true;  //Long Peixun's update: Initialize isRecEvent

    // init parameters
    this->InitParameter();

    // init local variables
    this->InitLocal();

    // Create toplevel widget
    this->CreateWidget(title, fWindowWidth, fWindowHeight);

    // connections to messaging queues
    this->InitConnections();

    this->SetBackgroundPixmap(GetPic("background.gif"));

    f_bossMode = false;
    int optchar;
    static char	optstring[] = "e:g:B:";
    opterr = 0;
    fEventFile = NULL;

    // detail information about getopt can be get by man order in shell
    while ((optchar = getopt(argc, argv, optstring)) != -1)
    {
        switch (optchar) {
        case 'g':
            f_geoFile = optarg;
            cout << "Load Geometry file with arguments" << endl;
            cout << "Geometry: " << f_geoFile << endl;  //Long Peixun's update: GeoFile -> Geometry
            break;
        case 'e':
            f_evtFile = optarg;
            cout << "Load Event file with argments" << endl;
            cout << "Evtfile: " << f_evtFile << endl;
            break;
        case 'B':
            f_bossMode = true;
            f_pid = atoi(optarg);
            cout << "Run in the Boss Framework: " << f_bossMode << endl;
            cout << "parent pid: " << f_pid << endl;
            break;
        case '?':
            cout << "Found an option that was not in optstring!" << endl;
        }
    }
    //Long Peixun's update: It is not necessary to use -e to specify event file
    if (optind < argc)
    {
        if (f_evtFile.Length() == 0)
        {
            f_evtFile = argv[optind];
            cout << "Load Event file with argments" << endl;
            cout << "Evtfile: " << f_evtFile << endl;
        }
    }
        //cout << "Left off at: " << argv[optind] << endl;

    //Long Peixun's update: There is no need to popup Load Geometry dialog box when launch BesVis
    //this->LoadGeoFile();

    if (f_bossMode == false)
    {
        //Long Peixun's update: Open geometry file by using -g
        if (f_geoFile.Length() != 0)
        {
            if (FileExists(f_geoFile)) OpenGeoFile(f_geoFile);
            else
            {
                if (f_geoFile.Contains("/"))
                {
                    HandleError("Geometry file doesn't exist!");
                    f_geoFile = TString("");
                }
                else
                {   //f_geoFile only contains file name
                    //Allow abbreviation of geometry file
                    if (!f_geoFile.EndsWith(".root") && !f_geoFile.EndsWith(".gdml"))
                        f_geoFile = TString("geom_") + f_geoFile + TString(".root");

                    f_geoFile = fBesVisPath + TString("/geom/") + f_geoFile;
                    //try to find geometry file in {$BesVisPath}/geom/
                    if (FileExists(f_geoFile)) OpenGeoFile(f_geoFile);
                    else
                    {
                        HandleError("Geometry file doesn't exist!");
                        f_geoFile = TString("");
                    }
                }
            }
        }
        if (f_evtFile.Length() != 0)
        {
            //Long Peixun's update: Open event file by using -e
            if (FileExists(f_evtFile))
            {
                if (f_geoFile.Length() == 0) OpenEventFile(f_evtFile, true);
                else OpenEventFile(f_evtFile, false);
            }
            else
            {
                HandleError("Event file doesn't exist!");
                f_evtFile = TString("");
            }

            //  this->GetEvent(0);
            //Long Peixun's update: Set status after open event file
            UpdateCurrentPad();
            UpdateBesInputFields();
        }
    }
    else if (f_bossMode == true){
        fAutoDisplayEvent = kTRUE;
	    f_runStatus = RUN_SMOOTH;
        cout << "Load event file: " << f_evtFile << endl;
        fCurrentEvent = 0;
        this->GetRecEvent();
    }

    this->UpdateStatus();
}

//___________________________________________________________________

BesClient::~BesClient() {
    //
    // BesClient standard destructor
    if ( gDebug ) {
        cout << "BesClient dtor called" << endl;
    }

    fWidgets->Delete();
    delete fWidgets;
}

//__________________________________________________________________

void BesClient::CreateWidget(const char* title, UInt_t width, UInt_t height) {
    //
    // Create display

    //CreateHorizontalRuler();

    // Create menu bar
    CreateMenuBar();
    //fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 10, 100, 2, 2);
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 1, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(fMenuBar, fLayout);

    //CreateHorizontalRuler();

    // Create upButton bar
    CreateUpButtonBar();
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 10, 0, 0, 1);
    fWidgets->Add(fLayout);
    this->AddFrame(fUpButtonBarFrame, fLayout);

    //CreateHorizontalRuler();

    // Create title bar
    CreateTitleBar();
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(fTitleFrame, fLayout);

    //CreateHorizontalRuler();

    // Create main frame
    CreateMainFrame();
    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(fMainFrame, fLayout);

    // Create display mode bar
    CreateDisplayModeBar();
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(fDisplayModeBarFrame,  fLayout);

    // Create toolbar
    CreateToolBar();
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 0, 1, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(fToolBarFrame,  fLayout);

    // Create status bar
    CreateStatusBar();
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(fStatusBar, fLayout);

    // Toplevel widget layout
    TString ttitle(title);
    ttitle.Append("@");
    ttitle.Append(gSystem->HostName());
    SetWindowName(ttitle);
    SetIconName(title);
    SetIconPixmap( (fBesVisPath + TString("/icons/LogoBesVisMini.gif")).Data() );
    SetClassHints("BesVis", "Bes Event Visualisation");

    SetWMPosition(10, 10); // position of the window
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    MapSubwindows();
    Resize(GetDefaultSize());
    Resize(width, height);
    MapWindow();
}

void BesClient::CreateHorizontalRuler() {
    //
    // Add horizontal ruler
    // cout << " horizontal ruler" << endl;
    fLayout = new TGLayoutHints(kLHintsExpandX, 2, 2, 0, 0);
    fWidgets->Add(fLayout);
    this->AddFrame(new TGHorizontal3DLine(this), fLayout);
}

//__________________________________________________________________

void BesClient::CreateMenuBar() {
    //
    // Create the menu bar and popup menus

    // Layouts
    fMenuBarFirstItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,
            30, 0, 1, 2);
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,
                                           20, 0, 1, 2);
    fMenuBarLastItemLayout = new TGLayoutHints(kLHintsTop | kLHintsRight,
            20, 20, 1, 2);

    //const TGPicture *label2 = gClient->GetPicture("popMenuLabel2.gif");
    const TGPicture *viewGeometryIcon = gClient->GetPicture("viewGeometry.gif");
    //const TGPicture *normalPopupEntryIcon = gClient->GetPicture("normalPopupEntry1.gif");

    //
    // File menu
    // =========
    fMenuFile = new TGPopupMenu(gClient->GetRoot());
    //fMenuFile->SetBackgroundPixmap(GetPic("2.gif"));
    fMenuFile->AddEntry("Load &Geometry File...", kM_File_LoadGeo);
    fMenuFile->AddEntry("&Save Geometry as...", kM_File_SaveGeoAs);
    fMenuFile->AddEntry("Load &Event File...", kM_File_OpenEvent);
    fMenuFile->AddEntry("Save &Picture as...", kM_File_SavePicAs);
    fMenuFile->AddSeparator();
    fMenuFile->AddEntry("&Quit BesVis",   kM_File_Exit);

    //
    // Edit menu
    // =========
    fMenuEdit = new TGPopupMenu(gClient->GetRoot());
    //fMenuEdit->SetBackgroundPixmap(GetPic("popMenuBorder.gif"));
    fMenuEdit->AddEntry("&Editor", kM_Edit_Editor);

    //
    // View-Option Header menu
    // =======================
    fMenuViewOptionHeader = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionHeader->AddEntry("Show Header", kM_Header_Global);   //Long Peixun's update: kM_Header_On -> kM_Header_Global

    //
    // View-Option Mdc menu
    // ====================
    fMenuViewOptionMdc = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionMdc->AddEntry("Global", kM_Mdc_Global);
    fMenuViewOptionMdc->AddEntry("Tubes",   kM_Mdc_Tubes);
    fMenuViewOptionMdc->AddEntry("Wires",  kM_Mdc_Wires);
    //
    // View-Option Tof menu
    // ====================
    fMenuViewOptionTof = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionTof->AddEntry("Global", kM_Tof_Global);
    fMenuViewOptionTof->AddEntry("East",   kM_Tof_East);
    fMenuViewOptionTof->AddEntry("Barrel", kM_Tof_Barrel);
    fMenuViewOptionTof->AddEntry("West",   kM_Tof_West);
    //
    // View-Option Emc menu
    // ====================
    fMenuViewOptionEmc = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionEmc->AddEntry("Global", kM_Emc_Global);
    fMenuViewOptionEmc->AddEntry("East",   kM_Emc_East);
    fMenuViewOptionEmc->AddEntry("Barrel", kM_Emc_Barrel);
    fMenuViewOptionEmc->AddEntry("West",   kM_Emc_West);
    fMenuViewOptionEmc->AddEntry("Side",   kM_Emc_Side);
    //
    // View-Option Muc menu
    // ====================
    fMenuViewOptionMuc = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionMuc->AddEntry("Global", kM_Muc_Global);
    fMenuViewOptionMuc->AddEntry("East",   kM_Muc_East);
    fMenuViewOptionMuc->AddEntry("Barrel", kM_Muc_Barrel);
    fMenuViewOptionMuc->AddEntry("West",   kM_Muc_West);
    fMenuViewOptionMuc->AddEntry("Strips", kM_Muc_Strips);
    //
    // View-Option Full3D menu
    // ====================
    fMenuViewOptionFull3D = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionFull3D->AddEntry("Mdc", kM_Full3D_Mdc);
    fMenuViewOptionFull3D->AddEntry("Tof", kM_Full3D_Tof);
    fMenuViewOptionFull3D->AddEntry("Emc", kM_Full3D_Emc);
    fMenuViewOptionFull3D->AddEntry("Muc", kM_Full3D_Muc);
    //
    // View-Option Others menu
    // ====================
    fMenuViewOptionOthers = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionOthers->AddEntry("BeamPipe",    kM_BeamPipe);
    fMenuViewOptionOthers->AddEntry("ZRPlaneOnXY", kM_ZRPlaneOnXY);
    fMenuViewOptionOthers->AddEntry("Axis",        kM_Axis);

    //
    // View-Option Mdc Hits menu
    // ====================
    fMenuViewOptionMdcHits = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionMdcHits->AddEntry("Hits",kM_MdcHits_Hits);
    //
    // View-Option Tof Hits menu
    // =========================
    fMenuViewOptionTofHits = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionTofHits->AddEntry("Global Hits", kM_TofHits_Global);
    fMenuViewOptionTofHits->AddEntry("East   Hits", kM_TofHits_East);
    fMenuViewOptionTofHits->AddEntry("Barrel Hits", kM_TofHits_Barrel);
    fMenuViewOptionTofHits->AddEntry("West   Hits", kM_TofHits_West);
    //
    // View-Option Emc Hits menu
    // =========================
    fMenuViewOptionEmcHits = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionEmcHits->AddEntry("Global Hits", kM_EmcHits_Global);
    fMenuViewOptionEmcHits->AddEntry("East   Hits", kM_EmcHits_East);
    fMenuViewOptionEmcHits->AddEntry("Barrel Hits", kM_EmcHits_Barrel);
    fMenuViewOptionEmcHits->AddEntry("West   Hits", kM_EmcHits_West);
    fMenuViewOptionEmcHits->AddEntry("Side   Hits", kM_EmcHits_Side);
    //
    // View-Option Muc Hits menu
    // =========================
    fMenuViewOptionMucHits = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionMucHits->AddEntry("Global Hits", kM_MucHits_Global);
    fMenuViewOptionMucHits->AddEntry("East   Hits", kM_MucHits_East);
    fMenuViewOptionMucHits->AddEntry("Barrel Hits", kM_MucHits_Barrel);
    fMenuViewOptionMucHits->AddEntry("West   Hits", kM_MucHits_West);
    //
    // View-Option Tracks menu
    // =========================
    fMenuViewOptionTracks = new TGPopupMenu(gClient->GetRoot());
    fMenuViewOptionTracks->AddEntry("Global ", kM_Tracks_Global);
    fMenuViewOptionTracks->AddEntry("Mdc ", kM_Tracks_Mdc);
    fMenuViewOptionTracks->AddEntry("Tof ", kM_Tracks_Tof);
    fMenuViewOptionTracks->AddEntry("Emc ", kM_Tracks_Emc);
    fMenuViewOptionTracks->AddEntry("Muc ", kM_Tracks_Muc);
    fMenuViewOptionTracks->AddEntry("Ext ", kM_Tracks_Ext);

    //
    // View menu
    // =========
    fMenuView = new TGPopupMenu(gClient->GetRoot());
    //fMenuView->SetBackgroundPixmap(GetPic("2.gif"));
    fMenuView->AddLabel("General");
    fMenuView->AddSeparator();
    fMenuView->AddPopup("Header",fMenuViewOptionHeader);
    fMenuView->AddSeparator();
    fMenuView->AddLabel("Detector", viewGeometryIcon);
    fMenuView->AddSeparator();
    fMenuView->AddPopup("Mdc",fMenuViewOptionMdc);
    fMenuView->AddPopup("Tof",fMenuViewOptionTof);
    fMenuView->AddPopup("Emc",fMenuViewOptionEmc);
    fMenuView->AddPopup("Muc",fMenuViewOptionMuc);
    //fMenuView->AddPopup("Full3D",fMenuViewOptionFull3D);  //Long Peixun's update: No longer use Full3D
    fMenuView->AddPopup("Others",fMenuViewOptionOthers);
    fMenuView->AddSeparator();
    //fMenuView->AddLabel("DST");
    fMenuView->AddLabel("REC");
    fMenuView->AddSeparator();
    fMenuView->AddPopup("Mdc Hits",fMenuViewOptionMdcHits);
    fMenuView->AddPopup("Tof Hits",fMenuViewOptionTofHits);
    fMenuView->AddPopup("Emc Hits",fMenuViewOptionEmcHits);
    fMenuView->AddPopup("Muc Hits",fMenuViewOptionMucHits);
    fMenuView->AddPopup("Tracks",fMenuViewOptionTracks);
    fMenuView->AddSeparator();
    fMenuView->AddLabel("Special Views");
    fMenuView->AddSeparator();
    //fMenuView->AddEntry("View with X3D", kM_View_X3D, 0, normalPopupEntryIcon);
    fMenuView->AddEntry("View with X3D", kM_View_X3D);
    fMenuView->AddEntry("View with OpenGL", kM_View_OpenGL);

    //
    // Help menu
    // =========
    fMenuHelp = new TGPopupMenu(gClient->GetRoot());
    //fMenuHelp->SetBackgroundPixmap(GetPic("popMenuBorder.gif"));
    fMenuHelp->AddEntry("Help &Content", kM_Help_Content);
    fMenuHelp->AddSeparator();
    fMenuHelp->AddEntry("About", kM_Help_About);

    // Create the menubar and add popup menus
    fMenuBar = new BesGMenuBar(this, 1, 1, kHorizontalFrame);
    fMenuBar->SetBackgroundPixmap(GetPic("MenuBarBckgnd.gif"));

    fMenuBar->AddPopup("&File", fMenuFile, fMenuBarFirstItemLayout);
    //fMenuBar->AddPopup("&Edit", fMenuEdit, fMenuBarItemLayout); // dont show edit
    fMenuBar->AddPopup("&View", fMenuView, fMenuBarItemLayout);
    fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarLastItemLayout);

    fMenuBar->GetMenuTitle(0)->SetPicture(GetPic("MenuFile.gif"));
    fMenuBar->GetMenuTitle(0)->SetPictureHL(GetPic("MenuFileHL.gif"));
    fMenuBar->GetMenuTitle(1)->SetPicture(GetPic("MenuView.gif"));
    fMenuBar->GetMenuTitle(1)->SetPictureHL(GetPic("MenuViewHL.gif"));
    fMenuBar->GetMenuTitle(2)->SetPicture(GetPic("MenuHelp.gif"));
    fMenuBar->GetMenuTitle(2)->SetPictureHL(GetPic("MenuHelpHL.gif"));

    //
    // Connect menu buttons to actions. Menu button messages are
    // handled by the main frame (i.e. "this") HandleMenu() method., kHorizontalFrame);
    fMenuFile->Connect("Activated(Int_t)","BesClient",this,"HandleMenu(Int_t)");
    fMenuEdit->Connect("Activated(Int_t)","BesClient",this,"HandleMenu(Int_t)");
    fMenuHelp->Connect("Activated(Int_t)","BesClient",this,"HandleMenu(Int_t)");
    fMenuView->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionHeader->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionMdc->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionTof->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionEmc->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionMuc->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionFull3D->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionOthers->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionMdcHits->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionTofHits->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionEmcHits->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionMucHits->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");
    fMenuViewOptionTracks->Connect("Activated(Int_t)","BesClient",this,"HandleViewOptionMenu(Int_t)");

    /*
      TASImage *asImage = new TASImage("013.gif");
      Pixmap_t pic1 = asImage->GetPixmap();
      //asImage->Draw();
      Pixmap_t pic1 = gVirtualX->ReadGIF(0, 0, "013.gif");
      fMenuBar->SetBackgroundPixmap(pic1);
    */

    //fMenuBar->SetBackgroundPixmap(GetPic("background.gif"));

    /*
    TList *fMenuTitleList = fMenuBar->GetList();
    cout << "Menu title size " << fMenuTitleList->GetSize() << endl;
    for (Int_t iTitle = 0; iTitle < fMenuTitleList->GetSize(); iTitle++) {
      cout << ((TGMenuTitle*)fMenuTitleList->At(iTitle))->GetName() << endl;
     TGFrameElement *el = (TGFrameElement *)fMenuTitleList->At(iTitle);
     TGMenuTitle    *t = (TGMenuTitle *)el->fFrame;
     t->SetBackgroundPixmap(GetPic("test.gif"));
     cout << t->GetName() << endl;
     t->SetBackgroundColor(kBlack);
     if (iTitle == 0) {
       //t->SetHeight(10);
       t->SetForegroundColor(kWhite);
     }
     }
    */
}

//__________________________________________________________________

void BesClient::CreateUpButtonBar() {
    //
    fUpButtonBarFrame = new TGCompositeFrame(this, 300, 25, kHorizontalFrame);
    fUpButtonBarFrame->SetBackgroundPixmap(GetPic("UpButtonBarBckgnd.gif"));

    fUpButtonBarFirstItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 15,  0, 1, 2);
    fUpButtonBarItemLayout      = new TGLayoutHints(kLHintsTop | kLHintsLeft,  0,  0, 1, 2);
    fUpButtonBarLastItemLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft,  0, 13, 1, 2);

    Int_t width = 23, height = 22;
    Int_t h, charw, w;  // of number entry
    Int_t nUpSplitter = 0;

    // Up Button Splitter
    for (Int_t i = 0; i < kNUpSplitter; i++) {
        fUpButtonSplitter[i] = new BesGPictureButton(fUpButtonBarFrame,
                gClient->GetPicture("UpButtonSplitter.gif"));
        fUpButtonSplitter[i]->SetHeight(20);
        fUpButtonSplitter[i]->SetWidth(5);
        fUpButtonSplitter[i]->SetState(kButtonDisabled);
    }

    // Load Geo File
    fLoadGeoFileButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonLoadGeoFile.gif"),
            kM_Button_LoadGeoFile);
    fLoadGeoFileButton->SetPictureHL(gClient->GetPicture("ButtonLoadGeoFileHL.gif"));
    fLoadGeoFileButton->SetToolTipText("Load Geometry File");
    fLoadGeoFileButton->SetHeight(height);
    fLoadGeoFileButton->SetWidth(width);
    fLoadGeoFileButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fLoadGeoFileButton, fUpButtonBarFirstItemLayout);

    // Open Event File
    fOpenEventFileButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonOpenEventFile.gif"),
            kM_Button_OpenEventFile);
    fOpenEventFileButton->SetPictureHL(gClient->GetPicture("ButtonOpenEventFileHL.gif"));
    fOpenEventFileButton->SetToolTipText("Open Event File");
    fOpenEventFileButton->SetHeight(height);
    fOpenEventFileButton->SetWidth(width);
    fOpenEventFileButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fOpenEventFileButton, fUpButtonBarItemLayout);

    // Save Pic As
    fSavePicAsButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonSavePicAs.gif"),
            kM_Button_SavePicAs);
    fSavePicAsButton->SetPictureHL(gClient->GetPicture("ButtonSavePicAsHL.gif"));
    fSavePicAsButton->SetToolTipText("Save Picture as");
    fSavePicAsButton->SetHeight(height);
    fSavePicAsButton->SetWidth(width);
    fSavePicAsButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fSavePicAsButton, fUpButtonBarItemLayout);

    // Save Pic As PS
    fSavePicAsPSButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonSavePicAsPS.gif"),
            kM_Button_SavePicAsPS);
    fSavePicAsPSButton->SetPictureHL(gClient->GetPicture("ButtonSavePicAsPSHL.gif"));
    fSavePicAsPSButton->SetToolTipText("Save Picture as besvis.ps");
    fSavePicAsPSButton->SetHeight(height);
    fSavePicAsPSButton->SetWidth(width);
    fSavePicAsPSButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fSavePicAsPSButton, fUpButtonBarItemLayout);

    // Up Button Splitter
    fUpButtonBarFrame->AddFrame(fUpButtonSplitter[nUpSplitter], fUpButtonBarItemLayout);
    nUpSplitter++;

    // Refresh
    fRefreshButton = new BesGPictureButton(fUpButtonBarFrame,
                                           gClient->GetPicture("ButtonRefresh.gif"),
                                           kM_Button_Refresh);
    fRefreshButton->SetPictureHL(gClient->GetPicture("ButtonRefreshHL.gif"));
    fRefreshButton->SetToolTipText("Refresh");
    fRefreshButton->SetHeight(height);
    fRefreshButton->SetWidth(width);
    fRefreshButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fRefreshButton, fUpButtonBarItemLayout);

    // Reset Current
    fResetCurrentButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonResetCurrent.gif"),
            kM_Button_ResetCurrent);
    fResetCurrentButton->SetPictureHL(gClient->GetPicture("ButtonResetCurrentHL.gif"));
    fResetCurrentButton->SetToolTipText("Reset Current Pad");
    fResetCurrentButton->SetHeight(height);
    fResetCurrentButton->SetWidth(width);
    fResetCurrentButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fResetCurrentButton, fUpButtonBarItemLayout);

    // Reset All
    fResetAllButton = new BesGPictureButton(fUpButtonBarFrame,
                                            gClient->GetPicture("ButtonResetAll.gif"),
                                            kM_Button_ResetAll);
    fResetAllButton->SetPictureHL(gClient->GetPicture("ButtonResetAllHL.gif"));
    fResetAllButton->SetToolTipText("Reset All Pads");
    fResetAllButton->SetHeight(height);
    fResetAllButton->SetWidth(width);
    fResetAllButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fResetAllButton, fUpButtonBarItemLayout);

    // Up Button Splitter
    fUpButtonBarFrame->AddFrame(fUpButtonSplitter[nUpSplitter], fUpButtonBarItemLayout);
    nUpSplitter++;

    // Zoom Out
    fZoomOutButton = new BesGPictureButton(fUpButtonBarFrame,
                                           gClient->GetPicture("ButtonZoomOut.gif"),
                                           kM_Button_ZoomOut);
    fZoomOutButton->SetPictureHL(gClient->GetPicture("ButtonZoomOutHL.gif"));
    fZoomOutButton->SetToolTipText("Zoom Out");
    fZoomOutButton->SetHeight(height);
    fZoomOutButton->SetWidth(width);
    fZoomOutButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fZoomOutButton, fUpButtonBarItemLayout);

    // Zoom Ratio number
    fZoomRatioNumber = new TGNumberEntryField( fUpButtonBarFrame, kM_Button_ZoomRatioNumber,
            100.0,
            (TGNumberFormat::EStyle) 1, (TGNumberFormat::EAttribute) 0 );
    fZoomRatioNumber->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    h = Int_t(fZoomRatioNumber->GetDefaultHeight()/1.2); // default height 22
    charw = fZoomRatioNumber->GetCharWidth("0123456");
    w = charw * TMath::Abs(5) / 10 + 8 + 2 * h / 3 + 2; // 5 is digit width, default width 46
    //cout << h << " " << w << endl;
    fZoomRatioNumber->Resize(w, h);

    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY);
    fWidgets->Add(fLayout);
    fUpButtonBarFrame->AddFrame(fZoomRatioNumber, fLayout);

    // "%"
    fZoomRatioPercentButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonZoomRatioPercent.gif"));
    fZoomRatioPercentButton->SetHeight(20);
    fZoomRatioPercentButton->SetWidth(10);
    fZoomRatioPercentButton->SetState(kButtonDisabled);
    fUpButtonBarFrame->AddFrame(fZoomRatioPercentButton, fUpButtonBarItemLayout);

    // Zoom In
    fZoomInButton = new BesGPictureButton(fUpButtonBarFrame,
                                          gClient->GetPicture("ButtonZoomIn.gif"),
                                          kM_Button_ZoomIn);
    fZoomInButton->SetPictureHL(gClient->GetPicture("ButtonZoomInHL.gif"));
    fZoomInButton->SetToolTipText("Zoom In");
    fZoomInButton->SetHeight(height);
    fZoomInButton->SetWidth(width);
    fZoomInButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fZoomInButton, fUpButtonBarItemLayout);

    // Cursor Pick
    fCursorButton[0] =  new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonCursorPickST.gif"),
            kM_Button_CursorPick);
    fCursorButton[0]->SetState(true); // with "Button***ST" should set state on
    fCursorButton[0]->SetPictureHL(gClient->GetPicture("ButtonCursorPickHL.gif"));
    fCursorButton[0]->SetToolTipText("Select Tool");
    //fCursorButton[0]->SetState(kButtonEngaged);

    // Cursor Hand
    fCursorButton[1] =  new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonCursorHand.gif"),
            kM_Button_CursorHand);
    fCursorButton[1]->SetPictureHL(gClient->GetPicture("ButtonCursorHandHL.gif"));
    fCursorButton[1]->SetToolTipText("Hand Tool");

    for (int i = 0; i < kNCursorState; i++) {
        fCursorButton[i]->SetHeight(height);
        fCursorButton[i]->SetWidth(width);
        fCursorButton[i]->Connect("Clicked()", "BesClient", this, "HandleButtons()");
        fUpButtonBarFrame->AddFrame(fCursorButton[i], fUpButtonBarItemLayout);
    }

    // Up Button Splitter
    fUpButtonBarFrame->AddFrame(fUpButtonSplitter[nUpSplitter], fUpButtonBarItemLayout);
    nUpSplitter++;

    // Switch Display Mode
    fSwitchDisplayModeButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonSwitchDisplayMode.gif"),
            kM_Button_SwitchDisplayMode);
    fSwitchDisplayModeButton->SetPictureHL(gClient->GetPicture("ButtonSwitchDisplayModeHL.gif"));
    fSwitchDisplayModeButton->SetToolTipText("Switch Display Mode");
    fSwitchDisplayModeButton->SetHeight(height);
    fSwitchDisplayModeButton->SetWidth(width);
    fSwitchDisplayModeButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fSwitchDisplayModeButton, fUpButtonBarItemLayout);

    // Set Home
    fSetHomeButton = new BesGPictureButton(fUpButtonBarFrame,
                                           gClient->GetPicture("ButtonSetHome.gif"),
                                           kM_Button_SetHome);
    fSetHomeButton->SetPictureHL(gClient->GetPicture("ButtonSetHomeHL.gif"));
    fSetHomeButton->SetToolTipText("Set home position");
    fSetHomeButton->SetHeight(height);
    fSetHomeButton->SetWidth(width);
    fSetHomeButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fSetHomeButton, fUpButtonBarItemLayout);

    // Go Home
    fGoHomeButton = new BesGPictureButton(fUpButtonBarFrame,
                                          gClient->GetPicture("ButtonGoHome.gif"),
                                          kM_Button_GoHome);
    fGoHomeButton->SetPictureHL(gClient->GetPicture("ButtonGoHomeHL.gif"));
    fGoHomeButton->SetToolTipText("Go to home position");
    fGoHomeButton->SetHeight(height);
    fGoHomeButton->SetWidth(width);
    fGoHomeButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fGoHomeButton, fUpButtonBarItemLayout);

    // Save My style
    fSaveMyConfigButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonSaveMyConfig.gif"),
            kM_Button_SaveMyConfig);
    fSaveMyConfigButton->SetPictureHL(gClient->GetPicture("ButtonSaveMyConfigHL.gif"));
    fSaveMyConfigButton->SetToolTipText("Save My Style");
    fSaveMyConfigButton->SetHeight(height);
    fSaveMyConfigButton->SetWidth(width);
    fSaveMyConfigButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fSaveMyConfigButton, fUpButtonBarItemLayout);

    // Load My style
    fLoadMyConfigButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonLoadMyConfig.gif"),
            kM_Button_LoadMyConfig);
    fLoadMyConfigButton->SetPictureHL(gClient->GetPicture("ButtonLoadMyConfigHL.gif"));
    fLoadMyConfigButton->SetToolTipText("Load My Style");
    fLoadMyConfigButton->SetHeight(height);
    fLoadMyConfigButton->SetWidth(width);
    fLoadMyConfigButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fLoadMyConfigButton, fUpButtonBarItemLayout);

    // Display mdc wire color palette
    fPaletteButton = new BesGPictureButton(fUpButtonBarFrame,
            gClient->GetPicture("ButtonPalette.gif"),
            kM_Button_Palette);
    fPaletteButton->SetPictureHL(gClient->GetPicture("ButtonPaletteHL.gif"));
    fPaletteButton->SetToolTipText("Palette");
    fPaletteButton->SetHeight(height);
    fPaletteButton->SetWidth(width);
    fPaletteButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fPaletteButton, fUpButtonBarItemLayout);

    // Up Button Splitter
    fUpButtonBarFrame->AddFrame(fUpButtonSplitter[nUpSplitter], fUpButtonBarItemLayout);
    nUpSplitter++;

    // Help
    fHelpButton = new BesGPictureButton(fUpButtonBarFrame,
                                        gClient->GetPicture("ButtonHelp.gif"),
                                        kM_Button_Help);
    fHelpButton->SetPictureHL(gClient->GetPicture("ButtonHelpHL.gif"));
    fHelpButton->SetToolTipText("Help");
    fHelpButton->SetHeight(height);
    fHelpButton->SetWidth(width);
    fHelpButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fUpButtonBarFrame->AddFrame(fHelpButton, fUpButtonBarLastItemLayout);
}

//__________________________________________________________________

void BesClient::CreateDisplayModeBar() {

    fDisplayModeBarFrame = new TGCompositeFrame(this, 300, 16, kHorizontalFrame);
    fDisplayModeBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);

    fDisplayModeButton[0] = new BesGPictureButton(fDisplayModeBarFrame, gClient->GetPicture("DisplayMode2DST.gif"), kM_Button_DisplayMode2D);
    fDisplayModeButton[1] = new BesGPictureButton(fDisplayModeBarFrame, gClient->GetPicture("DisplayModeXY.gif"),  kM_Button_DisplayModeXY);
    fDisplayModeButton[2] = new BesGPictureButton(fDisplayModeBarFrame, gClient->GetPicture("DisplayModeZR.gif"),  kM_Button_DisplayModeZR);
    fDisplayModeButton[3] = new BesGPictureButton(fDisplayModeBarFrame, gClient->GetPicture("DisplayMode3D.gif"),  kM_Button_DisplayMode3D);
    fDisplayModeButton[4] = new BesGPictureButton(fDisplayModeBarFrame, gClient->GetPicture("DisplayModeAll.gif"), kM_Button_DisplayModeAll);

    fDisplayModeButton[0]->SetState(true);
    fDisplayModeButton[1]->SetState(false);
    fDisplayModeButton[2]->SetState(false);
    fDisplayModeButton[3]->SetState(false);
    fDisplayModeButton[4]->SetState(false);

    fDisplayModeButton[0]->SetPictureHL(gClient->GetPicture("DisplayMode2DHL.gif"));
    fDisplayModeButton[1]->SetPictureHL(gClient->GetPicture("DisplayModeXYHL.gif"));
    fDisplayModeButton[2]->SetPictureHL(gClient->GetPicture("DisplayModeZRHL.gif"));
    fDisplayModeButton[3]->SetPictureHL(gClient->GetPicture("DisplayMode3DHL.gif"));
    fDisplayModeButton[4]->SetPictureHL(gClient->GetPicture("DisplayModeAllHL.gif"));

    fDisplayModeButton[0]->SetToolTipText("XY+ZR view");
    fDisplayModeButton[1]->SetToolTipText("XY view");
    fDisplayModeButton[2]->SetToolTipText("ZR view");
    fDisplayModeButton[3]->SetToolTipText("3D view");
    fDisplayModeButton[4]->SetToolTipText("All view");

    Int_t width = 16, height = 16;
    for (Int_t i = 0; i < kNDisplayMode; i++) {
        fDisplayModeButton[i]->SetState(kButtonUp);
        fDisplayModeButton[i]->SetHeight(height);
        fDisplayModeButton[i]->SetWidth(width);
        fDisplayModeButton[i]->Connect("Clicked()", "BesClient", this, "HandleButtons()");
        fDisplayModeBarFrame->AddFrame(fDisplayModeButton[i], fDisplayModeBarItemLayout);
    }
}

//__________________________________________________________________

void BesClient::CreateToolBar() {

    fToolBarFrame = new TGCompositeFrame(this, 300, 25, kHorizontalFrame);
    fToolBarFrame->SetBackgroundPixmap(GetPic("ToolButtonBarBckgnd.gif"));

    fToolBarFirstItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 10,  0, 1, 2);
    fToolBarItemLayout      = new TGLayoutHints(kLHintsTop | kLHintsLeft,  0,  0, 1, 2);
    fToolBarLastItemLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft,  0, 14, 1, 2);

    Int_t width = 23, height = 22;
    Int_t nToolSplitter = 0;

    // Tool Button Splitter
    for (Int_t i = 0; i < kNToolSplitter; i++) {
        fToolButtonSplitter[i] = new BesGPictureButton(fToolBarFrame,
                gClient->GetPicture("ToolButtonSplitter.gif"));
        fToolButtonSplitter[i]->SetHeight(20);
        fToolButtonSplitter[i]->SetWidth(5);
        fToolButtonSplitter[i]->SetState(kButtonDisabled);
    }

    // Switch Pad
    fSwitchPadButton = new BesGPictureButton(fToolBarFrame,
            gClient->GetPicture("ButtonSwitchPad.gif"),
            kM_Button_SwitchPad);
    fSwitchPadButton->SetPictureHL(gClient->GetPicture("ButtonSwitchPadHL.gif"));
    fSwitchPadButton->SetToolTipText("Switch Among Pads");
    fSwitchPadButton->SetHeight(height);
    fSwitchPadButton->SetWidth(width);
    fSwitchPadButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fSwitchPadButton, fToolBarFirstItemLayout);

    // Tool Button Splitter
    fToolBarFrame->AddFrame(fToolButtonSplitter[nToolSplitter], fToolBarItemLayout);
    nToolSplitter++;

    // Show Info
    fShowInfoButton = new BesGPictureButton(fToolBarFrame,
                                            gClient->GetPicture("ButtonShowInfoST.gif"),
                                            kM_Button_ShowInfo);
    fShowInfoButton->SetState(true);
    fShowInfoButton->SetPictureHL(gClient->GetPicture("ButtonShowInfoHL.gif"));
    fShowInfoButton->SetToolTipText("Show Info");
    fShowInfoButton->SetHeight(height);
    fShowInfoButton->SetWidth(width);
    fShowInfoButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fShowInfoButton, fToolBarItemLayout);

    // Show Axis
    fShowAxisButton = new BesGPictureButton(fToolBarFrame,
                                            gClient->GetPicture("ButtonShowAxis.gif"),
                                            kM_Button_ShowAxis);
    fShowAxisButton->SetPictureHL(gClient->GetPicture("ButtonShowAxisHL.gif"));
    fShowAxisButton->SetToolTipText("Show Axis");
    fShowAxisButton->SetHeight(height);
    fShowAxisButton->SetWidth(width);
    fShowAxisButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fShowAxisButton, fToolBarItemLayout);

    // Tool Button Splitter
    fToolBarFrame->AddFrame(fToolButtonSplitter[nToolSplitter], fToolBarItemLayout);
    nToolSplitter++;

    // FishEye View
    fFishEyeViewButton = new BesGPictureButton(fToolBarFrame,
            gClient->GetPicture("ButtonFishEyeView.gif"),
            kM_Button_FishEyeView);
    fFishEyeViewButton->SetPictureHL(gClient->GetPicture("ButtonFishEyeViewHL.gif"));
    fFishEyeViewButton->SetToolTipText("FishEye View");
    fFishEyeViewButton->SetHeight(height);
    fFishEyeViewButton->SetWidth(width);
    fFishEyeViewButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fFishEyeViewButton, fToolBarItemLayout);

    // ParallelView
    fParallelViewButton = new BesGPictureButton(fToolBarFrame,
            gClient->GetPicture("ButtonParallelViewST.gif"),
            kM_Button_ParallelView);
    fParallelViewButton->SetState(true);
    fParallelViewButton->SetPictureHL(gClient->GetPicture("ButtonParallelViewHL.gif"));
    fParallelViewButton->SetToolTipText("Parallel View");
    fParallelViewButton->SetHeight(height);
    fParallelViewButton->SetWidth(width);
    fParallelViewButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fParallelViewButton, fToolBarItemLayout);

    // Perspective View
    fPerspectiveViewButton = new BesGPictureButton(fToolBarFrame,
            gClient->GetPicture("ButtonPerspectiveView.gif"),
            kM_Button_PerspectiveView);
    fPerspectiveViewButton->SetPictureHL(gClient->GetPicture("ButtonPerspectiveViewHL.gif"));
    fPerspectiveViewButton->SetToolTipText("Perspective View");
    fPerspectiveViewButton->SetHeight(height);
    fPerspectiveViewButton->SetWidth(width);
    fPerspectiveViewButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fPerspectiveViewButton, fToolBarItemLayout);

    // Tool Button Splitter
    fToolBarFrame->AddFrame(fToolButtonSplitter[nToolSplitter], fToolBarItemLayout);
    nToolSplitter++;

    // OpenGL
    fOpenGLButton = new BesGPictureButton(fToolBarFrame,
                                          gClient->GetPicture("ButtonOpenGL.gif"),
                                          kM_Button_OpenGL);
    fOpenGLButton->SetPictureHL(gClient->GetPicture("ButtonOpenGLHL.gif"));
    fOpenGLButton->SetToolTipText("OpenGL View");
    fOpenGLButton->SetHeight(height);
    fOpenGLButton->SetWidth(width);
    fOpenGLButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fOpenGLButton, fToolBarItemLayout);

    // X3D
    fX3DButton = new BesGPictureButton(fToolBarFrame,
                                       gClient->GetPicture("ButtonX3D.gif"),
                                       kM_Button_X3D);
    fX3DButton->SetPictureHL(gClient->GetPicture("ButtonX3DHL.gif"));
    fX3DButton->SetToolTipText("X3D View");
    fX3DButton->SetHeight(height);
    fX3DButton->SetWidth(width);
    fX3DButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");
    fToolBarFrame->AddFrame(fX3DButton, fToolBarItemLayout);

    // Tool Button Splitter
    fToolBarFrame->AddFrame(fToolButtonSplitter[nToolSplitter], fToolBarLastItemLayout);
    nToolSplitter++;
}

//__________________________________________________________________

void BesClient::CreateTitleBar() {
    //
    // Create the title bar

    // Create frame
    fTitleFrame = new TGHorizontalFrame(this, this->GetWidth(), 10);
}

//__________________________________________________________________

void BesClient::CreateStatusBar() {
    //
    // Create status bar
    Int_t nPart = 3;
    Int_t parts[] = {40, 50, 10};
    fStatusBar = new TGStatusBar(this, this->GetWidth(), 20, kHorizontalFrame);
    fStatusBar->SetParts(parts, nPart);
    fStatusBar->SetBackgroundPixmap(GetPic("StatusBarBckgnd.gif"));
    for (Int_t iPart = 0; iPart < nPart; iPart++) {
        fStatusBar->GetBarPart(iPart)->SetBackgroundPixmap(GetPic("StatusBarBckgnd.gif"));
    }

    // Set text
    //fStatusBar->SetText("Ready", 0);
    HandleStatusBar(GetCurrentPadString());   //Long Peixun's update: Using HandleStatusBar()
}

//__________________________________________________________________

void BesClient::CreateMainFrame() {
    //
    // Create main frame containing a tab list,
    // the canvas and a toolbar
    fMainFrame = new TGHorizontalFrame(this, this->GetWidth(), this->GetHeight()-26);

    // This vertical frame is needed for splitter
    fV1 = new TGVerticalFrame(fMainFrame, 250, fMainFrame->GetHeight(), kFixedWidth);

    // Create display tabs
    CreateCanvas();

    // Create control tabs
    CreateTabs();

    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 2, 0, 0);
    fWidgets->Add(fLayout);
    fV1->AddFrame(fTabs, fLayout);

    // Configure Splitter
    //   fV1->Resize(fTabs->GetWidth(), fV1->GetDefaultHeight());

    fMainFrame->AddFrame(fEmbeddedCanvas, fECLayout);

    fLayout = new TGLayoutHints(kLHintsRight | kLHintsExpandY);
    fWidgets->Add(fLayout);
    fMainFrame->AddFrame(fV1, fLayout);

    TGVSplitter *VSplitter = new TGVSplitter(fMainFrame);
    VSplitter->SetFrame(fV1, kFALSE);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
    fWidgets->Add(fLayout);
    fMainFrame->AddFrame(VSplitter, fLayout);
}


//__________________________________________________________________

void BesClient::CreateCanvas() {
    //
    // Create the embedded canvas
    // Create embedded canvas
    fECLayout =  new TGLayoutHints( kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
    fEmbeddedCanvas = new TRootEmbeddedCanvas(0, fMainFrame, fMainFrame->GetWidth()/4*3-12, fMainFrame->GetHeight()-46);
    Int_t wid = fEmbeddedCanvas->GetCanvasWindowId();
    fCanvas = new TCanvas("BesVis", fEmbeddedCanvas->GetWidth(), fEmbeddedCanvas->GetHeight()-10, wid);
    fEmbeddedCanvas->AdoptCanvas(fCanvas);
    fCanvas->cd();

    fDisplay = new BesVisDisplay("BESIII Event Display");
    fDisplay->SetCanvas(fCanvas);
    fDisplay->Init();

}

//__________________________________________________________________

void BesClient::CreateTabs() {

    //
    // Create tabs for event list and geometry check button lists
    fTabs = new TGTab(fV1, fV1->GetWidth(), fV1->GetHeight());
    //cout << "Tab width " << fV1->GetWidth() << " height " << fV1->GetHeight() << endl;
    //fTabs->SetBackgroundPixmap(GetPic("2.gif"));

    TGCompositeFrame *tf = 0;
    Int_t h, charw, w;

    //
    // "View" tab
    // ================
    tf = fTabs->AddTab("View");
    tf->SetBackgroundPixmap(GetPic("8.gif"));

    //
    // frame for auto control
    // ===================
    fAutoFrame = new TGCompositeFrame(tf, tf->GetWidth(), tf->GetHeight(), kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX , 15, 15, 15, 10);
    fWidgets->Add(fLayout);
    tf->AddFrame(fAutoFrame, fLayout);
    fAutoFrame->SetBackgroundPixmap(GetPic("AutoFrameBckgnd.gif"));

    // lable "Auto control"
    fAutoTextFrame = new TGCompositeFrame(fAutoFrame, fAutoFrame->GetWidth(), 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fAutoFrame->AddFrame(fAutoTextFrame, fLayout);
    fAutoTextFrame->SetBackgroundPixmap(GetPic("AutoControlBckgnd.gif"));

    fAutoTextLabel = new TGLabel(fAutoTextFrame,"Auto Control  ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX , 2, 73, 4, 0);
    fWidgets->Add(fLayout);
    fAutoTextFrame->AddFrame(fAutoTextLabel, fLayout);
    fAutoTextLabel->SetBackgroundPixmap(GetPic("AutoControlBckgnd.gif"));

    // --- Auto rotate ---
    fFrameAutoRotate = new TGGroupFrame(fAutoFrame, "");
    fLayout = new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0);
    //fLayout = new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5);
    fWidgets->Add(fLayout);
    fAutoFrame->AddFrame(fFrameAutoRotate, fLayout);

    // "Auto"
    fChkBtnAutoRotate = new TGCheckButton(fFrameAutoRotate, "Auto", kM_Button_AutoRotate);
    fLayout = new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 2);
    fFrameAutoRotate->AddFrame(fChkBtnAutoRotate, fLayout);
    fChkBtnAutoRotate->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    // rotate speed
    fRotateSpeedFrame = new TGCompositeFrame(fFrameAutoRotate, fFrameAutoRotate->GetWidth()-20, 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 2, 2);
    fWidgets->Add(fLayout);
    fFrameAutoRotate->AddFrame(fRotateSpeedFrame, fLayout);

    // input of rotate speed
    fNumEntryRotateSpeed = new TGNumberEntryField( fRotateSpeedFrame, kM_Button_RotateSpeed,
            this->GetRotateSpeed(),
            (TGNumberFormat::EStyle) 1, (TGNumberFormat::EAttribute) 1 );
    h = fNumEntryRotateSpeed->GetDefaultHeight();
    charw = fNumEntryRotateSpeed->GetCharWidth("0123456");
    w = charw * TMath::Abs(4) / 10 + 8 + 2 * h / 3; // 4 is digit width
    fNumEntryRotateSpeed->Resize(w, h);

    //fNumEntryRotateSpeed = new TGNumberEntry(fRotateSpeedFrame,this->GetRotateSpeed(), 5 ,kM_Button_RotateSpeed,(TGNumberFormat::EStyle) 1,(TGNumberFormat::EAttribute) 1);
    fNumEntryRotateSpeed->SetHeight(20);
    fNumEntryRotateSpeed->SetNumber(this->GetRotateSpeed());
    fNumEntryRotateSpeed->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryRotateSpeed->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 2, 2);
    fWidgets->Add(fLayout);
    fRotateSpeedFrame->AddFrame(fNumEntryRotateSpeed, fLayout);

    // label " deg/sec "
    fRotateSpeedLabel = new TGLabel(fRotateSpeedFrame," deg/sec   ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 4, 2, 2);
    fWidgets->Add(fLayout);
    fRotateSpeedFrame->AddFrame(fRotateSpeedLabel, fLayout);

    // rotate FPS
    fRotateFPSFrame = new TGCompositeFrame(fFrameAutoRotate, fFrameAutoRotate->GetWidth()-20, 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 2, 2);
    fWidgets->Add(fLayout);
    fFrameAutoRotate->AddFrame(fRotateFPSFrame, fLayout);

    // input of rotate speed
    fNumEntryRotateFPS = new TGNumberEntry(fRotateFPSFrame,this->GetRotateFPS(), 3, kM_Button_RotateFPS,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
    fNumEntryRotateFPS->SetHeight(20);
    fNumEntryRotateFPS->SetNumber(this->GetRotateFPS());
    fNumEntryRotateFPS->GetNumberEntry()->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryRotateFPS->GetNumberEntry()->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 2, 2);
    fWidgets->Add(fLayout);
    fRotateFPSFrame->AddFrame(fNumEntryRotateFPS, fLayout);

    // label " fps "
    fRotateFPSLabel = new TGLabel(fRotateFPSFrame,"  fps       ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 4, 2, 2);
    fWidgets->Add(fLayout);
    fRotateFPSFrame->AddFrame(fRotateFPSLabel, fLayout);


    //
    // frame for view control
    // ===================
    fViewFrame = new TGCompositeFrame(tf, tf->GetWidth(), tf->GetHeight(), kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX , 15, 15, 15, 10);
    fWidgets->Add(fLayout);
    tf->AddFrame(fViewFrame, fLayout);
    fViewFrame->SetBackgroundPixmap(GetPic("ViewFrameBckgnd.gif"));

    // lable "View control"
    fViewTextFrame = new TGCompositeFrame(fViewFrame, fViewFrame->GetWidth(), 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fViewFrame->AddFrame(fViewTextFrame, fLayout);
    fViewTextFrame->SetBackgroundPixmap(GetPic("ViewControlBckgnd.gif"));

    fViewTextLabel = new TGLabel(fViewTextFrame,"View Control  ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX , 2, 73, 4, 4);
    fWidgets->Add(fLayout);
    fViewTextFrame->AddFrame(fViewTextLabel, fLayout);
    fViewTextLabel->SetBackgroundPixmap(GetPic("ViewControlBckgnd.gif"));


    // rotate step
    fRotateStepFrame = new TGCompositeFrame(fViewFrame, fViewFrame->GetWidth()-20, 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 8, 5);
    fWidgets->Add(fLayout);
    fViewFrame->AddFrame(fRotateStepFrame, fLayout);

    // label " Step: "
    fRotateStepLabel = new TGLabel(fRotateStepFrame," Step:");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 10, 2, 0, 0);
    fWidgets->Add(fLayout);
    fRotateStepFrame->AddFrame(fRotateStepLabel, fLayout);

    // input of rotate step
    fNumEntryRotateStep = new TGNumberEntryField( fRotateStepFrame, kM_Button_RotateStep,
            this->GetRotateStep(),
            (TGNumberFormat::EStyle) 2, (TGNumberFormat::EAttribute) 1 );
    h = fNumEntryRotateStep->GetDefaultHeight();
    charw = fNumEntryRotateStep->GetCharWidth("0123456");
    w = charw * TMath::Abs(5) / 10 + 8 + 2 * h / 3; // 5 is digit width
    fNumEntryRotateStep->Resize(w, h);

    fNumEntryRotateStep->SetHeight(20);
    fNumEntryRotateStep->SetNumber(this->GetRotateStep());
    fNumEntryRotateStep->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryRotateStep->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 0, 0, 0);
    fWidgets->Add(fLayout);
    fRotateStepFrame->AddFrame(fNumEntryRotateStep, fLayout);

    // label " deg "
    fRotateStepUnitLabel = new TGLabel(fRotateStepFrame," deg ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 0, 4, 0, 0);
    fWidgets->Add(fLayout);
    fRotateStepFrame->AddFrame(fRotateStepUnitLabel, fLayout);

    // frame ViewContent
    fViewContentFrame = new TGCompositeFrame(fViewFrame, fViewFrame->GetWidth(), fViewFrame->GetHeight()-30, kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fViewFrame->AddFrame(fViewContentFrame, fLayout);

    // frame ViewContent Line1
    fViewContent1Frame = new TGCompositeFrame(fViewContentFrame, fViewFrame->GetWidth(), 39, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 0, 0, 3, 0);
    fWidgets->Add(fLayout);
    fViewContent1Frame->SetBackgroundPixmap(GetPic("ViewContent1Bckgnd.gif"));
    fViewContentFrame->AddFrame(fViewContent1Frame, fLayout);

    // button ViewCounterClockwise
    fViewCounterClockWiseButton = new BesGPictureButton(fViewContent1Frame,
            gClient->GetPicture("ViewCounterClockwise.gif"),
            kM_Button_ViewCounterClockWise);
    fViewCounterClockWiseButton->SetToolTipText("Rotate Counter Clockwise");
    fViewCounterClockWiseButton->SetHeight(37);
    fViewCounterClockWiseButton->SetWidth(37);
    fViewCounterClockWiseButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 22, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent1Frame->AddFrame(fViewCounterClockWiseButton, fLayout);

    // button ViewMoveUp
    fViewMoveUpButton = new BesGPictureButton(fViewContent1Frame,
            gClient->GetPicture("ViewMoveUp.gif"),
            kM_Button_ViewMoveUp);
    fViewMoveUpButton->SetToolTipText("Move Up");
    fViewMoveUpButton->SetHeight(20);
    fViewMoveUpButton->SetWidth(23);
    fViewMoveUpButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent1Frame->AddFrame(fViewMoveUpButton, fLayout);

    // button ViewClockwise
    fViewClockWiseButton = new BesGPictureButton(fViewContent1Frame,
            gClient->GetPicture("ViewClockwise.gif"),
            kM_Button_ViewClockWise);
    fViewClockWiseButton->SetToolTipText("Rotate Clockwise");
    fViewClockWiseButton->SetHeight(37);
    fViewClockWiseButton->SetWidth(37);
    fViewClockWiseButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 22, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent1Frame->AddFrame(fViewClockWiseButton, fLayout);

    // frame ViewContent Line2
    fViewContent2Frame = new TGCompositeFrame(fViewContentFrame, fViewFrame->GetWidth(), 23, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent2Frame->SetBackgroundPixmap(GetPic("ViewContent2Bckgnd.gif"));
    fViewContentFrame->AddFrame(fViewContent2Frame, fLayout);

    // button ViewMoveLeft
    fViewMoveLeftButton = new BesGPictureButton(fViewContent2Frame,
            gClient->GetPicture("ViewMoveLeft.gif"),
            kM_Button_ViewMoveLeft);
    fViewMoveLeftButton->SetToolTipText("Move Left");
    fViewMoveLeftButton->SetHeight(23);
    fViewMoveLeftButton->SetWidth(23);
    fViewMoveLeftButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 49, 2, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent2Frame->AddFrame(fViewMoveLeftButton, fLayout);

    // button ViewMoveMoveCenter
    fViewMoveCenterButton = new BesGPictureButton(fViewContent2Frame,
            gClient->GetPicture("ViewMoveCenter.gif"),
            kM_Button_ViewMoveCenter);
    fViewMoveCenterButton->SetToolTipText("Move Center");
    fViewMoveCenterButton->SetHeight(13);
    fViewMoveCenterButton->SetWidth(13);
    fViewMoveCenterButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent2Frame->AddFrame(fViewMoveCenterButton, fLayout);

    // button ViewMoveRight
    fViewMoveRightButton = new BesGPictureButton(fViewContent2Frame,
            gClient->GetPicture("ViewMoveRight.gif"),
            kM_Button_ViewMoveRight);
    fViewMoveRightButton->SetToolTipText("Move Right");
    fViewMoveRightButton->SetHeight(23);
    fViewMoveRightButton->SetWidth(23);
    fViewMoveRightButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 2, 49, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent2Frame->AddFrame(fViewMoveRightButton, fLayout);

    // frame ViewContent Line3
    fViewContent3Frame = new TGCompositeFrame(fViewContentFrame, fViewFrame->GetWidth(), 32, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 0, 0, 0, 3);
    fWidgets->Add(fLayout);
    fViewContent3Frame->SetBackgroundPixmap(GetPic("ViewContent3Bckgnd.gif"));
    fViewContentFrame->AddFrame(fViewContent3Frame, fLayout);

    // button ViewMoveDown
    fViewMoveDownButton = new BesGPictureButton(fViewContent3Frame,
            gClient->GetPicture("ViewMoveDown.gif"),
            kM_Button_ViewMoveDown);
    fViewMoveDownButton->SetToolTipText("Move Down");
    fViewMoveDownButton->SetHeight(32);
    fViewMoveDownButton->SetWidth(23);
    fViewMoveDownButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    //fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 69, 69, 0, 0);
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 69, 0, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent3Frame->AddFrame(fViewMoveDownButton, fLayout);

    // reset angle button
    fViewResetAngleButton = new BesGPictureButton(fViewContent3Frame,
            gClient->GetPicture("ViewResetAngle.gif"),
            kM_Button_ViewResetAngle);

    fViewResetAngleButton->SetToolTipText("Reset Angles");
    fViewResetAngleButton->SetHeight(27);
    fViewResetAngleButton->SetWidth(27);
    fViewResetAngleButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 30, 0, 0, 0);
    fWidgets->Add(fLayout);
    fViewContent3Frame->AddFrame(fViewResetAngleButton, fLayout);


    // frame ViewAngle
    fViewAngleFrame = new TGCompositeFrame(fViewContentFrame, fViewContentFrame->GetWidth(), 50, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 4, 4, 0, 0);
    fWidgets->Add(fLayout);
    fViewContentFrame->AddFrame(fViewAngleFrame, fLayout);

    // frame ViewAngleTheta
    fViewAngleThetaFrame = new TGCompositeFrame(fViewAngleFrame, fViewAngleFrame->GetWidth(), 50, kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 3, 3, 3, 3);
    fWidgets->Add(fLayout);
    fViewAngleFrame->AddFrame(fViewAngleThetaFrame, fLayout);

    // button ViewAngleThetaMinus
    fViewAngleThetaMinusButton = new BesGPictureButton(fViewAngleThetaFrame,
            gClient->GetPicture("ViewAngleMinus.gif"),
            kM_Button_ViewAngleThetaMinus);
    fViewAngleThetaMinusButton->SetToolTipText("Theta -");
    fViewAngleThetaMinusButton->SetHeight(22);
    fViewAngleThetaMinusButton->SetWidth(22);
    fViewAngleThetaMinusButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAngleThetaFrame->AddFrame(fViewAngleThetaMinusButton, fLayout);

    // slider for ViewAngleTheta
    fViewAngleThetaSlider = new TGVSlider(fViewAngleThetaFrame, 120, kSlider2 | kScaleBoth, kM_Slider_ViewAngleTheta);
    fViewAngleThetaSlider->Connect("PositionChanged(Int_t)", "BesClient", this, "HandleSliders(Int_t)");
    fViewAngleThetaSlider->SetRange(0, 180);
    fViewAngleThetaSlider->SetPosition(0);
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX | kLHintsExpandY, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAngleThetaFrame->AddFrame(fViewAngleThetaSlider, fLayout);

    // button ViewAngleThetaPlus
    fViewAngleThetaPlusButton = new BesGPictureButton(fViewAngleThetaFrame,
            gClient->GetPicture("ViewAnglePlus.gif"),
            kM_Button_ViewAngleThetaPlus);
    fViewAngleThetaPlusButton->SetToolTipText("Theta +");
    fViewAngleThetaPlusButton->SetHeight(22);
    fViewAngleThetaPlusButton->SetWidth(22);
    fViewAngleThetaPlusButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAngleThetaFrame->AddFrame(fViewAngleThetaPlusButton, fLayout);

    // number ViewAngleTheta
    //fViewAngleThetaNumber = new TGNumberEntry( fViewAngleThetaFrame, 0, 3, kM_Button_ViewAngleThetaNumber,
    //					     (TGNumberFormat::EStyle) 6, (TGNumberFormat::EAttribute) 0 );
    fViewAngleThetaNumber = new TGNumberEntryField( fViewAngleThetaFrame, kM_Button_ViewAngleThetaNumber,
            fViewAngleThetaSlider->GetPosition(),
            (TGNumberFormat::EStyle) 1, (TGNumberFormat::EAttribute) 0 );
    fViewAngleThetaNumber->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    h = fViewAngleThetaNumber->GetDefaultHeight();
    charw = fViewAngleThetaNumber->GetCharWidth("0123456");
    w = charw * TMath::Abs(4) / 10 + 8 + 2 * h / 3; // 4 is digit width
    fViewAngleThetaNumber->Resize(w, h);

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 2, 2, 2, 0);
    fWidgets->Add(fLayout);
    fViewAngleThetaFrame->AddFrame(fViewAngleThetaNumber, fLayout);

    // label theta
    fViewAngleThetaLabel = new TGLabel(fViewAngleThetaFrame,"theta");
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    fViewAngleThetaFrame->AddFrame(fViewAngleThetaLabel, fLayout);

    // frame ViewAnglePhi
    fViewAnglePhiFrame = new TGCompositeFrame(fViewAngleFrame, fViewAngleFrame->GetWidth(), 50, kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 3, 3, 3, 3);
    fWidgets->Add(fLayout);
    fViewAngleFrame->AddFrame(fViewAnglePhiFrame, fLayout);

    // button ViewAnglePhiMinus
    fViewAnglePhiMinusButton = new BesGPictureButton(fViewAnglePhiFrame,
            gClient->GetPicture("ViewAngleMinus.gif"),
            kM_Button_ViewAnglePhiMinus);
    fViewAnglePhiMinusButton->SetToolTipText("Phi -");
    fViewAnglePhiMinusButton->SetHeight(22);
    fViewAnglePhiMinusButton->SetWidth(22);
    fViewAnglePhiMinusButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAnglePhiFrame->AddFrame(fViewAnglePhiMinusButton, fLayout);

    // slider for ViewAnglePhi
    fViewAnglePhiSlider = new TGVSlider(fViewAnglePhiFrame, 120, kSlider2 | kScaleBoth, kM_Slider_ViewAnglePhi);
    fViewAnglePhiSlider->Connect("PositionChanged(Int_t)", "BesClient", this, "HandleSliders(Int_t)");
    fViewAnglePhiSlider->SetRange(0, 360);
    fViewAnglePhiSlider->SetPosition(0);
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX | kLHintsExpandY, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAnglePhiFrame->AddFrame(fViewAnglePhiSlider, fLayout);

    // button ViewAnglePhiPlus
    fViewAnglePhiPlusButton = new BesGPictureButton(fViewAnglePhiFrame,
            gClient->GetPicture("ViewAnglePlus.gif"),
            kM_Button_ViewAnglePhiPlus);
    fViewAnglePhiPlusButton->SetToolTipText("Phi +");
    fViewAnglePhiPlusButton->SetHeight(22);
    fViewAnglePhiPlusButton->SetWidth(22);
    fViewAnglePhiPlusButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAnglePhiFrame->AddFrame(fViewAnglePhiPlusButton, fLayout);

    // number ViewAnglePhi
    //fViewAnglePhiNumber = new TGNumberEntry( fViewAnglePhiFrame, 0, 3, kM_Button_ViewAnglePhiNumber,
    //					     (TGNumberFormat::EStyle) 6, (TGNumberFormat::EAttribute) 0 );
    fViewAnglePhiNumber = new TGNumberEntryField( fViewAnglePhiFrame, kM_Button_ViewAnglePhiNumber,
            fViewAnglePhiSlider->GetPosition(),
            (TGNumberFormat::EStyle) 1, (TGNumberFormat::EAttribute) 0 );
    fViewAnglePhiNumber->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    h = fViewAnglePhiNumber->GetDefaultHeight();
    charw = fViewAnglePhiNumber->GetCharWidth("0123456");
    w = charw * TMath::Abs(4) / 10 + 8 + 2 * h / 3; // 4 is digit width
    fViewAnglePhiNumber->Resize(w, h);

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 2, 2, 2, 0);
    fWidgets->Add(fLayout);
    fViewAnglePhiFrame->AddFrame(fViewAnglePhiNumber, fLayout);

    // label phi
    fViewAnglePhiLabel = new TGLabel(fViewAnglePhiFrame,"phi");
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    fViewAnglePhiFrame->AddFrame(fViewAnglePhiLabel, fLayout);

    // frame ViewAnglePsi
    fViewAnglePsiFrame = new TGCompositeFrame(fViewAngleFrame, fViewAngleFrame->GetWidth(), 50, kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop , 3, 3, 3, 3);
    fWidgets->Add(fLayout);
    fViewAngleFrame->AddFrame(fViewAnglePsiFrame, fLayout);

    // button ViewAnglePsiMinus
    fViewAnglePsiMinusButton = new BesGPictureButton(fViewAnglePsiFrame,
            gClient->GetPicture("ViewAngleMinus.gif"),
            kM_Button_ViewAnglePsiMinus);
    fViewAnglePsiMinusButton->SetToolTipText("Psi -");
    fViewAnglePsiMinusButton->SetHeight(22);
    fViewAnglePsiMinusButton->SetWidth(22);
    fViewAnglePsiMinusButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAnglePsiFrame->AddFrame(fViewAnglePsiMinusButton, fLayout);

    // slider for ViewAnglePsi
    fViewAnglePsiSlider = new TGVSlider(fViewAnglePsiFrame, 120, kSlider2 | kScaleBoth, kM_Slider_ViewAnglePsi);
    fViewAnglePsiSlider->Connect("PositionChanged(Int_t)", "BesClient", this, "HandleSliders(Int_t)");
    fViewAnglePsiSlider->SetRange(0, 360);
    fViewAnglePsiSlider->SetPosition(270);
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX | kLHintsExpandY, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAnglePsiFrame->AddFrame(fViewAnglePsiSlider, fLayout);

    // button ViewAnglePsiPlus
    fViewAnglePsiPlusButton = new BesGPictureButton(fViewAnglePsiFrame,
            gClient->GetPicture("ViewAnglePlus.gif"),
            kM_Button_ViewAnglePsiPlus);
    fViewAnglePsiPlusButton->SetToolTipText("Psi +");
    fViewAnglePsiPlusButton->SetHeight(22);
    fViewAnglePsiPlusButton->SetWidth(22);
    fViewAnglePsiPlusButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 0, 0);
    fWidgets->Add(fLayout);
    fViewAnglePsiFrame->AddFrame(fViewAnglePsiPlusButton, fLayout);

    // number ViewAnglePsi
    //fViewAnglePsiNumber = new TGNumberEntry( fViewAnglePsiFrame, 0, 3, kM_Button_ViewAnglePsiNumber,
    //					     (TGNumberFormat::EStyle) 6, (TGNumberFormat::EAttribute) 0 );
    fViewAnglePsiNumber = new TGNumberEntryField( fViewAnglePsiFrame, kM_Button_ViewAnglePsiNumber,
            fViewAnglePsiSlider->GetPosition(),
            (TGNumberFormat::EStyle) 1, (TGNumberFormat::EAttribute) 0 );
    fViewAnglePsiNumber->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    h = fViewAnglePsiNumber->GetDefaultHeight();
    charw = fViewAnglePsiNumber->GetCharWidth("0123456");
    w = charw * TMath::Abs(4) / 10 + 8 + 2 * h / 3; // 4 is digit width
    fViewAnglePsiNumber->Resize(w, h);

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 2, 2, 2, 0);
    fWidgets->Add(fLayout);
    fViewAnglePsiFrame->AddFrame(fViewAnglePsiNumber, fLayout);

    // label psi
    fViewAnglePsiLabel = new TGLabel(fViewAnglePsiFrame,"psi");
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    fViewAnglePsiFrame->AddFrame(fViewAnglePsiLabel, fLayout);

    //
    // "Event" tab
    // ================
    tf = fTabs->AddTab("Event");
    tf->SetBackgroundPixmap(GetPic("8.gif"));

    //
    // frame for event control
    // =====================
    fEventControlFrame = new TGCompositeFrame(tf, tf->GetWidth(), tf->GetHeight(), kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX , 15, 15, 15, 10);
    fWidgets->Add(fLayout);
    tf->AddFrame(fEventControlFrame, fLayout);
    fEventControlFrame->SetBackgroundPixmap(GetPic("EventFrameBckgnd.gif"));


    // lable "Event control"
    fEventControlTextFrame = new TGCompositeFrame(fEventControlFrame, fEventControlFrame->GetWidth(), 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fEventControlTextFrame, fLayout);
    fEventControlTextFrame->SetBackgroundPixmap(GetPic("EventControlBckgnd.gif"));

    fEventControlTextLabel = new TGLabel(fEventControlTextFrame,"Event Control ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX , 2, 73, 4, 4);
    fWidgets->Add(fLayout);
    fEventControlTextFrame->AddFrame(fEventControlTextLabel, fLayout);
    fEventControlTextLabel->SetBackgroundPixmap(GetPic("EventControlBckgnd.gif"));

    // input of run nr
    fBesRunFrame = new TGHorizontalFrame(fEventControlFrame, fEventControlFrame->GetWidth(), 25);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX , 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fBesRunFrame, fLayout);
    fBesRunFrame->SetBackgroundPixmap(GetPic("EventFrameBckgnd.gif"));

    fBesLabelRun = new TGLabel(fBesRunFrame, "Run : ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fBesRunFrame->AddFrame(fBesLabelRun, fLayout);

    fNumEntryRunNo = new TGNumberEntry(fBesRunFrame,this->GetBesRunNo(),12,kM_Button_BesRun,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1); // 16 is ditits number in the entry
    fNumEntryRunNo->SetHeight(20);
    fNumEntryRunNo->SetNumber(this->GetBesRunNo());
    fNumEntryRunNo->GetNumberEntry()->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryRunNo->GetNumberEntry()->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fBesRunFrame->AddFrame(fNumEntryRunNo, fLayout);

    // input of Event nr
    fBesEventFrame = new TGHorizontalFrame(fEventControlFrame, fEventControlFrame->GetWidth(), 25);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX , 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fBesEventFrame, fLayout);

    fBesLabelEvent = new TGLabel(fBesEventFrame, "Event : ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fBesEventFrame->AddFrame(fBesLabelEvent, fLayout);

    fNumEntryEventNo = new TGNumberEntry(fBesEventFrame,this->GetBesEventNo(),12,kM_Button_BesEvent,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
    fNumEntryEventNo->SetHeight(20);
    fNumEntryEventNo->SetNumber(this->GetBesEventNo());
    fNumEntryEventNo->GetNumberEntry()->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryEventNo->GetNumberEntry()->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fBesEventFrame->AddFrame(fNumEntryEventNo, fLayout);

    //
    // frame for event play
    // ====================

    // slider for event play
    fEventPlaySlider = new TGHSlider(fEventControlFrame, 40, kSlider1 | kScaleNo, kM_Slider_EventPlay);
    fEventPlaySlider->Connect("PositionChanged(Int_t)", "BesClient", this, "HandleSliders(Int_t)");
    fEventPlaySlider->SetRange(0, 100);
    fEventPlaySlider->SetPosition(0);
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX | kLHintsExpandX, 5, 5, 5, 0);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fEventPlaySlider, fLayout);

    // event play buttons
    fEventPlayButtonFrame = new TGCompositeFrame(fEventControlFrame, fEventControlFrame->GetWidth(), 30, kHorizontalFrame);
    //fEventNavFrame->SetBackgroundPixmap(GetPic("1.gif"));
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsTop, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fEventPlayButtonFrame, fLayout);

    // prev button
    fPrevEventButton = new BesGPictureButton(fEventPlayButtonFrame,
            gClient->GetPicture("ButtonEventPrev.gif"),
            kM_Button_PrevEvent);
    fPrevEventButton->SetToolTipText("Previous Event");
    fPrevEventButton->SetHeight(26);
    fPrevEventButton->SetWidth(25);
    fPrevEventButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 8, 0, 0, 4);
    fWidgets->Add(fLayout);
    fEventPlayButtonFrame->AddFrame(fPrevEventButton, fLayout);

    // next button
    fNextEventButton = new BesGPictureButton(fEventPlayButtonFrame,
            gClient->GetPicture("ButtonEventNext.gif"), kM_Button_NextEvent);
    fNextEventButton->SetToolTipText("Next Event");
    fNextEventButton->SetHeight(26);
    fNextEventButton->SetWidth(26);
    fNextEventButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 10, 0, 4);
    fWidgets->Add(fLayout);
    fEventPlayButtonFrame->AddFrame(fNextEventButton, fLayout);

    // play button
    fPlayEventButton = new BesGPictureButton(fEventPlayButtonFrame,
            gClient->GetPicture("ButtonEventPlay.gif"),
            kM_Button_PlayEvent);
    fPlayEventButton->SetToolTipText("Play Event");
    fPlayEventButton->SetHeight(35);
    fPlayEventButton->SetWidth(34);
    fPlayEventButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 0, 0, 4);
    fWidgets->Add(fLayout);
    fEventPlayButtonFrame->AddFrame(fPlayEventButton, fLayout);

    // first button
    fFirstEventButton = new BesGPictureButton(fEventPlayButtonFrame,
            gClient->GetPicture("ButtonEventReturn.gif"),
            kM_Button_FirstEvent);
    fFirstEventButton->SetToolTipText("First Event");
    fFirstEventButton->SetHeight(35);
    fFirstEventButton->SetWidth(25);
    fFirstEventButton->Connect("Clicked()", "BesClient", this, "HandleButtons()");

    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 8, 0, 4);
    fWidgets->Add(fLayout);
    fEventPlayButtonFrame->AddFrame(fFirstEventButton, fLayout);

    // lable "Event Play"
    fEventPlayTextFrame = new TGCompositeFrame(fEventControlFrame, fEventControlFrame->GetWidth(), 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsTop, 2, 2, 2, 2);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fEventPlayTextFrame, fLayout);

    fEventPlayTextLabel = new TGLabel(fEventPlayTextFrame,"Prev,Next,Play/Stop,Return");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 4, 2, 5);
    fWidgets->Add(fLayout);
    fEventPlayTextFrame->AddFrame(fEventPlayTextLabel, fLayout);


    // play speed
    fEventPlaySpeedFrame = new TGCompositeFrame(fEventControlFrame, fEventControlFrame->GetWidth(), 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsTop, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fEventControlFrame->AddFrame(fEventPlaySpeedFrame, fLayout);

    // input of speed
    fNumEntryEventPlaySpeed = new TGNumberEntry(fEventPlaySpeedFrame,(Double_t)this->GetEventPlaySpeed()/1000.0,4,
            kM_Button_EventPlaySpeed,
            (TGNumberFormat::EStyle) 1,(TGNumberFormat::EAttribute) 1);
    fNumEntryEventPlaySpeed->SetHeight(20);
    fNumEntryEventPlaySpeed->SetNumber(this->GetEventPlaySpeed()/1000.0);
    fNumEntryEventPlaySpeed->GetNumberEntry()->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryEventPlaySpeed->GetNumberEntry()->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fEventPlaySpeedFrame->AddFrame(fNumEntryEventPlaySpeed, fLayout);

    // label "Play Speed : "
    fEventPlaySpeedLabel = new TGLabel(fEventPlaySpeedFrame," sec/event ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 4, 4, 10);
    fWidgets->Add(fLayout);
    fEventPlaySpeedFrame->AddFrame(fEventPlaySpeedLabel, fLayout);


    /*
    //
    // frame for file list
    // ===================
    fEventListFrame = new TGCompositeFrame(tf, tf->GetWidth(), tf->GetHeight(), kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsBottom | kLHintsExpandX | kLHintsExpandY , 15, 15, 10, 200);
    fWidgets->Add(fLayout);
    //tf->AddFrame(fEventListFrame, fLayout);
    fEventListFrame->SetBackgroundPixmap(GetPic("12.gif"));

    // lable "File list"
    fEventListTextFrame = new TGCompositeFrame(fEventListFrame, fEventListFrame->GetWidth(), 30, kHorizontalFrame);
    fLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fEventListFrame->AddFrame(fEventListTextFrame, fLayout);
    fEventListTextFrame->SetBackgroundPixmap(GetPic("11.gif"));

    fEventListTextLabel = new TGLabel(fEventListTextFrame,"Event file list  ");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX , 2, 73, 4, 4);
    fWidgets->Add(fLayout);
    fEventListTextFrame->AddFrame(fEventListTextLabel, fLayout);
    fEventListTextLabel->SetBackgroundPixmap(GetPic("11.gif"));

    // frame EventListContent
    fEventListContentFrame = new TGCompositeFrame(fEventListFrame, fEventListFrame->GetWidth(), fEventListFrame->GetHeight()-30, kVerticalFrame);
    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY , 10, 10, 10, 10);
    fWidgets->Add(fLayout);
    fEventListFrame->AddFrame(fEventListContentFrame, fLayout);

    fEventListView = new TGCanvas(fEventListContentFrame, 2, 2, kSunkenFrame | kDoubleBorder);

    fEventListTree = new TGListTree(fEventListView->GetViewPort(), 2, 2, kHorizontalFrame);

    fEventListView->SetContainer(fEventListTree);
    fEventListTree->SetCanvas(fEventListView);
    fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
    fWidgets->Add(fLayout);
    fEventListContentFrame->AddFrame(fEventListView, fLayout);

    // Connect event list tree
    fEventListTree->Connect("Clicked(TGListTreeItem*, Int_t)",
      		"BesClient", this,
       	    "HandleEventList(TGListTreeItem*, Int_t)");

    // init active tree item
    fItem = 0;
    fRunItem = 0;
    */


    // "Show DST Info" tab
    // ================
    tf = fTabs->AddTab("REC");
    //tf = fTabs->AddTab("DST");
    tf->SetBackgroundPixmap(GetPic("8.gif"));

    //Header
    fFrameHeader =new TGGroupFrame(tf, "Header");
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 5); //Long Peixun's update: Reduce spacing, bottom 15 -> 5
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameHeader, fLayout);
    fChkBtnHeaderGlobal = new TGCheckButton(fFrameHeader, "Header", kM_Header_Global);
    fFrameHeader->AddFrame(fChkBtnHeaderGlobal);
    fChkBtnHeaderGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    // Hits
    fFrameHits = new TGGroupFrame(tf, "Hits");
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 5); //Long Peixun's update: Reduce spacing, bottom 15 -> 5
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameHits, fLayout);

    //------------------------------------------

    fChkBtnMdcHitsGlobal  = new TGCheckButton(fFrameHits, "Mdc hits", kM_MdcHits_Global);
    fFrameHits->AddFrame(fChkBtnMdcHitsGlobal);

    fChkBtnTofHitsGlobal  = new TGCheckButton(fFrameHits, "Tof  hits", kM_TofHits_Global);
    fFrameHits->AddFrame(fChkBtnTofHitsGlobal);

    fChkBtnTofHitsEast  = new TGCheckButton(fFrameHits, " East ", kM_TofHits_East);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fWidgets->Add(fLayout);
    fFrameHits->AddFrame(fChkBtnTofHitsEast, fLayout);

    fChkBtnTofHitsBarrel  = new TGCheckButton(fFrameHits, " Barrel ", kM_TofHits_Barrel);
    fFrameHits->AddFrame(fChkBtnTofHitsBarrel, fLayout);

    fChkBtnTofHitsWest  = new TGCheckButton(fFrameHits, " West ", kM_TofHits_West);
    fFrameHits->AddFrame(fChkBtnTofHitsWest, fLayout);

    fChkBtnEmcHitsGlobal  = new TGCheckButton(fFrameHits, "Emc hits", kM_EmcHits_Global);
    fFrameHits->AddFrame(fChkBtnEmcHitsGlobal);

    fChkBtnEmcHitsEast  = new TGCheckButton(fFrameHits, " East ", kM_EmcHits_East);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fWidgets->Add(fLayout);
    fFrameHits->AddFrame(fChkBtnEmcHitsEast, fLayout);

    fChkBtnEmcHitsBarrel  = new TGCheckButton(fFrameHits, " Barrel ", kM_EmcHits_Barrel);
    fFrameHits->AddFrame(fChkBtnEmcHitsBarrel, fLayout);

    fChkBtnEmcHitsWest  = new TGCheckButton(fFrameHits, " West ", kM_EmcHits_West);
    fFrameHits->AddFrame(fChkBtnEmcHitsWest, fLayout);

    fChkBtnEmcHitsSide  = new TGCheckButton(fFrameHits, " Side ", kM_EmcHits_Side);
    fFrameHits->AddFrame(fChkBtnEmcHitsSide, fLayout);

    fChkBtnMucHitsGlobal  = new TGCheckButton(fFrameHits, "Muc hits", kM_MucHits_Global);
    fFrameHits->AddFrame(fChkBtnMucHitsGlobal);

    fChkBtnMucHitsEast  = new TGCheckButton(fFrameHits, " East ", kM_MucHits_East);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fWidgets->Add(fLayout);
    fFrameHits->AddFrame(fChkBtnMucHitsEast, fLayout);

    fChkBtnMucHitsBarrel  = new TGCheckButton(fFrameHits, " Barrel ", kM_MucHits_Barrel);
    fFrameHits->AddFrame(fChkBtnMucHitsBarrel, fLayout);

    fChkBtnMucHitsWest  = new TGCheckButton(fFrameHits, " West ", kM_MucHits_West);
    fFrameHits->AddFrame(fChkBtnMucHitsWest, fLayout);

    fChkBtnMdcHitsGlobal->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTofHitsGlobal->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTofHitsEast  ->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTofHitsBarrel->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTofHitsWest  ->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnEmcHitsGlobal->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnEmcHitsEast  ->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnEmcHitsBarrel->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnEmcHitsWest  ->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnEmcHitsSide  ->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnMucHitsGlobal->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnMucHitsEast  ->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnMucHitsBarrel->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnMucHitsWest  ->Connect("Clicked()", "BesClient", this, "SetState()");

    // Tracks
    fFrameTracks = new TGGroupFrame(tf, "Tracks");
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 5); //Long Peixun's update: Reduce spacing, bottom 15 -> 5
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameTracks, fLayout);

    fChkBtnTracksGlobal = new TGCheckButton(fFrameTracks, "Tracks", kM_Tracks_Global);
    fFrameTracks->AddFrame(fChkBtnTracksGlobal);

    fChkBtnTracksMdc = new TGCheckButton(fFrameTracks, " Mdc ", kM_Tracks_Mdc);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fWidgets->Add(fLayout);
    fFrameTracks->AddFrame(fChkBtnTracksMdc, fLayout);

    fChkBtnTracksTof = new TGCheckButton(fFrameTracks, " Tof ", kM_Tracks_Tof);
    fFrameTracks->AddFrame(fChkBtnTracksTof, fLayout);

    fChkBtnTracksEmc = new TGCheckButton(fFrameTracks, " Emc ", kM_Tracks_Emc);
    fFrameTracks->AddFrame(fChkBtnTracksEmc, fLayout);

    fChkBtnTracksMuc = new TGCheckButton(fFrameTracks, " Muc ", kM_Tracks_Muc);
    fFrameTracks->AddFrame(fChkBtnTracksMuc, fLayout);

    fChkBtnTracksExt = new TGCheckButton(fFrameTracks, " Ext ", kM_Tracks_Ext);
    fFrameTracks->AddFrame(fChkBtnTracksExt, fLayout);

    fChkBtnTracksGlobal->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTracksMdc->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTracksTof->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTracksEmc->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTracksMuc->Connect("Clicked()", "BesClient", this, "SetState()");
    fChkBtnTracksExt->Connect("Clicked()", "BesClient", this, "SetState()");

    //
    // "Show Detector" tab
    // ===================
    tf = fTabs->AddTab("Detector");
    tf->SetBackgroundPixmap(GetPic("8.gif"));
    //tf->SetBackgroundPixmap(GetPic("3.gif"));

    fFrameSubDetector = new TGGroupFrame(tf, "Sub-Detector");

    // SubDetector
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 15);
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameSubDetector, fLayout);

    // Mdc
    fChkBtnMdcGlobal  = new TGCheckButton(fFrameSubDetector, "Mdc", kM_Mdc_Global);
    fLayout = new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0);
    fFrameSubDetector->AddFrame(fChkBtnMdcGlobal, fLayout);
    fChkBtnMdcGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMdcTubes   = new TGCheckButton(fFrameSubDetector, "Tubes", kM_Mdc_Tubes);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fFrameSubDetector->AddFrame(fChkBtnMdcTubes, fLayout);
    fChkBtnMdcTubes->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMdcWires   = new TGCheckButton(fFrameSubDetector, "Wires", kM_Mdc_Wires);
    fFrameSubDetector->AddFrame(fChkBtnMdcWires, fLayout);
    fChkBtnMdcWires->Connect("Clicked()", "BesClient", this, "SetState()");

    // Tof
    fChkBtnTofGlobal   = new TGCheckButton(fFrameSubDetector, "Tof", kM_Tof_Global);
    fLayout = new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0);
    fFrameSubDetector->AddFrame(fChkBtnTofGlobal, fLayout);
    fChkBtnTofGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnTofEast   = new TGCheckButton(fFrameSubDetector, "East EC", kM_Tof_East);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fFrameSubDetector->AddFrame(fChkBtnTofEast, fLayout);
    fChkBtnTofEast->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnTofBarrel   = new TGCheckButton(fFrameSubDetector, "Barrel", kM_Tof_Barrel);
    fFrameSubDetector->AddFrame(fChkBtnTofBarrel, fLayout);
    fChkBtnTofBarrel->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnTofWest   = new TGCheckButton(fFrameSubDetector, "West EC", kM_Tof_West);
    fFrameSubDetector->AddFrame(fChkBtnTofWest, fLayout);
    fChkBtnTofWest->Connect("Clicked()", "BesClient", this, "SetState()");

    // Emc
    fChkBtnEmcGlobal   = new TGCheckButton(fFrameSubDetector, "Emc", kM_Emc_Global);
    fLayout = new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0);
    fFrameSubDetector->AddFrame(fChkBtnEmcGlobal, fLayout);
    fChkBtnEmcGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnEmcEast   = new TGCheckButton(fFrameSubDetector, "East EC", kM_Emc_East);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fFrameSubDetector->AddFrame(fChkBtnEmcEast, fLayout);
    fChkBtnEmcEast->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnEmcBarrel   = new TGCheckButton(fFrameSubDetector, "Barrel", kM_Emc_Barrel);
    fFrameSubDetector->AddFrame(fChkBtnEmcBarrel, fLayout);
    fChkBtnEmcBarrel->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnEmcWest   = new TGCheckButton(fFrameSubDetector, "West EC", kM_Emc_West);
    fFrameSubDetector->AddFrame(fChkBtnEmcWest, fLayout);
    fChkBtnEmcWest->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnEmcSide   = new TGCheckButton(fFrameSubDetector, "Side ", kM_Emc_Side);
    fFrameSubDetector->AddFrame(fChkBtnEmcSide, fLayout);
    fChkBtnEmcSide->Connect("Clicked()", "BesClient", this, "SetState()");

    // Muc
    fChkBtnMucGlobal   = new TGCheckButton(fFrameSubDetector, "Muc", kM_Muc_Global);
    fLayout = new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0);
    fFrameSubDetector->AddFrame(fChkBtnMucGlobal, fLayout);
    fChkBtnMucGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMucEast   = new TGCheckButton(fFrameSubDetector, "East EC", kM_Muc_East);
    fLayout = new TGLayoutHints(kLHintsLeft, 20, 0, 0, 0);
    fFrameSubDetector->AddFrame(fChkBtnMucEast, fLayout);
    fChkBtnMucEast->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMucBarrel   = new TGCheckButton(fFrameSubDetector, "Barrel", kM_Muc_Barrel);
    fFrameSubDetector->AddFrame(fChkBtnMucBarrel, fLayout);
    fChkBtnMucBarrel->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMucWest   = new TGCheckButton(fFrameSubDetector, "West EC", kM_Muc_West);
    fFrameSubDetector->AddFrame(fChkBtnMucWest, fLayout);
    fChkBtnMucWest->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMucStrips   = new TGCheckButton(fFrameSubDetector, "Strips", kM_Muc_Strips);
    fFrameSubDetector->AddFrame(fChkBtnMucStrips, fLayout);
    fChkBtnMucStrips->Connect("Clicked()", "BesClient", this, "SetState()");

    // Other parts
    fFrameOthers = new TGGroupFrame (tf, "Others");
    fLayout = new TGLayoutHints (kLHintsExpandX, 15, 15, 15, 15);
    fWidgets->Add(fLayout);
    tf->AddFrame (fFrameOthers, fLayout);

    fChkBtnBeamPipe = new TGCheckButton (fFrameOthers, "Beam Pipe", kM_BeamPipe);
    fFrameOthers->AddFrame (fChkBtnBeamPipe);
    fChkBtnBeamPipe->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnZRPlaneOnXY = new TGCheckButton (fFrameOthers, "ZR plane on XY", kM_ZRPlaneOnXY);
    fFrameOthers->AddFrame (fChkBtnZRPlaneOnXY);
    fChkBtnZRPlaneOnXY->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnAxis = new TGCheckButton (fFrameOthers, "Axis", kM_Axis);
    fFrameOthers->AddFrame (fChkBtnAxis);
    fChkBtnAxis->Connect("Clicked()", "BesClient", this, "SetState()");

    //
    // "Others" tab
    // ================
    tf = fTabs->AddTab("Others");
    tf->SetBackgroundPixmap(GetPic("8.gif"));

    //Mdc TQMatch
    fFrameMdcMatch =new TGGroupFrame(tf, "Mdc Status");
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 15);
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameMdcMatch, fLayout);

    fChkBtnMdcTMatchGlobal = new TGCheckButton(fFrameMdcMatch, "T Fire", kM_Mdc_TMatch_Global);
    fChkBtnMdcTMatchGlobal->SetState(kButtonDown);
    //fChkBtnMdcTMatchGlobal->SetOn();
    fFrameMdcMatch->AddFrame(fChkBtnMdcTMatchGlobal);
    fChkBtnMdcTMatchGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMdcQMatchGlobal = new TGCheckButton(fFrameMdcMatch, "Q Fire", kM_Mdc_QMatch_Global);
    fChkBtnMdcQMatchGlobal->SetState(kButtonDown);
    //fChkBtnMdcQMatchGlobal->SetOn();
    fFrameMdcMatch->AddFrame(fChkBtnMdcQMatchGlobal);
    fChkBtnMdcQMatchGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnMdcQOverflowGlobal = new TGCheckButton(fFrameMdcMatch, "Q Not Overflow", kM_Mdc_QNotOverflow_Global);
    fChkBtnMdcQOverflowGlobal->SetState(kButtonUp);
    //fChkBtnMdcQOverflowGlobal->SetOn();
    fFrameMdcMatch->AddFrame(fChkBtnMdcQOverflowGlobal);
    fChkBtnMdcQOverflowGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    //Colorful Mdc Wire 
    fChkBtnMdcColorfulWireGlobal = new TGCheckButton(fFrameMdcMatch, "Colorful Wire", kM_Mdc_ColorfulWire_Global);
    fChkBtnMdcColorfulWireGlobal->SetState(kButtonUp);
    fFrameMdcMatch->AddFrame(fChkBtnMdcColorfulWireGlobal);
    fChkBtnMdcColorfulWireGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    //Mdc time substract event start time
    fChkBtnMdcTimeSubEvTimeGlobal = new TGCheckButton(fFrameMdcMatch, "Sub EvTime", kM_Mdc_MdcTimeSubEvTime_Global);
    fChkBtnMdcTimeSubEvTimeGlobal->SetState(kButtonUp);
    fFrameMdcMatch->AddFrame(fChkBtnMdcTimeSubEvTimeGlobal);
    fChkBtnMdcTimeSubEvTimeGlobal->Connect("Clicked()", "BesClient", this, "SetState()");
    
    //Tof TQMatch
    fFrameTofMatch =new TGGroupFrame(tf, "Tof TQ Match");
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 15);
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameTofMatch, fLayout);

    fChkBtnTofTMatchGlobal = new TGCheckButton(fFrameTofMatch, "T Match", kM_Tof_TMatch_Global);
    fChkBtnTofTMatchGlobal->SetState(kButtonDown);
    //fChkBtnTofTMatchGlobal->SetOn();
    fFrameTofMatch->AddFrame(fChkBtnTofTMatchGlobal);
    fChkBtnTofTMatchGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    fChkBtnTofQMatchGlobal = new TGCheckButton(fFrameTofMatch, "Q Match", kM_Tof_QMatch_Global);
    fChkBtnTofQMatchGlobal->SetState(kButtonDown);
    //fChkBtnTofQMatchGlobal->SetOn();
    fFrameTofMatch->AddFrame(fChkBtnTofQMatchGlobal);
    fChkBtnTofQMatchGlobal->Connect("Clicked()", "BesClient", this, "SetState()");

    // Magnetic field
    fFrameMagnetic =new TGGroupFrame(tf, "Magnetic Field(Tesla)");
    fLayout = new TGLayoutHints(kLHintsExpandX, 15, 15, 15, 15);
    fWidgets->Add(fLayout);
    tf->AddFrame(fFrameMagnetic, fLayout);

    // input of Magnetic field
    fNumEntryMagnetic = new TGNumberEntry(fFrameMagnetic,
                                          1.0, 4,
                                          kM_Button_Magnetic,
                                          TGNumberFormat::kNESRealTwo,
                                          TGNumberFormat::kNEAAnyNumber);
    fNumEntryMagnetic->SetHeight(20);
    fNumEntryMagnetic->SetNumber(1.0);
    fNumEntryMagnetic->GetNumberEntry()
    ->Connect("ReturnPressed()","BesClient",this,"ExecuteReturn()");
    fNumEntryMagnetic->GetNumberEntry()
    ->Connect("TabPressed()","BesClient",this,"ChangeFocus()");
    fLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4);
    fWidgets->Add(fLayout);
    fFrameMagnetic->AddFrame(fNumEntryMagnetic, fLayout);


    for (Int_t iTab = 0; iTab < fTabs->GetNumberOfTabs(); iTab++) {
        fTabs->GetTabTab(iTab)->SetBackgroundPixmap(GetPic("8.gif"));
    }
}

void BesClient::SetMdcQNotOverflow(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetMdcROOTGeo()->SetQNotOverflow(input);
    }
}


void BesClient::SetMdcTFire(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetMdcROOTGeo()->SetTFire(input);
    }
}

void BesClient::SetMdcQFire(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetMdcROOTGeo()->SetQFire(input);
    }
}

void BesClient::SetMdcColorfulWire(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetMdcROOTGeo()->SetColorfulWire(input);
    }
}

void BesClient::SetMdcTimeSubEvTime(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetMdcROOTGeo()->SetMdcTimeSubEvTime(input);
    }
}

void BesClient::SetTofTMatch(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetTofROOTGeo()->SetTMatch(input);
    }
}

void BesClient::SetTofQMatch(Bool_t input){
    if (gBesGeometry){
        gBesGeometry->GetTofROOTGeo()->SetQMatch(input);
    }
}
//_____________________________________________________

void BesClient::InitLocal() {
    //
    // init local variables
    if (gDebug) cout << "BesClient::InitLocal called!" << endl;

    // init widget list
    fWidgets = new TList();
    fLayout = 0;

    gGeometry = 0;
    gBesCursor = new BesCursor();
    gBesCursor->SetType(kBesPick);
    gBesCursor->SetShowInfo(true);

    fAutoDisplayEventTimer = new TTimer(fEventPlaySpeed);
    Connect(fAutoDisplayEventTimer,"Timeout()","BesClient",this,"AutoDisplayEventCommand()");

    fAutoRotateTimer = new TTimer((Int_t)1000/fRotateFPS);
    Connect(fAutoRotateTimer,"Timeout()","BesClient",this,"AutoRotateCommand()");

    /*TColor *milk        = */
    new TColor(1001, 1.0, 1.0, 225/255.0);
    /*TColor *pink        = */
    new TColor(1002, 227/255.0, 186/255.0, 227/255.0);
    /*TColor *lightYellow = */
    new TColor(1003, 232/255.0, 234/255.0, 117/255.0);
    /*TColor *orange      = */
    new TColor(1004, 254/255.0, 216/255.0,  31/255.0);
    /*TColor *lightBlue   = */
    new TColor(1005, 175/255.0, 192/255.0, 227/255.0);
    /*TColor *lightGreen  = */
    new TColor(1006, 192/255.0, 227/255.0, 226/255.0);
    /*TColor *lightBrown  = */
    new TColor(1007, 225/255.0, 199/255.0, 162/255.0);
}

//_____________________________________________________

void BesClient::InitParameter() {
    //
    // init local variables
    if (gDebug) cout << "BesClient::InitParameter called!" << endl;

    // init command line parameter
    fWindowWidth  = 1000;
    fWindowHeight = 710;

    fBesVisPath = TString(getenv("BESVISLIBROOT"));

    fEventPlaySpeed = 1000;  // 1000 ms/event
    fAutoDisplayEvent = kFALSE;

    for (Int_t i = 0; i < 3; i++) {
        fHomeX[i] = 0.0;
        fHomeY[i] = 0.0;
    }

    fMoveFactor  = 0.1;   // factor of move
    fRotateStep  = 1.0;   // 0.1 deg/frame
    fRotateSpeed = 10.0;  // 1.0 deg/sec
    fRotateFPS   = 10;    // 10 frame/sec

    fAutoRotate = kFALSE;
    fAutoRotateClockWise = 0;
    fAutoRotateTheta = 0;
    fAutoRotatePhi   = 0;
    fAutoRotatePsi   = 0;

    fBesRunNo   = 0;
    fBesEventNo = 0;

    fViewer3DMode = 0;

    //Long Peixun's update: Load help.txt to f_HelpText
    TString docfile = fBesVisPath + "/docs/help.txt";
    ifstream ifile(docfile.Data());
    if (ifile)
    {
        stringstream ss;
        int ch = 0;
        while ((ch = ifile.get()) != EOF)
        {
            ss << static_cast<char>(ch);
        }
        f_HelpText = ss.str();
    }
    ifile.close();

    //Long Peixun's update: Load geom_sel.txt
    docfile = fBesVisPath + "/docs/geom_sel.txt";
    InitGeoSelector(docfile.Data());
}

//_____________________________________________________
//Long Peixun's update: Initialize geometry selector
void BesClient::InitGeoSelector(const char* geomsel_file)
{
    ifstream ifile(geomsel_file);
    if (ifile)
    {
        stringstream ss;
        string line, fname;
        int startno;
        int ich = 0;
        while ((ich = ifile.get()) != EOF)
        {
            char ch = static_cast<char>(ich);
            if (ch == '\n' || ch == '\r')
            {
                line = ss.str();
                if (line.length() == 0 || line.find('#') != string::npos)
                {
                    ss.clear();
                    ss.str("");
                }
                else
                {
                    fname.clear();
                    ss >> fname;
                    startno = 0;
                    ss >> startno;
                    fSelector.addGeomFile(fname.c_str(), startno);
                    ss.clear();
                    ss.str("");
                }
            }
            else ss << ch;
        }
    }
    ifile.close();
}

//_____________________________________________________

void BesClient::InitConnections() {
    //
    // init connections to internal messaging queues
    if (gDebug) cout << "BesClient::InitConnections called!" << endl;

    // Connect canvas to mouse actions
    Connect((TCanvas*)fEmbeddedCanvas->GetCanvas(),"ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "BesClient",
            this, "ExecuteEvent(Int_t, Int_t, Int_t, TObject*)");

    // Connect close button
    Connect("CloseWindow()", "BesClient", this, "CloseWindow()");
}

//__________________________________________________________________

void BesClient::HandleMenu(Int_t id) {
    //
    // Handle menu items.

    switch (id) {

        // File
    case kM_File_LoadGeo:
        LoadGeoFile();
        break;
    case kM_File_SaveGeoAs:
        SaveGeoAs();
        break;
    case kM_File_OpenEvent:
        OpenEventFile();
        break;
    case kM_File_SavePicAs:
        SavePicAs();
        break;
    case kM_File_Close:
        gInterpreter->DeleteGlobal(this);
        delete this;
        break;
    case kM_File_Exit:
        CloseWindow();   // terminate theApp no need to use SendCloseMessage()
        break;

        // Edit
    case kM_Edit_Editor:
        fEmbeddedCanvas->GetCanvas()->EditorBar();
        break;

        // Help
    case kM_Help_Content:
        Help();
        break;

    case kM_Help_About:
        new BesAboutHelp(gClient->GetRoot(), this);
        break;

        // Default
    default:
        cerr << "Menu item" << id << "selected" << endl;
        break;
    }
}

//_____________________________________________________

void BesClient::HandleViewOptionMenu(Int_t id) {
    //
    // Handle view option menu items.
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) {
        cout << "BesClient::HandleViewOptionMenu(), BesView does not exist in this pad" << endl;
        return;
    }

    //Int_t i = 0, px = 0, py = 0;
    //TVirtualGeoPainter *painter = 0;

    switch (id) {
    //Long Peixun's update: kM_Header_On -> kM_Header_Global
    case kM_Header_Global:
        if ( fMenuViewOptionHeader->IsEntryChecked(kM_Header_Global) ) {
            fMenuViewOptionHeader->UnCheckEntry(kM_Header_Global);
            fDisplay->SetVisHeader(0);
        } else {
            fMenuViewOptionHeader->CheckEntry(kM_Header_Global);
            fDisplay->SetVisHeader(1);
        }
        break;

    case kM_Mdc_Global:
        if ( fMenuViewOptionMdc->IsEntryChecked(kM_Mdc_Global) ) {
            fMenuViewOptionMdc->UnCheckEntry(kM_Mdc_Global);
            view->SetVisMdcGlobal(0);
        } else {
            fMenuViewOptionMdc->CheckEntry(kM_Mdc_Global);
            view->SetVisMdcGlobal(1);
        }
        break;

    case kM_Mdc_Tubes:
        if ( fMenuViewOptionMdc->IsEntryChecked(kM_Mdc_Tubes) ) {
            fMenuViewOptionMdc->UnCheckEntry(kM_Mdc_Tubes);
            view->SetVisMdcTubes(0);
        } else {
            fMenuViewOptionMdc->CheckEntry(kM_Mdc_Tubes);
            view->SetVisMdcTubes(1);
        }
        break;

    case kM_Mdc_Wires:
        if ( fMenuViewOptionMdc->IsEntryChecked(kM_Mdc_Wires) ) {
            fMenuViewOptionMdc->UnCheckEntry(kM_Mdc_Wires);
            view->SetVisMdcWires(0);
        } else {
            fMenuViewOptionMdc->CheckEntry(kM_Mdc_Wires);
            view->SetVisMdcWires(1);
        }
        break;

    case kM_Tof_Global:
        if ( fMenuViewOptionTof->IsEntryChecked(kM_Tof_Global) ) {
            fMenuViewOptionTof->UnCheckEntry(kM_Tof_Global);
            view->SetVisTofGlobal(0);
        } else {
            fMenuViewOptionTof->CheckEntry(kM_Tof_Global);
            view->SetVisTofGlobal(1);
        }
        break;

    case kM_Tof_East:
        if ( fMenuViewOptionTof->IsEntryChecked(kM_Tof_East) ) {
            fMenuViewOptionTof->UnCheckEntry(kM_Tof_East);
            view->SetVisTofEast(0);
        } else {
            fMenuViewOptionTof->CheckEntry(kM_Tof_East);
            view->SetVisTofEast(1);
        }
        break;

    case kM_Tof_Barrel:
        if ( fMenuViewOptionTof->IsEntryChecked(kM_Tof_Barrel) ) {
            fMenuViewOptionTof->UnCheckEntry(kM_Tof_Barrel);
            view->SetVisTofBarrel(0);
        } else {
            fMenuViewOptionTof->CheckEntry(kM_Tof_Barrel);
            view->SetVisTofBarrel(1);
        }
        break;

    case kM_Tof_West:
        if ( fMenuViewOptionTof->IsEntryChecked(kM_Tof_West) ) {
            fMenuViewOptionTof->UnCheckEntry(kM_Tof_West);
            view->SetVisTofWest(0);
        } else {
            fMenuViewOptionTof->CheckEntry(kM_Tof_West);
            view->SetVisTofWest(1);
        }
        break;

    case kM_Emc_Global:
        if ( fMenuViewOptionEmc->IsEntryChecked(kM_Emc_Global) ) {
            fMenuViewOptionEmc->UnCheckEntry(kM_Emc_Global);
            view->SetVisEmcGlobal(0);
        } else {
            fMenuViewOptionEmc->CheckEntry(kM_Emc_Global);
            view->SetVisEmcGlobal(1);
        }
        break;

    case kM_Emc_East:
        if ( fMenuViewOptionEmc->IsEntryChecked(kM_Emc_East) ) {
            fMenuViewOptionEmc->UnCheckEntry(kM_Emc_East);
            view->SetVisEmcEast(0);
        } else {
            fMenuViewOptionEmc->CheckEntry(kM_Emc_East);
            view->SetVisEmcEast(1);
        }
        break;

    case kM_Emc_Barrel:
        if ( fMenuViewOptionEmc->IsEntryChecked(kM_Emc_Barrel) ) {
            fMenuViewOptionEmc->UnCheckEntry(kM_Emc_Barrel);
            view->SetVisEmcBarrel(0);
        } else {
            fMenuViewOptionEmc->CheckEntry(kM_Emc_Barrel);
            view->SetVisEmcBarrel(1);
        }
        break;

    case kM_Emc_West:
        if ( fMenuViewOptionEmc->IsEntryChecked(kM_Emc_West) ) {
            fMenuViewOptionEmc->UnCheckEntry(kM_Emc_West);
            view->SetVisEmcWest(0);
        } else {
            fMenuViewOptionEmc->CheckEntry(kM_Emc_West);
            view->SetVisEmcWest(1);
        }
        break;

    case kM_Emc_Side:
        if ( fMenuViewOptionEmc->IsEntryChecked(kM_Emc_Side) ) {
            fMenuViewOptionEmc->UnCheckEntry(kM_Emc_Side);
            view->SetVisEmcSide(0);
        } else {
            fMenuViewOptionEmc->CheckEntry(kM_Emc_Side);
            view->SetVisEmcSide(1);
        }
        break;

    case kM_Muc_Global:
        if ( fMenuViewOptionMuc->IsEntryChecked(kM_Muc_Global) ) {
            fMenuViewOptionMuc->UnCheckEntry(kM_Muc_Global);
            view->SetVisMucGlobal(0);
        } else {
            fMenuViewOptionMuc->CheckEntry(kM_Muc_Global);
            view->SetVisMucGlobal(1);
        }
        break;

    case kM_Muc_East:
        if ( fMenuViewOptionMuc->IsEntryChecked(kM_Muc_East) ) {
            fMenuViewOptionMuc->UnCheckEntry(kM_Muc_East);
            view->SetVisMucEast(0);
        } else {
            fMenuViewOptionMuc->CheckEntry(kM_Muc_East);
            view->SetVisMucEast(1);
        }
        break;

    case kM_Muc_Barrel:
        if ( fMenuViewOptionMuc->IsEntryChecked(kM_Muc_Barrel) ) {
            fMenuViewOptionMuc->UnCheckEntry(kM_Muc_Barrel);
            view->SetVisMucBarrel(0);
        } else {
            fMenuViewOptionMuc->CheckEntry(kM_Muc_Barrel);
            view->SetVisMucBarrel(1);
        }
        break;

    case kM_Muc_West:
        if ( fMenuViewOptionMuc->IsEntryChecked(kM_Muc_West) ) {
            fMenuViewOptionMuc->UnCheckEntry(kM_Muc_West);
            view->SetVisMucWest(0);
        } else {
            fMenuViewOptionMuc->CheckEntry(kM_Muc_West);
            view->SetVisMucWest(1);
        }
        break;

    case kM_Muc_Strips:
        if ( fMenuViewOptionMuc->IsEntryChecked(kM_Muc_Strips) ) {
            fMenuViewOptionMuc->UnCheckEntry(kM_Muc_Strips);
            view->SetVisMucStrips(0);
        } else {
            fMenuViewOptionMuc->CheckEntry(kM_Muc_Strips);
            view->SetVisMucStrips(1);
        }
        break;

    case kM_Full3D_Mdc:
        if ( fMenuViewOptionFull3D->IsEntryChecked(kM_Full3D_Mdc) ) {
            fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Mdc);
            view->SetVisFull3DMdc(0);
        } else {
            fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Mdc);
            view->SetVisFull3DMdc(1);
        }

        if (gBesGeometry) {
            gBesGeometry->GetMdcROOTGeo()->SetDetector();
            gBesGeometry->GetMdcROOTGeo()->SetVisMdcDetector();
        }
        break;

    case kM_Full3D_Tof:
        if ( fMenuViewOptionFull3D->IsEntryChecked(kM_Full3D_Tof) ) {
            fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Tof);
            view->SetVisFull3DTof(0);
        } else {
            fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Tof);
            view->SetVisFull3DTof(1);
        }

        if (gBesGeometry) {
            gBesGeometry->GetTofROOTGeo()->SetDetector();
            gBesGeometry->GetTofROOTGeo()->SetVisTofDetector();
        }
        break;

    case kM_Full3D_Emc:
        if ( fMenuViewOptionFull3D->IsEntryChecked(kM_Full3D_Emc) ) {
            fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Emc);
            view->SetVisFull3DEmc(0);
        } else {
            fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Emc);
            view->SetVisFull3DEmc(1);
        }

        if (gBesGeometry) {
            gBesGeometry->GetEmcROOTGeo()->SetDetector();
            gBesGeometry->GetEmcROOTGeo()->SetVisEmcDetector();
        }
        break;

    case kM_Full3D_Muc:
        if ( fMenuViewOptionFull3D->IsEntryChecked(kM_Full3D_Muc) ) {
            fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Muc);
            view->SetVisFull3DMuc(0);
        } else {
            fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Muc);
            view->SetVisFull3DMuc(1);
        }

        if (gBesGeometry) {
            gBesGeometry->GetMucROOTGeo()->SetDetector();
            gBesGeometry->GetMucROOTGeo()->SetVisMucDetector();
        }
        break;

    case kM_BeamPipe:
        if ( fMenuViewOptionOthers->IsEntryChecked(kM_BeamPipe) ) {
            fMenuViewOptionOthers->UnCheckEntry(kM_BeamPipe);
            view->SetVisBeamPipe(0);
        } else {
            fMenuViewOptionOthers->CheckEntry(kM_BeamPipe);
            view->SetVisBeamPipe(1);
        }
        break;

    case kM_ZRPlaneOnXY:
        if ( fMenuViewOptionOthers->IsEntryChecked(kM_ZRPlaneOnXY) ) {
            fMenuViewOptionOthers->UnCheckEntry(kM_ZRPlaneOnXY);
            view->SetVisZRPlaneOnXY(0);
        } else {
            fMenuViewOptionOthers->CheckEntry(kM_ZRPlaneOnXY);
            view->SetVisZRPlaneOnXY(1);
        }
        break;

    case kM_Axis:
        if ( fMenuViewOptionOthers->IsEntryChecked(kM_Axis) ) {
            fMenuViewOptionOthers->UnCheckEntry(kM_Axis);
            view->SetVisAxis(0);
        } else {
            fMenuViewOptionOthers->CheckEntry(kM_Axis);
            view->SetVisAxis(1);
        }
        break;

    case kM_MdcHits_Hits:
        if ( fMenuViewOptionMdcHits->IsEntryChecked(kM_MdcHits_Hits) ) {
            fMenuViewOptionMdcHits->UnCheckEntry(kM_MdcHits_Hits);
            view->SetVisMdcHits(0);
        } else {
            fMenuViewOptionMdcHits->CheckEntry(kM_MdcHits_Hits);
            view->SetVisMdcHits(1);
        }
        break;

    case kM_TofHits_Global:
        if ( fMenuViewOptionTofHits->IsEntryChecked(kM_TofHits_Global) ) {
            fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_Global);
            view->SetVisTofHitsGlobal(0);
        } else {
            fMenuViewOptionTofHits->CheckEntry(kM_TofHits_Global);
            view->SetVisTofHitsGlobal(1);
        }
        break;

    case kM_TofHits_East:
        if ( fMenuViewOptionTofHits->IsEntryChecked(kM_TofHits_East) ) {
            fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_East);
            view->SetVisTofHitsEast(0);
        } else {
            fMenuViewOptionTofHits->CheckEntry(kM_TofHits_East);
            view->SetVisTofHitsEast(1);
        }
        break;

    case kM_TofHits_Barrel:
        if ( fMenuViewOptionTofHits->IsEntryChecked(kM_TofHits_Barrel) ) {
            fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_Barrel);
            view->SetVisTofHitsBarrel(0);
        } else {
            fMenuViewOptionTofHits->CheckEntry(kM_TofHits_Barrel);
            view->SetVisTofHitsBarrel(1);
        }
        break;

    case kM_TofHits_West:
        if ( fMenuViewOptionTofHits->IsEntryChecked(kM_TofHits_West) ) {
            fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_West);
            view->SetVisTofHitsWest(0);
        } else {
            fMenuViewOptionTofHits->CheckEntry(kM_TofHits_West);
            view->SetVisTofHitsWest(1);
        }
        break;

    case kM_EmcHits_Global:
        if ( fMenuViewOptionEmcHits->IsEntryChecked(kM_EmcHits_Global) ) {
            fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_Global);
            view->SetVisEmcHitsGlobal(0);
        } else {
            fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_Global);
            view->SetVisEmcHitsGlobal(1);
        }
        break;

    case kM_EmcHits_East:
        if ( fMenuViewOptionEmcHits->IsEntryChecked(kM_EmcHits_East) ) {
            fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_East);
            view->SetVisEmcHitsEast(0);
        } else {
            fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_East);
            view->SetVisEmcHitsEast(1);
        }
        break;

    case kM_EmcHits_Barrel:
        if ( fMenuViewOptionEmcHits->IsEntryChecked(kM_EmcHits_Barrel) ) {
            fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_Barrel);
            view->SetVisEmcHitsBarrel(0);
        } else {
            fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_Barrel);
            view->SetVisEmcHitsBarrel(1);
        }
        break;

    case kM_EmcHits_West:
        if ( fMenuViewOptionEmcHits->IsEntryChecked(kM_EmcHits_West) ) {
            fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_West);
            view->SetVisEmcHitsWest(0);
        } else {
            fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_West);
            view->SetVisEmcHitsWest(1);
        }
        break;

    case kM_EmcHits_Side:
        if ( fMenuViewOptionEmcHits->IsEntryChecked(kM_EmcHits_Side) ) {
            fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_Side);
            view->SetVisEmcHitsSide(0);
        } else {
            fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_Side);
            view->SetVisEmcHitsSide(1);
        }
        break;

    case kM_MucHits_Global:
        if ( fMenuViewOptionMucHits->IsEntryChecked(kM_MucHits_Global) ) {
            fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_Global);
            view->SetVisMucHitsGlobal(0);
        } else {
            fMenuViewOptionMucHits->CheckEntry(kM_MucHits_Global);
            view->SetVisMucHitsGlobal(1);
        }
        break;

    case kM_MucHits_East:
        if ( fMenuViewOptionMucHits->IsEntryChecked(kM_MucHits_East) ) {
            fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_East);
            view->SetVisMucHitsEast(0);
        } else {
            fMenuViewOptionMucHits->CheckEntry(kM_MucHits_East);
            view->SetVisMucHitsEast(1);
        }
        break;

    case kM_MucHits_Barrel:
        if ( fMenuViewOptionMucHits->IsEntryChecked(kM_MucHits_Barrel) ) {
            fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_Barrel);
            view->SetVisMucHitsBarrel(0);
        } else {
            fMenuViewOptionMucHits->CheckEntry(kM_MucHits_Barrel);
            view->SetVisMucHitsBarrel(1);
        }
        break;

    case kM_MucHits_West:
        if ( fMenuViewOptionMucHits->IsEntryChecked(kM_MucHits_West) ) {
            fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_West);
            view->SetVisMucHitsWest(0);
        } else {
            fMenuViewOptionMucHits->CheckEntry(kM_MucHits_West);
            view->SetVisMucHitsWest(1);
        }
        break;

    case kM_Tracks_Global:
        if ( fMenuViewOptionTracks->IsEntryChecked(kM_Tracks_Global) ) {
            fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Global);
            view->SetVisTracksGlobal(0);
        } else {
            fMenuViewOptionTracks->CheckEntry(kM_Tracks_Global);
            view->SetVisTracksGlobal(1);
        }
        break;

    case kM_Tracks_Mdc:
        if ( fMenuViewOptionTracks->IsEntryChecked(kM_Tracks_Mdc) ) {
            fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Mdc);
            view->SetVisTracksMdc(0);
        } else {
            fMenuViewOptionTracks->CheckEntry(kM_Tracks_Mdc);
            view->SetVisTracksMdc(1);
        }
        break;

    case kM_Tracks_Tof:
        if ( fMenuViewOptionTracks->IsEntryChecked(kM_Tracks_Tof) ) {
            fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Tof);
            view->SetVisTracksTof(0);
        } else {
            fMenuViewOptionTracks->CheckEntry(kM_Tracks_Tof);
            view->SetVisTracksTof(1);
        }
        break;

    case kM_Tracks_Emc:
        if ( fMenuViewOptionTracks->IsEntryChecked(kM_Tracks_Emc) ) {
            fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Emc);
            view->SetVisTracksEmc(0);
        } else {
            fMenuViewOptionTracks->CheckEntry(kM_Tracks_Emc);
            view->SetVisTracksEmc(1);
        }
        break;

    case kM_Tracks_Muc:
        if ( fMenuViewOptionTracks->IsEntryChecked(kM_Tracks_Muc) ) {
            fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Muc);
            view->SetVisTracksMuc(0);
        } else {
            fMenuViewOptionTracks->CheckEntry(kM_Tracks_Muc);
            view->SetVisTracksMuc(1);
        }
        break;

    case kM_Tracks_Ext:
        if ( fMenuViewOptionTracks->IsEntryChecked(kM_Tracks_Ext) ) {
            fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Ext);
            view->SetVisTracksExt(0);
        } else {
            fMenuViewOptionTracks->CheckEntry(kM_Tracks_Ext);
            view->SetVisTracksExt(1);
        }
        break;

    case kM_View_X3D:
        X3D();
        break;

    case kM_View_OpenGL:
        OpenGL();
        break;

        // Default
    default:
        cerr << "MenuViewOption item" << id << "selected" << endl;
        break;
    }

    view->UpdateView(0);
    UpdateStatus();

    ((TCanvas*)fEmbeddedCanvas->GetCanvas())->Modified();
    ((TCanvas*)fEmbeddedCanvas->GetCanvas())->Update();
}

//__________________________________________________________________

void BesClient::HandleError(const char* msg) {
    //
    // Error handling
    new TGMsgBox(gClient->GetRoot(), this,
                 "Error", msg,
                 gClient->GetPicture("mb_stop_s.xpm"),
                 kMBOk);    //Long Peixun's update: kMBDismiss -> kMBOk
}

//__________________________________________________________________

void BesClient::HandleInfoBar(const char* msg) {
    //
    // Info handling
    fStatusBar->SetText(msg, 1);
}

//__________________________________________________________________

void BesClient::HandleStatusBar(const char* msg) {
    //
    // Client status handling
    fStatusBar->SetText(msg, 0);
}

//__________________________________________________________________

void BesClient::Show() {
    //
    // Show display
    MapWindow();
    MapRaised();
}

//__________________________________________________________________

void BesClient::LoadGeoFile() {
    //
    // Load BesVis geometry file

    //Long Peixun's update: Always popup open geometry dialog box
    // if ( f_geoFile.Length() != 0){
    //     cout << "Load geoFile f_geoFile: " << f_geoFile << endl;
    //     this->OpenGeoFile(f_geoFile);
    // }
    // else {
        // Open file browser
        static TString GeoDir = fBesVisPath + TString("/geom/"); // "."
        TGFileInfo fi;
        fi.fFileTypes = OpenGeoTypes;
        fi.fIniDir    = StrDup(GeoDir);
        char msg[256];
        sprintf(msg, "Loading Geometry File");
        HandleStatusBar(msg);
        new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
        if (&fi == 0) return;
        GeoDir = fi.fIniDir;
        this->OpenGeoFile(fi.fFilename); // fi.fFilename: full path of geofile
        fAutoGeom = false;      //Long Peixun's update: Current geometry was selected manually
    // }
}

//__________________________________________________________________

void BesClient::OpenGeoFile(TString fname) {
    //
    // Open BesVis geometry file
    //HandleStatusBar("Open Geometry File");
    if (fname.Length() == 0) return;
    char msg[256];
    if (gDebug) cout << fname << " fname Length " << fname.Length() << endl;

    //Long Peixun's update: consider whether fname contains path or not
    TString fpath, fpurename;
    if (fname.Contains("/"))
    {   //fname contains both path and file name
        Ssiz_t posLastSlash = fname.Last('/');   // /scratch/youzy/BesVis/geom.root
        //                                                               |
        fpath = fname;
        fpath = fpath.Remove(posLastSlash+1, fname.Length()-posLastSlash-1);
        fpurename = fname;          //Long Peixun's update: Get pure name of file
        fpurename = fpurename.Remove(0, posLastSlash + 1);
        if (fpath.Length() == 0) return;   //  Cancel Button pressed.
    }
    else
    {   //fname contains only file name
        fpath = TString("");
        fpurename = fname;
    }

    //Long Peixun's update: Judge whether this geometry file is default geometry file;
    TString geompath = fBesVisPath + TString("/geom/");
    if (fname.BeginsWith(geompath))
    {
        if (fSelector.hasGeomFile(fpurename.Data()))
        {
            fCurGeom = fpurename;
            cout << "Open a default geometry " << fpurename.Data() << endl;
        }
        else
        {
            fCurGeom = TString("");
            cout << "Open a user-defined geometry " << fpurename.Data() << endl;
        }
    }
    else
    {
        fCurGeom = TString("");
        cout << "Open a user-defined geometry " << fpurename.Data() << endl;
    }

    //Long Peixun's update: Rebuilding geometry is normal operation
    // if (fDisplay->GetBesGeometry()) {
    //     Int_t ret;
    //     const char *txt;
    //     txt = Form("Geometry has already been build, Yes to rebuild it? \n (Each rebuild increase memory ~80MB)");
    //     new TGMsgBox(fClient->GetRoot(), GetMainFrame(),
    //                  "Geometry Exist", txt, kMBIconExclamation,
    //                  kMBYes | kMBNo, &ret);
    //     if (ret == kMBNo)	return;
    // }

    if ( fname.EndsWith(".gdml") ) {    //Long Peixun update: Contains->EndsWith
        //Long Peixun's update: Decide whether load MRPC and CGEM by the name of selected file
        bool mrpc = false, cgem = false;
        fpurename.ToUpper();
        if (fpurename.Contains("MRPC")) mrpc = true;
        //if (fpurename.Contains("CGEM")) cgem = true;

        fDisplay->InitGeometryFromGDML(fpath, mrpc, cgem);
        fDisplay->Draw();
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
        return ;
    }

    if ( fname.EndsWith(".root") ) {    //Long Peixun update: Contains->EndsWith
        fGeoFile = TFile::Open(fname, "read");

        TGeoVolume *volBes = (TGeoVolume*)(fGeoFile->Get("volBes"));

        if (volBes) {
            fDisplay->InitGeometryFromROOT(volBes);
            fDisplay->Draw();
            HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
            return ;
        }
        else {
            this->HandleError("This File is not a valid BesVis geometry file : \n TopVolume \"volBes\" not found!");
            sprintf(msg, "This File is not a valid BesVis geometry file");
            HandleStatusBar(msg);
            return;
        }
    }
}

//_____________________________________________________

void BesClient::SaveGeoAs() {
    //
    // save object of Top Volume "volBes" to a ROOT file
    // Open file browser

    if (!fDisplay) {
        this->HandleError("fDisplay not found !");
        return;
    }

    if (!fDisplay->GetBesGeometry()) {
        this->HandleError("Bes Geometry not found, Load from ROOT or GDML files first !");
        return;
    }

    TGeoVolume *volBes = fDisplay->GetBesGeometry()->GetVolBes();
    if (volBes) {
        // get name to save volBes to
        static TString EventDir(".");
        TGFileInfo fi;
        fi.fFileTypes = SaveGeoTypes;
        fi.fIniDir    = StrDup(EventDir);
        char msg[256];
        sprintf(msg, "Saving Geometry ----> BUSY!");
        HandleStatusBar(msg);
        new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);

        //Long Peixun's update: Consider cancel button
        if (fi.fFilename)
        {
            //Long Peixun's update: Add extension at the end of filename.
            TString filename = fi.fFilename;
            const char* ptype = SaveGeoTypes[fi.fFileTypeIdx + 1] + 1;
            if (!filename.EndsWith(ptype)) filename += ptype;
            TFile *f = new TFile(filename.Data() ,"RECREATE");
            volBes->Write();
            f->Close();
        }
    }
    else {
        this->HandleError("TGeoVolume \"volBes\" not found !");
        return;
    }

    HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: "Ready" -> GetCurrentPadString()
}

//__________________________________________________________________
//Long Peixun's update: Modify OpenEventFile to adapt to geometry selection
void BesClient::OpenEventFile() {
    //
    // Load BesVis event file
    bool auto_sel_geom = true;
    if (gBesGeometry)
    {
        Int_t clkbtn = 0;
        new TGMsgBox(gClient->GetRoot(), this, "BesVis",
            "Detector geometry has existed. Do you want to use current geometry?",
            gClient->GetPicture("mb_question_s.xpm"), kMBYes | kMBNo | kMBCancel, &clkbtn);
        if (clkbtn & kMBYes)
        {
            auto_sel_geom = false;
            fAutoGeom = false;
        }
        else if (clkbtn & kMBNo) auto_sel_geom = true;
        else return;
    }

    // Open file browser
    static TString EventDir(".");
    TGFileInfo fi;
    fi.fFileTypes = OpenEventTypes;
    fi.fIniDir    = StrDup(EventDir);
    char msg[256];
    sprintf(msg, "Loading Event File");
    HandleStatusBar(msg);
    new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
    EventDir = fi.fIniDir;
    this->OpenEventFile(fi.fFilename, auto_sel_geom);
}

//__________________________________________________________________
//Long Peixun's update: Add a parameter to determine whether choose a detector geometry automatically when open an event file.
void BesClient::OpenEventFile(TString fname, bool auto_sel_geom) {
    //
    // Open BesVis event file

    std::cout << "OpenEventFile: " << fname << std::endl;
    HandleStatusBar("Open Event File");
    char msg[256];
    //f_evtFile_1 = fname;      //Long Peixun's update: remove useless variable
    
    //Long Peixun's update: Contains -> EndsWith; Add .dst
    if (fname.EndsWith(".root") || fname.EndsWith(".rtraw") || fname.EndsWith(".rec") || fname.EndsWith(".dst"))
    {
        //Long Peixun's update: Delete old file and events
        if (fEventFile) fEventFile->Close();
        delete fEventFile;
        fEventFile = 0;
        delete fEvent;
        fEvent = 0;
        gEvent = fEvent;
        fEventTree = 0;

        fEventFile = TFile::Open(fname, "read");

        if (fEventFile)
        {
            //Long Peixun's update: Check whether this event is REC event
            if (fname.EndsWith(".rec")) isRecEvent = true;
            else isRecEvent = false;

            fEvent = new BesEvent();
            gEvent = fEvent;
            fEventTree = (TTree*)fEventFile->Get("Event");

            //Long Peixun's update: Load events only if open file successfully
            if (fEventTree)
            {
                Long64_t nEvents = fEventTree->GetEntries();
                fBesEventNo = 0;

                //Long Peixun's update: fix EventSlider range error
                if (nEvents > 0)
                {
                    if(f_bossMode == false)
                    {
                        fDigiEvent = 0;
                        fEvtHeader = 0;
                        //fTrigEvent = 0;
                        fRecEvent_1 = 0;
                        fDstEvent = 0;      //Long Peixun's update: for dst file
                        fEventTree->SetBranchAddress("TDigiEvent" , &fDigiEvent);
                        fEventTree->SetBranchAddress("TRecEvent" ,  &fRecEvent_1);
                        fEventTree->SetBranchAddress("TEvtHeader" ,  &fEvtHeader);
                        fEventTree->SetBranchAddress("TDstEvent", &fDstEvent);  //Long Peixun's update: for dst file
                        //fEventTree->SetBranchAddress("TTrigEvent" ,  &fTrigEvent);
                    }
                    GetEvent(fBesEventNo, true, auto_sel_geom);
                    fEventPlaySlider->SetRange(0, nEvents-1);
                    fEventPlaySlider->SetPosition(0);
                }
                else
                {
                    fEventPlaySlider->SetRange(0, 0);
                    fEventPlaySlider->SetPosition(0);
                }
            }
            else
            {
                this->HandleError("This File is not a valid BesVis ROOT Event File : \n Event Tree \"Event\" not found!");
                sprintf(msg, "This File contains no Event Tree \"Event\"");
                HandleStatusBar(msg);
                return;
            }
        }
        else
        {
            this->HandleError("This File is not a valid BesVis ROOT Event File!");
            sprintf(msg, "This File is not valid");
            HandleStatusBar(msg);
            return;
        }

        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: "Ready" -> GetCurrentPadString()
    }
}

//_____________________________________________________

void BesClient::SavePicAs() {
    //
    // save picture of canvas in file
    // Open file browser

    // get name to save events to
    static TString EventDir(".");
    TGFileInfo fi;
    fi.fFileTypes = SavePicTypes;
    fi.fIniDir    = StrDup(EventDir);
    char msg[256];
    sprintf(msg, "Saving Picture ----> BUSY!");
    HandleStatusBar(msg);
    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);

    if ( fi.fFilename ) {
        TString filename = fi.fFilename;

        //Long Peixun's update: Add extension at the end of filename.
        const char* ptype = SavePicTypes[fi.fFileTypeIdx + 1] + 1;
        if (!filename.EndsWith(ptype)) filename += ptype;

        if ( filename.EndsWith("gif") ||  filename.EndsWith("GIF") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"gif");
        }
        else if ( filename.EndsWith("jpg") ||  filename.EndsWith("JPG") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"jpg");
        }
        else if ( filename.EndsWith("eps") ||  filename.EndsWith("EPS") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"eps");
        }
        else if ( filename.EndsWith("ps") ||  filename.EndsWith("PS") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"ps");
        }
        else if ( filename.EndsWith("xpm") ||  filename.EndsWith("XPM") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"xmp");
        }
        else if ( filename.EndsWith("png") ||  filename.EndsWith("PNG") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"png");
        }
        else if ( filename.EndsWith("tiff") ||  filename.EndsWith("TIFF") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"tiff");
        }
        else if ( filename.EndsWith("cxx") ||  filename.EndsWith("CXX") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"cxx");
        }
        else if ( filename.EndsWith("xml") ||  filename.EndsWith("XML") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"xml");
        }
        else if ( filename.EndsWith("root") ||  filename.EndsWith("ROOT") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"root");
        }
        //Long Peixun's update: Remove hint of filename being empty
    }
    HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: "Ready" -> GetCurrentPadString()
}

//_____________________________________________________

void BesClient::SavePicAsPS() {
    //
    // save picture of canvas in file
    // Open file browser

    // get name to save events to
    static TString PSEventDir(".");
    TGFileInfo fi;
    fi.fFileTypes = SavePicPS;
    fi.fIniDir    = StrDup(PSEventDir);
    char msg[256];
    sprintf(msg, "Saving Picture as PS ----> BUSY!");
    HandleStatusBar(msg);
    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);

    if ( fi.fFilename ) {
        TString filename = fi.fFilename;
        if ( filename.EndsWith("ps") ||  filename.EndsWith("PS") ) {
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"ps");
        }
        else {
            filename += ".ps";
            fEmbeddedCanvas->GetCanvas()->Print(filename.Data(),"ps");
        }
    }
    HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: "Ready" -> GetCurrentPadString()
}

//_____________________________________________________

void BesClient::SetHome() {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) {
        Double_t x1, y1, x2, y2;
        gPad->GetRange(x1, y1, x2, y2);
        //cout << "x1 " << x1 << " x2 " << x2 << " y1 " << y1 << " y2 " << y2 << endl;
        Int_t iPad = 0;
        if (view->GetViewType() == kXYView) iPad = 0;
        else if (view->GetViewType() == kZRView) iPad = 1;
        else if (view->GetViewType() == k3DView) iPad = 2;
        fHomeX[iPad] = 0.5*(x1+x2);
        fHomeY[iPad] = 0.5*(y1+y2);
    }
}

//_____________________________________________________

void BesClient::GoHome() {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (view) {
        Double_t x1, y1, x2, y2, dx, dy;
        gPad->GetRange(x1, y1, x2, y2);
        dx = x2 - x1;
        dy = y2 - y1;

        Int_t iPad = 0;
        if (view->GetViewType() == kXYView) iPad = 0;
        else if (view->GetViewType() == kZRView) iPad = 1;
        else if (view->GetViewType() == k3DView) iPad = 2;

        gPad->Range(fHomeX[iPad]-0.5*dx, fHomeY[iPad]-0.5*dy, fHomeX[iPad]+0.5*dx, fHomeY[iPad]+0.5*dy);
    }
}

//_____________________________________________________

void BesClient::SaveMyConfig() {
    //
    // save current style to ~/.besvisMyConfig.txt

    // Int_t ret;
    // const char *txt;
    // txt = Form("Save current style to ~/.besvisMyConfig.txt, Yes to save it? ");
    // new TGMsgBox(fClient->GetRoot(), GetMainFrame(),
    //              "Save My Style", txt, kMBIconExclamation,
    //              kMBYes | kMBNo, &ret);
    // if (ret == kMBNo)	return;
    if (!fDisplay) return;

    //Long Peixun's update: Popup a save dialog
    static TString EventDir(".");
    TGFileInfo fi;
    fi.fFileTypes = StyleTypes;
    fi.fIniDir = StrDup(EventDir);
    char msg[256];
    sprintf(msg, "Saving Style ----> BUSY!");
    HandleStatusBar(msg);
    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);

    if (fi.fFilename)
    {
        //Long Peixun's update: Handle save file process
        TString filename = fi.fFilename;
        const char* ptype = StyleTypes[fi.fFileTypeIdx + 1] + 1;
        if (!filename.EndsWith(ptype)) filename += ptype;
        ofstream fileMyConfig(filename.Data(), ios_base::out);

        //Long Peixun's update: Add flag of BesVis at the head of file
        fileMyConfig << "BesVisStyle";

        BesView *viewXY = dynamic_cast<BesView*>(fDisplay->GetPadXY()->GetView());
        if (viewXY) {
            BesStatus *status = viewXY->GetStatusXY();
            fileMyConfig << *status;
            //cout << "viewXY " << *status << endl;
        }

        BesView *viewZR = dynamic_cast<BesView*>(fDisplay->GetPadZR()->GetView());
        if (viewZR) {
            BesStatus *status = viewZR->GetStatusZR();
            fileMyConfig << *status;
            //cout << "viewZR " << *status << endl;
        }

        BesView *view3D = dynamic_cast<BesView*>(fDisplay->GetPad3D()->GetView());
        if (view3D) {
            BesStatus *status = view3D->GetStatus3D();
            fileMyConfig << *status;
            //cout << "view3D " << *status << endl;
        }

        //Long Peixun's update: Add normal CGEM unfolded view config
        BesStatus statuscgem;
        statuscgem.Default(kCgemUFView0);
        fileMyConfig << statuscgem;
        statuscgem.Default(kCgemUFView1);
        fileMyConfig << statuscgem;
        statuscgem.Default(kCgemUFView2);
        fileMyConfig << statuscgem;

        fileMyConfig.close();   //Long Peixun's update: Close file stream
        cout << "Save my style finished." << endl;
    }
    HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: "Ready" -> GetCurrentPadString()
}

//_____________________________________________________

void BesClient::LoadMyConfig() {
    //
    // load my style from ~/.besvisMyConfig.txt

    //string homePath = getenv("HOME");
    //string fileName = homePath + string("/.besvisMyConfig.txt");
    //cout << "fileName " << fileName << endl;
    if (!fDisplay) return;

    //Long Peixun's update: Popup open file dialog
    static TString EventDir(".");
    TGFileInfo fi;
    fi.fFileTypes = StyleTypes;
    fi.fIniDir = StrDup(EventDir);
    char msg[256];
    sprintf(msg, "Loading Style File");
    HandleStatusBar(msg);
    new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
    EventDir = fi.fIniDir;

    if (fi.fFilename)
    {
        ifstream fileMyConfig(fi.fFilename);
    
        //Long Peixun's update: Check header of file
        string header;
        fileMyConfig >> header;
        if (header != "BesVisStyle")
        {
            HandleError("This file is not a BesVis style file!");
        }
        else
        {
            BesView *viewXY = dynamic_cast<BesView*>(fDisplay->GetPadXY()->GetView());
            if (viewXY) {
                BesStatus status;
                fileMyConfig >> status;
                viewXY->GetStatusCurrent()->Transfer(&status, true);
            }

            BesView *viewZR = dynamic_cast<BesView*>(fDisplay->GetPadZR()->GetView());
            if (viewZR) {
                BesStatus status;
                fileMyConfig >> status;
                viewZR->GetStatusCurrent()->Transfer(&status, true);
            }

            BesView *view3D = dynamic_cast<BesView*>(fDisplay->GetPad3D()->GetView());
            if (view3D) {
                BesStatus status;
                fileMyConfig >> status;
                view3D->GetStatusCurrent()->Transfer(&status, true);
            }
        }

        //Long Peixun's update: Close file stream
        fileMyConfig.close();
        UpdateStatus();
        UpdateAllView();
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: "Ready" -> GetCurrentPadString()
    }
}

//_____________________________________________________

void BesClient::LoadMdcPalette() {
    cout<<"BesClient Loading PaletteAxis ... "<<endl;

    new BesMdcPalette(gClient->GetRoot(), this);

    //TRootHelpDialog* hd1 = new TRootHelpDialog(this, "Mdc Wire Palette",200, 600);
    //hd1->ChangeSubframesBackground(GetPic("MdcPalette.gif"));
    //hd1->Popup();
}

//_____________________________________________________

void BesClient::Help() {

    //char str[32];
    //sprintf(str, "BesVis Help %s...", gROOT->GetVersion());
    //TRootHelpDialog * hd = new TRootHelpDialog(this, str, 600, 400);

    TRootHelpDialog * hd = new TRootHelpDialog(this, "Help on BesVis...", 600, 400);
    hd->SetText(f_HelpText.c_str());
    hd->Popup();
}

//_____________________________________________________

void BesClient::SetAllDisplayModeButtonUnHL() {

    fDisplayModeButton[0]->SetPicture(gClient->GetPicture("DisplayMode2D.gif"));
    fDisplayModeButton[1]->SetPicture(gClient->GetPicture("DisplayModeXY.gif"));
    fDisplayModeButton[2]->SetPicture(gClient->GetPicture("DisplayModeZR.gif"));
    fDisplayModeButton[3]->SetPicture(gClient->GetPicture("DisplayMode3D.gif"));
    fDisplayModeButton[4]->SetPicture(gClient->GetPicture("DisplayModeAll.gif"));

    for (Int_t i = 0; i < kNDisplayMode; i++) fDisplayModeButton[i]->SetState(false); //Long Peixun's update: Ajust display mode
}

//_____________________________________________________

void BesClient::X3D() {

    /*
    painter = gGeoManager->GetGeomPainter();
    //if (!painter) return;
    painter->SetRaytracing(true);
    //if (!gPad) return;
    gPad->Modified();
    gPad->Update();
    */

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) return;

    if (view && view->GetViewType() & k3DView)
    {
        //gPad->GetViewer3D();      //Long Peixun's update: This line is unnecessary
        // use TPad::Getviewer3D() instead of depreciated function x3d()
        // changed by tianhl at Mon Aug 20 2007
        fViewer3DMode = 1;
        //Long Peixun's update: Add parameter "x3d"; dynamic_cast
        TViewerX3D *x3d = dynamic_cast<TViewerX3D*>(gPad->GetViewer3D("x3d"));
        if (!x3d) cout << " x3d does not exist "<< endl;
        else x3d->ExecCommand(0,0,'r');
    }
    else
    {
        //Long Peixun's update: Switching to 3D View instead of showing error message
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[3]->SetState(true);
		fDisplayModeButton[3]->SetPicture(gClient->GetPicture("DisplayMode3DST.gif"));
		fDisplay->SwitchDisplayMode(3);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
        X3D();
    }
}

//_____________________________________________________

void BesClient::OpenGL() {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) return;

    //Long Peixun's update: Remove annotation
    if (view->GetViewType() & k3DView)
    {
        // use TPad::GetViewer3D() instead of depreciated function TPad::x3d
        // changed by tianhl at Mon Aug 20 2007
        fViewer3DMode = 2;
        //Long Peixun's update: TVirtualViewer3D -> TGLViewer; Don't reset when update; dynamic_cast
        TGLViewer *ogl = dynamic_cast<TGLViewer*>(gPad->GetViewer3D("ogl"));
        if (ogl)
        {
            ogl->SetResetCamerasOnUpdate(false);
            ogl->SetClearColor(kWhite);
        }
        else cout << " ogl does not exist " << endl;
        UpdateCurrentPad();
    }
    else
    {
        //Long Peixun's update: Switching to 3D View instead of showing error message
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[3]->SetState(true);
		fDisplayModeButton[3]->SetPicture(gClient->GetPicture("DisplayMode3DST.gif"));
		fDisplay->SwitchDisplayMode(3);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
        OpenGL();
    }
}


//__________________________________________________________________

void BesClient::CloseWindow() {
    //
    // Got close message for this MainFrame. Calls parent CloseWindow())
    // (which destroys the window) and terminate the application.
    // The close message is generated by the window manager when its close
    // window menu item is selected.
    TGMainFrame::CloseWindow();
    gApplication->Terminate(0);  // 0 = OK
}

//_____________________________________________________
//Long Peixun's update: Get run number and choose a detector geometry automatically when in need.
Bool_t BesClient::GetEvent(Long64_t i, bool openfile, bool sel_geom)
{
    if (fEventTree) {

	Long64_t nEvents = fEventTree->GetEntries();
	if (i >= 0 && i < nEvents) {
        
	    fDigiEvent = 0;
	    fEvtHeader = 0;
	    fRecEvTime = 0;
	    //fTrigEvent = 0;
	    TDisTrack * fRecEvent = new TDisTrack();
	    if (f_bossMode == true)
        {
            TBranch *digiEvent = fEventTree->GetBranch("TDigiEvent");
            TBranch *evtHeader = fEventTree->GetBranch("TEvtHeader");
            //TBranch *trigEvent = fEventTree->GetBranch("TTrigEvent");
            TBranch *disTrack  = fEventTree->GetBranch("TDisTrack");
            digiEvent->SetAddress(&fDigiEvent);
            //trigEvent->SetAddress(&fTrigEvent);
            disTrack->SetAddress(&fRecEvent);
            disTrack->GetEntry(i);
            digiEvent->GetEntry(i);
            evtHeader->GetEntry(i);
            //trigEvent->GetEntry(i);
	    }
        
	    if (f_bossMode == false)
        {
            //Long Peixun add these to debug opening event
            cout << "[LPX] Get Event Entry test begin" << endl;
		    fEventTree->GetEntry(i);    //There are some errors here when open event file again
            cout << "[LPX] Successfully get Event Entry" << endl;

            //Long Peixun's update: Judge the change of run number
            /* Long64_t newrunno = fEvtHeader->getRunId();
            cout << "[LPX] fAutoGeom == ";
            if (fAutoGeom) cout << "true" << endl;
            else cout << "false" << endl;
            if (fAutoGeom && fBesRunNo != newrunno && !sel_geom)
            {
                cout << "[LPX] fBesRunNo has changed!" << endl;
                const char* ogfile = fSelector.getGeomFileNameFromRunNo(fBesRunNo);
                const char* ngfile = fSelector.getGeomFileNameFromRunNo(newrunno);
                if (strcmp(ogfile, ngfile) != 0 && ngfile[0] != '\0')
                {
                    Int_t clkbtn = 0;
                    new TGMsgBox(gClient->GetRoot(), this, "BesVis",
                        "The run number has changed. Do you want to let BesVis change the detector geometry into a suitable one?",
                        gClient->GetPicture("mb_question_s.xpm"), kMBYes | kMBNo, &clkbtn);
                    if (clkbtn & kMBYes)
                    {
                        cout << "Select geometry file: " << ngfile << endl;
                        TString geomfile = fBesVisPath + TString("/geom/");
                        geomfile += ngfile;
                        OpenGeoFile(geomfile);
                    }
                }
            } */

            //Long Peixun's update: Set fBesRunNo
            fBesRunNo = fEvtHeader->getRunId();
            cout << "Run Number: " << fBesRunNo << endl;

            //Long Peixun's update: if flag sel_geom is true, select a suitable detector geometry
            if (openfile)
            {
                const char* ret = fSelector.getGeomFileNameFromRunNo(fBesRunNo);
                if (sel_geom)
                {
                    if (ret[0] == '\0')
                    {
                        HandleError("There is no suitable delector geometry! Please select a geometry file.");
                        LoadGeoFile();
                    }
                    else
                    {
                        cout << "Select geometry file: " << ret << endl;
                        TString geomfile = fBesVisPath + TString("/geom/");
                        geomfile += ret;
                        OpenGeoFile(geomfile);
                        fAutoGeom = true;
                    }
                }
                else
                {
                    if (fCurGeom.Length() > 0)
                    {
                        TString ts_ret = TString(ret);
                        if (ts_ret != fCurGeom)
                        {
                            new TGMsgBox(fClient->GetRoot(), GetMainFrame(), "BesVis",
                                "Maybe you have selected an unsuitable geometry!",
                                kMBIconExclamation, kMBDismiss);
                        }
                    }
                    fAutoGeom = false;
                }
            }

            //yzhang get event start time
            if(fRecEvent_1){
                if(fRecEvent_1->getEvTimeCol()->GetEntries()==1){
                fRecEvTime = (TRecEvTime*) fRecEvent_1->getEvTimeCol()->At(0);
                }else{
                cout<<"WARNING:EsTimeCol size!=1, size="<<fRecEvent_1->getEvTimeCol()->GetEntries()<<endl;
                }
            }
            //zhangy

            int no = 0;
            
            //recTrack1 = new TRecMdcTrack[20];
            //if contains reconstruction information
            if (fRecEvent_1)
            {
                //Mdc tracks
                //Long Peixun's update: Remove "if"; Move "delete [] recTrack1" here
                no = 0;
                delete [] recTrack1;
                if (fRecEvent_1->getRecMdcTrackCol())
                    no = (fRecEvent_1->getRecMdcTrackCol())->GetEntries();
                //if (no>20) no=20;
                recTrack1 = new TRecMdcTrack[no];   //Long Peixun's update: Remove limit of Mdc tracks number
                for (int i = 0; i < no; i++){
                    const TRecMdcTrack* recTrack = fRecEvent_1->getRecMdcTrack(i);
                    recTrack1[i].setTRecMdcTrack(recTrack);   //Long Peixun's update: "(pointer+i)->" -> "pointer[i].", more clear
                    fRecEvent->addRecMdcTrack(recTrack1 + i);
                }

                ////yzhang
                //if (kalTrack){
                //    delete [] kalTrack;
                //    kalTrack = NULL;
                //}
                //kalTrack = new TRecMdcKalTrack[20];
                //no=0;
                //if (fRecEvent_1->getRecMdcKalTrackCol()){
                //    no=(fRecEvent_1->getRecMdcKalTrackCol())->GetEntries();
                //}
                //if (no>20) no=20;
                //for (int i=0;i<no;i++){
                //    TRecMdcKalTrack* kalTrack1 = const_cast<TRecMdcKalTrack*> (fRecEvent_1->getRecMdcKalTrack(i));
                //    //(kalTrack+i)->setTRecMdcKalTrack(kalTrack1);
                //    fRecEvent->addRecMdcKalTrack(kalTrack1);
                //}
                ////zhangy

                //Tof tracks
                //Long Peixun's update: Remove "if"
                delete [] tofTrack;
                no = 0;
                //tofTrack = new TRecTofTrack[200];
                if (fRecEvent_1->getTofTrackCol())
                    no = (fRecEvent_1->getTofTrackCol())->GetEntries();
                //if (no>200) no =200;
                tofTrack = new TRecTofTrack[no];    //Long Peixun's update: Remove limit of Tof tracks number
                for (int i = 0; i < no; i++){
                    const TRecTofTrack* tofTrack1 = fRecEvent_1->getTofTrack(i);
                    tofTrack[i].setTRecTofTrack(tofTrack1);   //Long Peixun's update: "(pointer+i)->" -> "pointer[i].", more clear
                    fRecEvent->addTofTrack(tofTrack + i);
                }

                //Mdc hits
                //Long Peixun's update: Remove "if"
                delete [] mdchit;
                //mdchit = new TRecMdcHit[1000];
                no = 0;
                if (fRecEvent_1->getRecMdcHitCol())
                    no = (fRecEvent_1->getRecMdcHitCol())->GetEntries();
                //if (no>1000) no =1000;
                mdchit = new TRecMdcHit[no];    //Long Peixun's update: Remove limit of Mdc hits number
                for (int i = 0; i < no; i++){
                    const TRecMdcHit* mdchit1 = fRecEvent_1->getRecMdcHit(i);
                    mdchit[i].setTRecMdcHit(mdchit1);   //Long Peixun's update: "(pointer+i)->" -> "pointer[i].", more clear
                    fRecEvent->addRecMdcHit(mdchit + i);
                }

                //Muc Hits
                //Long Peixun's update: Remove "if"
                delete [] muctrk;
                //muctrk = new TRecMucTrack[20];
                no = 0;
                if (fRecEvent_1->getMucTrackCol())
                    no = (fRecEvent_1->getMucTrackCol())->GetEntries();
                //if (no>20) no=20;
                muctrk = new TRecMucTrack[no];  //Long Peixun's update: Remove limit of Muc tracks number
                for (int i = 0; i < no; i++){
                    const TRecMucTrack* mucTrack1 = fRecEvent_1->getMucTrack(i);
                    muctrk[i].setTRecMucTrack(mucTrack1);   //Long Peixun's update: "(pointer+i)->" -> "pointer[i].", more clear
                    fRecEvent->addMucTrack(muctrk + i);
                }

                //Emc shower
                //Long Peixun's update: Remove "if"
                delete [] emcshower;
                //emcshower = new TRecEmcShower[20];
                no = 0;
                if (fRecEvent_1->getEmcShowerCol())
                    no = (fRecEvent_1->getEmcShowerCol())->GetEntries();
                //if (no>20) no=20;
                emcshower = new TRecEmcShower[no];  //Long Peixun's update: Remove limit of Emc shower number
                for (int i = 0; i < no; i++){
                    const TRecEmcShower* rec_emc = fRecEvent_1->getEmcShower(i);
                    emcshower[i].setTRecEmcShower(rec_emc); //Long Peixun's update: "(pointer+i)->" -> "pointer[i].", more clear
                    fRecEvent->addEmcShower(emcshower + i);
                }
            }
            //Long Peixun's update: For dst file, convert dst tracks to rec tracks
            else if (fDstEvent)
            {
                cout << "BesClient Construct DST tracks" << endl;
                //Mdc dst tracks
                no = 0;
                delete [] recTrack1;
                if (fDstEvent->getMdcTrackCol())
                    no = (fDstEvent->getMdcTrackCol())->GetEntries();
                recTrack1 = new TRecMdcTrack[no];
                for (int i = 0; i < no; i++)
                {
                    cvtMdcDst2MdcRec(recTrack1 + i, fDstEvent->getMdcTrack(i));  //Convert dst tracks to rec tracks
                    fRecEvent->addRecMdcTrack(recTrack1 + i);
                }

                //Tof dst tracks
                no = 0;
                delete [] tofTrack;
                if (fDstEvent->getTofTrackCol())
                    no = (fDstEvent->getTofTrackCol())->GetEntries();
                tofTrack = new TRecTofTrack[no];
                for (int i = 0; i < no; i++)
                {
                    cvtTofDst2TofRec(tofTrack + i, fDstEvent->getTofTrack(i));  //Convert dst tracks to rec tracks
                    fRecEvent->addTofTrack(tofTrack + i);
                }

                //Emc dst tracks
                no = 0;
                delete [] emcshower;
                if (fDstEvent->getEmcTrackCol())
                    no = (fDstEvent->getEmcTrackCol())->GetEntries();
                emcshower = new TRecEmcShower[no];
                for (int i = 0; i < no; i++)
                {
                    cvtEmcDst2EmcRec(emcshower + i, fDstEvent->getEmcTrack(i));  //Convert dst tracks to rec tracks
                    fRecEvent->addEmcShower(emcshower + i);
                }

                //Muc dst tracks
                no = 0;
                delete [] muctrk;
                if (fDstEvent->getMucTrackCol())
                    no = (fDstEvent->getMucTrackCol())->GetEntries();
                muctrk = new TRecMucTrack[no];
                for (int i = 0; i < no; i++)
                {
                    cvtMucDst2MucRec(muctrk + i, fDstEvent->getMucTrack(i));  //Convert dst tracks to rec tracks
                    fRecEvent->addMucTrack(muctrk + i);
                }
            }
        }
        
        //Long Peixun's update: Reset detector 3D color
        fDisplay->GetBesGeometry()->SetPhysicalDefaultVis();
	    //fEvent->SetEvent(fDigiEvent, fRecEvent, fEvtHeader, fTrigEvent);
        //Long Peixun's update: Add "isRecEvent"
        fEvent->SetEvent(fDigiEvent, fRecEvent, fEvtHeader, fRecEvTime, isRecEvent); 

	    //delete fRecEvent;
	    UpdateAllView();
	    UpdateStatus();
	    return true;
	}
	else {
	    fAutoDisplayEvent = kFALSE;
	    if (fAutoDisplayEventTimer) {
		fAutoDisplayEventTimer->TurnOff();
		fPlayEventButton->SetPicture(gClient->GetPicture("ButtonEventPlay.gif"));
	    }

	    ostringstream s;
	    s << "Request event entry " << i
		<< " does not exist ! \n valid ("
		<< 0 << "~" << nEvents - 1  << ")";
	    this->HandleError(s.str().c_str());
	    return false;
	}
    }
    else {
	fAutoDisplayEvent = kFALSE;
	if (fAutoDisplayEventTimer) fAutoDisplayEventTimer->TurnOff();
	this->HandleError("Event Tree does not exist !");
	return false;
    }
}

Bool_t BesClient::GetRecEvent(){
    int		semid, shmid, n, runNo;
    int		*shmptr;
    int         sem_value_F, sem_value_O;

    // set autodisplay on
    if (fAutoDisplayEvent){
	if (fAutoDisplayEventTimer)
	    fAutoDisplayEventTimer->TurnOn();
	fPlayEventButton->SetPicture(gClient->GetPicture("ButtonEventStop.gif"));
    }

    // access semaphore
    if ((semid = semget(f_pid, 2, 0)) == -1){
	perror("concumer -- access -- semget");
	exit(0);
    }
    else {
	acquire.sem_num = OUTPUT_STORE;
	//*******************************
	// Debug information
	//*******************************
	//std::cout << "acquire.sem_num: " << OUTPUT_STORE << std::endl;
	//std::cout << "acquire.sem_num: " << acquire.sem_num << std::endl;
	//std::cout << "acquire.sem_op:  " << acquire.sem_op << std::endl;
	//
	//std::cout << "Current Event No. : " << fCurrentEvent << std::endl;
	//std::cout << "besvis has gotten semid: " << semid << std::endl;
	//std::cout << "before change OUTPUT_STORE" << std::endl;

	//if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
	//  perror("Can not get FREE_SPACE");
	//}
	////if (sem_value_F == 1) return true;
	//std::cout << "Semaphore FREE_SPACE has value of(refer 0) " <<  sem_value_F << std::endl;


	//std::cout << "OUTPUT_STORE will decrease from 1 to 0" << std::endl;
	//*******************************
	// operate semaphore:OUTPUT_STORE
	//*******************************
	if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
	    perror("Can not get OUTPUT_STORE");
	}
	//std::cout << "Semaphore OUTPUT_STORE has value of(refer 1) " << sem_value_O << std::endl;
	if (sem_value_O == 0) return true;
	if (f_runStatus == RUN_ERROR){
	    release.sem_num = FREE_SPACE;
	    if (semop(semid, &release, 1) == -1){
		perror("consumer -- increase -- freeSpace");
		exit(0);
	    }
	    std::cout << "read data error " <<  std::endl;
	    f_runStatus = RUN_SMOOTH;
	    return true;
	}
	f_runStatus = RUN_ERROR;

	if (semop(semid, &acquire, 1) == -1){
	    perror("consumer -- decrease -- storage");
	    exit(0);
	}

	//*******************************
	// Debug information
	//*******************************
	//std::cout << "Current Event No. : " << fCurrentEvent << std::endl;
	//std::cout << "besvis has gotten semid: " << semid << std::endl;
	//std::cout << "besvis will read data" << std::endl;
	//std::cout << "OUTPUT_STORE must decrease from 1 to 0" << std::endl;

	//if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
	//  perror("Can not get FREE_SPACE");
	//}
	//std::cout << "Semaphore FREE_SPACE has value of(refer 0) " <<  sem_value_F << std::endl;
	//
	//if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
	//  perror("Can not get OUTPUT_STORE");
	//}
	//std::cout << "Semaphore OUTPUT_STORE has value of(refer 0) " << sem_value_O << std::endl;
	//*******************************
	if (fCurrentEvent >= 1){
	    if (fDigiEvent){
		//fDigiEvent->Clear();
		delete fDigiEvent;
		fDigiEvent=0;
	    }
	    if (fEvtHeader){
		fEvtHeader->Clear();
		delete fEvtHeader;
		fEvtHeader=0;
	    }
	    //if (fTrigEvent){
	    //    //fTrigEvent->Clear();
	    //    delete fTrigEvent;
	    //    fTrigEvent=0;
	    //}
	    if (fEvent){
		delete fEvent;
		fEvent = NULL;
	    }
	}


	//*******************************
	// read from share file
	//*******************************
	OpenEventFile(f_evtFile);
	if (fEventFile == NULL){
	    return true;
	}
	if (fEventTree)
	    delete fEventTree;
	fEventFile->Close();
	delete fEventFile;
	//*******************************

	release.sem_num = FREE_SPACE;
	//*******************************
	// Debug information
	//*******************************
	//std::cout << "release.sem_num: " << FREE_SPACE << std::endl;
	//std::cout << "release.sem_num: " << release.sem_num << std::endl;
	//std::cout << "release.sem_op:  " << release.sem_op << std::endl;
	//
	//std::cout << "Current Event No. : " << fCurrentEvent << std::endl;
	//std::cout << "besvis has gotten semid: " << semid << std::endl;
	//std::cout << "besvis has read data" << std::endl;
	//std::cout << "before change FREE_SPACE" << std::endl;

	//if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
	//  perror("Can not get FREE_SPACE");
	//}
	//std::cout << "Semaphore FREE_SPACE has value of(refer 0) " <<  sem_value_F << std::endl;
	//
	//if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
	//  perror("Can not get OUTPUT_STORE");
	//}
	//std::cout << "Semaphore OUTPUT_STORE has value of(refer 0) " << sem_value_O << std::endl;
	//
	//std::cout << "FREE_SPACE will increase from 0 to 1" << std::endl;
	//*******************************
	// operate semaphore:FREE_SPACE
	//*******************************
	if (semop(semid, &release, 1) == -1){
	    perror("consumer -- increase -- freeSpace");
	    exit(0);
	}
	std::cout << "Current Event No. : " << fCurrentEvent++ << std::endl;
	//*******************************
	// Debug information
	//*******************************
	//std::cout << "besvis has gotten semid: " << semid << std::endl;
	//std::cout << "besvis has read data" << std::endl;
	//std::cout << "FREE_SPACE must increase from 0 to 1" << std::endl;

	//if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
	//  perror("Can not get FREE_SPACE");
	//}
	//std::cout << "Semaphore FREE_SPACE has value of(refer 1) " <<  sem_value_F << std::endl;
	//
	//if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
	//  perror("Can not get OUTPUT_STORE");
	//}
	//std::cout << "Semaphore OUTPUT_STORE has value of(refer 0) " << sem_value_O << std::endl;
	//*******************************
    }
    f_runStatus = RUN_SMOOTH;
    return true;
}
//_____________________________________________________

Bool_t BesClient::NextEvent()
{
    //Long Peixun's update: Check range before shift
    if (!fEventTree) return false;
    if (fBesEventNo + 1 >= fEventTree->GetEntries()) return false;
    
    Bool_t status;
    fBesEventNo++;
    if ( f_bossMode == false) {
	status = GetEvent(fBesEventNo);
    }
    else if ( f_bossMode == true) {
	std::cout << "In Boss Mode, execute NextEvent()" << std::endl;
	status = GetRecEvent();
    }
    if (!status) fBesEventNo--;
    return status;
}

//_____________________________________________________

Bool_t BesClient::PrevEvent()
{
    //Long Peixun's update: Check range before shift
    if (!fEventTree) return false;
    if (fBesEventNo <= 0) return false;

    Bool_t status;
    fBesEventNo--;
    if ( f_bossMode == false){
	status = GetEvent(fBesEventNo);
    }
    else if ( f_bossMode == true){
	// add error box
	this->HandleError("Boss Mode can not get previous event!");
    }
    if (!status) fBesEventNo++;
    return status;
}

//_____________________________________________________

Bool_t BesClient::FirstEvent()
{
    Bool_t status;
    if ( f_bossMode == false){
	fBesEventNo = 0;
	status = GetEvent(fBesEventNo);
    }
    else if ( f_bossMode == true){
	// add error box
	this->HandleError("Boss Mode can not get first event!");
    }
    return status;
}

//_____________________________________________________
//Long Peixun's update: Get last event
Bool_t BesClient::LastEvent()
{
    Bool_t status;
    if ( f_bossMode == false){
	    fBesEventNo = fEventTree->GetEntries() - 1;
	    status = GetEvent(fBesEventNo);
    }
        else if ( f_bossMode == true){
        // add error box
        this->HandleError("Boss Mode can not get last event!");
    }
    return status;
}

//_____________________________________________________

void BesClient::AutoDisplayEvent()
{
    //if (fEventTree) {
    fAutoDisplayEvent = !fAutoDisplayEvent;
    std::cout << "(AutoDisplayEvent)fAutoDisplayEvent: " << fAutoDisplayEvent << std::endl;
    if (fAutoDisplayEventTimer) {
	if (fAutoDisplayEvent) {
	    fPlayEventButton->SetPicture(gClient->GetPicture("ButtonEventStop.gif"));
	    fAutoDisplayEventTimer->TurnOn();
	}
	else {
	    fPlayEventButton->SetPicture(gClient->GetPicture("ButtonEventPlay.gif"));
	    fAutoDisplayEventTimer->TurnOff();
	}
    }
    //}

    /*
       fAutoDisplayEvent = kTRUE;
       if (gDebug) cout << "Into AutoDisplayEvent(), fAutoDisplayEvent = " << fAutoDisplayEvent << " fEventPlaySpeed " << fEventPlaySpeed << endl;

       if (fAutoDisplayEventTimer) fAutoDisplayEventTimer->TurnOn();
       */
}

//_____________________________________________________

void BesClient::AutoDisplayEventCommand()
{
    NextEvent();
    UpdateBesInputFields();
}

//_____________________________________________________

void BesClient::AutoRotate()
{
    fAutoRotate = !fAutoRotate;

    if (!fAutoRotate) {
	fAutoRotateClockWise = 0;
	fAutoRotateTheta = 0;
	fAutoRotatePhi   = 0;
	fAutoRotatePsi   = 0;
    }

    if (fAutoRotateTimer) {
	if (fAutoRotate) fAutoRotateTimer->TurnOn();
	else fAutoRotateTimer->TurnOff();
    }
}

//_____________________________________________________

void BesClient::AutoRotateCommand()
{
    if (fAutoRotateClockWise != 0) {
	RotateClockWise(fAutoRotateClockWise);
    }

    if (fAutoRotateTheta != 0) {
	RotateTheta(fAutoRotateTheta);
    }

    if (fAutoRotatePhi != 0) {
	RotatePhi(fAutoRotatePhi);
    }

    if (fAutoRotatePsi != 0) {
	RotatePsi(fAutoRotatePsi);
    }

    /*
       Int_t iret;
       if (fDisplay->GetPadXY()->GetView()) {
       fDisplay->GetPadXY()->GetView()->SetView(fDisplay->GetPadXY()->GetView()->GetLongitude()+fRotateStep,
       fDisplay->GetPadXY()->GetView()->GetLatitude(),
       fDisplay->GetPadXY()->GetView()->GetPsi(), iret);
       fDisplay->GetPadXY()->Modified();
       fDisplay->GetPadXY()->Update();
       }
       if (fDisplay->GetPadZR()->GetView()) {
       fDisplay->GetPadZR()->GetView()->SetView(fDisplay->GetPadZR()->GetView()->GetLongitude()+fRotateStep,
       fDisplay->GetPadZR()->GetView()->GetLatitude(),
       fDisplay->GetPadZR()->GetView()->GetPsi(), iret);
       fDisplay->GetPadZR()->Modified();
       fDisplay->GetPadZR()->Update();
       }
       */

    UpdateCurrentPad();
    UpdateBesInputFields();
}

//_____________________________________________________

void BesClient::RotateClockWise(int clockwise) // -1 = counterclockwise
{
    Double_t phi = 0.0;
    Int_t    iret;
    if (fDisplay->GetPadXY()->GetView()) {
	phi = fDisplay->GetPadXY()->GetView()->GetLongitude();
	phi += clockwise * fRotateStep;
	fDisplay->GetPadXY()->GetView()->SetView(phi,
		fDisplay->GetPadXY()->GetView()->GetLatitude(),
		fDisplay->GetPadXY()->GetView()->GetPsi(), iret);
	fDisplay->GetPadXY()->Modified();
	fDisplay->GetPadXY()->Update();
    }
    if (fDisplay->GetPadZR()->GetView()) {
	//phi = fDisplay->GetPadZR()->GetView()->GetLongitude();
	//phi += clockwise * fRotateStep;
	phi += 180.0;
	fDisplay->GetPadZR()->GetView()->SetView(phi,
		fDisplay->GetPadZR()->GetView()->GetLatitude(),
		fDisplay->GetPadZR()->GetView()->GetPsi(), iret);
	fDisplay->GetPadZR()->Modified();
	fDisplay->GetPadZR()->Update();
    }
}

//_____________________________________________________

void BesClient::RotateTheta(int pn) // 1 plus, -1 minus
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (view) {
	Double_t theta = view->GetLatitude() + pn*fRotateStep;
	Int_t iret;
	SetRange(theta, 0.0, 180.0);
	view->SetView(view->GetLongitude(), theta, view->GetPsi(), iret);
	//gPad->Modified();
	//gPad->Update();
    }
}

//_____________________________________________________

void BesClient::RotatePhi(int pn) // 1 plus, -1 minus
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (view) {
	Double_t phi = view->GetLongitude() + pn*fRotateStep;
	Int_t iret;
	SetRange(phi, 0.0, 360.0);
	view->SetView(phi, view->GetLatitude(), view->GetPsi(), iret);
	//gPad->Modified();
	//gPad->Update();
    }
}

//_____________________________________________________

void BesClient::RotatePsi(int pn) // 1 plus, -1 minus
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (view) {
	Double_t psi = view->GetPsi() + pn*fRotateStep;
	Int_t iret;
	SetRange(psi, 0.0, 360.0);
	view->SetView(view->GetLongitude(), view->GetLatitude(), psi, iret);
	//gPad->Modified();
	//gPad->Update();
    }
}

//__________________________________________________________________

void BesClient::HandleEventList(TGListTreeItem *entry, Int_t btn) {
    //
    // Event list handling for buttons

    if ( entry->GetFirstChild() != 0 ) {
	// Run folder
	if ( entry->IsOpen() ) {
	    fEventListTree->CloseItem(entry);
	} else {
	    fEventListTree->OpenItem(entry);
	}
    } else {
	fEventListTree->HighlightItem(entry);
	gClient->NeedRedraw(fEventListTree);

	// Event item
	fItem = entry;
	fRunItem = entry->GetParent();
	TString msg1("Displaying Run ");
	HandleStatusBar(msg1.Data());

	SetState(); // includes canvas update

	TString msg2("Run ");
	HandleStatusBar(msg2.Data());
    }

    // Redraw canvas
    TCanvas *canvas = (TCanvas*)fEmbeddedCanvas->GetCanvas();
    canvas->Modified();
    canvas->Update();
}

//__________________________________________________________________

void BesClient::ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel) {
    //
    // Actions in reponse to mouse button events

    // Get view from current pad
    if ( !gBesGeometry ) {
        //cout << "there is not BesGeometry" << endl; 	// added by tianhl to debug event
        return;
    }


    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    TString viewInfo;
    if (view) viewInfo = TString(view->GetObjectInfo(px, py));

    switch (event)
    {
	case kKeyPress:
	    // px = char code of pressed key
	    // py = counter
	    if ( py <= 0 ) py = 1; // Reset counter

        //Long Peixun's update: Modify the easy key
        switch (Char_t(px))
        {
		case '.':                   //Long Peixun modify: Next event: nN -> .>
		case '>':
		    NextEvent();
		    break;
		case ',':                   //Long Peixun modify: Prev event: pP -> ,<
		case '<':
		    PrevEvent();
		    break;
        case 'm':                   //Long Peixun add: First event: mM
        case 'M':
            FirstEvent();
            break;
        case '/':                   //Long Peixun add: Last event: /?
        case '?':
            LastEvent();
            break;
		case 'g':                   //Save picture as
		case 'G':
		    SavePicAs();
            break;
        }

        if (!view) break;   //Long Peixun's update: check view pointer before execute keys which need view

        //Long Peixun's update: Modify the easy key
	    switch (Char_t(px))
        {
		case '-':                   //Long Peixun modify: Zoom out: -_qQ
		case '_':
		case 'q':
		case 'Q':
		    view->ZoomOut();
            fEmbeddedCanvas->GetCanvas()->Update();
		    break;
		case '+':                   //Long Peixun modify: Zoom in: +=eE
		case '=':
		case 'e':
		case 'E':
		    view->ZoomIn();
            fEmbeddedCanvas->GetCanvas()->Update();
		    break;
		case 'a':                   //Long Peixun modify: Move left: aD
		case 'D':
		    view->Move(-10,0);
		    fEmbeddedCanvas->GetCanvas()->Update();
		    break;
		case 'd':                   //Long Peixun modify: Move right: dA
		case 'A':
		    view->Move(10,0);
		    fEmbeddedCanvas->GetCanvas()->Update();
		    break;
		case 'w':                   //Long Peixun modify: Move up: wS
		case 'S':
		    view->Move(0,-10);
		    fEmbeddedCanvas->GetCanvas()->Update();
		    break;
		case 's':                   //Long Peixun modify: Move down: sW
		case 'W':
		    view->Move(0,10);
		    fEmbeddedCanvas->GetCanvas()->Update();
		    break;
        case 'o':                   //Long Peixun add: Move center: oO
        case 'O':
            view->Center();
            fEmbeddedCanvas->GetCanvas()->Update();
            break;
	    }
	    break;

	default:
	    if ( sel != 0 ) {
            //if ( !gPad->InheritsFrom("BesCanvas") ) {
            if ( gPad->GetName() != TString("PadHeader")) {
                TString info(sel->GetTitle());
                info.Append(": ");
                info.Append(sel->GetObjectInfo(px, py));
                //info.Append(viewInfo);
                HandleInfoBar(info.Data());
            }
	    }
	    break;
	}

	UpdateBesInputFields();
}

    //__________________________________________________________________

    void BesClient::SetState(Int_t id) {
	//
	// set geometry state and update Canvas
	if (id == -1) {
	    TGButton *btn = (TGButton *) gTQSender;
	    id = btn->WidgetId();
	}

	BesView *view = dynamic_cast<BesView*>(gPad->GetView());
	if ( view ) {
	    switch (id) {

		case   kM_Header_Global:
            //Long Peixun's update: Add Header control code
            fDisplay->SetVisHeader(fChkBtnHeaderGlobal->GetState());
		    break;

		case   kM_Mdc_Global:
		    view->SetVisMdcGlobal(fChkBtnMdcGlobal->GetState());
		    //gBesGeometry->GetMdcROOTGeo()->SetDetectorOn();
		    //cout << "Mdc Global vis " << endl;
		    break;

		case   kM_Mdc_Tubes:
		    view->SetVisMdcTubes(fChkBtnMdcTubes->GetState());
		    //HandleViewOptionMenu(kM_Mdc_Tubes);
		    break;

		case   kM_Mdc_Wires:
		    view->SetVisMdcWires(fChkBtnMdcWires->GetState());
		    //HandleViewOptionMenu(kM_Mdc_Wires);
		    break;

		case   kM_Tof_Global:
		    view->SetVisTofGlobal(fChkBtnTofGlobal->GetState());
		    //cout << "Tof Global vis " << endl;
		    break;

		case   kM_Tof_East:
		    view->SetVisTofEast(fChkBtnTofEast->GetState());
		    break;

		case   kM_Tof_Barrel:
		    view->SetVisTofBarrel(fChkBtnTofBarrel->GetState());
		    break;

		case   kM_Tof_West:
		    view->SetVisTofWest(fChkBtnTofWest->GetState());
		    break;

		case   kM_Emc_Global:
		    view->SetVisEmcGlobal(fChkBtnEmcGlobal->GetState());
		    //cout << "Emc Global vis " << endl;
		    break;

		case   kM_Emc_East:
		    view->SetVisEmcEast(fChkBtnEmcEast->GetState());
		    break;

		case   kM_Emc_Barrel:
		    view->SetVisEmcBarrel(fChkBtnEmcBarrel->GetState());
		    break;

		case   kM_Emc_West:
		    view->SetVisEmcWest(fChkBtnEmcWest->GetState());
		    break;

		case   kM_Emc_Side:
		    view->SetVisEmcSide(fChkBtnEmcSide->GetState());
		    break;

		case   kM_Muc_Global:
		    view->SetVisMucGlobal(fChkBtnMucGlobal->GetState());
		    //cout << "Muc Global vis " << endl;
		    break;

		case   kM_Muc_East:
		    view->SetVisMucEast(fChkBtnMucEast->GetState());
		    break;

		case   kM_Muc_Barrel:
		    view->SetVisMucBarrel(fChkBtnMucBarrel->GetState());
		    break;

		case   kM_Muc_West:
		    view->SetVisMucWest(fChkBtnMucWest->GetState());
		    break;

		case   kM_Muc_Strips:
		    view->SetVisMucStrips(fChkBtnMucStrips->GetState());
		    break;

		case   kM_BeamPipe:
		    view->SetVisBeamPipe(fChkBtnBeamPipe->GetState());
		    break;

		case   kM_ZRPlaneOnXY:
		    view->SetVisZRPlaneOnXY(fChkBtnZRPlaneOnXY->GetState());
		    break;

		case   kM_Axis:
		    view->SetVisAxis(fChkBtnAxis->GetState());
		    break;

		case   kM_MdcHits_Global:
		    view->SetVisMdcHitsGlobal(fChkBtnMdcHitsGlobal->GetState());
		    break;

		case   kM_TofHits_Global:
		    view->SetVisTofHitsGlobal(fChkBtnTofHitsGlobal->GetState());
		    break;

		case   kM_TofHits_East:
		    view->SetVisTofHitsEast(fChkBtnTofHitsEast->GetState());
		    break;

		case   kM_TofHits_Barrel:
		    view->SetVisTofHitsBarrel(fChkBtnTofHitsBarrel->GetState());
		    break;

		case   kM_TofHits_West:
		    view->SetVisTofHitsWest(fChkBtnTofHitsWest->GetState());
		    break;

		case   kM_EmcHits_Global:
		    view->SetVisEmcHitsGlobal(fChkBtnEmcHitsGlobal->GetState());
		    break;

		case   kM_EmcHits_East:
		    view->SetVisEmcHitsEast(fChkBtnEmcHitsEast->GetState());
		    break;

		case   kM_EmcHits_Barrel:
		    view->SetVisEmcHitsBarrel(fChkBtnEmcHitsBarrel->GetState());
		    break;

		case   kM_EmcHits_West:
		    view->SetVisEmcHitsWest(fChkBtnEmcHitsWest->GetState());
		    break;

		case   kM_EmcHits_Side:
		    view->SetVisEmcHitsSide(fChkBtnEmcHitsSide->GetState());
		    break;

		case   kM_MucHits_Global:
		    view->SetVisMucHitsGlobal(fChkBtnMucHitsGlobal->GetState());
		    break;

		case   kM_MucHits_East:
		    view->SetVisMucHitsEast(fChkBtnMucHitsEast->GetState());
		    break;

		case   kM_MucHits_Barrel:
		    view->SetVisMucHitsBarrel(fChkBtnMucHitsBarrel->GetState());
		    break;

		case   kM_MucHits_West:
		    view->SetVisMucHitsWest(fChkBtnMucHitsWest->GetState());
		    break;

		case   kM_Tracks_Global:
		    view->SetVisTracksGlobal(fChkBtnTracksGlobal->GetState());
		    break;

		case   kM_Tracks_Mdc:
		    view->SetVisTracksMdc(fChkBtnTracksMdc->GetState());
		    break;

		case   kM_Tracks_Tof:
		    view->SetVisTracksTof(fChkBtnTracksTof->GetState());
		    break;

		case   kM_Tracks_Emc:
		    view->SetVisTracksEmc(fChkBtnTracksEmc->GetState());
		    break;

		case   kM_Tracks_Muc:
		    view->SetVisTracksMuc(fChkBtnTracksMuc->GetState());
		    break;

		case   kM_Tracks_Ext:
		    view->SetVisTracksExt(fChkBtnTracksExt->GetState());
		    break;

		case   kM_Mdc_TMatch_Global:
		    this->SetMdcTFire(fChkBtnMdcTMatchGlobal->GetState());
		    break;

		case   kM_Mdc_QMatch_Global:
		    this->SetMdcQFire(fChkBtnMdcQMatchGlobal->GetState());
		    break;

		    //case   kM_Mdc_TOverflow_Global:
		    //  this->SetMdcTOverflow(fChkBtnMdcTOverflowGlobal->GetState());
		    //  break;

		case   kM_Mdc_QNotOverflow_Global:
		    this->SetMdcQNotOverflow(fChkBtnMdcQOverflowGlobal->GetState());
		    break;

		case   kM_Mdc_ColorfulWire_Global:
		    this->SetMdcColorfulWire(fChkBtnMdcColorfulWireGlobal->GetState());
		    break;

		case   kM_Mdc_MdcTimeSubEvTime_Global:
		    this->SetMdcTimeSubEvTime(fChkBtnMdcTimeSubEvTimeGlobal->GetState());
		    break;

		case   kM_Tof_TMatch_Global:
		    this->SetTofTMatch(fChkBtnTofTMatchGlobal->GetState());
		    break;

		case   kM_Tof_QMatch_Global:
		    this->SetTofQMatch(fChkBtnTofQMatchGlobal->GetState());
		    break;
	    }


	    view->UpdateView(0);

	    ((TCanvas*)fEmbeddedCanvas->GetCanvas())->Modified();
	    ((TCanvas*)fEmbeddedCanvas->GetCanvas())->Update();
	}

	UpdateStatus();
    }

    //_____________________________________________________

void BesClient::UpdateStatus() {
	//
	// get status from active BesView instance
	BesView *view = dynamic_cast<BesView*>(gPad->GetView());

	if ( view ) {
	    fZoomRatioNumber->SetNumber(view->GetStatusCurrent()->fZoom*100.0);//yzhang
	    view->SetZoomRatio(view->GetStatusCurrent()->fZoom);//yzhang

	    // fish eye tick in zview menu
	    view->SetFishEye(view->GetFishEyeStatus());

	    // Mdc global
	    fChkBtnMdcGlobal->SetOn(view->GetVisMdcGlobal());

	    // Mdc Tubes
	    fChkBtnMdcTubes->SetOn(view->GetVisMdcTubes());

	    // Mdc Wires
	    fChkBtnMdcWires->SetOn(view->GetVisMdcWires());

	    // Tof global
	    fChkBtnTofGlobal->SetOn(view->GetVisTofGlobal());

	    // Tof east
	    fChkBtnTofEast->SetOn(view->GetVisTofEast());

	    // Tof barrel
	    fChkBtnTofBarrel->SetOn(view->GetVisTofBarrel());

	    // Tof west
	    fChkBtnTofWest->SetOn(view->GetVisTofWest());

	    // Emc global
	    fChkBtnEmcGlobal->SetOn(view->GetVisEmcGlobal());

	    // Emc east
	    fChkBtnEmcEast->SetOn(view->GetVisEmcEast());

	    // Emc barrel
	    fChkBtnEmcBarrel->SetOn(view->GetVisEmcBarrel());

	    // Emc west
	    fChkBtnEmcWest->SetOn(view->GetVisEmcWest());

	    // Emc side
	    fChkBtnEmcSide->SetOn(view->GetVisEmcSide());

	    // Muc global
	    fChkBtnMucGlobal->SetOn(view->GetVisMucGlobal());

	    // Muc east
	    fChkBtnMucEast->SetOn(view->GetVisMucEast());

	    // Muc barrel
	    fChkBtnMucBarrel->SetOn(view->GetVisMucBarrel());

	    // Muc west
	    fChkBtnMucWest->SetOn(view->GetVisMucWest());

	    // Muc strips
	    fChkBtnMucStrips->SetOn(view->GetVisMucStrips());

	    // BeamPipe
	    fChkBtnBeamPipe->SetOn(view->GetVisBeamPipe());

	    // ZRPlaneOnXY
	    fChkBtnZRPlaneOnXY->SetOn(view->GetVisZRPlaneOnXY());

	    // Axis
	    fChkBtnAxis->SetOn(view->GetVisAxis());

        //Long Peixun's update: for Header
        fChkBtnHeaderGlobal->SetOn(fDisplay->GetVisHeader());

	    // Mdc Hits global
	    fChkBtnMdcHitsGlobal->SetOn(view->GetVisMdcHitsGlobal());

	    // Tof Hits global
	    fChkBtnTofHitsGlobal->SetOn(view->GetVisTofHitsGlobal());

	    // Tof Hits east
	    fChkBtnTofHitsEast->SetOn(view->GetVisTofHitsEast());

	    // Tof Hits barrel
	    fChkBtnTofHitsBarrel->SetOn(view->GetVisTofHitsBarrel());

	    // Tof Hits west
	    fChkBtnTofHitsWest->SetOn(view->GetVisTofHitsWest());

	    // Emc Hits global
	    fChkBtnEmcHitsGlobal->SetOn(view->GetVisEmcHitsGlobal());

	    // Emc Hits east
	    fChkBtnEmcHitsEast->SetOn(view->GetVisEmcHitsEast());

	    // Emc Hits barrel
	    fChkBtnEmcHitsBarrel->SetOn(view->GetVisEmcHitsBarrel());

	    // Emc Hits west
	    fChkBtnEmcHitsWest->SetOn(view->GetVisEmcHitsWest());

	    // Emc Hits side
	    fChkBtnEmcHitsSide->SetOn(view->GetVisEmcHitsSide());

	    // Muc Hits global
	    fChkBtnMucHitsGlobal->SetOn(view->GetVisMucHitsGlobal());

	    // Muc Hits east
	    fChkBtnMucHitsEast->SetOn(view->GetVisMucHitsEast());

	    // Muc Hits barrel
	    fChkBtnMucHitsBarrel->SetOn(view->GetVisMucHitsBarrel());

	    // Muc Hits west
	    fChkBtnMucHitsWest->SetOn(view->GetVisMucHitsWest());

	    // Tracks global
	    fChkBtnTracksGlobal->SetOn(view->GetVisTracksGlobal());

	    // Tracks mdc
	    fChkBtnTracksMdc->SetOn(view->GetVisTracksMdc());

	    // Tracks tof
	    fChkBtnTracksTof->SetOn(view->GetVisTracksTof());

	    // Tracks emc
	    fChkBtnTracksEmc->SetOn(view->GetVisTracksEmc());

	    // Tracks muc
	    fChkBtnTracksMuc->SetOn(view->GetVisTracksMuc());

	    // Tracks ext
	    fChkBtnTracksExt->SetOn(view->GetVisTracksExt());

        //Long Peixun's update: Header
        if (fDisplay->GetVisHeader())
            fMenuViewOptionHeader->CheckEntry(kM_Header_Global);
        else
            fMenuViewOptionHeader->UnCheckEntry(kM_Header_Global);

	    // Mdc Global
	    if ( view->GetVisMdcGlobal() )
		fMenuViewOptionMdc->CheckEntry(kM_Mdc_Global);
	    else
		fMenuViewOptionMdc->UnCheckEntry(kM_Mdc_Global);

	    // Mdc Tubes
	    if ( view->GetVisMdcTubes() )
		fMenuViewOptionMdc->CheckEntry(kM_Mdc_Tubes);
	    else
		fMenuViewOptionMdc->UnCheckEntry(kM_Mdc_Tubes);

	    // Mdc Wires
	    if ( view->GetVisMdcWires() )
		fMenuViewOptionMdc->CheckEntry(kM_Mdc_Wires);
	    else
		fMenuViewOptionMdc->UnCheckEntry(kM_Mdc_Wires);

	    // Tof Global
	    if ( view->GetVisTofGlobal() )
		fMenuViewOptionTof->CheckEntry(kM_Tof_Global);
	    else
		fMenuViewOptionTof->UnCheckEntry(kM_Tof_Global);

	    // Tof East
	    if ( view->GetVisTofEast() )
		fMenuViewOptionTof->CheckEntry(kM_Tof_East);
	    else
		fMenuViewOptionTof->UnCheckEntry(kM_Tof_East);

	    // Tof Barrel
	    if ( view->GetVisTofBarrel() )
		fMenuViewOptionTof->CheckEntry(kM_Tof_Barrel);
	    else
		fMenuViewOptionTof->UnCheckEntry(kM_Tof_Barrel);

	    // Tof West
	    if ( view->GetVisTofWest() )
		fMenuViewOptionTof->CheckEntry(kM_Tof_West);
	    else
		fMenuViewOptionTof->UnCheckEntry(kM_Tof_West);

	    // Emc Global
	    if ( view->GetVisEmcGlobal() )
		fMenuViewOptionEmc->CheckEntry(kM_Emc_Global);
	    else
		fMenuViewOptionEmc->UnCheckEntry(kM_Emc_Global);

	    // Emc East
	    if ( view->GetVisEmcEast() )
		fMenuViewOptionEmc->CheckEntry(kM_Emc_East);
	    else
		fMenuViewOptionEmc->UnCheckEntry(kM_Emc_East);

	    // Emc Barrel
	    if ( view->GetVisEmcBarrel() )
		fMenuViewOptionEmc->CheckEntry(kM_Emc_Barrel);
	    else
		fMenuViewOptionEmc->UnCheckEntry(kM_Emc_Barrel);

	    // Emc West
	    if ( view->GetVisEmcWest() )
		fMenuViewOptionEmc->CheckEntry(kM_Emc_West);
	    else
		fMenuViewOptionEmc->UnCheckEntry(kM_Emc_West);

	    // Emc Side
	    if ( view->GetVisEmcSide() )
		fMenuViewOptionEmc->CheckEntry(kM_Emc_Side);
	    else
		fMenuViewOptionEmc->UnCheckEntry(kM_Emc_Side);

	    // Muc Global
	    if ( view->GetVisMucGlobal() )
		fMenuViewOptionMuc->CheckEntry(kM_Muc_Global);
	    else
		fMenuViewOptionMuc->UnCheckEntry(kM_Muc_Global);

	    // Muc East
	    if ( view->GetVisMucEast() )
		fMenuViewOptionMuc->CheckEntry(kM_Muc_East);
	    else
		fMenuViewOptionMuc->UnCheckEntry(kM_Muc_East);

	    // Muc Barrel
	    if ( view->GetVisMucBarrel() )
		fMenuViewOptionMuc->CheckEntry(kM_Muc_Barrel);
	    else
		fMenuViewOptionMuc->UnCheckEntry(kM_Muc_Barrel);

	    // Muc West
	    if ( view->GetVisMucWest() )
		fMenuViewOptionMuc->CheckEntry(kM_Muc_West);
	    else
		fMenuViewOptionMuc->UnCheckEntry(kM_Muc_West);

	    // Muc Strips
	    if ( view->GetVisMucStrips() )
		fMenuViewOptionMuc->CheckEntry(kM_Muc_Strips);
	    else
		fMenuViewOptionMuc->UnCheckEntry(kM_Muc_Strips);

	    // Full3D Mdc
	    if ( view->GetVisFull3DMdc() )
		fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Mdc);
	    else
		fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Mdc);

	    // Full3D Tof
	    if ( view->GetVisFull3DTof() )
		fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Tof);
	    else
		fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Tof);

	    // Full3D Emc
	    if ( view->GetVisFull3DEmc() )
		fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Emc);
	    else
		fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Emc);

	    // Full3D Muc
	    if ( view->GetVisFull3DMuc() )
		fMenuViewOptionFull3D->CheckEntry(kM_Full3D_Muc);
	    else
		fMenuViewOptionFull3D->UnCheckEntry(kM_Full3D_Muc);

	    // BeamPipe
	    if ( view->GetVisBeamPipe() )
		fMenuViewOptionOthers->CheckEntry(kM_BeamPipe);
	    else
		fMenuViewOptionOthers->UnCheckEntry(kM_BeamPipe);

	    // ZRPlaneOnXY
	    if ( view->GetVisZRPlaneOnXY() )
		fMenuViewOptionOthers->CheckEntry(kM_ZRPlaneOnXY);
	    else
		fMenuViewOptionOthers->UnCheckEntry(kM_ZRPlaneOnXY);

	    // Axis
	    if ( view->GetVisAxis() ) {
		fMenuViewOptionOthers->CheckEntry(kM_Axis);
		fShowAxisButton->SetPicture(gClient->GetPicture("ButtonShowAxisST.gif"));
		fShowAxisButton->SetState(true);
	    }
	    else {
		fMenuViewOptionOthers->UnCheckEntry(kM_Axis);
		fShowAxisButton->SetPicture(gClient->GetPicture("ButtonShowAxis.gif"));
		fShowAxisButton->SetState(false);
	    }

	    // Mdc Hits
	    if ( view->GetVisMdcHits() )
		fMenuViewOptionMdcHits->CheckEntry(kM_MdcHits_Hits);
	    else
		fMenuViewOptionMdcHits->UnCheckEntry(kM_MdcHits_Hits);

	    // Tof hits Global
	    if ( view->GetVisTofHitsGlobal() )
		fMenuViewOptionTofHits->CheckEntry(kM_TofHits_Global);
	    else
		fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_Global);

	    // Tof hits East
	    if ( view->GetVisTofHitsEast() )
		fMenuViewOptionTofHits->CheckEntry(kM_TofHits_East);
	    else
		fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_East);

	    // Tof hits Barrel
	    if ( view->GetVisTofHitsBarrel() )
		fMenuViewOptionTofHits->CheckEntry(kM_TofHits_Barrel);
	    else
		fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_Barrel);

	    // Tof hits West
	    if ( view->GetVisTofHitsWest() )
		fMenuViewOptionTofHits->CheckEntry(kM_TofHits_West);
	    else
		fMenuViewOptionTofHits->UnCheckEntry(kM_TofHits_West);

	    // Emc hits Global
	    if ( view->GetVisEmcHitsGlobal() )
		fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_Global);
	    else
		fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_Global);

	    // Emc hits East
	    if ( view->GetVisEmcHitsEast() )
		fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_East);
	    else
		fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_East);

	    // Emc hits Barrel
	    if ( view->GetVisEmcHitsBarrel() )
		fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_Barrel);
	    else
		fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_Barrel);

	    // Emc hits West
	    if ( view->GetVisEmcHitsWest() )
		fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_West);
	    else
		fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_West);

	    // Emc hits Side
	    if ( view->GetVisEmcHitsSide() )
		fMenuViewOptionEmcHits->CheckEntry(kM_EmcHits_Side);
	    else
		fMenuViewOptionEmcHits->UnCheckEntry(kM_EmcHits_Side);

	    // Muc hits Global
	    if ( view->GetVisMucHitsGlobal() )
		fMenuViewOptionMucHits->CheckEntry(kM_MucHits_Global);
	    else
		fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_Global);

	    // Muc hits East
	    if ( view->GetVisMucHitsEast() )
		fMenuViewOptionMucHits->CheckEntry(kM_MucHits_East);
	    else
		fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_East);

	    // Muc hits Barrel
	    if ( view->GetVisMucHitsBarrel() )
		fMenuViewOptionMucHits->CheckEntry(kM_MucHits_Barrel);
	    else
		fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_Barrel);

	    // Muc hits West
	    if ( view->GetVisMucHitsWest() )
		fMenuViewOptionMucHits->CheckEntry(kM_MucHits_West);
	    else
		fMenuViewOptionMucHits->UnCheckEntry(kM_MucHits_West);

	    // Tracks Global
	    if ( view->GetVisTracksGlobal() )
		fMenuViewOptionTracks->CheckEntry(kM_Tracks_Global);
	    else
		fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Global);

	    // Tracks Mdc
	    if ( view->GetVisTracksMdc() )
		fMenuViewOptionTracks->CheckEntry(kM_Tracks_Mdc);
	    else
		fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Mdc);

	    // Tracks Tof
	    if ( view->GetVisTracksTof() )
		fMenuViewOptionTracks->CheckEntry(kM_Tracks_Tof);
	    else
		fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Tof);

	    // Tracks Emc
	    if ( view->GetVisTracksEmc() )
		fMenuViewOptionTracks->CheckEntry(kM_Tracks_Emc);
	    else
		fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Emc);

	    // Tracks Muc
	    if ( view->GetVisTracksMuc() )
		fMenuViewOptionTracks->CheckEntry(kM_Tracks_Muc);
	    else
		fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Muc);

	    // Tracks Ext
	    if ( view->GetVisTracksExt() )
		fMenuViewOptionTracks->CheckEntry(kM_Tracks_Ext);
	    else
		fMenuViewOptionTracks->UnCheckEntry(kM_Tracks_Ext);

	    // Fish Eye View
	    if ( view->GetFishEye() ) {
		fFishEyeViewButton->SetPicture(gClient->GetPicture("ButtonFishEyeViewST.gif"));
	    }
	    else {
		fFishEyeViewButton->SetPicture(gClient->GetPicture("ButtonFishEyeView.gif"));
	    }

	    // Parallel or Perspective View
	    if ( view->IsPerspective() ) {
		fParallelViewButton->SetPicture(gClient->GetPicture("ButtonParallelView.gif"));
		fPerspectiveViewButton->SetPicture(gClient->GetPicture("ButtonPerspectiveViewST.gif"));
	    }
	    else {
		fParallelViewButton->SetPicture(gClient->GetPicture("ButtonParallelViewST.gif"));
		fPerspectiveViewButton->SetPicture(gClient->GetPicture("ButtonPerspectiveView.gif"));
	    }
	}
	UpdateBesInputFields();
}

//__________________________________________________________________

void BesClient::HandleButtons(Int_t id) {
	//
	// Handle different buttons
	if (id == -1) {
	    TGButton *btn = (TGButton *) gTQSender;
	    id = btn->WidgetId();
	}

	//TCanvas *canvas = (TCanvas*)fEmbeddedCanvas->GetCanvas();
	TString query = "";
	Int_t displayMode = 0;

	Double_t xmin=0.0, ymin=0.0, xmax=0.0, ymax=0.0;
	if (gPad) {
	    xmin = gPad->GetX1();
	    ymin = gPad->GetY1();
	    xmax = gPad->GetX2();
	    ymax = gPad->GetY2();
	}

	BesView *view = 0;
	if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());
	Int_t iret;
	//Double_t theta, phi, psi;

	switch ( id ) {

	    case kM_Button_LoadGeoFile:  // Load geometry file
		LoadGeoFile();
		fLoadGeoFileButton->SetPicture(gClient->GetPicture("ButtonLoadGeoFile.gif"));
		break;

	    case kM_Button_OpenEventFile:  // Load event file
		OpenEventFile();
		fOpenEventFileButton->SetPicture(gClient->GetPicture("ButtonOpenEventFile.gif"));
		break;

	    case kM_Button_SavePicAs:  // Save picture as
		SavePicAs();
		fSavePicAsButton->SetPicture(gClient->GetPicture("ButtonSavePicAs.gif"));
		break;

	    case kM_Button_SavePicAsPS:  // Save picture as *.ps
		fSavePicAsPSButton->SetPicture(gClient->GetPicture("ButtonSavePicAsPSHL.gif"));
		//SavePicAsPS();
		fEmbeddedCanvas->GetCanvas()->Print("besvis.ps", "ps");
		fSavePicAsPSButton->SetPicture(gClient->GetPicture("ButtonSavePicAsPS.gif"));
		break;

	    case kM_Button_Refresh:  // Refresh all pads
		//fDisplay->SwitchDisplayMode(fDisplay->GetDisplayMode());
            if (view) fDisplay->Refresh();  //Long Peixun's update: Refresh all view
            HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
		//if (view) view->UpdateView(0);
		break;

	    case kM_Button_ResetCurrent:  // reset active pad to default
		if (view) view->Reset();
		break;

	    case kM_Button_ResetAll:  // Reset all pads to Default
		if (view) fDisplay->Reset();
		break;

	    case kM_Button_CursorPick:  // Cursor Pick
		gBesCursor->SetType(kBesPick);
		fCursorButton[0]->SetPicture(gClient->GetPicture("ButtonCursorPickST.gif"));
		fCursorButton[0]->SetState(true);
		fCursorButton[1]->SetPicture(gClient->GetPicture("ButtonCursorHand.gif"));
		fCursorButton[1]->SetState(false);
		//fCursorButton[0]->SetState(kButtonEngaged);
		//fCursorButton[1]->SetState(kButtonUp);
		break;

	    case kM_Button_CursorHand:  // Cursor Hand
		gBesCursor->SetType(kBesHand);
		fCursorButton[0]->SetPicture(gClient->GetPicture("ButtonCursorPick.gif"));
		fCursorButton[0]->SetState(false);
		fCursorButton[1]->SetPicture(gClient->GetPicture("ButtonCursorHandST.gif"));
		fCursorButton[1]->SetState(true);
		break;

	    case kM_Button_ZoomOut:  // Zoom out
		if (view) view->ZoomOut();
		break;

	    case kM_Button_ZoomIn:  // Zoom in
		if (view) view->ZoomIn();
		break;

	    case kM_Button_SetHome: // Set Home position
		SetHome();
		break;

	    case kM_Button_GoHome: // Go Home position
		GoHome();
		break;

	    case kM_Button_SaveMyConfig:
		SaveMyConfig();
		fSaveMyConfigButton->SetPicture(gClient->GetPicture("ButtonSaveMyConfig.gif"));
		break;

	    case kM_Button_LoadMyConfig:
		LoadMyConfig();
		fLoadMyConfigButton->SetPicture(gClient->GetPicture("ButtonLoadMyConfig.gif"));
		break;

	    case kM_Button_Palette:
		LoadMdcPalette();
		fPaletteButton->SetPicture(gClient->GetPicture("ButtonPalette.gif"));
		break;

	    case kM_Button_Help:
		Help();
		break;

	    case kM_Button_ShowInfo: // Show Info
		gBesCursor->SetShowInfo(!gBesCursor->GetShowInfo());
		if (gBesCursor->GetShowInfo()) {
		    fShowInfoButton->SetState(true);
		    fShowInfoButton->SetPicture(gClient->GetPicture("ButtonShowInfoST.gif"));
		}
		else {
		    fShowInfoButton->SetState(false);
		    fShowInfoButton->SetPicture(gClient->GetPicture("ButtonShowInfo.gif"));
		}
		break;

	    case kM_Button_ShowAxis: // Show Axis
		if (view) {
		    view->SetVisAxis(!view->GetVisAxis());
		    if (view->GetVisAxis()) {
			fShowAxisButton->SetPicture(gClient->GetPicture("ButtonShowAxisST.gif"));
			fShowAxisButton->SetState(true);
		    }
		    else {
			fShowAxisButton->SetPicture(gClient->GetPicture("ButtonShowAxis.gif"));
			fShowAxisButton->SetState(false);
		    }
		    view->UpdateView(0);
		}
		break;

	    case kM_Button_FishEyeView: // FishEye View
		if (view) {
		    view->SetFishEye(!view->GetFishEye());
		    if (view->GetFishEye()) {
			fFishEyeViewButton->SetPicture(gClient->GetPicture("ButtonFishEyeViewST.gif"));
			fFishEyeViewButton->SetState(true);
		    }
		    else {
			fFishEyeViewButton->SetPicture(gClient->GetPicture("ButtonFishEyeView.gif"));
			fFishEyeViewButton->SetState(false);
		    }
		}
		break;

	    case kM_Button_ParallelView: // Parallel View
		if (view && view->IsPerspective()) {
		    fParallelViewButton->SetPicture(gClient->GetPicture("ButtonParallelViewST.gif"));
		    fParallelViewButton->SetState(true);
		    fPerspectiveViewButton->SetPicture(gClient->GetPicture("ButtonPerspectiveView.gif"));
		    fPerspectiveViewButton->SetState(false);
		    //view->SetParralel();
		    view->SetParallel();
		    // use SetParallel() instead of depreciated function SetParralel()
		    // changed by tianhl at Mon Aug 20 2007
		    view->UpdateView(0);
		}
		break;

	    case kM_Button_PerspectiveView: // Perspective View
		if (view && !view->IsPerspective()) {
		    fParallelViewButton->SetPicture(gClient->GetPicture("ButtonParallelView.gif"));
		    fParallelViewButton->SetState(false);
		    fPerspectiveViewButton->SetPicture(gClient->GetPicture("ButtonPerspectiveViewST.gif"));
		    fPerspectiveViewButton->SetState(true);
		    view->SetPerspective();
		    view->UpdateView(0);
		}
		break;

	    case kM_Button_X3D: // X3D
		X3D();
		fX3DButton->SetPicture(gClient->GetPicture("ButtonX3D.gif"));
		break;

	    case kM_Button_OpenGL: // OpenGL
		OpenGL();
		fOpenGLButton->SetPicture(gClient->GetPicture("ButtonOpenGL.gif"));
		break;

	    case kM_Button_NextEvent:
		NextEvent();
		break;

	    case kM_Button_PrevEvent:
		PrevEvent();
		break;

	    case kM_Button_PlayEvent:
		AutoDisplayEvent();
		break;

	    case kM_Button_FirstEvent:
		FirstEvent();
		break;

	    case kM_Button_ViewResetAngle:
        if (view)
        {   //Long Peixun's update: Consider view == NULL
            if (view->GetViewType() == k3DView) view->Front();
            if (view->GetViewType() == kXYView) view->SetView(  0,  0, 270, iret);
            if (view->GetViewType() == kZRView) view->SetView(180, 90,  90, iret);
        }
		break;

	    case kM_Button_ViewCounterClockWise:
		RotateClockWise(-1);
		if (fAutoRotate) {
		    if (fAutoRotateClockWise != -1) {
			fAutoRotateClockWise = -1;
			fAutoRotatePhi = 0;
		    }
		    else fAutoRotateClockWise = 0;
		}
		break;

	    case kM_Button_ViewClockWise:
		RotateClockWise(1);
		if (fAutoRotate) {
		    if (fAutoRotateClockWise != 1) {
			fAutoRotateClockWise = 1;
			fAutoRotatePhi = 0;
		    }
		    else fAutoRotateClockWise = 0;
		}
		break;

	    case kM_Button_ViewMoveUp:
        //Long Peixun's update: Consider view == NULL
		if (view) view->Move(0,-10);    //Long Peixun's update: Fix up and down direction
		//if (gPad) gPad->Range(xmin, ymin+fMoveFactor*(ymax-ymin), xmax, ymax+fMoveFactor*(ymax-ymin));
		break;

	    case kM_Button_ViewMoveDown:
        //Long Peixun's update: Consider view == NULL
		if (view) view->Move(0,10);     //Long Peixun's update: Fix up and down direction
		break;

	    case kM_Button_ViewMoveLeft:
        //Long Peixun's update: Consider view == NULL
		if (view) view->Move(-10,0);
		break;

	    case kM_Button_ViewMoveRight:
        //Long Peixun's update: Consider view == NULL
		if (view) view->Move(10,0);
		break;

	    case kM_Button_ViewMoveCenter:
        //Long Peixun's update: Consider view == NULL
		if (view) view->Center();
		break;

	    case kM_Button_ViewAngleThetaPlus:
		RotateTheta(1);
		if (fAutoRotate) {
		    if (fAutoRotateTheta != 1) fAutoRotateTheta = 1;
		    else fAutoRotateTheta = 0;
		}
		break;

	    case kM_Button_ViewAngleThetaMinus:
		RotateTheta(-1);
		if (fAutoRotate) {
		    if (fAutoRotateTheta != -1) fAutoRotateTheta = -1;
		    else fAutoRotateTheta = 0;
		}
		break;

	    case kM_Button_ViewAnglePhiPlus:
		RotatePhi(1);
		if (fAutoRotate) {
		    if (fAutoRotatePhi != 1) {
			fAutoRotatePhi = 1;
			fAutoRotateClockWise = 0;
		    }
		    else fAutoRotatePhi = 0;
		}
		break;

	    case kM_Button_ViewAnglePhiMinus:
		RotatePhi(-1);
		if (fAutoRotate) {
		    if (fAutoRotatePhi != -1) {
			fAutoRotatePhi = -1;
			fAutoRotateClockWise = 0;
		    }
		    else fAutoRotatePhi = 0;
		}
		break;

	    case kM_Button_ViewAnglePsiPlus:
		if (view->GetViewType() == k3DView) {
		    RotatePsi(1);
		    if (fAutoRotate) {
			if (fAutoRotatePsi != 1) fAutoRotatePsi = 1;
			else fAutoRotatePsi = 0;
		    }
		}
		break;

	    case kM_Button_ViewAnglePsiMinus:
		if (view->GetViewType() == k3DView) {
		    RotatePsi(-1);
		    if (fAutoRotate) {
			if (fAutoRotatePsi != -1) fAutoRotatePsi = -1;
			else fAutoRotatePsi = 0;
		    }
		}
		break;

	    case kM_Button_AutoRotate:
		AutoRotate();
		break;

	    case kM_Button_DisplayMode2D:
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[0]->SetState(true);
		fDisplayModeButton[0]->SetPicture(gClient->GetPicture("DisplayMode2DST.gif"));
		fDisplay->SwitchDisplayMode(0);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
		break;

	    case kM_Button_DisplayModeXY:
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[1]->SetState(true);
		fDisplayModeButton[1]->SetPicture(gClient->GetPicture("DisplayModeXYST.gif"));
		fDisplay->SwitchDisplayMode(1);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
		break;

	    case kM_Button_DisplayModeZR:
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[2]->SetState(true);
		fDisplayModeButton[2]->SetPicture(gClient->GetPicture("DisplayModeZRST.gif"));
		fDisplay->SwitchDisplayMode(2);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
		break;

	    case kM_Button_DisplayMode3D:
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[3]->SetState(true);
		fDisplayModeButton[3]->SetPicture(gClient->GetPicture("DisplayMode3DST.gif"));
		fDisplay->SwitchDisplayMode(3);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
		break;

	    case kM_Button_DisplayModeAll:
		SetAllDisplayModeButtonUnHL();
		fDisplayModeButton[4]->SetState(true);
		fDisplayModeButton[4]->SetPicture(gClient->GetPicture("DisplayModeAllST.gif"));
		fDisplay->SwitchDisplayMode(4);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
		break;

	    case kM_Button_SwitchDisplayMode:
		displayMode = fDisplay->GetDisplayMode();
		displayMode++;
		if (displayMode >= kNDisplayMode) displayMode = 0;  //Long Peixun's update: Adjust display mode
		fDisplay->SwitchDisplayMode(displayMode);
        HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar

		SetAllDisplayModeButtonUnHL();
		switch (displayMode)
        {
		case 0 :
			fDisplayModeButton[displayMode]->SetPicture(gClient->GetPicture("DisplayMode2DST.gif"));
			break;
		case 1 :
			fDisplayModeButton[displayMode]->SetPicture(gClient->GetPicture("DisplayModeXYST.gif"));
			break;
		case 2 :
			fDisplayModeButton[displayMode]->SetPicture(gClient->GetPicture("DisplayModeZRST.gif"));
			break;
		case 3 :
			fDisplayModeButton[displayMode]->SetPicture(gClient->GetPicture("DisplayMode3DST.gif"));
			break;
		case 4 :
			fDisplayModeButton[displayMode]->SetPicture(gClient->GetPicture("DisplayModeAllST.gif"));
			break;
		default:
			break;
		}
		break;

	    case kM_Button_SwitchPad:
            fDisplay->SwitchPad();
            HandleStatusBar(GetCurrentPadString());  //Long Peixun's update: Update status bar
            break;
	}

	UpdateStatus();
	UpdateCurrentPad();
	UpdateBesInputFields();
}

    //__________________________________________________________________

    void BesClient::HandleSliders(Int_t slider)
    {
	//
	// Handle slider events
	if (gDebug) cout << "BesClient::DoSlider called!" << endl;

	TGButton *btn = (TGButton *) gTQSender;
	Int_t id = btn->WidgetId();

	BesView *view = dynamic_cast<BesView*>(gPad->GetView());
	Int_t iret;

	switch (id) {

	    case kM_Slider_EventPlay:
		fEventPlaySlider->SetPosition(slider);
		if (fEventTree) {
		    fBesEventNo = slider;
		    this->GetEvent(fBesEventNo);
		}
		break;

	    case kM_Slider_ViewAngleTheta:
		if (view) {
		    view->SetView(view->GetLongitude(), slider, view->GetPsi(), iret);
		}
		break;

	    case kM_Slider_ViewAnglePhi:
		if (view) {
		    view->SetView(slider, view->GetLatitude(), view->GetPsi(), iret);
		}
		break;

	    case kM_Slider_ViewAnglePsi:
		if (view && view->GetViewType() == k3DView) {
		    view->SetView(view->GetLongitude(), view->GetLatitude(), slider, iret);
		}
		break;
	}

	UpdateCurrentPad();
	UpdateBesInputFields();
    }

    //_________________________________________________________________

    void BesClient::UpdateAllView()
    {
	TCanvas *canvas = (TCanvas*)fEmbeddedCanvas->GetCanvas();
	TPad *curPad = (TPad*)gPad;//canvas->GetSelectedPad();

	fDisplay->GetPadXY()->cd();
	BesView *view = dynamic_cast<BesView*>(fDisplay->GetPadXY()->GetView());
	if (view) {
	    view->UpdateView(0);
	    cout << "update xy view" << endl;
	}
	else cout << "no xy view" << endl;

	fDisplay->GetPadZR()->cd();
	view = dynamic_cast<BesView*>(fDisplay->GetPadZR()->GetView());
	if (view) {
	    view->UpdateView(0);
	    cout << "update zr view" << endl;
	}
	else cout << "no zr view" << endl;

	fDisplay->GetPad3D()->cd();
	view = dynamic_cast<BesView*>(fDisplay->GetPad3D()->GetView());
	if (view) {
	    view->UpdateView(0);
	    cout << "update 3d view" << endl;
	}
	else cout << "no 3d view" << endl;

	// Header show be drawn last, as it will update all pads and makes tracks in pad drawn first
	fDisplay->DrawHeader();
    fDisplay->Refresh();    //Long Peixun's update: Refresh all views

	curPad->cd();

	UpdateBesInputFields();

	// Redraw canvas
	canvas->Modified();
	canvas->Update();
    }

    //_________________________________________________________________

    void BesClient::UpdateCurrentPad()
    {
	if (gPad) {
	    gPad->Modified();
	    gPad->Update();
	}

	//BesView *view = dynamic_cast<BesView*>(gPad->GetView());

	//TViewerX3D *x3d = 0;
	//x3d = (TViewerX3D*)gPad->GetViewer3D();
	//if (fViewer3DMode == 1 && view && x3d) {
	//    x3d->ExecCommand(Int_t(2*view->GetLatitude()), Int_t(2*view->GetLongitude()), 0);  //rotate
	//}

	////TViewerOpenGL *ogl = 0;
	////ogl = (TViewerOpenGL*)gPad->GetViewer3D();
	//// update from 4.04 to 5.14
	//TVirtualViewer3D *ogl = 0;
	//ogl = (TVirtualViewer3D*)gPad->GetViewer3D("ogl");
	//if (fViewer3DMode == 2 && view && ogl) {
	//    gVirtualGL->ClearGLColor(0.0,0.0,0.0,0.0);   // set GL background color
	//    gVirtualGL->SetGLLineWidth(5);

	//    Double_t deltaTheta = view->GetLatitude()  - fViewThetaPreStep;
	//    Double_t deltaPhi   = view->GetLongitude() - fViewPhiPreStep;
	//    if (deltaTheta >  90.0)  deltaTheta -= 180.0;
	//    if (deltaTheta < -90.0)  deltaTheta += 180.0;
	//    if (deltaPhi   >  180.0) deltaPhi   -= 360.0;
	//    if (deltaPhi   < -180.0) deltaPhi   += 360.0;

	//    // update from 4.04 to 5.14, TViewerOpenGL has been removed,
	//    // TVirtualViewer3D has none those memthods
	//    //UInt_t width = ogl->GetWidth();
	//    //UInt_t height = ogl->GetHeight();
	//    //UInt_t xPos = width/2, yPos = height/2;

	//    //Event_t *event1 = new Event_t;
	//    //event1->fType = kButtonPress;
	//    //event1->fX = xPos; //(Int_t)view->GetLatitude();
	//    //event1->fY = yPos; //(Int_t)view->GetLongitude();
	//    //event1->fCode = kButton1;
	//    //ogl->HandleContainerButton(event1);


	//    //Event_t *event2 = new Event_t;
	//    //event2->fType = kMotionNotify;
	//    //event2->fX = (Int_t)(xPos + deltaTheta);
	//    //event2->fY = (Int_t)(yPos + deltaPhi);
	//    //ogl->HandleContainerMotion(event2);

	//    //Event_t *event3 = new Event_t;
	//    //event3->fType = kButtonRelease;
	//    //event3->fX = (Int_t)(xPos + deltaTheta); //(view->GetLatitude()  + deltaPhi);
	//    //event3->fY = (Int_t)(yPos + deltaPhi); //(view->GetLongitude() + deltaTheta);
	//    //event3->fCode = kButton1;
	//    //ogl->HandleContainerButton(event3);

	//    //fViewThetaPreStep = view->GetLatitude();
	//    //fViewPhiPreStep   = view->GetLongitude();

	//    //delete event1;
	//    //delete event2;
	//    //delete event3;
	//}
    }

    //_________________________________________________________________

    void BesClient::UpdateBesInputFields()
    {
	fNumEntryRunNo->SetNumber(GetBesRunNo());
	fNumEntryEventNo->SetIntNumber(GetBesEventNo());
	fNumEntryEventPlaySpeed->SetNumber(Double_t(GetEventPlaySpeed()) / 1000.0);
	fEventPlaySlider->SetPosition(GetBesEventNo());

	fNumEntryRotateStep->SetNumber(GetRotateStep());
	fNumEntryRotateSpeed->SetNumber(GetRotateSpeed());
	fNumEntryRotateFPS->SetIntNumber(GetRotateFPS());

	BesView *view = dynamic_cast<BesView*>(gPad->GetView());
	if (view) {
	    fZoomRatioNumber->SetNumber(view->GetZoomRatio()*100.0);

	    Double_t theta = view->GetLatitude();
	    Double_t phi   = view->GetLongitude();
	    Double_t psi   = view->GetPsi();
	    SetRange(theta, 0.0, 180.0);
	    SetRange(phi,   0.0, 360.0);
	    SetRange(psi,   0.0, 360.0);
	    fViewAngleThetaNumber->SetNumber(theta);
	    fViewAnglePhiNumber->SetNumber(phi);
	    fViewAnglePsiNumber->SetNumber(psi);
	}
	fViewAngleThetaSlider->SetPosition((Int_t)fViewAngleThetaNumber->GetNumber());
	fViewAnglePhiSlider->SetPosition((Int_t)fViewAnglePhiNumber->GetNumber());
	fViewAnglePsiSlider->SetPosition((Int_t)fViewAnglePsiNumber->GetNumber());

	fChkBtnAutoRotate->SetOn(fAutoRotate);
    }

    //_____________________________________________________

    void BesClient::ChangeFocus() {
	//
	// change focus on pressed tab
	if (gDebug) cout << "BesClient::ChangeFocus called!" << endl;

	if ( gTQSender == fNumEntryRunNo->GetNumberEntry() ) {
	    fNumEntryEventNo->GetNumberEntry()->SelectAll();
	    fNumEntryEventNo->GetNumberEntry()->SetFocus();
	}
	else if ( gTQSender == fNumEntryEventPlaySpeed->GetNumberEntry() ) {
	    fNumEntryEventPlaySpeed->GetNumberEntry()->SelectAll();
	    fNumEntryEventPlaySpeed->GetNumberEntry()->SetFocus();
	}
	else if ( gTQSender == fNumEntryMagnetic->GetNumberEntry() ) {
	    fNumEntryMagnetic->GetNumberEntry()->SelectAll();
	    fNumEntryMagnetic->GetNumberEntry()->SetFocus();
	}
    }

    //_____________________________________________________

    void BesClient::ExecuteReturn() {
	//
	// execute if return was pressed
	if (gDebug) cout << "BesClient::ExecuteReturn called!" << endl;

	BesView *view = dynamic_cast<BesView*>(gPad->GetView());
	//Double_t theta, phi, psi;
	Int_t iret;

	if ( gTQSender == fZoomRatioNumber ) {
	    if (view) view->SetZoomRatio(fZoomRatioNumber->GetNumber()/100.0);
	}

	if ( gTQSender == fNumEntryEventNo->GetNumberEntry() ) {
        //fBesEventNo = fNumEntryEventNo->GetIntNumber();
	    //this->GetEvent(fBesEventNo);

        //Long Peixun's update: If event doesn't exist, don't change fBesEventNo
        Long64_t tempno = fBesEventNo;
        fBesEventNo = fNumEntryEventNo->GetIntNumber();
	    if (!this->GetEvent(fBesEventNo)) fBesEventNo = tempno;
	}

	else if ( gTQSender == fNumEntryEventPlaySpeed->GetNumberEntry() ) {
	    fEventPlaySpeed = Int_t(fNumEntryEventPlaySpeed->GetNumber() * 1000);
	    fAutoDisplayEventTimer->SetTime(fEventPlaySpeed);
	}

	else if ( gTQSender == fNumEntryRotateSpeed ) {
	    fRotateSpeed = fNumEntryRotateSpeed->GetNumber();
	    this->SetRotateStep();
	    cout << "fRotateSpeed " << fRotateSpeed << " fRotateStep " << fRotateStep <<  endl;
	    //fAutoRotateTimer->SetTime((Int_t)1000/this->GetRotateFPS());
	}

	else if ( gTQSender == fNumEntryRotateFPS->GetNumberEntry() ) {
	    fRotateFPS = fNumEntryRotateFPS->GetIntNumber();
	    this->SetRotateSpeed();
	    fAutoRotateTimer->SetTime((Int_t)1000/fRotateFPS);
	    cout << "fRotateFPS " << fRotateFPS << " fRotateStep " << fRotateStep <<  endl;
	}

	else if ( gTQSender == fNumEntryMagnetic->GetNumberEntry() ) {
	    if (gEvent){
		    gEvent->SetMagnetic(fNumEntryMagnetic->GetNumber());
            GetEvent(fBesEventNo);    //Long Peixun's update: Update display when change the magnetic field
	    }
	}

	else if ( gTQSender == fNumEntryRotateStep ) {
	    fRotateStep = fNumEntryRotateStep->GetNumber();
	    fRotateSpeed = fRotateStep * fRotateFPS;
	    cout << "fRotateSpeed " << fRotateSpeed << " fRotateStep " << fRotateStep <<  endl;
	}

	else if ( gTQSender == fViewAngleThetaNumber ) {
	    if (view) {
		view->SetView(view->GetLongitude(), fViewAngleThetaNumber->GetNumber(), view->GetPsi(), iret);
	    }
	}

	else if ( gTQSender == fViewAnglePhiNumber ) {
	    if (view) {
		view->SetView(fViewAnglePhiNumber->GetNumber(), view->GetLatitude(), view->GetPsi(), iret);
	    }
	}

	else if ( gTQSender == fViewAnglePsiNumber ) {
	    if (view && view->GetViewType() == k3DView) {
		view->SetView(view->GetLongitude(), view->GetLatitude(), fViewAnglePsiNumber->GetNumber(), iret);
	    }
	}

	fEmbeddedCanvas->RequestFocus();  // RequestFocus to let Hot Key "QWEASD.." work in ExecuteEvent, or it doesnt work after input
	UpdateCurrentPad();
	UpdateBesInputFields();
    }

    Pixmap_t BesClient::GetPic(const char *file)
    {
	TString filePath = fBesVisPath;
	filePath += "/icons/";
	filePath += file;

	TASImage asImage(filePath);
	Pixmap_t pic = asImage.GetPixmap();
	//asImage->Draw();
	return pic;
    }

    Bool_t BesClient::FileExists(TString fname)
    {
	// gSystem return 0 if exist, 1 for not exist
	return (!gSystem->AccessPathName(fname, kFileExists));
    }

    // makes min <= input < max
    void BesClient::SetRange(Double_t &input, Double_t min, Double_t max)
    {
	Double_t range = max - min;
	if (input < min) {
	    do {
		input += range;
	    }
	    while (input < min);
	}

	if (input >= max) {
	    do {
		input -= range;
	    }
	    while (input >= max);
	}
    }

//Long Peixun's update: Return current pad hint string
const char* BesClient::GetCurrentPadString()
{
    if ((TPad*)gPad == fDisplay->GetPadXY()) return "XY view";
    else if ((TPad*)gPad == fDisplay->GetPadZR()) return "ZR view";
    else if ((TPad*)gPad == fDisplay->GetPad3D()) return "3D view";
    else return "Ready";
}
