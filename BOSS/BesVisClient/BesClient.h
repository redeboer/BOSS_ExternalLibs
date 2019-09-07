#ifndef BES_Client_H
#define BES_Client_H

#include <cstdio>         //Long Peixun's update: stdio.h -> cstdio
#include <unistd.h>
#include <cstdlib>        //Long Peixun's update: stdlib.h -> cstdlib
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
//#include <wait.h>
#include <cstring>        //Long Peixun's update: string.h -> cstring
#include <string>         //Long Peixun's update: std::string class

#include <TSystem.h>
#include <TGFrame.h>
#include <TGeometry.h>
#include <TGNumberEntry.h>
#include <TViewerX3D.h>
#include <TTimer.h>
#include <TASImage.h>

//Long Peixun's update: remove redundant include
// #include <stdio.h>
// #include <stdlib.h>

//Long Peixun's update: for geometry selection
#include "BesGeoSelector.h"
#include "BesVisLib/BesGMenuBar.h"
#include "BesVisLib/BesGPictureButton.h"
#include "BesVisLib/BesVisDisplay.h"
#include "BesVisLib/BesEvent.h"
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TTrigEvent.h"
#include "RootEventData/TRecEvTime.h"
#include "RootEventData/TRecEmcShower.h"
#include "RootEventData/TDstEvent.h"          //Long Peixun's update: for dst file

// semphore
union semun{
	int	        val;
	struct semid_ds	*buf;
	ushort	        *array;
};

//struct sembuf acquire = {0, -1, SEM_UNDO}; 
//struct sembuf release = {0,  1, SEM_UNDO};

enum {FREE_SPACE, OUTPUT_STORE};
enum runStatus{RUN_ERROR, RUN_SMOOTH};


// BesVis GUI command IDs
#ifndef BES_EZCommand
#define BES_EZCommand
enum EZCommand {
  kM_File_LoadGeo,
  kM_File_SaveGeoAs,
  kM_File_OpenEvent,
  kM_File_SavePicAs,
  kM_File_Close,
  kM_File_Exit,
  kM_Edit_Editor,
  kM_Option_TimerSpeed,
  kM_Option_DefaultPicType,
  kM_Help_Content,
  kM_Help_About
};
#endif

// Bes button command IDs
#ifndef BES_EZButtons
#define BES_EZButtons
enum EZButtons {
  kM_Button_LoadGeoFile,
  kM_Button_SaveGeoAs,
  kM_Button_OpenEventFile,
  kM_Button_SavePicAs,
  kM_Button_SavePicAsPS,
  kM_Button_Refresh,
  kM_Button_ResetCurrent,
  kM_Button_ResetAll,
  kM_Button_CursorPick,
  kM_Button_CursorHand,
  kM_Button_ZoomIn,
  kM_Button_ZoomOut,
  kM_Button_ZoomRatioNumber,
  kM_Button_SwitchDisplayMode,
  kM_Button_SetHome,
  kM_Button_GoHome,
  kM_Button_SaveMyConfig,
  kM_Button_LoadMyConfig,
  kM_Button_Palette,
  kM_Button_Help,
  kM_Button_DisplayMode2D,
  kM_Button_DisplayModeXY,
  kM_Button_DisplayModeZR,
  kM_Button_DisplayMode3D,
  kM_Button_DisplayModeAll,
  kM_Button_SwitchPad,
  kM_Button_ShowInfo,
  kM_Button_ShowAxis,
  kM_Button_FishEyeView,
  kM_Button_ParallelView,
  kM_Button_PerspectiveView,
  kM_Button_X3D,
  kM_Button_OpenGL,
  kM_Button_Online,
  kM_Button_File,
  kM_Button_Down,
  kM_Button_Bes,
  kM_Slider_EventPlay,
  kM_Button_PrevEvent,
  kM_Button_NextEvent,
  kM_Button_PlayEvent,
  kM_Button_FirstEvent,
  kM_Button_ViewResetAngle,
  kM_Button_ViewCounterClockWise,
  kM_Button_ViewClockWise,
  kM_Button_ViewMoveUp,
  kM_Button_ViewMoveLeft,
  kM_Button_ViewMoveRight,
  kM_Button_ViewMoveDown,
  kM_Button_ViewMoveCenter,
  kM_Slider_ViewAngleTheta,
  kM_Button_ViewAngleThetaPlus,
  kM_Button_ViewAngleThetaMinus,
  kM_Button_ViewAngleThetaNumber,
  kM_Slider_ViewAnglePhi,
  kM_Button_ViewAnglePhiPlus,
  kM_Button_ViewAnglePhiMinus,
  kM_Button_ViewAnglePhiNumber,
  kM_Slider_ViewAnglePsi,
  kM_Button_ViewAnglePsiPlus,
  kM_Button_ViewAnglePsiMinus,
  kM_Button_ViewAnglePsiNumber,
  kM_Button_RotateStep,
  kM_Button_AutoRotate,
  kM_Button_RotateSpeed,
  kM_Button_RotateFPS,
  kM_Button_BesRun,
  kM_Button_BesEvent,
  kM_Button_EventPlaySpeed,
  kM_Button_BesSamplePos,
  kM_Button_BesVar,
  kM_Button_BesBit,
  kM_Button_BesSample,
  kM_Button_BesGet,
  kM_Button_BesReset,
  kM_Button_DownZED,
  kM_Button_DownFZ,
  kM_Button_DownMDST,
  kM_Button_DownRaw,
  kM_Button_DownRawIncl,
  kM_Button_DownBrowse,
  kM_Button_DownEvents,
  kM_Button_DownSkip,
  kM_Button_DownGet,
  kM_Button_OnlineStart,
  kM_Button_OnlineTime,
  kM_Button_OnlineTime2,
  kM_Button_PRStart,
  kM_Button_PRTime,
  kM_Button_PRTime2,
  kM_Button_Magnetic,
  kM_Slider_Zoom
};
#endif

