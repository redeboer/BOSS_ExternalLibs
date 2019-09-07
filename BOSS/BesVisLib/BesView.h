//
// BesView.h
//

#ifndef BES_BESVIEW
#define BES_BESVIEW

#include "BesTView.h"
#include <TPad.h>
#include <TCanvas.h>
#include <TPaveLabel.h>

#include "BesStatus.h"
#include "BesGeometry.h"

#ifndef BES_EBESViewType
#define BES_EBESViewType

enum EBESViewType {
  k3DView   = BIT(0),
  kXYView   = BIT(1),
  kZRView   = BIT(2),
  //Long Peixun's update: CGEM unfolded display for each layer
  kCgemUFView0 = BIT(3),
  kCgemUFView1 = BIT(4),
  kCgemUFView2 = BIT(5)
};
#endif

class BesView : public BesTView {

 private:
  Bool_t       fFishEye;              // variable needed for toggle in menu
  EBESViewType fViewType;             // View type
  Float_t      fPhiRangeMin;          // Minimum of phi range for Z-R projection (in degrees)
  Float_t      fPhiRangeMax;          // Maximum of phi range for Z-R projection (in degrees)
  TPaveLabel  *fLabel;                // Label
  Float_t      fMarkerSize;           // size for marker
  BesStatus   *fStatus3D;             // 3D view status flags
  BesStatus   *fStatusXY;             // XY view status flags
  BesStatus   *fStatusZR;             // ZR view status flags

  BesStatus   *fStatusCurrent;        // current view status flags
  Double_t     fZoomFactor;           // zoom factor
  Double_t     fExtentDefault;
  Float_t      m_BesR;
  Float_t      m_BesZ;
  Double_t     ViewCenter[2];         //Long Peixun's update: Coordinate of the center of View

 public:
  BesView();
  virtual ~BesView();

