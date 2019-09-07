//
// ZHelix.h
//
// $Author: tianhl $
// $Date: 2007/10/11 07:16:03 $
// $Revision: 1.1.1.1 $
//

#ifndef ZEVIS_ZHelix
#define ZEVIS_ZHelix

#include <TPolyLine3D.h>
#include <TString.h>
#include <TMath.h>
#include <TPad.h>
#include "BesView.h"

#ifndef ZEVIS_EZHelixRangeType
#define ZEVIS_EZHelixRangeType
enum EZHelixRangeType {
  kHelixPhi,
  kHelixX,
  kHelixY,
  kHelixZ,
  kHelixR
};
#endif

#ifndef ZEVIS_TrackType
#define ZEVIS_TrackType
enum TrackType {
  kVctrhl,
  kVctpar,
  kVcparsec,
  kZtprhl,
  kZttrhl,
  kZttrprm,
  kZttrsec
};
#endif

class ZHelix : public TPolyLine3D {

 protected:
  TrackType        fTrackType;    // Type of track
  Double_t         fAzim;         // Azimuth
  Double_t         fQovR;         // Q/R
  Double_t         fQxDh;         // Q*D_h
  Double_t         fTDip;         // tan(Dip)
  Double_t         fRefX;         // Helix reference point x coordinate
  Double_t         fRefY;         // Helix reference point y coordinate
  Double_t         fRefZ;         // Helix reference point z coordinate
  Double_t         fRange[2];     // Range of helix (in phi)
  EZHelixRangeType fRType;        // Helix range type
  Float_t          fChi2;         // Chi2 of the fit
  Int_t            fNDoF;         // Degrees of freedom
  Double_t         fSinAzim;      // Sin(Azimuth)
  Double_t         fCosAzim;      // Cos(Azimuth)
  // Transient data members
  Int_t            fRSign;        //! sign of radius
  Double_t         fPhiI;         // phi inner
  Double_t         fPhiO;         // phi outer
  Bool_t           fEnable;       // enable of drawing of helix, could be set false for bad tracks


 public:
  ZHelix();
  ZHelix(Double_t Azim, Double_t QovR, Double_t QxDh, Double_t refx, 
	 Double_t refy, Double_t refz, Double_t TDip, Double_t phii, 
	 Double_t phio, Float_t Chi2, Int_t NDoF, 
	 EZHelixRangeType RangeType = kHelixPhi,
	 Double_t RangeMin = 0, Double_t RangeMax = 2*TMath::Pi());
  virtual ~ZHelix();

  void Phi2XYZ(Double_t phi, Double_t& x, Double_t& y, Double_t& z);
  void Phi2ZR(Double_t phi, Double_t& z, Double_t& r);
  void SetRange(EZHelixRangeType RangeType,
		Double_t RangeMin, Double_t RangeMax);
  void SetTrackType(TrackType tt) {fTrackType = tt;};
  virtual void SetPoints(Option_t *option = "3D");
  Double_t X2Phi(Double_t x);
  Double_t Y2Phi(Double_t y);
  Double_t Z2Phi(Double_t z);
  Double_t R2Phi(Double_t r);
  Double_t Phi2S(Double_t phi);
  virtual void Print(Option_t *option = "") const; // *MENU*
  virtual void Draw(Option_t *option = "3D");
  virtual void Paint(Option_t *option = "3D");

  // Dummies
  inline virtual Int_t SetNextPoint(Double_t x, Double_t y, Double_t z) {
    return TPolyLine3D::SetNextPoint(x, y, z);
  }
  inline virtual void SetPoint(Int_t point, Double_t x, Double_t y, Double_t z) {
    TPolyLine3D::SetPoint(point, x, y, z);
  }
  inline virtual void Delete(Option_t *option) { TObject::Delete(option); }
  inline virtual void DrawClass() { TObject::DrawClass(); }
  inline virtual void DrawClone(Option_t *option) { TObject::DrawClone(option); }
  inline virtual void SetDrawOption(Option_t *option) { TObject::SetDrawOption(option); }
  inline virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py) {;}

  ClassDef(ZHelix,8) // ZeVis Helix
};

#endif