// BesVis running modes
#ifndef BES_EZMode
#define BES_EZMode
enum EZMode {
  kModeOnline = BIT(0),
  kModeFile   = BIT(1),
  kModeDown   = BIT(2),
  kModeBes    = BIT(3),
  kModePR     = BIT(4)
};
#endif

#ifndef BES_EZMenuViewOption
#define BES_EZMenuViewOption
enum EZMenuViewOption {
  kM_Header_Global,
  //kM_Header_On,   //Long Peixun's update: remove kM_Header_On, because we just need kM_Header_Global
  kM_Mdc_Global,
  kM_Mdc_Tubes,
  kM_Mdc_Wires,
  kM_Tof_Global,
  kM_Tof_East,
  kM_Tof_Barrel,
  kM_Tof_West,
  kM_Emc_Global,
  kM_Emc_East,
  kM_Emc_Barrel,
  kM_Emc_West,
  kM_Emc_Side,
  kM_Muc_Global,
  kM_Muc_East,
  kM_Muc_Barrel,
  kM_Muc_West,
  kM_Muc_Strips,
  kM_Full3D_Mdc,
  kM_Full3D_Tof,
  kM_Full3D_Emc,
  kM_Full3D_Muc,
  kM_BeamPipe,
  kM_ZRPlaneOnXY,
  kM_Axis,
  kM_MdcHits_Global,
  kM_MdcHits_Hits,
  kM_TofHits_Global,
  kM_TofHits_East,
  kM_TofHits_Barrel,
  kM_TofHits_West,
  kM_EmcHits_Global,
  kM_EmcHits_East,
  kM_EmcHits_Barrel,
  kM_EmcHits_West,
  kM_EmcHits_Side,
  kM_MucHits_Global,
  kM_MucHits_East,
  kM_MucHits_Barrel,
  kM_MucHits_West,
  kM_Tracks_Global,
  kM_Tracks_Mdc,
  kM_Tracks_Tof,
  kM_Tracks_Emc,
  kM_Tracks_Muc,
  kM_Tracks_Ext,
  kM_View_X3D,
  kM_View_OpenGL,
  kM_Standard_On,
  kM_User_On,
  //kM_Mdc_TOverflow_Global,
  kM_Mdc_QNotOverflow_Global,
  kM_Mdc_ColorfulWire_Global,
  kM_Mdc_MdcTimeSubEvTime_Global,
  kM_Mdc_TMatch_Global,
  kM_Mdc_QMatch_Global,
  kM_Tof_TMatch_Global,
  kM_Tof_QMatch_Global
};
#endif