  virtual void  SetViewType(EBESViewType ViewType, Bool_t resetview = kFALSE);
  virtual void  SetViewTypeBare(EBESViewType ViewType, Bool_t resetview = kFALSE);
  virtual void  DefaultRange();
  virtual void  ClearPad();
  virtual void  DrawLabel();
  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);

  virtual Int_t GetRSign(Float_t Phi);
  virtual void  UpdateView(Bool_t resetview = kFALSE );
  virtual void  WCtoNDC(const Float_t *pw, Float_t *pn);
  virtual void  WCtoNDC(const Double_t *pw, Double_t *pn);
  virtual void  NDCtoWC(const Float_t* pn, Float_t* pw);
  virtual void  NDCtoWC(const Double_t* pn, Double_t* pw);
  virtual char *GetObjectInfo(Int_t px, Int_t py) const;

  void SetZoomRatio(Double_t ratio);
  inline Double_t GetZoomRatio() { return fExtentDefault/GetExtent(); }
  inline EBESViewType GetViewType() { return fViewType; }
  inline Float_t GetMarkerSize()    { return fMarkerSize;}

  // menu
  inline void View3D() { SetViewType(k3DView,1); }                // *MENU*
  inline void ViewXY() { SetViewType(kXYView,1); }                // *MENU*
  inline void ViewZR() { SetViewType(kZRView,1); }                // *MENU*

  void ZoomIn();                                                  // *MENU*
  void ZoomOut();                                                 // *MENU*
  void Center();                                                  // *MENU*
  void Move(Int_t px, Int_t py);
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
  void SetPhiRange(Float_t PhiMin = 0, Float_t PhiMax = 180); 
  void Reset();                                                    // *MENU*
  void Refresh();   //Long Peixun's update: Refresh

  inline void SetMarkerSize(Float_t size) { fMarkerSize = size; }

  //Long Peixun's update: Add access functions for CGEM visibility
  inline Bool_t GetVisCgemGlobal()               { return fStatusCurrent->fCgemGlobal; }
  inline Bool_t GetVisCgemLayers()               { return fStatusCurrent->fCgemLayers; }
  inline Bool_t GetVisCgemXStrips()              { return fStatusCurrent->fCgemXStrips; }
  inline Bool_t GetVisCgemVStrips()              { return fStatusCurrent->fCgemVStrips; }

  inline void SetVisCgemGlobal(Bool_t input)     { fStatusCurrent->fCgemGlobal = input; }
  inline void SetVisCgemLayers(Bool_t input)     { fStatusCurrent->fCgemLayers = input; }
  inline void SetVisCgemXStrips(Bool_t input)    { fStatusCurrent->fCgemXStrips = input; }
  inline void SetVisCgemVStrips(Bool_t input)    { fStatusCurrent->fCgemVStrips = input; }

  //access functions for Mdc visibility
  inline Bool_t GetVisMdcGlobal()                { return fStatusCurrent->fMdcGlobal; }
  inline Bool_t GetVisMdcTubes()                 { return fStatusCurrent->fMdcTubes;   }
  inline Bool_t GetVisMdcWires()                 { return fStatusCurrent->fMdcWires;  }

  inline void SetVisMdcGlobal (Bool_t input)     { fStatusCurrent->fMdcGlobal = input; }
  inline void SetVisMdcTubes(Bool_t input)       { fStatusCurrent->fMdcTubes  = input; }
  inline void SetVisMdcWires(Bool_t input)       { fStatusCurrent->fMdcWires  = input; }

  //access functions for Tof visibility
  inline Bool_t GetVisTofGlobal()                { return fStatusCurrent->fTofGlobal; }
  inline Bool_t GetVisTofEast()                  { return fStatusCurrent->fTofEast; }
  inline Bool_t GetVisTofBarrel()                { return fStatusCurrent->fTofBarrel; }
  inline Bool_t GetVisTofWest()                  { return fStatusCurrent->fTofWest; }
  inline Float_t GetVisZoom()                    { return fStatusCurrent->fZoom; }
  
  inline void SetVisTofGlobal(Bool_t input)      { fStatusCurrent->fTofGlobal = input; }
  inline void SetVisTofEast(Bool_t input)        { fStatusCurrent->fTofEast   = input; }
  inline void SetVisTofBarrel(Bool_t input)      { fStatusCurrent->fTofBarrel = input; }
  inline void SetVisTofWest(Bool_t input)        { fStatusCurrent->fTofWest   = input; }
  inline void SetVisZoom(Float_t input)          { fStatusCurrent->fZoom = input; }

  //access functions for Emc visibility
  inline Bool_t GetVisEmcGlobal()                { return fStatusCurrent->fEmcGlobal; }
  inline Bool_t GetVisEmcEast()                  { return fStatusCurrent->fEmcEast; }
  inline Bool_t GetVisEmcBarrel()                { return fStatusCurrent->fEmcBarrel; }
  inline Bool_t GetVisEmcWest()                  { return fStatusCurrent->fEmcWest; }
  inline Bool_t GetVisEmcSide()                  { return fStatusCurrent->fEmcSide; }
  
  inline void SetVisEmcGlobal(Bool_t input)      { fStatusCurrent->fEmcGlobal = input; }
  inline void SetVisEmcEast(Bool_t input)        { fStatusCurrent->fEmcEast   = input; }
  inline void SetVisEmcBarrel(Bool_t input)      { fStatusCurrent->fEmcBarrel = input; }
  inline void SetVisEmcWest(Bool_t input)        { fStatusCurrent->fEmcWest   = input; }
  inline void SetVisEmcSide(Bool_t input)        { fStatusCurrent->fEmcSide   = input; }

  //access functions for Muc visibility
  inline Bool_t GetVisMucGlobal()                { return fStatusCurrent->fMucGlobal; }
  inline Bool_t GetVisMucEast()                  { return fStatusCurrent->fMucEast; }
  inline Bool_t GetVisMucBarrel()                { return fStatusCurrent->fMucBarrel; }
  inline Bool_t GetVisMucWest()                  { return fStatusCurrent->fMucWest; }
  inline Bool_t GetVisMucStrips()                { return fStatusCurrent->fMucStrips; }
  
  inline void SetVisMucGlobal(Bool_t input)      { fStatusCurrent->fMucGlobal = input; }
  inline void SetVisMucEast(Bool_t input)        { fStatusCurrent->fMucEast   = input; }
  inline void SetVisMucBarrel(Bool_t input)      { fStatusCurrent->fMucBarrel = input; }
  inline void SetVisMucWest(Bool_t input)        { fStatusCurrent->fMucWest   = input; }
  inline void SetVisMucStrips(Bool_t input)      { fStatusCurrent->fMucStrips = input; }

  //access functions for Full3D visibility
  inline Bool_t GetVisFull3DCgem()               { return fStatusCurrent->fFull3DCgem; }
  inline Bool_t GetVisFull3DMdc()                { return fStatusCurrent->fFull3DMdc; }
  inline Bool_t GetVisFull3DTof()                { return fStatusCurrent->fFull3DTof; }
  inline Bool_t GetVisFull3DEmc()                { return fStatusCurrent->fFull3DEmc; }
  inline Bool_t GetVisFull3DMuc()                { return fStatusCurrent->fFull3DMuc; }
  
  inline void SetVisFull3DCgem(Bool_t input)      { fStatusCurrent->fFull3DCgem = input; }
  inline void SetVisFull3DMdc(Bool_t input)      { fStatusCurrent->fFull3DMdc = input; }
  inline void SetVisFull3DTof(Bool_t input)      { fStatusCurrent->fFull3DTof = input; }
  inline void SetVisFull3DEmc(Bool_t input)      { fStatusCurrent->fFull3DEmc = input; }
  inline void SetVisFull3DMuc(Bool_t input)      { fStatusCurrent->fFull3DMuc = input; }

  //access functions for other parts visibility
  inline Bool_t GetVisBeamPipe()                 { return fStatusCurrent->fBeamPipe;       }
  inline Bool_t GetVisZRPlaneOnXY()              { return fStatusCurrent->fZRPlaneOnXY;    }
  inline Bool_t GetVisAxis()                     { return fStatusCurrent->fAxis;           }

  inline void SetVisBeamPipe(Bool_t input)       { fStatusCurrent->fBeamPipe    = input;   }
  inline void SetVisZRPlaneOnXY(Bool_t input)    { fStatusCurrent->fZRPlaneOnXY = input;   }
  inline void SetVisAxis(Bool_t input)           { fStatusCurrent->fAxis        = input;   }

  //Long Peixun's update: access functions for Cgem hits
  inline Bool_t GetVisCgemHitsGlobal()           { return fStatusCurrent->fCgemHitsGlobal; }
  inline Bool_t GetVisCgemHitsXStrip()           { return fStatusCurrent->fCgemHitsXStrip; }
  inline Bool_t GetVisCgemHitsVStrip()           { return fStatusCurrent->fCgemHitsVStrip; }
  inline Bool_t GetVisCgemHitsClusters()         { return fStatusCurrent->fCgemHitsClusters; }

  inline void SetVisCgemHitsGlobal(Bool_t input) { fStatusCurrent->fCgemHitsGlobal = input; }
  inline void SetVisCgemHitsXStrip(Bool_t input) { fStatusCurrent->fCgemHitsXStrip = input; }
  inline void SetVisCgemHitsVStrip(Bool_t input) { fStatusCurrent->fCgemHitsVStrip = input; }
  inline void SetVisCgemHitsClusters(Bool_t input) { fStatusCurrent->fCgemHitsClusters = input; }

  // access functions for Mdc hits
  inline Bool_t GetVisMdcHitsGlobal()            { return fStatusCurrent->fMdcHitsGlobal;  }
  inline Bool_t GetVisMdcHits()                  { return fStatusCurrent->fMdcHits;        }

  inline void SetVisMdcHitsGlobal (Bool_t input) { fStatusCurrent->fMdcHitsGlobal = input; }
  inline void SetVisMdcHits(Bool_t input)        { fStatusCurrent->fMdcHits   = input;     }

  // access functions for Tof hits
  inline Bool_t GetVisTofHitsGlobal()            { return fStatusCurrent->fTofHitsGlobal;  }
  inline Bool_t GetVisTofHitsEast()              { return fStatusCurrent->fTofHitsEast;    }
  inline Bool_t GetVisTofHitsBarrel()            { return fStatusCurrent->fTofHitsBarrel;  }
  inline Bool_t GetVisTofHitsWest()              { return fStatusCurrent->fTofHitsWest;    }

  inline void SetVisTofHitsGlobal (Bool_t input) { fStatusCurrent->fTofHitsGlobal = input; }
  inline void SetVisTofHitsEast(Bool_t input)    { fStatusCurrent->fTofHitsEast   = input; }
  inline void SetVisTofHitsBarrel(Bool_t input)  { fStatusCurrent->fTofHitsBarrel = input; }
  inline void SetVisTofHitsWest(Bool_t input)    { fStatusCurrent->fTofHitsWest   = input; }

  // access functions for Emc hits
  inline Bool_t GetVisEmcHitsGlobal()            { return fStatusCurrent->fEmcHitsGlobal;  }
  inline Bool_t GetVisEmcHitsEast()              { return fStatusCurrent->fEmcHitsEast;    }
  inline Bool_t GetVisEmcHitsBarrel()            { return fStatusCurrent->fEmcHitsBarrel;  }
  inline Bool_t GetVisEmcHitsWest()              { return fStatusCurrent->fEmcHitsWest;    }
  inline Bool_t GetVisEmcHitsSide()              { return fStatusCurrent->fEmcHitsSide;    }

  inline void SetVisEmcHitsGlobal (Bool_t input) { fStatusCurrent->fEmcHitsGlobal = input; }
  inline void SetVisEmcHitsEast(Bool_t input)    { fStatusCurrent->fEmcHitsEast   = input; }
  inline void SetVisEmcHitsBarrel(Bool_t input)  { fStatusCurrent->fEmcHitsBarrel = input; }
  inline void SetVisEmcHitsWest(Bool_t input)    { fStatusCurrent->fEmcHitsWest   = input; }
  inline void SetVisEmcHitsSide(Bool_t input)    { fStatusCurrent->fEmcHitsSide   = input; }

  // access functions for Muc hits
  inline Bool_t GetVisMucHitsGlobal()            { return fStatusCurrent->fMucHitsGlobal;  }
  inline Bool_t GetVisMucHitsEast()              { return fStatusCurrent->fMucHitsEast;    }
  inline Bool_t GetVisMucHitsBarrel()            { return fStatusCurrent->fMucHitsBarrel;  }
  inline Bool_t GetVisMucHitsWest()              { return fStatusCurrent->fMucHitsWest;    }

  inline void SetVisMucHitsGlobal (Bool_t input) { fStatusCurrent->fMucHitsGlobal = input; }
  inline void SetVisMucHitsEast(Bool_t input)    { fStatusCurrent->fMucHitsEast   = input; }
  inline void SetVisMucHitsBarrel(Bool_t input)  { fStatusCurrent->fMucHitsBarrel = input; }
  inline void SetVisMucHitsWest(Bool_t input)    { fStatusCurrent->fMucHitsWest   = input; }

  // access functions for tracks
  inline Bool_t GetVisTracksGlobal()             { return fStatusCurrent->fTracksGlobal;   }
  inline Bool_t GetVisTracksMdc()                { return fStatusCurrent->fTracksMdc;      }
  inline Bool_t GetVisTracksTof()                { return fStatusCurrent->fTracksTof;      }
  inline Bool_t GetVisTracksEmc()                { return fStatusCurrent->fTracksEmc;      }
  inline Bool_t GetVisTracksMuc()                { return fStatusCurrent->fTracksMuc;      }
  inline Bool_t GetVisTracksExt()                { return fStatusCurrent->fTracksExt;      }

  inline void SetVisTracksGlobal(Bool_t input)         { fStatusCurrent->fTracksGlobal = input; }
  inline void SetVisTracksMdc(Bool_t input)            { fStatusCurrent->fTracksMdc    = input; }
  inline void SetVisTracksTof(Bool_t input)            { fStatusCurrent->fTracksTof    = input; }
  inline void SetVisTracksEmc(Bool_t input)            { fStatusCurrent->fTracksEmc    = input; }
  inline void SetVisTracksMuc(Bool_t input)            { fStatusCurrent->fTracksMuc    = input; }
  inline void SetVisTracksExt(Bool_t input)            { fStatusCurrent->fTracksExt    = input; }

  // access functions for fish eye
  inline Bool_t GetFishEyeStatus() { return fStatusCurrent->fFishEye; }
  inline Bool_t GetFishEye()   { return fFishEye; }
  inline void   SetFishEye(Bool_t input = 0)   {
    if ( fViewType & k3DView ) {
      fFishEye = 0;
      fStatusCurrent->fFishEye = 0;
    } else {
      fFishEye = input;
      fStatusCurrent->fFishEye = input;
    }
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
  inline BesStatus* GetStatus3D()      { return fStatus3D; };
  inline BesStatus* GetStatusXY()      { return fStatusXY; };
  inline BesStatus* GetStatusZR()      { return fStatusZR; };

  ClassDef(BesView,0) // BesVis View
};

#endif
