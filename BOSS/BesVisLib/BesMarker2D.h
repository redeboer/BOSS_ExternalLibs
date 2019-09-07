
#ifndef BES_Marker2D
#define BES_Marker2D

#include <TMarker.h>

class BesView;
class TPad;
class TString;
class TPaveText;


class BesMarker2D : public TMarker  {
  
 private:
  TString   fName;  // name of line
  TString   fTitle; // title of line
  Float_t   fWCX;     // marker x pos in world coordinaten
  Float_t   fWCY;     // marker y pos in world coordinaten
  Float_t   fWCZ;     // marker z pos in world coordinaten
  Int_t     fSizeMultiple; // marker size mulitple * TView->GetMarkerSize 

 public:
  BesMarker2D();
  BesMarker2D(const char* name, const char* title, Float_t x, Float_t y, Float_t z);
  BesMarker2D(const char* name, const char* title, Double_t x, Double_t y, Double_t z);
  virtual ~BesMarker2D();
  
  //virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
  virtual void  Draw(Option_t *option = "");
  virtual void  Paint(Option_t *option = "");

  inline virtual void  GetCenterPixel(Int_t &px, Int_t &py);
  inline virtual void  SetMarkerSizeMultiple(Int_t input) { fSizeMultiple = input; }
  inline virtual Int_t GetMarkerSizeMultiple() { return fSizeMultiple; }

  inline virtual const char  *GetName() const {return fName.Data();}
  inline virtual const char  *GetTitle() const {return fTitle.Data();}

  inline virtual Float_t GetWCX() { return fWCX; }
  inline virtual Float_t GetWCY() { return fWCY; }
  inline virtual Float_t GetWCZ() { return fWCZ; }
  
  inline virtual void SetWCX(Float_t input) { fWCX = input; }
  inline virtual void SetWCY(Float_t input) { fWCY = input; }
  inline virtual void SetWCZ(Float_t input) { fWCZ = input; }

  virtual void SetParameter(const char* name, const char* title, Float_t x, Float_t y, Float_t z);
  virtual char *GetObjectInfo(Int_t px, Int_t py) const;

  ClassDef(BesMarker2D,2) //  Bes 2D marker
};

#endif