class TList;
class TRootEmbeddedCanvas;
class TApplication;
class TInterpreter;
class TFile;
class TTimer;
class TBranch;
class TTree;
class TGPopupMenu;
class TGStatusBar;
class TGHorizontal3DLine;
class TGLayoutHints;
class TGMenuBar;
class TGNumberEntry;
class TGNumberFormat;
class TGLabel;
class TGHButtonGroup;
class TGRadioButton;
class TGCheckButton;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;
class TGHProgressBar;
class TGButton;
class TGPictureButton;
class TGFileInfo;
class TGFileDialog;
class TGMsgBox;
class TGGroupFrame;
class TGTab;
class TGVSplitter;
class TGCanvas;
class TGListTree;
class TGListTreeItem;
class TGHSlider;
class TGVSlider;
class TCanvas;
class TView;
class TGListTreeItem;

class BesClient : public TGMainFrame {

 private:
  runStatus           f_runStatus;
  TGeometry           *fGeometry;                     // Bes geometry
  TFile               *fGeoFile;                      // ROOT geometry file contains volBes
  TFile               *fEventFile;                      // Current event file
  TTree               *fEventTree;                      // Current event tree
  BesEvent            *fEvent;                        // Current Bes event
  TEvtHeader	      *fEvtHeader;
  TRecEvTime          *fRecEvTime;
  //TTrigEvent	      *fTrigEvent;
  TDigiEvent          *fDigiEvent;
  TRecTrackEvent  *fRecEvent_1;
  TRecMdcTrack *recTrack1;
  TRecMucTrack *muctrk;
  TRecTofTrack *tofTrack;
  //TRecMdcKalTrack *kalTrack;
  TRecMdcHit   *mdchit;
  TRecEmcShower *emcshower;	    
  TDstEvent* fDstEvent;       //Long Peixun's update: for tracks from dst file
  Bool_t isRecEvent;          //Long Peixun's update: is this event rec event?

  Int_t            NCluster;                    //...
  
  TGLayoutHints       *fMenuBarLayout;                // Menu bar layout
  TGLayoutHints       *fMenuBarFirstItemLayout;       // First Item popup menu layout
  TGLayoutHints       *fMenuBarItemLayout;            // Item popup menu layout
  TGLayoutHints       *fMenuBarLastItemLayout;        // Last  Item popup menu layout
  TGLayoutHints       *fECLayout;                     // Embedded canvas layout

  BesGMenuBar         *fMenuBar;                      // Menu bar

  TGPopupMenu         *fMenuFile;                     // File menu
  TGPopupMenu         *fMenuEdit;                     // Edit menu
  TGPopupMenu         *fMenuView;                     // View menu
  TGPopupMenu         *fMenuHelp;                     // Help menu

  TGPopupMenu         *fMenuViewOptionHeader;
  TGPopupMenu         *fMenuViewOptionMdc;
  TGPopupMenu         *fMenuViewOptionTof;
  TGPopupMenu         *fMenuViewOptionEmc;
  TGPopupMenu         *fMenuViewOptionMuc;
  TGPopupMenu         *fMenuViewOptionFull3D;
  TGPopupMenu         *fMenuViewOptionOthers;
  TGPopupMenu         *fMenuViewOptionMdcHits;
  TGPopupMenu         *fMenuViewOptionTofHits;
  TGPopupMenu         *fMenuViewOptionEmcHits;
  TGPopupMenu         *fMenuViewOptionMucHits;
  TGPopupMenu         *fMenuViewOptionTracks;

  TGLayoutHints       *fUpButtonBarFirstItemLayout;   // Up button bar first button layout
  TGLayoutHints       *fUpButtonBarItemLayout;        // Up button bar button layout
  TGLayoutHints       *fUpButtonBarLastItemLayout;    // Up button bar last button layout
  TGCompositeFrame    *fUpButtonBarFrame;             // Up button bar frame

