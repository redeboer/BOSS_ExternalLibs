//
// Bes2DView.h
//
// $Author: maqm $
// 2005-07-21
// Modified from zevis ZView.h
//

#ifndef BESVIEW_H
#define BESVIEW_H

#ifndef BESSTATUS_H
#include "BesStatus.h"
#endif

#include "BesTView.h"

class TPaveLabel;
class TGeometry;
class TPad;
class TROOT;
//class TMath;
class TCanvas;

class Bes2DView : public BesTView {

 private:
  Bool_t       fFishEye;                    // variable needed for toggle in menu
  Int_t        f2DViewType;           // View type
  Float_t      fPhiRangeMin;          // Minimum of phi range for Z-R projection (in degrees)
  Float_t      fPhiRangeMax;          // Maximum of phi range for Z-R projection (in degrees)
  TPaveLabel  *fLabel;                // Label
  Float_t      fMarkerSize;           // size for marker
  BesStatus   *fStatus3D;             // 3D view status flags
  BesStatus   *fStatusXY;             // XY view status flags
  BesStatus   *fStatusZR;             // ZR view status flags
  BesStatus   *fStatusCurrent;        // current view status flags
  Double_t     fZoomFactor;           // zoom factor

 public:
  Bes2DView();
  virtual ~Bes2DView();

  virtual void ClearPad();
  virtual void DrawLabel();
  virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);
  virtual Int_t GetRSign(Float_t Phi);
  virtual void UpdateView(Bool_t resetview = kFALSE );
  virtual void WCtoNDC(const Float_t *pw, Float_t *pn);
  virtual void WCtoNDC(const Double_t *pw, Double_t *pn);
  virtual void NDCtoWC(const Float_t* pn, Float_t* pw);
  virtual void NDCtoWC(const Double_t* pn, Double_t* pw);
  virtual char *GetObjectInfo(Int_t px, Int_t py) const;
  
  inline void    Set2DViewType(Int_t viewType) { f2DViewType = viewType; }
  inline Int_t   Get2DViewType() { return f2DViewType; }
  inline Float_t GetMarkerSize() { return fMarkerSize;}

  // menu
  /*
  inline void View3D() { SetViewType(k3DView,1); }                // *MENU*
  inline void ViewXY() { SetViewType(kXYView,1); }                // *MENU*
  inline void ViewZR() { SetViewType(kZRView,1); }                // *MENU*
  */
  void Zoom();                                                    // *MENU*
  void UnZoom();                                                  // *MENU*
  void Center();                                                  // *MENU*
  void Move(Int_t px, Int_t py);
  /*
  inline void Front3D() {
    this->View3D();
    this->Front(); }                                              // *MENU*
  void Side3D();                                                  // *MENU*
  inline void Top3D() {
    this->View3D();
    Int_t iret;
    this->SetView(-90,90,270,iret);

    //    this->Top();
  }                                                                // *MENU*
  void SetPhiRange(Float_t PhiMin = 0, Float_t PhiMax = 180);      // *MENU*
  */
  void Reset();                                                    // *MENU*
//   void Plot_Et_Eta_Phi();                                          // *MENU*
  inline void SetMarkerSize(Float_t size) { fMarkerSize = size; }

  // access functions for fish eye
  inline Bool_t GetFishEyeStatus() { return fStatusCurrent->fFishEye; }
  inline Bool_t GetFishEye()   { return fFishEye; }
  inline void SetFishEye(Bool_t input = 0)   {
    /*
      if ( fViewType & k3DView ) {
      fFishEye = 0;
      fStatusCurrent->fFishEye = 0;
    } else {
    */
     fFishEye = input;
     fStatusCurrent->fFishEye = input;
  
  } // *TOGGLE*


  inline void PrintCurrent() { fStatusCurrent->Print(); }

  // clean up right mouse button menu
  inline virtual void Centered() { BesTView::Centered(); }
  inline virtual void Front() { BesTView::Top(); }
  inline virtual void Side() { BesTView::Side(); }
  inline virtual void Top() { BesTView::Front(); }
  //  inline virtual void ShowAxis() { BesTView::ShowAxis(); }
  inline virtual void ZoomMove() { BesTView::ZoomMove(); }
  inline virtual void AdjustScales(TVirtualPad* pad = 0) { BesTView::AdjustScales(pad); }
  inline virtual void Delete(Option_t* option) { BesTView::Delete(option); }
  inline virtual void DrawClass() { BesTView::DrawClass(); }
  inline virtual TObject* DrawClone(Option_t* option) { return BesTView::DrawClone(option); }

#ifndef CL__DEBUG
  inline virtual void Dump() { BesTView::Dump(); }
  inline virtual void Inspect() { BesTView::Inspect(); }
#endif

  inline virtual void SetDrawOption(Option_t* option) { BesTView::SetDrawOption(option); }
  inline virtual void SetLineAttributes() { BesTView::SetLineAttributes(); }

  inline virtual void SetZoomFactor(Double_t factor) { fZoomFactor = factor; };
  inline virtual Double_t GetZoomFactor() { return fZoomFactor; }
  inline BesStatus* GetStatusCurrent() { return fStatusCurrent; };

  ClassDef(Bes2DView,0) // Bes View
};

#endif
