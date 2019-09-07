
#ifndef MDC_2D_WIRE_H
#define MDC_2D_WIRE_H

#include <TNamed.h>
#include <TAttLine.h>
#include <TAttFill.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TVector3.h>

#include "BesCircle2D.h"
#include "BesPolygon2D.h"
#include "BesMarker2D.h"
#include "BesPaveText.h"

class Mdc2DWire : public TNamed, public TAttLine, public TAttFill
{
 public:
  
  Mdc2DWire();
  Mdc2DWire(const char* name, const char* title, Double_t rmin, Double_t rmax, Double_t dz,
	    Double_t phi, Double_t *center );
  Mdc2DWire(const char* name, const char* title, Int_t N, Double_t *P);
  ~Mdc2DWire();

  virtual void  Init();
  virtual void  SetStyle();
  virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);

  virtual void  SetHighlighted(bool status = true) { fHighlighted = status; }
  virtual void  SetFired(bool status = true) { fFired = status; }
  virtual bool  IsHighlighted() { return fHighlighted; }
  virtual bool  IsFired() { return fFired; }

  virtual void  AddInfo(TString info) { fInfoCon.push_back(info); }
  virtual void  CloseInfo();
  virtual void  ClearInfo();

  virtual void  Draw(Option_t *option = "");
  virtual void  Paint(Option_t *option = "");
  virtual char *GetObjectInfo(Int_t px, Int_t py) const;

  virtual bool  HasZRSection();
  Double_t Range360(Double_t input);
  void SetTime(Double_t time); 
  void SetCharge(Double_t charge); 
  void SetColorfulWire(Bool_t colorfulWire, Bool_t subEvTime); 
  void SetQOverflow(Bool_t qOvfl);
  void SetEvTime(Double_t time);
  //Long Peixun's update: raw channel data for mdc hits display
  void SetTimeChannel(UInt_t tc);
  void SetChargeChannel(UInt_t cc);

  //Long Peixun's update: Get Mdc Wires information
  Double_t GetTime() const { return mdcTime; }
  Double_t GetCharge() const { return mdcCharge; }
  Double_t GetEvTime() const { return evTime; }
  Bool_t GetQOverflow() const { return qOverflow; }
  UInt_t GetTimeChannel() const { return timeChannel; }
  UInt_t GetChargeChannel() const { return chargeChannel; }

 private:

  Int_t       fWireType; // 1: Axial wire  2: Stereo wire

  BesPolygon2D *fTube;
  BesCircle2D  *fWireCircle;
  BesPolygon2D *fWireCone;
  BesMarker2D  *fZRPosMarker2D; 

  Double_t fWestHole[3], fEastHole[3];
  Double_t fWestPhi, fEastPhi;
  Double_t fZRSectionPos[3];
  Bool_t   fZRSectionFlag;

  std::vector<TString> fInfoCon;
  BesPaveText *fTip;
  Bool_t fHighlighted;
  Bool_t fFired;
  Bool_t fColorfulWire;
  Bool_t qOverflow;

  // style
  Int_t lcTube;
  Int_t lwTube;
  Int_t fcTube;
  Int_t fsTube;

  Int_t lcWire;
  Int_t lwCircle;
  Int_t lwCone;
  Int_t fcCircle;
  Int_t fcCone;
  Int_t fsCircle;
  Int_t fsCone; 

  Int_t lcWireFired;
  Int_t lwCircleFired;
  Int_t lwConeFired;
  Int_t fcCircleFired;
  Int_t fcConeFired;
  Int_t fsCircleFired;
  Int_t fsConeFired;

  Int_t lcWireHL;
  Int_t lwCircleHL;
  Int_t lwConeHL;
  Int_t fcCircleHL;
  Int_t fcConeHL;
  Int_t fsCircleHL;
  Int_t fsConeHL;

  Int_t sizeMarker;
  Int_t cMarker;
  Int_t cMarkerFired;
  Int_t cMarkerHL;
  Int_t sMarker; 
  Int_t sMarkerFired;
  Int_t sMarkerHL;

  Double_t mdcTime;
  Double_t mdcCharge;
  Double_t evTime; 
  //Long Peixun's update: raw channel data
  UInt_t timeChannel;
  UInt_t chargeChannel;

  ClassDef(Mdc2DWire,1) // Mdc 2D Wire
};

#endif