  static const int     kNUpSplitter = 4;
  BesGPictureButton   *fUpButtonSplitter[kNUpSplitter];   // Up button bar splitter
  BesGPictureButton   *fLoadGeoFileButton;            // load geometry file
  BesGPictureButton   *fOpenEventFileButton;          // open event file
  BesGPictureButton   *fSaveGeoAsButton;              // save geometry as root file
  BesGPictureButton   *fSavePicAsButton;              // save picture as
  BesGPictureButton   *fSavePicAsPSButton;           // save picture as pdf
  static const int     kNCursorState = 2;
  BesGPictureButton   *fRefreshButton;                // Refresh
  BesGPictureButton   *fResetCurrentButton;           // Reset current pad
  BesGPictureButton   *fResetAllButton;               // Reset all pad
  BesGPictureButton   *fCursorButton[kNCursorState];  // cursor state, pick or hand
  BesGPictureButton   *fZoomInButton;                 // ZoomIn
  BesGPictureButton   *fZoomOutButton;                // ZoomOut
  TGNumberEntryField  *fZoomRatioNumber;              // ZoomRatio number
  BesGPictureButton   *fZoomRatioPercentButton;       // "%" picture
  BesGPictureButton   *fSwitchDisplayModeButton;      // button to switch among display modes
  BesGPictureButton   *fSetHomeButton;                // Set home position
  BesGPictureButton   *fGoHomeButton;                 // Go to home position
  BesGPictureButton   *fSaveMyConfigButton;           // Save my style
  BesGPictureButton   *fLoadMyConfigButton;           // Load my style
  BesGPictureButton   *fPaletteButton;                // Display palette axis
  BesGPictureButton   *fHelpButton;                   // Help

  TGLayoutHints       *fDisplayModeBarItemLayout;     // Display mode bar button layout
  TGCompositeFrame    *fDisplayModeBarFrame;          // Display mode bar frame
  static const int     kNDisplayMode = 5;
  BesGPictureButton   *fDisplayModeButton[kNDisplayMode];  // button to switch among display modes

  TGLayoutHints       *fToolBarFirstItemLayout;       // Tool bar first button layout
  TGLayoutHints       *fToolBarItemLayout;            // Tool bar button layout
  TGLayoutHints       *fToolBarLastItemLayout;        // Tool bar last button layout
  TGCompositeFrame    *fToolBarFrame;                 // Tool bar frame

  static const int     kNToolSplitter = 4;
  BesGPictureButton   *fToolButtonSplitter[kNToolSplitter];   // Tool button bar splitter
  BesGPictureButton   *fSwitchPadButton;              // button to switch among pads
  BesGPictureButton   *fShowInfoButton;               // Show info or not
  BesGPictureButton   *fShowAxisButton;               // Show axis or not
  BesGPictureButton   *fFishEyeViewButton;            // FishEye     view
  BesGPictureButton   *fParallelViewButton;           // Parallel    view
  BesGPictureButton   *fPerspectiveViewButton;        // Perspective view
  BesGPictureButton   *fX3DButton;                    // X3D    view
  BesGPictureButton   *fOpenGLButton;                 // OpenGL view
  
  TGCanvas            *fEventListView;                // Canvas for event list tree
  TRootEmbeddedCanvas *fEmbeddedCanvas;               // Embedded Canvas
  TCanvas             *fCanvas;
  BesVisDisplay       *fDisplay;

  TGVerticalFrame     *fV1;                           // Splitter for tabs

  TGTab               *fTabs;                         // Tabs for event

  TGListTree          *fEventListTree;                  // Event list tree
  TGListTreeItem      *fItem;                         // Active event list tree item
  TGListTreeItem      *fRunItem;                      // Active run list tree item


  TGCompositeFrame    *fEventDownFrame;                 // tab fame for download mode
  TGCompositeFrame    *fEventControlFrame;              // Frame for event control
  TGCompositeFrame    *fEventControlTextFrame;          // Frame for label "Event Control"
  TGCompositeFrame    *fEventNavFrame;                  // Frame for Event navigation 
  TGCompositeFrame    *fEventPlayTextFrame;             // Frame for label "Event Play"
  TGCompositeFrame    *fEventPlayButtonFrame;           // Frame for buttons "prev, next, play, stop"
  TGCompositeFrame    *fEventPlaySpeedFrame;            // Frame for set event play speed
  TGCompositeFrame    *fEventListFrame;                 // Frame for event list browser
  TGCompositeFrame    *fEventListTextFrame;             // Frame for label "Event file list"
  TGCompositeFrame    *fEventListContentFrame;          // Frame for event list content

