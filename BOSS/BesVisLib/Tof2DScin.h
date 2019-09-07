
#ifndef TOF_2D_SCIN_H
#define TOF_2D_SCIN_H

#include <vector>
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

class Tof2DScin : public TNamed, public TAttLine, public TAttFill
{
 public:
  
  Tof2DScin();
  Tof2DScin(const char* name, const char* title, Int_t N, Double_t *P, Int_t part);
  ~Tof2DScin();

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

  virtual void  SetTime(Double_t time) { fTime = time; }
  virtual void  SetCharge(Double_t charge) { fCharge = charge; }
  virtual Double_t GetTime()   { return fTime; }
  virtual Double_t GetCharge() { return fCharge; }
  virtual void  ResetTimeCharge() { fTime = 0.0; fCharge = 0.0; }

  virtual void  Draw(Option_t *option = "");
  virtual void  Paint(Option_t *option = "");
  virtual char *GetObjectInfo(Int_t px, Int_t py) const;
  
  virtual Int_t GetPart() { return fPart; }
  virtual bool  HasZRSection();
  Double_t GetAngle(Double_t x, Double_t y); // degree, 0~360
  Double_t Range360(Double_t input);

  //Long Peixun's update: Get Tof Wires information
  void SetTimeChannel(UInt_t tc) { timeChannel = tc; }
  void SetChargeChannel(UInt_t cc) { chargeChannel = cc; }
  UInt_t GetTimeChannel() const { return timeChannel; }
  UInt_t GetChargeChannel() const { return chargeChannel; }

 private:

  Int_t         fPart;
  BesPolygon2D *fScinXY;
  BesPolygon2D *fScinZR;
  BesPolygon2D *fScinXYFired;
  BesPolygon2D *fScinZRFired;

  static const Int_t fChargeMax = 50; 
  Double_t fTime, fCharge;

  Double_t fZRSectionTolerance[3];  // to avoid on zrSection nothing drawn sometimes
  Double_t fPhiMin, fPhiMax;
  
  std::vector<TString> fInfoCon;
  BesPaveText *fTip;
  bool fHighlighted;
  bool fFired;
  
  // style
  Int_t lcScin;
  Int_t lwScin;
  Int_t fcScin;
  Int_t fsScin;

  Int_t lcScinFired;
  Int_t lwScinFired;
  Int_t fcScinFired;
  Int_t fsScinFired;

  Int_t lcScinHL;
  Int_t lwScinHL;
  Int_t fcScinHL;
  Int_t fsScinHL;

  Int_t lcScinFiredHL;
  Int_t lwScinFiredHL;
  Int_t fcScinFiredHL;
  Int_t fsScinFiredHL;

  //Long Peixun's update: raw channel data
  UInt_t timeChannel;
  UInt_t chargeChannel;

  ClassDef(Tof2DScin,1) // Tof 2D Scin

};

#endif
