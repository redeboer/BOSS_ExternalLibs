#ifndef BesVisDisplay_H
#define BesVisDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// BesVisDisplay                                                          //
//                                                                      //
// Utility class to display ATLAS outline, tracks, clusters, jets,..    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <TQObject.h>
#include <TCanvas.h>
#include <TArc.h>
#include <TWbox.h>
#include <TString.h>
#include <TGeoVolume.h>
#include <TView.h>
#include <TImage.h>
#include <TPaveText.h>

#include "MdcROOTGeo.h"
#include "TofROOTGeo.h"
#include "EmcROOTGeo.h"
#include "MucROOTGeo.h"
#include "BesView.h"
#include "Bes2DView.h"
#include "BesGeometry.h"
#include "BesHeader.h"
#include  <vector>

using std::vector;

class TPolyLine3D;

class BesVisDisplay : public TQObject {

private:

  Int_t            fDisplayMode;          // Flag for display mode, 0: 2D XY+ZR, 1: 2D XY, 2: 2D ZR, 3: 3D, 4: 2D+3D
                                          //Long Peixun's setting: 5 for CGEM Unfolded display, 6 for CGEM Unfolded all view
  TPad            *fPadHeader;
  TPad            *fPadXY;
  TPad            *fPadZR;
  TPad            *fPad3D;

  BesHeader       *fBesHeader;
  TImage          *fHeaderImage;
  TImage          *fHeaderHImage;
  TImage          *fHeaderVImage;
  //TImage          *fMdcPaletteImage;

  Double_t         fLatticeWidth;  // Lattice Width
  Double_t         fTextSizeTitle; // Text Size: Title
  Double_t         fTextSizeTime;  // Text Size: Time
  Double_t         fTextSizeData;  // Text Size: Data
  Double_t         fTextSizeFlt;   // Text Size: FLT
  Double_t         fCoordx;        // x coordinate of text
  Double_t         fCoordy;        // y coordinate of text
  Double_t         fDistanceOfRows;// Distance between rows
  
  Bool_t           fMdcOn;
  Bool_t           fTofOn;
  Bool_t           fEmcOn;

  BesGeometry     *fBesGeometry;
  
  Bool_t            fDrawAllViews;         //Flag True if AllViews selected
  Bool_t            fDrawParticles;        //Flag True if particles to be drawn
  Bool_t            fDrawParticles2;  
  Bool_t            fVisHeader;            //Long Peixun's update: visibility flag
  
  Bool_t            fDrawClusters;         //Flag True if clusters to be drawn
  Float_t           fPTcut;                //PT cut to display objects
  Float_t           fPTcutEGMUNU;          //PT cut for Electrons, Gammas, MUons, Neutrinos
  Float_t           fRin;                  //Inner ATLAS radius
  Float_t           fRout;                 //Outer ATLAS radius
  Float_t           fZin;                  //Inner ATLAS length along Z
  Float_t           fZout;                 //Outer ATLAS length along Z
  Float_t           fTheta;                //Viewing angle theta
  Float_t           fPhi;                  //Viewing angle phi
  Float_t           fBesR;                 //BesR
  Float_t           fBesZ;                 //BesZ
  TCanvas          *fCanvas;               //Pointer to the display canvas
  TPad             *fTrigPad;              //Pointer to the trigger pad 
  TPad             *fButtons;              //Pointer to the buttons pad
  TPad             *fPad;                  //Pointer to the event display main pad
  TArc             *fEM1;                  //Pointer to arc showing ON/OFF trigger EM1
  TArc             *fPH1;                  //Pointer to arc showing ON/OFF trigger PH1
  TArc             *fEM2;                  //Pointer to arc showing ON/OFF trigger EM2
  TArc             *fMU1;                  //Pointer to arc showing ON/OFF trigger MU1
  TArc             *fMU2;                  //Pointer to arc showing ON/OFF trigger MU2
  TArc             *fEMU;                  //Pointer to arc showing ON/OFF trigger EMU
  TArc             *fJT1;                  //Pointer to arc showing ON/OFF trigger JT1
  TArc             *fJT3;                  //Pointer to arc showing ON/OFF trigger JT3
  TArc             *fJT4;                  //Pointer to arc showing ON/OFF trigger JT4
  TArc             *fALL;                  //Pointer to arc showing ON/OFF trigger ALL
  TGeoVolume       *fBes;                  //Bes world volume
  Int_t             fRayPicCount;          //Counter of picture
  