  TGCompositeFrame    *fAutoFrame;                    // Frame for auto control
  TGCompositeFrame    *fAutoTextFrame;                // Frame for label "Auto Control"

  TGCompositeFrame    *fViewFrame;                    // Frame for view control
  TGCompositeFrame    *fViewTextFrame;                // Frame for label "View Control"
  TGCompositeFrame    *fViewContentFrame;             // Frame for view content
  TGCompositeFrame    *fViewContent1Frame;            // Frame for view content first line
  TGCompositeFrame    *fViewContent2Frame;            // Frame for view content seconde line
  TGCompositeFrame    *fViewContent3Frame;            // Frame for view content third line
  TGCompositeFrame    *fViewAngleFrame;               // Frame for view angles
  TGCompositeFrame    *fViewAngleThetaFrame;          // Frame for view theta
  TGCompositeFrame    *fViewAnglePhiFrame;            // Frame for view phi
  TGCompositeFrame    *fViewAnglePsiFrame;            // Frame for view psi

  TGNumberEntry       *fNumEntryRunNo;                // Bes input of run number
  TGNumberEntry       *fNumEntryEventNo;              // Bes input of event number
  TGNumberEntry       *fNumEntryEventPlaySpeed;       // event play speed number
  //TGNumberEntry       *fBesSamplePos;               // Bes input of sample position

  TGLabel             *fEventControlTextLabel;        // Label "Event control"
  TGLabel             *fBesLabelRun;                  // Label "Run : "
  TGLabel             *fBesLabelEvent;                // Label "Event : "
  TGLabel             *fEventPlayTextLabel;           // Label "Event Play"
  TGLabel             *fEventPlaySpeedLabel;          // Label "Play Speed : "
  TGLabel             *fEventListTextLabel;           // Label "Event file list"
  TGLabel             *fViewTextLabel;                // Label "View control"
  TGLabel             *fAutoTextLabel;                // Label "Auto control"

  TGHSlider           *fEventPlaySlider;              // slider for event play
  BesGPictureButton   *fPrevEventButton;              // next event button
  BesGPictureButton   *fNextEventButton;              // prev event button
  BesGPictureButton   *fPlayEventButton;              // next event button
  BesGPictureButton   *fFirstEventButton;             // prev event button

  BesGPictureButton   *fViewResetAngleButton;         // View reset angle button
  BesGPictureButton   *fViewCounterClockWiseButton;   // View CounterClockWise button
  BesGPictureButton   *fViewClockWiseButton;          // View ClockWise button
  BesGPictureButton   *fViewMoveUpButton;             // View MoveUp button
  BesGPictureButton   *fViewMoveLeftButton;           // View MoveLeft button
  BesGPictureButton   *fViewMoveRightButton;          // View MoveRight button
  BesGPictureButton   *fViewMoveDownButton;           // View MoveDown button
  BesGPictureButton   *fViewMoveCenterButton;         // View MoveCenter button

  BesGPictureButton   *fViewAngleThetaPlusButton;     // View AngleTheta + button
  BesGPictureButton   *fViewAngleThetaMinusButton;    // View AngleTheta - button
  TGVSlider           *fViewAngleThetaSlider;         // View AngleTheta slider
  TGNumberEntryField  *fViewAngleThetaNumber;         // View AngleTheta number
  TGLabel             *fViewAngleThetaLabel;          // Label "Theta"

  BesGPictureButton   *fViewAnglePhiPlusButton;       // View AnglePhi + button
  BesGPictureButton   *fViewAnglePhiMinusButton;      // View AnglePhi - button
  TGVSlider           *fViewAnglePhiSlider;           // View AnglePhi slider
  TGNumberEntryField  *fViewAnglePhiNumber;           // View AnglePhi number
  TGLabel             *fViewAnglePhiLabel;            // Label "Phi"

  BesGPictureButton   *fViewAnglePsiPlusButton;       // View AnglePsi + button
  BesGPictureButton   *fViewAnglePsiMinusButton;      // View AnglePsi - button
  TGVSlider           *fViewAnglePsiSlider;           // View AnglePsi slider
  TGNumberEntryField  *fViewAnglePsiNumber;           // View AnglePsi number
  TGLabel             *fViewAnglePsiLabel;            // Label "Psi"

