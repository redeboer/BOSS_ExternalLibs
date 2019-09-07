
#ifndef EMC_2D_CRYSTAL_H
#define EMC_2D_CRYSTAL_H

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

class Emc2DCrystal : public TNamed, public TAttLine, public TAttFill
{
 public:
  
  Emc2DCrystal();
  Emc2DCrystal(const char* name, const char* title, Int_t N, Double_t *P, Int_t part, Int_t theta);
  ~Emc2DCrystal();

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

  virtual Int_t GetPart()  { return fPart; }
  virtual Int_t GetTheta() { return fTheta; }
  virtual bool  HasZRSection();
  Double_t GetAngle(Double_t x, Double_t y); // degree, 0~360
  Double_t Range360(Double_t input);
  virtual bool  IsXYVisible();
  virtual bool  IsSideVisible();

  virtual void  SetXYPoint(Double_t *p1, Double_t *p2, Double_t *newP, Double_t z);
  virtual void  SetZRPoint(Double_t *p1, Double_t *p2, Double_t *newP, Double_t r);
  virtual void  ProjectECToSide(Double_t *p1, Double_t *newP);
 private:

  Int_t         fPart;
  Int_t         fTheta;
  BesPolygon2D *fCrystalXY;
  BesPolygon2D *fCrystalZR;
  BesPolygon2D *fCrystalSide;
  BesPolygon2D *fCrystalXYFired;
  BesPolygon2D *fCrystalZRFired;
  BesPolygon2D *fCrystalSideFired;

  static const Int_t fChargeMax = 300; 
  Double_t fTime, fCharge;

  Double_t fZRSectionTolerance[3];  // to avoid on zrSection nothing drawn sometimes
  Double_t fPhiMin, fPhiMax;
  
  //static const Int_t ecXYPointZ = 1420.0; // theta0:1407.71, theta5:1416.8
  //static const Int_t brZRPointR = 990.0;  // these two doule define the tube of emc, R=brZRPointR, Z=+-ecXYPointZ;
  // 2011.02.24 root update from 5.14 to 5.24
  static const Int_t ecXYPointZ = 1420; // theta0:1407.71, theta5:1416.8
  static const Int_t brZRPointR = 990;  // these two doule define the tube of emc, R=brZRPointR, Z=+-ecXYPointZ;
  static const Int_t sideDeltaPhi = 21;   // deg, side emc viewPhi +- sideDeltaPhi range 

  std::vector<TString> fInfoCon;
  BesPaveText *fTip;
  bool fHighlighted;
  bool fFired;
  
  // style
  Int_t lcCrystal; // line color
  Int_t lwCrystal; // line width
  Int_t fcCrystal; // fill color
  Int_t fsCrystal; // fill style
  Int_t fsCrystalTrans;

  Int_t lcCrystalFired;
  Int_t lwCrystalFired;
  Int_t fcCrystalFired;
  Int_t fsCrystalFired;

  Int_t lcCrystalHL;
  Int_t lwCrystalHL;
  Int_t fcCrystalHL;
  Int_t fsCrystalHL;

  Int_t lcCrystalFiredHL;
  Int_t lwCrystalFiredHL;
  Int_t fcCrystalFiredHL;
  Int_t fsCrystalFiredHL;

  ClassDef(Emc2DCrystal,1) // Emc 2D Crystal

};

#endif