  MdcROOTGeo       *fMdcROOTGeo;           //MdcROOTGeo
  TofROOTGeo       *fTofROOTGeo;           //TofROOTGeo
  EmcROOTGeo       *fEmcROOTGeo;           //EmcROOTGeo
  MucROOTGeo       *fMucROOTGeo;           //MucROOTGeo

  Int_t             fMdcCon;               //Mdc Construction flag
  Int_t             fTofCon;               //Tof Construction flag
  Int_t             fEmcCon;               //Emc Construction flag
  Int_t             fMucCon;               //Muc Construction flag
  Int_t             fAllVis;               //All node visible flag
  Int_t             fQuarterVis;           //Quarter  visible flag
  Int_t             fHalfVis;              //Half     visible flag
  Int_t             fNoEndVis;              //NoEnd    visible flag

  BesView          *fxyView;               // XY View pointer;
  BesView          *fzrView;               // ZR View pointer;
  BesView          *f3DView;               // 3D View pointer;
  
 public:
                     BesVisDisplay();
                     BesVisDisplay(const char *title);
   virtual          ~BesVisDisplay();

   virtual void      SetCanvas(TCanvas *c=0);

   inline  void      SetDisplayMode(Int_t mode) { fDisplayMode = mode; }
   inline  Int_t     GetDisplayMode() { return fDisplayMode; }
   inline  Bool_t    IsVHeader()
   {
        if (fDisplayMode == 0 || fDisplayMode == 4) return false;
        else return true;
   }
   virtual void      SwitchDisplayMode(Int_t mode);
   virtual void      ResizePad();
   virtual void      SwitchPad();
   TPad *            GetPadXY() { return fPadXY; }
   TPad *            GetPadZR() { return fPadZR; }
   TPad *            GetPad3D() { return fPad3D; }
   virtual void      DrawHeader();
   virtual void      Reset();
   virtual void      Refresh();   //Long Peixun's update: Refresh views

   //Long Peixun's update: Add parameters for MRPC and CGEM
   virtual void      InitGeometryFromGDML(const TString fPath, bool mrpc = false, bool cgem = false);
   virtual void      InitGeometryFromROOT(TGeoVolume *bes);
   BesGeometry*      GetBesGeometry() { return fBesGeometry; }
   
   virtual void      SetMdcOn(Bool_t MdcOn);  // *TOGGLE*
   inline Bool_t     GetMdcOn() { return fMdcOn; }
   virtual void      SetTofOn(Bool_t TofOn);  // *TOGGLE*
   inline Bool_t     GetTofOn() { return fTofOn; }
   virtual void      SetEmcOn(Bool_t EmcOn);  // *TOGGLE*
   inline Bool_t     GetEmcOn() { return fEmcOn; }
   
   //Long Peixun's update: Get and Set Header visibility flag
   virtual void   SetVisHeader(Bool_t val);
   inline  Bool_t GetVisHeader() const { return fVisHeader; }

   virtual Bool_t    AllViews() {return fDrawAllViews;}
   virtual void      Clear(Option_t *option="");
   //virtual void      DisplayButtons();
   virtual void      DisplayTrigger(Int_t trig);
   virtual Int_t     DistancetoPrimitive(Int_t px, Int_t py);
   virtual void      Draw(Option_t *option="");
           void      Draw2D(Option_t *option="");
           void      Draw3D(Option_t *option="");
           void      Draw2DXY(Option_t *option="");
           void      Draw2DZR(Option_t *option="");
   virtual void      DrawAllViews();
   Bool_t            DrawClusters() {return fDrawClusters;}
   Bool_t            DrawParticles() {return fDrawParticles;}