  TGCompositeFrame    *fRotateStepFrame;              // Frame for RotateStep
  TGLabel             *fRotateStepLabel;              // Label "Deg"
  TGNumberEntryField  *fNumEntryRotateStep;           // Rotate step
  TGLabel             *fRotateStepUnitLabel;          // Label "Deg"

  TGGroupFrame        *fFrameAutoRotate;              // Frame for AutoRotate
  TGCheckButton       *fChkBtnAutoRotate;             // CheckButton AutoRotate;

  TGCompositeFrame    *fRotateSpeedFrame;             // Frame for RotateSpeed
  TGNumberEntryField  *fNumEntryRotateSpeed;          // Auto Rotate speed number
  TGLabel             *fRotateSpeedLabel;             // Label "Deg/Sec"

  TGCompositeFrame    *fRotateFPSFrame;               // Frame for RotateFPS
  TGNumberEntry       *fNumEntryRotateFPS;            // Auto Rotate fps number
  TGLabel             *fRotateFPSLabel;               // Label "fps"


  TGStatusBar         *fStatusBar;                    // Status bar

  TGGroupFrame        *fFrameHeader;                  // Frame for Header
  TGGroupFrame        *fFrameHits;                    // Frame for Hits
  TGGroupFrame        *fFrameTracks;                  // Frame for Tracks

  TGGroupFrame        *fFrameSubDetector;       
  //TGGroupFrame        *fFrameMdc;                     
  //TGGroupFrame        *fFrameTof;                    
  //TGGroupFrame        *fFrameEmc;                     
  //TGGroupFrame        *fFrameMuc;                    
  TGGroupFrame        *fFrameOthers;                

  TGGroupFrame        *fFrameMdcMatch;                  // Frame for TQMatch
  TGGroupFrame        *fFrameTofMatch;                  // Frame for TQMatch
  TGGroupFrame        *fFrameMagnetic;               // Frame for magnetic field

  TGHorizontalFrame   *fTitleFrame;                   // Title frame
  TGHorizontalFrame   *fMainFrame;                    // Main frame

  TGHorizontalFrame   *fBesRunFrame;                  // Bes Run nr input frame
  TGHorizontalFrame   *fBesEventFrame;                  // Bes Event nr input frame

  TGCheckButton       *fChkBtnHeaderGlobal;

  TGCheckButton       *fChkBtnMdcHitsGlobal;
  TGCheckButton       *fChkBtnTofHitsGlobal;
  TGCheckButton       *fChkBtnTofHitsEast;
  TGCheckButton       *fChkBtnTofHitsBarrel;
  TGCheckButton       *fChkBtnTofHitsWest;
  TGCheckButton       *fChkBtnEmcHitsGlobal;
  TGCheckButton       *fChkBtnEmcHitsEast;
  TGCheckButton       *fChkBtnEmcHitsBarrel;
  TGCheckButton       *fChkBtnEmcHitsWest;
  TGCheckButton       *fChkBtnEmcHitsSide;
  TGCheckButton       *fChkBtnMucHitsGlobal;
  TGCheckButton       *fChkBtnMucHitsEast;
  TGCheckButton       *fChkBtnMucHitsBarrel;
  TGCheckButton       *fChkBtnMucHitsWest;

  TGCheckButton       *fChkBtnTracksGlobal;
  TGCheckButton       *fChkBtnTracksMdc;
  TGCheckButton       *fChkBtnTracksTof;
  TGCheckButton       *fChkBtnTracksEmc;
  TGCheckButton       *fChkBtnTracksMuc;
  TGCheckButton       *fChkBtnTracksExt;

  TGCheckButton       *fChkBtnMdcGlobal;
  TGCheckButton       *fChkBtnMdcTubes;
  TGCheckButton       *fChkBtnMdcWires;
  TGCheckButton       *fChkBtnTofGlobal;
  TGCheckButton       *fChkBtnTofEast;
  TGCheckButton       *fChkBtnTofBarrel;
  TGCheckButton       *fChkBtnTofWest;
  TGCheckButton       *fChkBtnEmcGlobal;
  TGCheckButton       *fChkBtnEmcEast;
  TGCheckButton       *fChkBtnEmcBarrel;
  TGCheckButton       *fChkBtnEmcWest;
  TGCheckButton       *fChkBtnEmcSide;
  TGCheckButton       *fChkBtnMucGlobal; 
  TGCheckButton       *fChkBtnMucEast;
  TGCheckButton       *fChkBtnMucBarrel;
  TGCheckButton       *fChkBtnMucWest;
  TGCheckButton       *fChkBtnMucStrips;
  
