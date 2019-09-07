
#ifndef MUC_2D_STRIP_H
#define MUC_2D_STRIP_H

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

class Muc2DStrip : public TNamed, public TAttLine, public TAttFill
{
 public:
  
  Muc2DStrip();
  Muc2DStrip(const char* name, const char* title, Int_t N, Double_t *P, Int_t part, Int_t seg, Int_t gap, Int_t strip);
  ~Muc2DStrip();

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

  virtual Int_t GetPart()  { return fPart; }
  virtual Int_t GetSeg()   { return fSeg; }
  virtual Int_t GetGap()   { return fGap; }
  virtual Int_t GetStrip() { return fStrip; }

  virtual bool  IsZRVisible();
  Double_t GetAngle(Double_t x, Double_t y); // degree, 0~360
  Double_t Range360(Double_t input);

 private:

  Int_t         fPart;
  Int_t         fSeg;
  Int_t         fGap;
  Int_t         fStrip;
  
  BesPolygon2D *fStripXY;
  BesPolygon2D *fStripZR;
  
  std::vector<TString> fInfoCon;
  BesPaveText *fTip;
  bool fHighlighted;
  bool fFired;
  
  // style
  Int_t lcStrip; // line color
  Int_t lwStrip; // line width
  Int_t lsStrip; // line width
  Int_t fcStrip; // fill color
  Int_t fsStrip; // fill style

  Int_t lcStripFired;
  Int_t lwStripFired;
  Int_t lsStripFired;
  Int_t fcStripFired;
  Int_t fsStripFired;

  Int_t lcStripHL;
  Int_t lwStripHL;
  Int_t lsStripHL;
  Int_t fcStripHL;
  Int_t fsStripHL;

  static const Int_t m_kDrawLineRange = 2000; // if view x range > 2000, a barrel strip is very small, so dont draw edge lines 
  ClassDef(Muc2DStrip,1) // Muc 2D Strip

};

#endif