   Bool_t            DrawParticles2() {return fDrawParticles2;}
   
   virtual void      DrawTitle(Option_t *option="");
   virtual void      DrawView(Float_t theta, Float_t phi);
   void              DrawViewRange(Double_t x0, Double_t y0, Double_t z0, Double_t zoomSize);
   virtual void      DrawViewGL();
   virtual void      DrawViewX3D();
   virtual void      DrawViewRaytracer(Float_t theta, Float_t phi, Float_t psi);
   virtual void      DrawViewX3D(char option);
   void              DrawImageSequence(Int_t thetaStart, Int_t thetaStep);
   virtual void      ExecuteEvent(Int_t event, Int_t px, Int_t py);
   virtual void      GetEvent(Long64_t event); // *MENU*
   TGeoVolume       *GetVolBes() {return fBes;}
   MdcROOTGeo       *GetMdcROOTGeo() {return fMdcROOTGeo;}
   TofROOTGeo       *GetTofROOTGeo() {return fTofROOTGeo;}
   EmcROOTGeo       *GetEmcROOTGeo() {return fEmcROOTGeo;}
   MucROOTGeo       *GetMucROOTGeo() {return fMucROOTGeo;}
   void              Init();
   //TNode            *Nodin() {return fNodin;}
   //TTUBE            *Tubin() {return fTubin;}
   TPad             *Pad() {return fPad;}
   virtual void      Paint(Option_t *option="");
   virtual void      PaintFruit(TObject *obj, Float_t eta, Float_t phi, Float_t pt, Int_t type, Option_t *option="");
   virtual void      PaintParticles(Option_t *option="");
   
   Float_t           PTcut() {return fPTcut;}
   Float_t           PTcutEGMUNU() {return fPTcutEGMUNU;}
   Float_t           Rin() {return fRin;}
   Float_t           Rout() {return fRout;}
   virtual void      SetDrawClusters(Bool_t draw=kTRUE) {fDrawClusters=draw;}   // *MENU*
   virtual void      SetDrawParticles(Bool_t draw=kTRUE) {fDrawParticles=draw;} // *MENU*
   
   virtual void      SetPTcut(Float_t ptcut=0.4); // *MENU*
   virtual void      SetPTcutEGMUNU(Float_t ptcut=5); // *MENU*
   virtual void      SetView(Float_t theta, Float_t phi);
   virtual void      ShowNextEvent(Int_t delta=1);
   void              SetMdcCon(Int_t mdcCon) {fMdcCon=mdcCon;}
   void              SetTofCon(Int_t tofCon) {fTofCon=tofCon;}
   void              SetEmcCon(Int_t emcCon) {fEmcCon=emcCon;}
   void              SetMucCon(Int_t mucCon) {fMucCon=mucCon;}
   void              SetAllVisible(Int_t vis)     {fAllVis     = vis;}
   void              SetQuarterVisible(Int_t vis) {fQuarterVis = vis;}
   void              SetHalfVisible(Int_t vis)    {fHalfVis    = vis;}
   void              SetNoEndVisible(Int_t vis)   {fNoEndVis   = vis;}
   void              SetMdcFiredCell();
   void              SetTofFiredCell();
   void              SetEmcFiredCell();
   void              SetMucFiredCell();
   virtual void      SizeFruit() const;
   virtual void      SizeParticles() const;

   Float_t           Zin() {return fZin;}
   Float_t           Zout() {return fZout;}

   ClassDef(BesVisDisplay, 1)   //Utility class to display ATLAS outline, tracks, clusters, jets,..
};

R__EXTERN BesVisDisplay *gDisplay;

#endif