  TGCheckButton       *fChkBtnBeamPipe;
  TGCheckButton       *fChkBtnZRPlaneOnXY;
  TGCheckButton       *fChkBtnAxis;

  TGCheckButton       *fChkBtnMdcTOverflowGlobal;
  TGCheckButton       *fChkBtnMdcQOverflowGlobal;
  TGCheckButton       *fChkBtnMdcTMatchGlobal;
  TGCheckButton       *fChkBtnMdcQMatchGlobal;
  TGCheckButton       *fChkBtnMdcColorfulWireGlobal;
  TGCheckButton       *fChkBtnMdcTimeSubEvTimeGlobal;
  TGCheckButton       *fChkBtnTofTMatchGlobal;
  TGCheckButton       *fChkBtnTofQMatchGlobal;

  TGNumberEntry       *fNumEntryMagnetic;             // magnetic number
  
  TString              fBesVisPath;

  Long64_t             fBesRunNo;                     // run number of BES request
  Long64_t             fBesEventNo;                   // event number of BES request
  Long64_t	       fCurrentEvent;
  Int_t                fEventPlaySpeed;               // event play speed 
  Bool_t               fAutoDisplayEvent;             // auto display event 
  TTimer              *fAutoDisplayEventTimer;        

  Double_t             fMoveFactor;                   // factor of move
  Double_t             fRotateStep;                   // rotate angle in each step 
  Int_t                fRotateFPS;                    // rotae fps, frame per second, 24? 10?
  Double_t             fRotateSpeed;                  // rotate angle per second, deg/sec
  Bool_t               fAutoRotate;                   // auto rotate flag
  TTimer              *fAutoRotateTimer;              // auto rotate timer
  Int_t                fAutoRotateClockWise;          // auto rotate clockwise flag
  Int_t                fAutoRotateTheta;              // auto rotate theta flag
  Int_t                fAutoRotatePhi;                // auto rotate phi   flag
  Int_t                fAutoRotatePsi;                // auto rotate psi   flag

  Int_t                fViewer3DMode;                 // Mode of Viewer3D, 1:x3d, 2:OpenGL
  //TViewerX3D          *fx3d;
  Double_t             fViewThetaPreStep;
  Double_t             fViewPhiPreStep;

  Double_t             fHomeX[3], fHomeY[3];          // Home position of three pads

  Int_t                fWindowWidth;                  // zevis window width
  Int_t                fWindowHeight;                 // zevis window height

  TList               *fWidgets;                      // list to hold TGLayoutHints for destruction
  TGLayoutHints       *fLayout;                       // dummy TGLayoutHints;

  // added by tianhl to add besvis to bossframework
  TString    f_evtFile;
  TString    f_geoFile; 
  Bool_t     f_bossMode;
  Int_t	     f_pid; 
  //Long Peixun's update: remove useless variable
  //TString   f_evtFile_1; //huangb add

  //Long Peixun's update: Help text loaded from help.txt
  std::string f_HelpText;
  //Long Peixun's update: for geometry selection
  BesGeoSelector fSelector;
  //Long Peixun's update: current geometry file (for default geom)
  TString fCurGeom;
  //Long Peixun's update: whether current geometry is selected automatically
  Bool_t  fAutoGeom;

 private:
  //void SetMdcTOverflow(Bool_t input);
  void SetMdcQNotOverflow(Bool_t input);
  void SetMdcTFire(Bool_t input);
  void SetMdcQFire(Bool_t input);
  void SetMdcColorfulWire(Bool_t input);
  void SetMdcTimeSubEvTime(Bool_t input);
  void SetTofTMatch(Bool_t input);
  void SetTofQMatch(Bool_t input);
  
