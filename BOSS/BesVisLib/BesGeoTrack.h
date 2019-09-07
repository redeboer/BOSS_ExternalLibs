
#ifndef BESGEOTRACK_H
#define BESGEOTRACK_H

#include <vector>
#include "TPaveText.h"
#include "TGeoTrack.h"

#include "BesMarker2D.h"
#include "BesPaveText.h"

class BesGeoTrack : public TGeoTrack {

 public:

  BesGeoTrack();
  BesGeoTrack(Int_t id, Int_t pdgcode, TVirtualGeoTrack *parent=0, TObject *particle=0);
  virtual ~BesGeoTrack();

  virtual void Init();
  virtual void AddHit(TObject* obj) { fHits.Add(obj); }
  virtual void Add3DHit(TObject* obj) { f3DHits.Add(obj); }   //Long Peixun's update: for 3D track display
  virtual void AddInfo(TString info) { fInfoCon.push_back(info); }
  virtual void CloseInfo();
  virtual void SetCharge(Int_t charge);
  virtual void SetTrackType(Int_t type) { fType = type; }

  virtual void  AddPoint(Double_t x, Double_t y, Double_t z, Double_t t);
  virtual void  SetMarker(Double_t x, Double_t y, Double_t z);
  virtual void  Construct3DLine();    //Long Peixun's update: construct but not draw
  //BesMarker2D  *GetMarker() { return fMarker; }

  virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
  virtual void  Draw(Option_t *option = "");
  virtual void  Paint(Option_t *option = "");
  virtual char *GetObjectInfo(Int_t px, Int_t py) const;

 private:

  TObjArray fHits; 
  TObjArray f3DHits;    //Long Peixun's update: for 3D track display
  std::vector<TString> fInfoCon;
  Int_t    fCharge;
  Int_t    fType; // 1: Mdc, 2: Tof, 3: Emc, 4: Muc, 5: Ext
                  //Long Peixun's update: 6: cluster in CGEM view, 7: cluster in CGEM UF view

  BesPaveText   *fTip;
  TPolyLine3D   *fPolyLine3D;
  BesMarker2D   *fMarker;
  
  ClassDef(BesGeoTrack,1) //  BesGeoTrack
};

#endif