 public:
  BesClient(const TGWindow *p, const char* title, UInt_t width, UInt_t height, Option_t *option = "", Int_t argc = 0, char **argv = 0);
  virtual ~BesClient();
  virtual void InitLocal();
  virtual void InitParameter(); 
  virtual void InitConnections();
  //Long Peixun's update: Initialization of geometry selector
  virtual void InitGeoSelector(const char* geomsel_file);

  virtual void CreateWidget(const char* title, UInt_t width, UInt_t height);
  virtual void CreateHorizontalRuler();
  virtual void CreateMenuBar();
  virtual void CreateUpButtonBar();
  virtual void CreateDisplayModeBar();
  virtual void CreateToolBar();
  virtual void CreateTitleBar();
  virtual void CreateStatusBar();
  virtual void CreateMainFrame();
  virtual void CreateCanvas();
  virtual void CreateTabs();
 
  virtual void HandleMenu(Int_t id);
  virtual void HandleViewOptionMenu(Int_t id);
  virtual void HandleError(const char* msg);
  virtual void HandleInfoBar(const char* msg);
  virtual void HandleStatusBar(const char* msg);
  virtual void HandleButtons(Int_t id=-1);
  virtual void HandleSliders(Int_t id);
  virtual void HandleEventList(TGListTreeItem *entry, Int_t btn);
  virtual void UpdateBesInputFields();
 
  virtual void LoadGeoFile();
  virtual void OpenGeoFile(TString filename);
  virtual void SaveGeoAs();

  //Long Peixun's update: Add a parameter to determine whether choose a detector geometry automatically when open an event file.
  virtual void OpenEventFile();
  virtual void OpenEventFile(TString filename, bool auto_sel_geom = false);

  virtual void SavePicAs();
  virtual void SavePicAsPS();
  virtual void SetHome();
  virtual void GoHome();
  virtual void SaveMyConfig();
  virtual void LoadMyConfig();
  virtual void LoadMdcPalette();
  virtual void Help();

  virtual void SetAllDisplayModeButtonUnHL();

  virtual void X3D();
  virtual void OpenGL();

  virtual void Show();
  virtual void CloseWindow();

  //Long Peixun's update: Get run number and choose a detector geometry automatically when in need.
  virtual Bool_t GetEvent(Long64_t i, bool openfile = false, bool sel_geom = false);
  virtual Bool_t GetRecEvent();
  virtual Bool_t NextEvent();
  virtual Bool_t PrevEvent();
  virtual void   AutoDisplayEvent();
  virtual void   AutoDisplayEventCommand();
  virtual Bool_t FirstEvent();
  virtual Bool_t LastEvent();     //Long Peixun's update: Get last event

  virtual void   RotateClockWise(int clockwise);
  virtual void   RotateTheta(int pn);
  virtual void   RotatePhi(int pn);
  virtual void   RotatePsi(int pn);
  virtual void   AutoRotate();
  virtual void   AutoRotateCommand();

  virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel);
  virtual void SetState(Int_t id = -1);
  virtual void UpdateStatus();
  virtual void UpdateAllView();
  virtual void UpdateCurrentPad();
  virtual void ChangeFocus();
  virtual void ExecuteReturn();

  virtual Pixmap_t GetPic(const char *file);
  virtual Bool_t   FileExists(TString fname);
  virtual void     SetRange(Double_t &input, Double_t min, Double_t max);
  virtual const char* GetCurrentPadString();  //Long Peixun's update: Return current pad hint string

  inline Long64_t GetBesRunNo()                 { return fBesRunNo;        }
  inline Long64_t GetBesEventNo()               { return fBesEventNo;      }
  inline Int_t GetEventPlaySpeed()           { return fEventPlaySpeed;    }

  inline Int_t    GetRotateFPS()                { return fRotateFPS;      }
  inline Double_t GetRotateSpeed()              { return fRotateSpeed;    }
  inline Double_t GetRotateStep()               { return fRotateStep; }
  inline void     SetRotateSpeed()              { fRotateSpeed = fRotateStep * fRotateFPS; }
  inline void     SetRotateStep()               { if (fRotateFPS != 0) fRotateStep = fRotateSpeed/fRotateFPS; else fRotateStep = 0.0; }

  ClassDef(BesClient,0) // Bes client
};

#endif
